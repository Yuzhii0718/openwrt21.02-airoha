// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
#include <crypto/aes.h>
#include <crypto/ctr.h>
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
#include <crypto/internal/des.h>
#endif
#include <linux/dma-mapping.h>

#include "eip93-cipher.h"
#include "eip93-common.h"
#include "eip93-regs.h"

#include <linux/foe_hook.h>

void airoha_skcipher_handle_result(struct crypto_async_request *async, int err)
{
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(async->tfm);
	struct airoha_device *airoha = ctx->airoha;
	struct skcipher_request *req = skcipher_request_cast(async);
	struct airoha_cipher_reqctx *rctx = skcipher_request_ctx(req);

	airoha_unmap_dma(airoha, rctx, req->src, req->dst);
	airoha_handle_result(airoha, rctx, req->iv);

	
	if (ra_sw_nat_hook_free)
		ra_sw_nat_hook_free(req->base.data);

	
	skcipher_request_complete(req, err);
}

static int airoha_skcipher_send_req(struct crypto_async_request *async)
{
	struct skcipher_request *req = skcipher_request_cast(async);
	struct airoha_cipher_reqctx *rctx = skcipher_request_ctx(req);
	//printk("@@@@@@@@@@@ === *** req:[0x%llx]async[0x%llx]\n",req, async);
	int err;

	err = check_valid_request(rctx);

	if (err) {
		skcipher_request_complete(req, err);
		return err;
	}
	//printk("@@@@@@@@@@@@@@@@@@@@@@@@@ req->iv(0x%llx)\n", req->iv);
	return airoha_send_req(async, req->iv, rctx);
}

/* Crypto skcipher API functions */
static int airoha_skcipher_cra_init(struct crypto_tfm *tfm)
{
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct airoha_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct airoha_alg_template, alg.skcipher.base);

	crypto_skcipher_set_reqsize(__crypto_skcipher_cast(tfm),
					sizeof(struct airoha_cipher_reqctx));

	memset(ctx, 0, sizeof(*ctx));
	ctx->airoha = tmpl->airoha;

	ctx->sa_in = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ctx->sa_in)
		return -ENOMEM;
	//printk("@@@ ctx->sa_in[0x%llx] saSize[%d]\n",ctx->sa_in, sizeof(struct saRecord_s));
	ctx->sa_base_in = dma_map_single(ctx->airoha->dev, ctx->sa_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	ctx->sa_out = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ctx->sa_out)
		return -ENOMEM;

	ctx->sa_base_out = dma_map_single(ctx->airoha->dev, ctx->sa_out,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
	return 0;
}

