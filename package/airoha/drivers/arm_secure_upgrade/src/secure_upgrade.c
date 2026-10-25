/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±)
are protected under relevant copyright laws. The information contained herein
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or
its licensors. Without the prior written permission of EcoNet and/or its licensors,
any reproduction, modification, use or disclosure of EcoNet Software, and
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡±
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE.
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/buffer_head.h>
#include <linux/libcompileoption.h>
#include <spi/secure_upgrade.h>

#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <crypto/hash.h>
#include <crypto/sha.h>

#include <asm-generic/io.h>
#include <asm/cacheflush.h>

#include "arm-smccc.h"

#include <linux/proc_fs.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define HASH_MODE_MASK			(0x1)
#define HASH_MAX_DIGEST_SIZE	(64)
#define FIP_MAX_HEADER_SIZE		(0x2000)
#define DATA_BUFF_SIZE			(0x100000)

#define TOC_HEADER_NAME			0xAA640001
#define SIP_VERIFY_BL33			0x82000205

/* ToC Entry UUIDs */
#define UUID_NON_TRUSTED_FIRMWARE_BL33 \
	{{0xd6,  0xd0, 0xee, 0xa7}, {0xfc, 0xea}, {0xd5, 0x4b}, 0x97, 0x82, {0x99, 0x34, 0xf2, 0x34, 0xb6, 0xe4} }

typedef struct uuid
{
	uint8_t		time_low[4];
	uint8_t		time_mid[2];
	uint8_t		time_hi_and_version[2];
	uint8_t		clock_seq_hi_and_reserved;
	uint8_t		clock_seq_low;
	uint8_t		node[6];
} uuid_toc_t;


typedef struct fip_toc_header
{
	uint32_t	name;
	uint32_t	serial_number;
	uint64_t	flags;
} fip_toc_header_t;

typedef struct fip_toc_entry
{
	uuid_toc_t		uuid;
	uint64_t	offset_address;
	uint64_t	size;
	uint64_t	flags;
} fip_toc_entry_t;

typedef struct hash_alg
{
	const char *name;			/* Name of algorithm */
	int digest_size;			/* Length of digest */
} hash_alg_t;

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static unsigned char sha_alg = 0;
static unsigned char *buffer_get = NULL;
static unsigned char *buffer_set = NULL;
static struct crypto_shash *shash = NULL;
static struct shash_desc *desc = NULL;

static unsigned char *output = NULL;
static unsigned char *fip_header = NULL;
static unsigned int key_status = 0;

static struct platform_device *virt_dev = NULL;

static hash_alg_t sha_algo[2] = {
	{
		"sha256",
		SHA256_DIGEST_SIZE,
	},
	{
		"sha512",
		SHA512_DIGEST_SIZE,
	},
};

uuid_toc_t img_uuid[1] = {
	UUID_NON_TRUSTED_FIRMWARE_BL33,
};

unsigned int img_smc[1] = {
	SIP_VERIFY_BL33,
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

static inline int compare_uuids(const uuid_toc_t *uuid1, const uuid_toc_t *uuid2)
{
	return memcmp(uuid1, uuid2, sizeof(uuid_toc_t));
}

unsigned int parse_fip(char *buf, unsigned int buf_size, unsigned int *offset, unsigned int *size)
{
	char *bufend = NULL;
	int tcboot = 0;
	fip_toc_header_t *toc_header = NULL;
	fip_toc_entry_t *toc_entry = NULL;

	toc_header = (fip_toc_header_t *) buf;

	if (toc_header->name != TOC_HEADER_NAME)
	{
		return 0;
	}

	toc_entry = (fip_toc_entry_t *)(toc_header + 1);
	bufend = buf + buf_size;

	/* Walk through each ToC entry in the file. */
	while ((char *)toc_entry + sizeof(fip_toc_entry_t) - 1 < bufend)
	{
		if (compare_uuids(&(toc_entry->uuid), &img_uuid[tcboot]) == 0)
		{
			*offset = (unsigned int) toc_entry->offset_address;
			*size = (unsigned int) toc_entry->size;

			return img_smc[tcboot];
		}

		toc_entry++;
	}

	return 0;
}

static unsigned long check_Secure_Key_Status (void)
{
	unsigned long r0 = 0, r1 = 0, r2 = 0, r3 = 0;
	struct arm_smccc_res res;

	r0 = 0x82000001;	/* efuse */
	r1 = 0x59454B43;	/* CKEY */
	r2 = 1;				/* Disable print message in BL31 */

	arm_smccc_smc(r0, r1, r2, r3, 0, 0, 0 ,0, &res);

	return res.a0;
}

static unsigned long get_sha_alg(void)
{
	unsigned long r0 = 0, r1 = 0, r2 = 0, r3 = 0;
	struct arm_smccc_res res;

	r0 = 0x82000003;
	arm_smccc_smc(r0, r1, r2, r3, 0, 0, 0 ,0, &res);

	return res.a0;
}

static unsigned long verify_img(unsigned int smc_id)
{
	unsigned long r0 = 0, r1 = 0, r2 = 0, r3 = 0;
	struct arm_smccc_res res;

	r0 = (unsigned long) smc_id;
	r1 = (unsigned long) dma_map_single(&virt_dev->dev, output, HASH_MAX_DIGEST_SIZE, DMA_TO_DEVICE);
	r2 = (unsigned long) dma_map_single(&virt_dev->dev, fip_header, FIP_MAX_HEADER_SIZE, DMA_TO_DEVICE);
	r3 = (unsigned long) FIP_MAX_HEADER_SIZE;

	arm_smccc_smc(r0, r1, r2, r3, 0, 0, 0 ,0, &res);

	dma_unmap_single(&virt_dev->dev, (dma_addr_t) r1, HASH_MAX_DIGEST_SIZE, DMA_TO_DEVICE);
	dma_unmap_single(&virt_dev->dev, (dma_addr_t) r2, FIP_MAX_HEADER_SIZE, DMA_TO_DEVICE);
	return res.a0;
}

static long secure_upgrade_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct secure_upgrade_ioctl_cmd data;
	int i = 0, ret = -1;
	int data_len = 0;
	unsigned int offset = 0, size = 0;
	unsigned int smc_id = 0;

	/* variables for file operation */
	struct file *srcf = NULL;
	char *src = NULL;
	mm_segment_t orgfs;

	memset(&data, 0, sizeof(data));
	orgfs = get_fs();
	set_fs(KERNEL_DS);

	switch (cmd)
	{
		case SECURE_IOC_SET:
		{
			/* retrieve firmware path from user space */
			if (copy_from_user(&data, (void __user *) arg, sizeof(data)))
			{
				printk("copy_from_user_error\n");
				goto secure_upgrade_fail;
			}

			/* verify tclinux.bin if the system is in secure boot mode */

			/*
			 * backup current process address space
			 * set up the process address space as the limit of virtual space
			 * then, call file open function
			 */
			data.path[sizeof(data.path) - 1] = '\0';
			src = data.path;
			srcf = filp_open(src, O_RDONLY, 0);
			if(IS_ERR(srcf))
			{
				printk("\nError: file open failed\n");
				goto secure_upgrade_fail;
			}

			if (buffer_set == NULL)
			{
				goto secure_upgrade_fail;
			}

			fip_header = (buffer_set + PAGE_ALIGN(DATA_BUFF_SIZE));
			output = (fip_header + PAGE_ALIGN(FIP_MAX_HEADER_SIZE));

			srcf->f_pos = data.offset;

			memset(fip_header, 0, FIP_MAX_HEADER_SIZE);
			data_len = ecnt_kernel_fs_read(srcf, fip_header, FIP_MAX_HEADER_SIZE, &srcf->f_pos);

			if (data_len < 0 || data_len != FIP_MAX_HEADER_SIZE)
			{
				goto secure_upgrade_fail;
			}

			smc_id = parse_fip(fip_header, FIP_MAX_HEADER_SIZE, &offset, &size);
			if (smc_id == 0)
			{
				goto secure_upgrade_fail;
			}

			ret = crypto_shash_init(desc);
			if (ret < 0)
				goto secure_upgrade_fail;

			srcf->f_pos = (data.offset + offset);
			do
			{
				if (size >= DATA_BUFF_SIZE)
					i = DATA_BUFF_SIZE;
				else
					i = size;
				memset(buffer_set, 0, DATA_BUFF_SIZE);
				data_len = ecnt_kernel_fs_read(srcf, buffer_set, i, &srcf->f_pos);
				if ((data_len < 0) || (data_len != i))
				{
					ret = -1;
					goto secure_upgrade_fail;
				}

				ret = crypto_shash_update(desc, buffer_set, data_len);
				if (ret < 0)
					goto secure_upgrade_fail;

				size -= data_len;
			} while (size);

			ret = crypto_shash_final(desc, output);
			if (ret < 0)
				goto secure_upgrade_fail;

			ret = verify_img(smc_id);
		}
		break;
		case SECURE_IOC_GET:
		{
			/* retrieve firmware path from user space */
			if (copy_from_user(&data, (void __user *) arg, sizeof(data))) 
			{
				printk("copy_from_user_error\n");
				goto secure_upgrade_fail;
			}

			/* verify tclinux.bin if the system is in secure boot mode */

			/*
			 * backup current process address space
			 * set up the process address space as the limit of virtual space 
			 * then, call file open function
			 */
			data.path[sizeof(data.path) - 1] = '\0';
			src = data.path;
			srcf = filp_open(src, O_RDONLY, 0);
			if(IS_ERR(srcf))
			{
				printk("\nError: file open failed\n");
				goto secure_upgrade_fail;
			}

			if (buffer_get == NULL)
			{
				goto secure_upgrade_fail;
			}

			srcf->f_pos = data.offset;

			memset(buffer_get, 0, FIP_MAX_HEADER_SIZE);
			data_len = ecnt_kernel_fs_read(srcf, buffer_get, FIP_MAX_HEADER_SIZE, &srcf->f_pos);

			if (data_len < 0 || data_len != FIP_MAX_HEADER_SIZE)
			{
				goto secure_upgrade_fail;
			}
			smc_id = parse_fip(buffer_get, FIP_MAX_HEADER_SIZE, &offset, &size);
			if (smc_id == 0)
			{
				goto secure_upgrade_fail;
			}

			data.offset = offset;
			if (copy_to_user((void __user *) arg, &data, sizeof(data))) 
			{
				printk("copy_error_user_error\n");
				goto secure_upgrade_fail;
			}


			ret = 0;
		}
		break;

		default:
			printk("\nError: Not SECURE_IOC_SET command! \n");
	}

secure_upgrade_fail:
	set_fs(orgfs);

    if(srcf)
	    filp_close(srcf, NULL);

	if(buffer_set)
	{
		//kfree(buffer_set);
		//buffer_set = NULL;
	}
	if(buffer_get)
	{
		//kfree(buffer_get);
		//buffer_get = NULL;
	}

	return ret;
}

int secure_upgrade_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations secure_upgrade_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = secure_upgrade_ioctl,
	.open = secure_upgrade_open,
	.release = NULL,
};

static int secure_boot_check_key_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;

	if ( key_status )
	{
		len = sprintf(page, "The key exists in this device\n");
	}
	else
	{
		len = sprintf(page, "This key is empty in this device\n");
	}

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

static int secure_upgrade_init(void)
{
	int ret = 0, i = 0;
    size_t desc_size = 0;
	struct proc_dir_entry *secure_boot_proc = NULL;

	sha_alg = (get_sha_alg() & HASH_MODE_MASK);
	key_status = check_Secure_Key_Status();

	printk("\r\nsecure_upgrade init.\n");

	shash = crypto_alloc_shash(sha_algo[sha_alg].name, 0, 0);
	if (IS_ERR(shash))
	{
		ret = PTR_ERR(shash);
		goto out;
	}

	desc_size = crypto_shash_descsize(shash) + sizeof(struct shash_desc);
	desc = kzalloc(desc_size, GFP_KERNEL);
	if (desc == NULL)
	{
		ret = -ENOMEM;
		goto out_free_shash;
	}

	desc->tfm = shash;

	i = (PAGE_ALIGN(DATA_BUFF_SIZE) + PAGE_ALIGN(FIP_MAX_HEADER_SIZE) + PAGE_ALIGN(HASH_MAX_DIGEST_SIZE));
	buffer_set = (unsigned char *) kzalloc(i, GFP_KERNEL | GFP_DMA);
	
	buffer_get = (unsigned char *) kzalloc(FIP_MAX_HEADER_SIZE, GFP_KERNEL);
	
	/* secure_upgrade ioctl */
	ret = register_chrdev(SECURE_UPGRADE_MAJOR, "secure_upgrade", &secure_upgrade_fops);
	if (ret < 0)
		goto out_free_desc;

	virt_dev = platform_device_register_simple("secure_upgrade", -1, NULL, 0);
	if (IS_ERR(virt_dev))
	{
		ret = PTR_ERR(virt_dev);
		unregister_chrdev(SECURE_UPGRADE_MAJOR, "secure_upgrade");
		goto out_free_desc;
	}

	secure_boot_proc = create_proc_entry("tc3162/secure_boot_check_key", 0, NULL);
	secure_boot_proc->read_proc = secure_boot_check_key_read_proc;

	printk("\rsecure_upgrade init done.\n");
	return ret;

out_free_desc:
    kfree(desc);
out_free_shash:
    kfree(shash);
out:
    return ret;
}
static void secure_upgrade_exit(void)
{
	remove_proc_entry("tc3162/secure_boot_check_key", NULL);

	unregister_chrdev(SECURE_UPGRADE_MAJOR, "secure_upgrade");
	platform_device_unregister(virt_dev);
    kfree(desc);
    kfree(shash);
	if(buffer_set)
	{
		kfree(buffer_set);
		buffer_set = NULL;
	}
	if(buffer_get)
	{
		kfree(buffer_get);
		buffer_get = NULL;
	}
	printk("\r\nsecure_upgrade exit.\n");
}
module_init(secure_upgrade_init);
module_exit(secure_upgrade_exit);
MODULE_LICENSE("GPL");

