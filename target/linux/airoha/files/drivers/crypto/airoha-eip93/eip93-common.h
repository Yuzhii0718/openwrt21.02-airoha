/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#ifndef _EIP93_COMMON_H_
#define _EIP93_COMMON_H_

#include "eip93-main.h"

inline int airoha_put_descriptor(struct airoha_device *airoha,
					struct eip93_descriptor_s *desc);

inline void *airoha_get_descriptor(struct airoha_device *airoha);

inline int airoha_get_free_saState(struct airoha_device *airoha);

void airoha_set_saRecord(struct saRecord_s *saRecord, const unsigned int keylen,
				const u32 flags);

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_HMAC)
int airoha_authenc_setkey(struct crypto_shash *cshash, struct saRecord_s *sa,
			const u8 *authkey, unsigned int authkeylen);
#endif

#endif /* _EIP93_COMMON_H_ */
