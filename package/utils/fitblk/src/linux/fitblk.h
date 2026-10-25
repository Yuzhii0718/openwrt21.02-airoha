/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 * Local copy of the kernel uapi header include/uapi/linux/fitblk.h.
 *
 * The kernel header is only installed into $(STAGING_DIR)/usr/include by
 * headers_install once the kernel has been configured, which is not
 * guaranteed to happen before this package is built. Keep a copy here so
 * fitblk builds standalone; it must stay in sync with the kernel patch
 * target/linux/generic/pending-5.4/510-block-add-uImage.FIT-subimage-block-driver.patch
 */
#ifndef _UAPI_LINUX_FITBLK_H
#define _UAPI_LINUX_FITBLK_H

/*
 * IOCTL commands --- we will commandeer 0x46 ('F')
 */
#define FITBLK_RELEASE             0x4600

#endif /* _UAPI_LINUX_FITBLK_H */
