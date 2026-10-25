//SPDX-License-Identifier: GPL-2,0
/*
 * Copyright (C) 2023 Airoha company 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#define USEC_POLL 2
#define TIMEOUT_POLL 10000


#define TRNG_IP_RDY 0x800
#define TRNG_NS_SEL_DAT_EN 0x804
#define TRNG_TEST_MODE_SHA_DONE 0x838
#define TRNG_DATA 0x83C

#define TRNG_IP_READY BIT(0)
#define TRNG_START BIT(31)
#define TRNG_DATA_RDY BIT(31)

#define DRBG_HW_INIT 0x80000002

extern void scu_Enable_Module_Clock(u8 module);

#define to_airoha_trng(p) container_of(p, struct airoha_trng, rng)

struct airoha_trng
{
	void __iomem *base;
	struct hwrng rng;
};

static char trng_driver_version[]= "v1.01";

static int airoha_trng_init(struct hwrng *rng)
{
	struct airoha_trng *priv = to_airoha_trng(rng);
	u32 val;
	int ret;

	scu_Enable_Module_Clock(1);

	ret = readl_poll_timeout_atomic(priv->base + TRNG_IP_RDY, val,
					val & TRNG_IP_READY, USEC_POLL,
					TIMEOUT_POLL);
	if (ret)
		pr_warn("airoha-trng: IP not ready (%d)\n", ret);

	writel(DRBG_HW_INIT, priv->base + TRNG_NS_SEL_DAT_EN);
	return 0;
}

static void airoha_trng_cleanup(struct hwrng *rng)
{
	u32 val;
	struct airoha_trng *priv = to_airoha_trng(rng);
	
	val = readl(priv->base + TRNG_NS_SEL_DAT_EN);
	val &= ~TRNG_START;
	writel(val,priv->base + TRNG_NS_SEL_DAT_EN);
	
}

static int airoha_trng_wait_ready(struct hwrng *rng, bool wait)
{
	struct airoha_trng *priv = to_airoha_trng(rng);
	int ready;
	
	ready = readl(priv->base + TRNG_TEST_MODE_SHA_DONE) & TRNG_DATA_RDY;
	if (!ready && wait)
		readl_poll_timeout_atomic(priv->base + TRNG_TEST_MODE_SHA_DONE, ready,
					  ready & TRNG_DATA_RDY, USEC_POLL,
					  TIMEOUT_POLL);

	return ready;
}

static int airoha_trng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	int retval = 0;
	struct airoha_trng *priv = to_airoha_trng(rng);
	
	while(max >= sizeof(u32)){
		if(!airoha_trng_wait_ready(rng, wait))
		{
			pr_warn_ratelimited("data output always not ready\n");
			break;
		}
		
		*(u32 *)buf = readl(priv->base + TRNG_DATA);
		retval += sizeof(u32);
		buf += sizeof(u32);
		max -=sizeof(u32);
	}
	
	return retval || !wait? retval : -EIO;
}

static int airoha_trng_probe(struct platform_device *pdev)
{
	struct resource *res;
	int ret;
	struct airoha_trng *priv ;
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res){
		dev_err(&pdev->dev, "no iomem resource\n");
			return -ENXIO;
	}
	priv = devm_kzalloc(&pdev->dev, sizeof (*priv), GFP_KERNEL);
	if(!priv)
		return -ENOMEM;
	
	priv->rng.name = pdev->name;
	priv->rng.init = airoha_trng_init;
	priv->rng.cleanup = airoha_trng_cleanup;
	priv->rng.read = airoha_trng_read;
	priv->rng.priv = (unsigned long)&pdev->dev;
	priv->rng.quality = 900;
	
	priv->base = devm_ioremap_resource(&pdev->dev, res);
	
	if(IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	ret = devm_hwrng_register(&pdev->dev, &priv->rng);
	if(ret){
		dev_err(&pdev->dev, "failed to register rng device: %d\n", ret);
		return ret;
	}
	
	dev_set_drvdata(&pdev->dev, priv);
	dev_info(&pdev->dev, "[%s] registerd TRNG driver\n", trng_driver_version);
	
	return 0;
}

static const struct of_device_id airoha_trng_match[] = {
	{.compatible = "airoha,airoha-trng"},
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, airoha_trng_match);

static struct platform_driver airoha_trng_driver = {
	.probe  = airoha_trng_probe,
	.driver = {
		.name = "airoha-trng",
		.of_match_table = airoha_trng_match,
	},
};

module_platform_driver(airoha_trng_driver);

MODULE_DESCRIPTION("Airoha True Random Number Generator Driver");
MODULE_AUTHOR("Lyon Lin <Lyon.lin@airlha.com>");