static void airoha_skcipher_cra_exit(struct crypto_tfm *tfm)
{
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(tfm);

	dma_unmap_single(ctx->airoha->dev, ctx->sa_base_in,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	dma_unmap_single(ctx->airoha->dev, ctx->sa_base_out,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	kfree(ctx->sa_in);
	kfree(ctx->sa_out);
}

static int airoha_skcipher_setkey(struct crypto_skcipher *ctfm, const u8 *key,
				 unsigned int len)
{
	struct crypto_tfm *tfm = crypto_skcipher_tfm(ctfm);
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct airoha_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct airoha_alg_template, alg.skcipher.base);
	struct saRecord_s *saRecord = ctx->sa_out;
	u32 flags = tmpl->flags;
	u32 nonce = 0;
	unsigned int keylen = len;
	int sa_size = sizeof(struct saRecord_s);
	int err = -EINVAL;

	if (!key || !keylen)
		return err;

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	if (IS_RFC3686(flags)) {
		if (len < CTR_RFC3686_NONCE_SIZE)
			return err;

		keylen = len - CTR_RFC3686_NONCE_SIZE;
		memcpy(&nonce, key + keylen, CTR_RFC3686_NONCE_SIZE);
	}
#endif

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	if (flags & AIROHA_ALG_DES) {
		ctx->blksize = DES_BLOCK_SIZE;
		err = verify_skcipher_des_key(ctfm, key);
	}
	if (flags & AIROHA_ALG_3DES) {
		ctx->blksize = DES3_EDE_BLOCK_SIZE;
		err = verify_skcipher_des3_key(ctfm, key);
	}
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	if (flags & AIROHA_ALG_AES) {
		struct crypto_aes_ctx aes;

		ctx->blksize = AES_BLOCK_SIZE;
		err = aes_expandkey(&aes, key, keylen);
	}
#endif
	if (err)
		return err;

	dma_unmap_single(ctx->airoha->dev, ctx->sa_base_in, sa_size,
								DMA_TO_DEVICE);

	dma_unmap_single(ctx->airoha->dev, ctx->sa_base_out, sa_size,
								DMA_TO_DEVICE);

	airoha_set_saRecord(saRecord, keylen, flags);

	memcpy(saRecord->saKey, key, keylen);
	ctx->saNonce = nonce;
	saRecord->saNonce = nonce;
	saRecord->saCmd0.bits.direction = 0;

	memcpy(ctx->sa_in, saRecord, sa_size);
	ctx->sa_in->saCmd0.bits.direction = 1;

	ctx->sa_base_out = dma_map_single(ctx->airoha->dev, ctx->sa_out, sa_size,
								DMA_TO_DEVICE);

	ctx->sa_base_in = dma_map_single(ctx->airoha->dev, ctx->sa_in, sa_size,
								DMA_TO_DEVICE);
	return err;
}

static int airoha_skcipher_crypt(struct skcipher_request *req)
{
	struct airoha_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_async_request *async = &req->base;
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct crypto_skcipher *skcipher = crypto_skcipher_reqtfm(req);

	if (!req->cryptlen)
		return 0;

	rctx->assoclen = 0;
	rctx->textsize = req->cryptlen;
	rctx->authsize = 0;
	rctx->sg_src = req->src;
	rctx->sg_dst = req->dst;
	rctx->ivsize = crypto_skcipher_ivsize(skcipher);
	rctx->blksize = ctx->blksize;
	rctx->flags |= AIROHA_DESC_SKCIPHER;
	if (!IS_ECB(rctx->flags))
		rctx->flags |= AIROHA_DESC_DMA_IV;
		//printk("@@@@@@@@@@@ ### req:[0x%llx]async[0x%llx]\n",req, async);
	return airoha_skcipher_send_req(async);
}

static int airoha_skcipher_encrypt(struct skcipher_request *req)
{
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct airoha_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct airoha_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct airoha_alg_template, alg.skcipher.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= AIROHA_ENCRYPT;
	rctx->saRecord_base = ctx->sa_base_out;

	return airoha_skcipher_crypt(req);
}

static int airoha_skcipher_decrypt(struct skcipher_request *req)
{
	struct airoha_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct airoha_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct airoha_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct airoha_alg_template, alg.skcipher.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= AIROHA_DECRYPT;
	rctx->saRecord_base = ctx->sa_base_in;

	return airoha_skcipher_crypt(req);
}

/* Available algorithms in this module */
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
struct airoha_alg_template airoha_alg_ecb_aes = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_ECB | AIROHA_ALG_AES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(aes)",
			.cra_driver_name = "ecb(aes-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_cbc_aes = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_CBC | AIROHA_ALG_AES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(aes)",
			.cra_driver_name = "cbc(aes-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_ctr_aes = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_CTR | AIROHA_ALG_AES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "ctr(aes)",
			.cra_driver_name = "ctr(aes-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
				     CRYPTO_ALG_NEED_FALLBACK |
				     CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_rfc3686_aes = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_CTR | AIROHA_MODE_RFC3686 | AIROHA_ALG_AES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE + CTR_RFC3686_NONCE_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE + CTR_RFC3686_NONCE_SIZE,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.base = {
			.cra_name = "rfc3686(ctr(aes))",
			.cra_driver_name = "rfc3686(ctr(aes-eip93))",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
struct airoha_alg_template airoha_alg_ecb_des = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_ECB | AIROHA_ALG_DES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = DES_KEY_SIZE,
		.max_keysize = DES_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(des)",
			.cra_driver_name = "ebc(des-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_cbc_des = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_CBC | AIROHA_ALG_DES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = DES_KEY_SIZE,
		.max_keysize = DES_KEY_SIZE,
		.ivsize	= DES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des)",
			.cra_driver_name = "cbc(des-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_ecb_des3_ede = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_ECB | AIROHA_ALG_3DES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = DES3_EDE_KEY_SIZE,
		.max_keysize = DES3_EDE_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(des3_ede)",
			.cra_driver_name = "ecb(des3_ede-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct airoha_alg_template airoha_alg_cbc_des3_ede = {
	.type = AIROHA_ALG_TYPE_SKCIPHER,
	.flags = AIROHA_MODE_CBC | AIROHA_ALG_3DES,
	.alg.skcipher = {
		.setkey = airoha_skcipher_setkey,
		.encrypt = airoha_skcipher_encrypt,
		.decrypt = airoha_skcipher_decrypt,
		.min_keysize = DES3_EDE_KEY_SIZE,
		.max_keysize = DES3_EDE_KEY_SIZE,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des3_ede)",
			.cra_driver_name = "cbc(des3_ede-eip93)",
			.cra_priority = AIROHA_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct airoha_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = airoha_skcipher_cra_init,
			.cra_exit = airoha_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif
