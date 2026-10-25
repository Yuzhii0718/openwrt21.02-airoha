// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#include <linux/atomic.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#include "eip93-main.h"
#include "eip93-regs.h"
#include "eip93-common.h"
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_SKCIPHER)
#include "eip93-cipher.h"
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
#include "eip93-aes.h"
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
#include "eip93-des.h"
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AEAD)
#include "eip93-aead.h"
#endif

static struct airoha_alg_template *airoha_algs[] = {
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	&airoha_alg_ecb_des,
	&airoha_alg_cbc_des,
	&airoha_alg_ecb_des3_ede,
	&airoha_alg_cbc_des3_ede,
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	&airoha_alg_ecb_aes,
	&airoha_alg_cbc_aes,
	&airoha_alg_ctr_aes,
	&airoha_alg_rfc3686_aes,
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AEAD)
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	&airoha_alg_authenc_hmac_md5_cbc_des,
	&airoha_alg_authenc_hmac_sha1_cbc_des,
	&airoha_alg_authenc_hmac_sha224_cbc_des,
	&airoha_alg_authenc_hmac_sha256_cbc_des,
	&airoha_alg_authenc_hmac_md5_cbc_des3_ede,
	&airoha_alg_authenc_hmac_sha1_cbc_des3_ede,
	&airoha_alg_authenc_hmac_sha224_cbc_des3_ede,
	&airoha_alg_authenc_hmac_sha256_cbc_des3_ede,
#endif
	&airoha_alg_authenc_hmac_md5_cbc_aes,
	&airoha_alg_authenc_hmac_sha1_cbc_aes,
	&airoha_alg_authenc_hmac_sha224_cbc_aes,
	&airoha_alg_authenc_hmac_sha256_cbc_aes,
	&airoha_alg_authenc_hmac_md5_rfc3686_aes,
	&airoha_alg_authenc_hmac_sha1_rfc3686_aes,
	&airoha_alg_authenc_hmac_sha224_rfc3686_aes,
	&airoha_alg_authenc_hmac_sha256_rfc3686_aes,
#endif
};
u32 g_int;
inline void airoha_irq_disable(struct airoha_device *airoha, u32 mask)
{
	__raw_writel(mask, airoha->base + EIP93_REG_MASK_DISABLE);
}

inline void airoha_irq_enable(struct airoha_device *airoha, u32 mask)
{
	__raw_writel(mask, airoha->base + EIP93_REG_MASK_ENABLE);
}

inline void airoha_irq_clear(struct airoha_device *airoha, u32 mask)
{
	__raw_writel(mask, airoha->base + EIP93_REG_INT_CLR);
}

static void airoha_unregister_algs(unsigned int i)
{
	unsigned int j;

	for (j = 0; j < i; j++) {
		switch (airoha_algs[j]->type) {
		case AIROHA_ALG_TYPE_SKCIPHER:
			crypto_unregister_skcipher(&airoha_algs[j]->alg.skcipher);
			break;
		case AIROHA_ALG_TYPE_AEAD:
			crypto_unregister_aead(&airoha_algs[j]->alg.aead);
			break;
		}
	}
}

static int airoha_register_algs(struct airoha_device *airoha)
{
	unsigned int i;
	int err = 0;

	for (i = 0; i < ARRAY_SIZE(airoha_algs); i++) {
		airoha_algs[i]->airoha = airoha;

		switch (airoha_algs[i]->type) {
		case AIROHA_ALG_TYPE_SKCIPHER:
			err = crypto_register_skcipher(&airoha_algs[i]->alg.skcipher);
			break;
		case AIROHA_ALG_TYPE_AEAD:
			err = crypto_register_aead(&airoha_algs[i]->alg.aead);
			break;
		}
		if (err)
			goto fail;
	}

	return 0;

fail:
	airoha_unregister_algs(i);

	return err;
}

