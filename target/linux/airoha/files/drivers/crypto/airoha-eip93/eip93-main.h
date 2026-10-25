/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */
#ifndef _EIP93_MAIN_H_
#define _EIP93_MAIN_H_

#include <crypto/internal/aead.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/rng.h>
#include <crypto/internal/skcipher.h>
#include <linux/device.h>
#include <linux/interrupt.h>

#define AIROHA_RING_SIZE			512
#define AIROHA_RING_BUSY			32
#define AIROHA_CRA_PRIORITY		1500

/* cipher algorithms */
#define AIROHA_ALG_DES			BIT(0)
#define AIROHA_ALG_3DES			BIT(1)
#define AIROHA_ALG_AES			BIT(2)
#define AIROHA_ALG_MASK			GENMASK(2, 0)
/* hash and hmac algorithms */
#define AIROHA_HASH_MD5			BIT(3)
#define AIROHA_HASH_SHA1			BIT(4)
#define AIROHA_HASH_SHA224			BIT(5)
#define AIROHA_HASH_SHA256			BIT(6)
#define AIROHA_HASH_HMAC			BIT(7)
#define AIROHA_HASH_MASK			GENMASK(6, 3)
/* cipher modes */
#define AIROHA_MODE_CBC			BIT(8)
#define AIROHA_MODE_ECB			BIT(9)
#define AIROHA_MODE_CTR			BIT(10)
#define AIROHA_MODE_RFC3686		BIT(11)
#define AIROHA_MODE_MASK			GENMASK(10, 8)

/* cipher encryption/decryption operations */
#define AIROHA_ENCRYPT			BIT(12)
#define AIROHA_DECRYPT			BIT(13)

#define AIROHA_BUSY			BIT(14)

/* descriptor flags */
#define AIROHA_DESC_ASYNC			BIT(31)
#define AIROHA_DESC_SKCIPHER		BIT(30)
#define AIROHA_DESC_AEAD			BIT(29)
#define AIROHA_DESC_AHASH			BIT(28)
#define AIROHA_DESC_PRNG			BIT(27)
#define AIROHA_DESC_FAKE_HMAC		BIT(26)
#define AIROHA_DESC_LAST			BIT(25)
#define AIROHA_DESC_FINISH			BIT(24)
#define AIROHA_DESC_IPSEC			BIT(23)
#define AIROHA_DESC_DMA_IV			BIT(22)

#define IS_DES(flags)			(flags & AIROHA_ALG_DES)
#define IS_3DES(flags)			(flags & AIROHA_ALG_3DES)
#define IS_AES(flags)			(flags & AIROHA_ALG_AES)

#define IS_HASH_MD5(flags)		(flags & AIROHA_HASH_MD5)
#define IS_HASH_SHA1(flags)		(flags & AIROHA_HASH_SHA1)
#define IS_HASH_SHA224(flags)		(flags & AIROHA_HASH_SHA224)
#define IS_HASH_SHA256(flags)		(flags & AIROHA_HASH_SHA256)
#define IS_HMAC(flags)			(flags & AIROHA_HASH_HMAC)

#define IS_CBC(mode)			(mode & AIROHA_MODE_CBC)
#define IS_ECB(mode)			(mode & AIROHA_MODE_ECB)
#define IS_CTR(mode)			(mode & AIROHA_MODE_CTR)
#define IS_RFC3686(mode)		(mode & AIROHA_MODE_RFC3686)

#define IS_BUSY(flags)			(flags & AIROHA_BUSY)
#define IS_DMA_IV(flags)		(flags & AIROHA_DESC_DMA_IV)

#define IS_ENCRYPT(dir)			(dir & AIROHA_ENCRYPT)
#define IS_DECRYPT(dir)			(dir & AIROHA_DECRYPT)

#define IS_CIPHER(flags)		(flags & (AIROHA_ALG_DES || \
						AIROHA_ALG_3DES ||  \
						AIROHA_ALG_AES))

#define IS_HASH(flags)			(flags & (AIROHA_HASH_MD5 ||  \
						AIROHA_HASH_SHA1 ||   \
						AIROHA_HASH_SHA224 || \
						AIROHA_HASH_SHA256))

/**
 * struct airoha_device - crypto engine device structure
 */
struct airoha_device {
	void __iomem		*base;
	struct device		*dev;
	struct clk		*clk;
	int			irq;
	struct airoha_ring		*ring;
	struct airoha_state_pool	*saState_pool;
};

struct airoha_desc_ring {
	void			*base;
	void			*base_end;
	dma_addr_t		base_dma;
	/* write and read pointers */
	void			*read;
	void			*write;
	/* descriptor element offset */
	u32			offset;
};

struct airoha_state_pool {
	void			*base;
	dma_addr_t		base_dma;
	bool			in_use;
};

struct airoha_ring {
	struct tasklet_struct		done_task;
	/* command/result rings */
	struct airoha_desc_ring		cdr;
	struct airoha_desc_ring		rdr;
	spinlock_t			write_lock;
	spinlock_t			read_lock;
	atomic_t			free;
	/* saState */
	struct airoha_state_pool		*saState_pool;
	void				*saState;
	dma_addr_t			saState_dma;
};

enum airoha_alg_type {
	AIROHA_ALG_TYPE_AEAD,
	AIROHA_ALG_TYPE_SKCIPHER,
};

struct airoha_alg_template {
	struct airoha_device	*airoha;
	enum airoha_alg_type	type;
	u32			flags;
	union {
		struct aead_alg		aead;
		struct skcipher_alg	skcipher;
	} alg;
};

#endif /* _EIP93_MAIN_H_ */