static void airoha_handle_result_descriptor(struct airoha_device *airoha)
{
	struct crypto_async_request *async;
	struct eip93_descriptor_s *rdesc;
	bool last_entry;
	u32 flags, i;
	int handled, ready, err;
	union peCrtlStat_w done1;
	union peLength_w done2;

get_more:
	handled = 0;

	ready = readl(airoha->base + EIP93_REG_PE_RD_COUNT) & GENMASK(10, 0);

	if (!ready) {
		airoha_irq_clear(airoha, EIP93_INT_PE_RDRTHRESH_REQ);
		airoha_irq_enable(airoha, EIP93_INT_PE_RDRTHRESH_REQ);
		return;
	}

	last_entry = false;

	while (ready) {
		rdesc = airoha_get_descriptor(airoha);
		if (IS_ERR(rdesc)) {
			dev_err(airoha->dev, "Ndesc: %d nreq: %d\n",
				handled, ready);
			err = -EIO;
			break;
		}
		else
		{
			/*
			u32 *ptr = (u32 *)rdesc;
			for(i=0; i<8; i++)
			{
				printk("@@@@@@@@@@@@@ des(%d)[0x%x]\n", i, ptr[i]);
			}
			*/
			//g_int++;
			//printk("@@@@@@@@@@@@@@@@ Get Data (%d)@@@@@@@@@@@@@@@@@@@@@\n",g_int);
			//printk("<=");
			//printk("@@@@@@@@@ rdesc->arc4Addr[0x%x]]n", phys_to_virt(rdesc->arc4Addr));
		}
		/* make sure DMA is finished writing */
		do {
			done1.word = READ_ONCE(rdesc->peCrtlStat.word);
			done2.word = READ_ONCE(rdesc->peLength.word);
		} while ((!done1.bits.peReady) || (!done2.bits.peReady));

		err = rdesc->peCrtlStat.bits.errStatus;

		flags = rdesc->userId;
		//async = (struct crypto_async_request *)((rdesc->arc4Addr)|PAGE_OFFSET);
		u64 tmpsync = phys_to_virt(rdesc->arc4Addr);
		async = (struct crypto_async_request *)(tmpsync|PAGE_OFFSET);
		writel(1, airoha->base + EIP93_REG_PE_RD_COUNT);
		airoha_irq_clear(airoha, EIP93_INT_PE_RDRTHRESH_REQ);

		handled++;
		ready--;

		if (flags & AIROHA_DESC_LAST) {
			last_entry = true;
			break;
		}
	}

	if (!last_entry)
		goto get_more;
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_SKCIPHER)
	if (flags & AIROHA_DESC_SKCIPHER)
		airoha_skcipher_handle_result(async, err);
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AEAD)
	if (flags & AIROHA_DESC_AEAD)
		airoha_aead_handle_result(async, err);
#endif
	goto get_more;
}

static void airoha_done_task(unsigned long data)
{
	struct airoha_device *airoha = (struct airoha_device *)data;

	airoha_handle_result_descriptor(airoha);
}

static irqreturn_t airoha_irq_handler(int irq, void *dev_id)
{
	struct airoha_device *airoha = (struct airoha_device *)dev_id;
	u32 irq_status;

	irq_status = readl(airoha->base + EIP93_REG_INT_MASK_STAT);

	if (irq_status & EIP93_INT_PE_RDRTHRESH_REQ) {
		airoha_irq_disable(airoha, EIP93_INT_PE_RDRTHRESH_REQ);
		tasklet_schedule(&airoha->ring->done_task);
		return IRQ_HANDLED;
	}

	airoha_irq_clear(airoha, irq_status);
	if (irq_status)
		airoha_irq_disable(airoha, irq_status);

	return IRQ_NONE;
}

static void airoha_initialize(struct airoha_device *airoha)
{
	union peConfig_w peConfig;
	union peEndianCfg_w peEndianCfg;
	union peIntCfg_w peIntCfg;
	union peClockCfg_w peClockCfg;
	union peBufThresh_w peBufThresh;
	union peRingThresh_w peRingThresh;

	/* Reset Engine and setup Mode */
	peConfig.word = 0;
	peConfig.bits.resetPE = 1;
	peConfig.bits.resetRing = 1;
	peConfig.bits.peMode = 3;
	peConfig.bits.enCDRupdate = 1;

	writel(peConfig.word, airoha->base + EIP93_REG_PE_CONFIG);

	udelay(10);

	peConfig.bits.resetPE = 0;
	peConfig.bits.resetRing = 0;

	writel(peConfig.word, airoha->base + EIP93_REG_PE_CONFIG);

	/* Initialize the BYTE_ORDER_CFG register */
	peEndianCfg.word = 0;
	writel(peEndianCfg.word, airoha->base + EIP93_REG_PE_ENDIAN_CONFIG);

	/* Initialize the INT_CFG register */
	peIntCfg.word = 0;
	writel(peIntCfg.word, airoha->base + EIP93_REG_INT_CFG);

	/* Config Clocks */
	peClockCfg.word = 0;
	peClockCfg.bits.enPEclk = 1;
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	peClockCfg.bits.enDESclk = 1;
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	peClockCfg.bits.enAESclk = 1;
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_HMAC)
	peClockCfg.bits.enHASHclk = 1;
#endif
	writel(peClockCfg.word, airoha->base + EIP93_REG_PE_CLOCK_CTRL);

	/* Config DMA thresholds */
	peBufThresh.word = 0;
	peBufThresh.bits.inputBuffer  = 128;
	peBufThresh.bits.outputBuffer = 128;

	writel(peBufThresh.word, airoha->base + EIP93_REG_PE_BUF_THRESH);

	/* Clear/ack all interrupts before disable all */
	airoha_irq_clear(airoha, 0xFFFFFFFF);
	airoha_irq_disable(airoha, 0xFFFFFFFF);

	/* Config Ring Threshold */
	peRingThresh.word = 0;
	peRingThresh.bits.CDRThresh = AIROHA_RING_SIZE - AIROHA_RING_BUSY;
	peRingThresh.bits.RDRThresh = 0;
	peRingThresh.bits.RDTimeout = 5;
	peRingThresh.bits.enTimeout = 1;

	writel(peRingThresh.word, airoha->base + EIP93_REG_PE_RING_THRESH);
}

static void airoha_desc_free(struct airoha_device *airoha)
{
	writel(0, airoha->base + EIP93_REG_PE_RING_CONFIG);
	writel(0, airoha->base + EIP93_REG_PE_CDR_BASE);
	writel(0, airoha->base + EIP93_REG_PE_RDR_BASE);
}

static int airoha_set_ring(struct airoha_device *airoha, struct airoha_desc_ring *ring,
			int Offset)
{
	ring->offset = Offset;
	ring->base = dmam_alloc_coherent(airoha->dev, Offset * AIROHA_RING_SIZE,
					&ring->base_dma, GFP_KERNEL);
	if (!ring->base)
		return -ENOMEM;

	ring->write = ring->base;
	ring->base_end = ring->base + Offset * (AIROHA_RING_SIZE - 1);
	ring->read  = ring->base;

	return 0;
}

static int airoha_desc_init(struct airoha_device *airoha)
{
	struct airoha_state_pool *saState_pool;
	struct airoha_desc_ring *cdr = &airoha->ring->cdr;
	struct airoha_desc_ring *rdr = &airoha->ring->rdr;
	union peRingCfg_w peRingCfg;
	int RingOffset, err, i;

	RingOffset = sizeof(struct eip93_descriptor_s);

	err = airoha_set_ring(airoha, cdr, RingOffset);
	if (err)
		return err;

	err = airoha_set_ring(airoha, rdr, RingOffset);
	if (err)
		return err;

	writel((u32)cdr->base_dma, airoha->base + EIP93_REG_PE_CDR_BASE);
	writel((u32)rdr->base_dma, airoha->base + EIP93_REG_PE_RDR_BASE);

	peRingCfg.word = 0;
	peRingCfg.bits.ringSize = AIROHA_RING_SIZE - 1;
	peRingCfg.bits.ringOffset =  RingOffset / 4;

	writel(peRingCfg.word, airoha->base + EIP93_REG_PE_RING_CONFIG);

	atomic_set(&airoha->ring->free, AIROHA_RING_SIZE - 1);
	/* Create State record DMA pool */
	RingOffset = sizeof(struct saState_s);
	airoha->ring->saState = dmam_alloc_coherent(airoha->dev,
					RingOffset * AIROHA_RING_SIZE,
					&airoha->ring->saState_dma, GFP_KERNEL);
	if (!airoha->ring->saState)
		return -ENOMEM;

	airoha->ring->saState_pool = devm_kcalloc(airoha->dev, 1,
				sizeof(struct airoha_state_pool) * AIROHA_RING_SIZE,
				GFP_KERNEL);

	for (i = 0; i < AIROHA_RING_SIZE; i++) {
		saState_pool = &airoha->ring->saState_pool[i];
		saState_pool->base = airoha->ring->saState + (i * RingOffset);
		saState_pool->base_dma = airoha->ring->saState_dma + (i * RingOffset);
		saState_pool->in_use = false;
	}

	return 0;
}

static void airoha_cleanup(struct airoha_device *airoha)
{
	tasklet_kill(&airoha->ring->done_task);

	/* Clear/ack all interrupts before disable all */
	airoha_irq_clear(airoha, 0xFFFFFFFF);
	airoha_irq_disable(airoha, 0xFFFFFFFF);

	writel(0, airoha->base + EIP93_REG_PE_CLOCK_CTRL);

	airoha_desc_free(airoha);
}

static int airoha_crypto_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct airoha_device *airoha;
	struct resource *res;
	int err;
	g_int = 0;
	
	//printk("====================================== airoha_crypto_probe ======================================\n");
	
	airoha = devm_kzalloc(dev, sizeof(*airoha), GFP_KERNEL);
	if (!airoha)
		return -ENOMEM;

	airoha->dev = dev;
	platform_set_drvdata(pdev, airoha);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	airoha->base = devm_ioremap_resource(&pdev->dev, res);

	if (IS_ERR(airoha->base))
		return PTR_ERR(airoha->base);

	airoha->irq = platform_get_irq(pdev, 0);

	if (airoha->irq < 0)
		return airoha->irq;

	err = devm_request_threaded_irq(airoha->dev, airoha->irq, airoha_irq_handler,
					NULL, IRQF_ONESHOT,
					dev_name(airoha->dev), airoha);

	airoha->ring = devm_kcalloc(airoha->dev, 1, sizeof(*airoha->ring), GFP_KERNEL);

	if (!airoha->ring)
		return -ENOMEM;

	err = airoha_desc_init(airoha);
	if (err)
		return err;

	tasklet_init(&airoha->ring->done_task, airoha_done_task, (unsigned long)airoha);

	spin_lock_init(&airoha->ring->read_lock);
	spin_lock_init(&airoha->ring->write_lock);

	airoha_initialize(airoha);

	/* Init. finished, enable RDR interupt */
	airoha_irq_enable(airoha, EIP93_INT_PE_RDRTHRESH_REQ);

	err = airoha_register_algs(airoha);
	if (err) {
		airoha_cleanup(airoha);
		return err;
	}

	dev_info(airoha->dev, "EIP93 Crypto Engine Initialized.");

	return 0;
}

static int airoha_crypto_remove(struct platform_device *pdev)
{
	struct airoha_device *airoha = platform_get_drvdata(pdev);

	airoha_unregister_algs(ARRAY_SIZE(airoha_algs));
	airoha_cleanup(airoha);
	dev_info(airoha->dev, "EIP93 removed.\n");

	return 0;
}

#if defined(CONFIG_OF)
static const struct of_device_id airoha_crypto_of_match[] = {
	{ .compatible = "airoha,airoha-eip93", },
	{}
};
MODULE_DEVICE_TABLE(of, airoha_crypto_of_match);
#endif

static struct platform_driver airoha_crypto_driver = {
	.probe = airoha_crypto_probe,
	.remove = airoha_crypto_remove,
	.driver = {
		.name = "airoha-eip93",
		.of_match_table = of_match_ptr(airoha_crypto_of_match),
	},
};
module_platform_driver(airoha_crypto_driver);

MODULE_AUTHOR("Richard van Schagen <vschagen@cs.com>");
MODULE_ALIAS("platform:" KBUILD_MODNAME);
MODULE_DESCRIPTION("Airoha EIP-93 crypto engine driver");
MODULE_LICENSE("GPL v2");
