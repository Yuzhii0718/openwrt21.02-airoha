/***************************************************************************************
 *      Copyright(c) 2014 ECONET Incorporation All rights reserved.
 *
 *      This is unpublished proprietary source code of ECONET Incorporation
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 ***************************************************************************************
 */

/*======================================================================================
 * MODULE NAME: spi
 * FILE NAME: nand_flash_otp.c
 * VERSION: 1.00
 * PURPOSE: To Provide NAND flash OTP Access interface.
 * NOTES:
 *
 * FUNCTIONS
 *
 *
 * DEPENDENCIES
 *
 * * $History: $
 * MODIFICTION HISTORY:
 * ** This is the first versoin for creating to support the functions of
 *    current module.
 *
 *======================================================================================
 */

/* INCLUDE FILE DECLARATIONS --------------------------------------------------------- */
#include <linux/types.h>
#include "spi/spi_nand_flash.h"
#include "spi/spi_nfi.h"
#include "spi/nand_flash_otp.h"

#if !defined(TCSUPPORT_CPU_EN7580) && !defined(TCSUPPORT_CPU_EN7528) && !defined(TCSUPPORT_CPU_EN7527)
#if defined(TCSUPPORT_CPU_EN7521) && defined(TCSUPPORT_SECURE_BOOT)
#define TCSUPPORT_SECURE_BOOT_7526
#endif
#endif

#ifndef TCSUPPORT_SECURE_BOOT_7526
#if defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP)
#include "mbedtls/rsa.h"
#include "mbedtls/md_internal.h"
#include "mbedtls/platform.h"
#include "sHeader.h"

unsigned char memory_buf[16 * 1024];
unsigned char hash[32];
/* used to store max 1GB image hash */
unsigned char hash_store[32 * 1024];
#define RSA_KEY_SIZE 2048
#define ECNT_SHA256_HW
#define ECNT_SHA256_HW_LEN (0xFFFFF)
#endif
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)

#else
#include <flashhal.h>
#include <asm/tc3162.h>
#endif

/* NAMING CONSTANT DECLARATIONS ------------------------------------------------------ */

/* FUNCTION DECLARATIONS ------------------------------------------------------ */

/* MACRO DECLARATIONS ---------------------------------------------------------------- */
#define printf					prom_printf
#define RSA_PUBLIC_KEY_OTP_PAGE	(2)

/* TYPE DECLARATIONS ----------------------------------------------------------------- */

/* STATIC VARIABLE DECLARATIONS ------------------------------------------------------ */
#if defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP) || defined(TCSUPPORT_SECURE_BOOT_7526)
static u8 page_data[_SPI_NAND_PAGE_SIZE];
#endif

/* LOCAL SUBPROGRAM BODIES------------------------------------------------------------ */
#if 0
void show_nand_otp_status(void)
{
	SPI_NAND_FLASH_RTN_T retVal;
	struct SPI_NAND_FLASH_INFO_T flashInfo;
	NAND_FLASH_OTP_STS_T status;

	if(nand_otp_status(&status) != 0) {
		printf("Get OTP status fail.\n");
		return;
	}

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return;
	}

	printf("%s OTP_PRT = %d, OPT_EN = %d\n", flashInfo.ptr_name,
											status.otp_locked,
											status.otp_enabled);

	return;
}
#endif

int nand_otp_status(NAND_FLASH_OTP_STS_T *status, u32 page)
{
	SPI_NAND_FLASH_RTN_T retVal;
	u8 feature;
	struct SPI_NAND_FLASH_INFO_T flashInfo = {0};
	
	if(isSpiNandAndCtrlECC) {
		printf("OTP operation only supported by flash ECC.\n");
		return -1;
	}

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return -1;
	}

	if(flashInfo.otp_page_num == -1) {
		printf("[error] do not support OTP function.\n");
		return -1;
	}

	spi_nand_protocol_set_otp(NAND_FLASH_OTP_ENABLE);
	spi_nand_select_die(page);

	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
		return -1;
	}

	status->otp_locked = (feature >> 7) & 0x1;
	status->otp_enabled = (feature >> 6) & 0x1;

#if OTP_DBG
	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_PROTECTION, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
		return -1;
	}
	printf("NAND Flash register 0x%02x = 0x%x.\n", _SPI_NAND_ADDR_PROTECTION, feature);

	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
		return -1;
	}
	printf("NAND Flash register 0x%02x = 0x%x.\n", _SPI_NAND_ADDR_FEATURE, feature);
#endif

	spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);

	return 0;
}

int nand_otp_en(NAND_FLASH_OTP_ENABLE_T otp_state, u32 page)
{
	SPI_NAND_FLASH_RTN_T retVal;
	u8 feature;
	struct SPI_NAND_FLASH_INFO_T flashInfo = {0};
	
	if(isSpiNandAndCtrlECC) {
		printf("OTP operation only supported by flash ECC.\n");
		return -1;
	}

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return -1;
	}

	if(flashInfo.otp_page_num == -1) {
		printf("[error] do not support OTP function.\n");
		return -1;
	}

	if(otp_state == NAND_FLASH_OTP_ENABLE) {
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_ENABLE);
	}

	spi_nand_select_die(page);

	/* get feature */
	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
		return -1;
	}

	/* set enable OTP */
	if(otp_state == NAND_FLASH_OTP_ENABLE) {
		feature |= (0x1 << 6);
	} else {
		feature &= ~(0x1 << 6);
	}

	/* set feature */
	retVal = spi_nand_protocol_set_feature(_SPI_NAND_ADDR_FEATURE, feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Set flash feature error.\n");
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
		return -1;
	}

	if(otp_state == NAND_FLASH_OTP_DISABLE) {
		spi_nand_protocol_set_otp(NAND_FLASH_OTP_DISABLE);
	}
	
	return 0;
}

int nand_otp_lock(u32 page)
{
	SPI_NAND_FLASH_RTN_T retVal = SPI_NAND_FLASH_RTN_NO_ERROR;
	SPI_NAND_FLASH_RTN_T prog_exe_status = SPI_NAND_FLASH_RTN_NO_ERROR;
	u8 feature;
	int ret = -1;
	struct SPI_NAND_FLASH_INFO_T flashInfo = {0};

	if(isSpiNandAndCtrlECC) {
		printf("OTP operation only supported by flash ECC.\n");
		return -1;
	}

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return -1;
	}

	if(flashInfo.otp_page_num == -1) {
		printf("[error] do not support OTP function.\n");
		return -1;
	}

	if(nand_otp_en(NAND_FLASH_OTP_ENABLE, page) != 0) {
		printf("Set OTP enable fail.\n");
		return -1;
	}

	spi_nand_select_die(page);

#if OTP_DBG
	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		goto exit_lock;
	}
	printf("[lock] 1. NAND Flash register 0x%02x = 0x%x.\n", _SPI_NAND_ADDR_FEATURE, feature);
#endif

	/* get feature */
	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		goto exit_lock;
	}

	/* set lock */
	feature |= (0x1 << 7);
	retVal = spi_nand_protocol_set_feature(_SPI_NAND_ADDR_FEATURE, feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Set flash feature error.\n");
		goto exit_lock;
	}

#if OTP_DBG
	retVal = spi_nand_protocol_get_feature(_SPI_NAND_ADDR_FEATURE, &feature);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash feature error.\n");
		goto exit_lock;
	}
	printf("[lock] 2. NAND Flash register 0x%02x = 0x%x.\n", _SPI_NAND_ADDR_FEATURE, feature);
#endif

	/* enable write */
	retVal = spi_nand_protocol_write_enable();
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Write enable fail.\n");
		goto exit_lock;
	}

	/* Execute program data into SPI NAND chip  */
	prog_exe_status = spi_nand_protocol_program_execute(0);

	/* Disable write */
	retVal = spi_nand_protocol_write_disable();

	if(nand_otp_en(NAND_FLASH_OTP_DISABLE, page) != 0) {
		printf("Set OTP disable fail.\n");
		return -1;
	}
	
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		return(retVal);
	}
	if(prog_exe_status != SPI_NAND_FLASH_RTN_NO_ERROR) {
		return (prog_exe_status);
	}

	return 0;

exit_lock:
	if(nand_otp_en(NAND_FLASH_OTP_DISABLE, page) != 0) {
		printf("Set OTP disable fail.\n");
	}
	return ret;
}

static int nand_otp_read(u32 page, u8 *data, u32 len)
{
	struct SPI_NAND_FLASH_INFO_T flashInfo = {0};
	SPI_NAND_FLASH_RTN_T retVal;
	int	ret = -1;
	SPI_NAND_FLASH_RTN_T status;
	u32 retlen;

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return -1;
	}

	if(len > flashInfo.page_size) {
		printf("[error] Read OTP length:%u is bigger than flash page size:%u.\n", len, flashInfo.page_size);
		return -1;
	}

	if(flashInfo.otp_page_num == -1) {
		printf("[error] do not support OTP function.\n");
		return -1;
	}

	if(nand_otp_en(NAND_FLASH_OTP_ENABLE, page) != 0) {
		printf("Set OTP enable fail.\n");
		return -1;
	}

	ret = nandflash_read(page * flashInfo.page_size,
						len,
						&retlen,
						data,
						&status,
						1);

	if(nand_otp_en(NAND_FLASH_OTP_DISABLE, page) != 0) {
		printf("Set OTP disable fail.\n");
		return -1;
	}
	
	if(ret != 0) {
		printf("Flash read error.\n");
		return -1;
	}

	return 0;
}

static int nand_otp_write(u32 page, u8 *data, u32 len)
{
	struct SPI_NAND_FLASH_INFO_T flashInfo = {0};
	SPI_NAND_FLASH_RTN_T retVal;
	int	ret;
	NAND_FLASH_OTP_STS_T status;
	u32 retlen;

	retVal = SPI_NAND_Flash_Get_Flash_Info(&flashInfo);
	if(retVal != SPI_NAND_FLASH_RTN_NO_ERROR) {
		printf("Get flash Info error.\n");
		return -1;
	}

	if(len > flashInfo.page_size) {
		printf("[error] Write OTP length:%u is bigger than flash page size:%u.\n", len, flashInfo.page_size);
		return -1;
	}

	if(flashInfo.otp_page_num == -1) {
		printf("[error] do not support OTP function.\n");
		return -1;
	}
	
	if(nand_otp_status(&status, page) != 0) {
		printf("Get OTP status fail.\n");
		return -1;
	}

	if(status.otp_locked) {
		printf("OTP has been locked.\n");
		return -1;
	}

	if(nand_otp_en(NAND_FLASH_OTP_ENABLE, page) != 0) {
		printf("Set OTP enable fail.\n");
		return -1;
	}

	ret = nandflash_write(page * flashInfo.page_size,
						len,
						&retlen,
						data,
						1);

	if(nand_otp_en(NAND_FLASH_OTP_DISABLE, page) != 0) {
		printf("Set OTP disable fail.\n");
		return -1;
	}
	
	if(ret != 0) {
		printf("Flash write error.\n");
		return -1;
	}

	return 0;
}

#if defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP) || defined(TCSUPPORT_SECURE_BOOT_7526)
int nand_otp_read_rsa_pub_key(u8 *data, u32 len)
{
	/* default use OTP page 2 */
	return nand_otp_read(RSA_PUBLIC_KEY_OTP_PAGE, data, len);
}

void show_nand_otp_rsa_pub_key(u32 len)
{
	if(nand_otp_read_rsa_pub_key(page_data, len) != 0) {
		printf("Read RSA public key fail.\n");
	}
	
	show_array(page_data, len, "RSA public key");

	return;
}

#if defined(TCSUPPORT_SECURE_BOOT_7526)
int is_nand_otp_rsa_pub_key_read_ok(void)
{
    int i;

	if(nand_otp_read_rsa_pub_key(page_data, 1) != 0)
        return 0;
    else
        return 1;
}

int is_nand_otp_rsa_pub_key_default_value(u32 len)
{
    int i;

	if(nand_otp_read_rsa_pub_key(page_data, len) != 0) {
	    printf("Read RSA public key fail.\n");
        return 0;
	}
	
	for (i=0; i<len; i++) {
        if (page_data[i]!=0xff)
            return 0;
    }

	return 1;
}
#endif

NAND_FLASH_OTP_RSA_PUBKEY_T nand_otp_is_rsa_pub_key(u32 len)
{
	u32 i;

	if(nand_otp_read_rsa_pub_key(page_data, len) != 0) {
		printf("Read RSA public key fail.\n");
		return NAND_FLASH_OTP_HAS_NO_RSA_PUBKEY;
	}

	for(i = 0; i < len; i++) {
		if(page_data[i] != 0xFF) {
			return NAND_FLASH_OTP_HAS_RSA_PUBKEY;
		}
	}
	
	return NAND_FLASH_OTP_HAS_NO_RSA_PUBKEY;
}

int nand_otp_write_rsa_pub_key(u8 *data, u32 len)
{
	int ret;
	
	/* default use OTP page 2 */
	ret = nand_otp_write(RSA_PUBLIC_KEY_OTP_PAGE, data, len);

	if(nand_otp_lock(RSA_PUBLIC_KEY_OTP_PAGE) != 0) {
		prom_printf("OTP lock fail.\n");
		return -1;
	}

	return ret;
}

#ifndef TCSUPPORT_SECURE_BOOT_7526 /* 7526 uses the rsa_verify() in bootrom/bootram/lib/efuse.c */
SIGNATURE_VERIFY_STATUS_T rsa_verify(SECURE_IMG_T *img)
{
	int c, ret;
    size_t i;
	SIGNATURE_VERIFY_STATUS_T status;
    mbedtls_rsa_context rsa;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];	//store digest (256 bytes)
    mbedtls_mpi_uint *p;
	unsigned int imgStart, imgRemainLen, hashLen;
	int loop;
	/* key is 2048bits */
	unsigned long rsa_key[2048/8/sizeof(unsigned long)];

#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf init");
#endif

	memset(rsa_key, 0, sizeof(rsa_key));
	/* key is 2048bits = 256bytes in OTP */
	if(nand_otp_read_rsa_pub_key((u8 *)rsa_key, 2048/8) != 0) {
		return SIGNATURE_VERIFY_INCORRECT;
	}

#if RSA_KEY_DBG
	show_array(rsa_key, sizeof(rsa_key), "read rsa_key");
#endif

	mbedtls_memory_buffer_alloc_init(memory_buf, sizeof(memory_buf));
    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf 1");
#endif

	/* E key length(word) */
	rsa.E.s = 1;
	rsa.E.n = 1;
	if( ( p = (mbedtls_mpi_uint*)mbedtls_calloc( rsa.E.n, sizeof(mbedtls_mpi_uint) ) ) == NULL )
            return( SIGNATURE_VERIFY_INCORRECT );
	rsa.E.p = p;
	rsa.E.p[0] = 0x010001;

	/* N key length(word) */
	rsa.N.s = 1;
	rsa.N.n = ((RSA_KEY_SIZE / 8) / sizeof(mbedtls_mpi_uint));
	if( ( p = (mbedtls_mpi_uint*)mbedtls_calloc( rsa.N.n, sizeof(mbedtls_mpi_uint) ) ) == NULL )
            return( SIGNATURE_VERIFY_INCORRECT );
	rsa.N.p = p;
	/* Store key */
    for(i = 0; i < rsa.N.n; i++) {
		rsa.N.p[i] = rsa_key[rsa.N.n - i - 1];
    }
	/* signature length(bytes) */
    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;

#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf 2");
#endif

    /*
     * Extract the RSA signature from the text file
     */
	for(i = 0; i < rsa.len; i++) {
		buf[i] = img->sHeader->v1.sig[i];
	}
	
#if SIGNATURE_DBG
	show_array(buf, rsa.len, "signature");
#endif

    /*
     * Compute the SHA-256 hash of the input file and
     * verify the signature
     */
#ifdef ECNT_SHA256_HW
	imgStart = (unsigned int)img->start;
	imgRemainLen = (unsigned int)img->len;
	loop = (imgRemainLen / ECNT_SHA256_HW_LEN);
	if((imgRemainLen % ECNT_SHA256_HW_LEN) != 0) {
		loop += 1;
	}

#if SHA256_DBG
	show_array(imgStart, 0x120, "boot.img");
#endif

	for(i = 0; i < loop; i++) {
		hashLen = (imgRemainLen >= ECNT_SHA256_HW_LEN) ? ECNT_SHA256_HW_LEN : imgRemainLen;
#if SHA256_DBG
		printf("imgStart:0x%x\n", imgStart);
		printf("hashLen:0x%x\n", hashLen);
#endif
		sha256_init();
		if(sha256(imgStart, hashLen, &hash_store[i * 32]) != 0) {
			status = SIGNATURE_VERIFY_INCORRECT;
			goto exit;
		}
		imgStart += hashLen;
		imgRemainLen -= hashLen;

#if SHA256_DBG
		printf("hash_store:%d\n", i);
		show_array(&hash_store[i * 32], 32, "hash_store");
#endif
	}

	/* Second hash layer */
	if(loop > 1) {
		sha256_init();
		if(sha256(hash_store, loop * 32, hash) != 0) {
			status = SIGNATURE_VERIFY_INCORRECT;
			goto exit;
		}
	} else {
		memcpy(hash, hash_store, 32);
	}
#else
    if( ( ret = mbedtls_md_file(&mbedtls_sha256_info, img, hash) ) != 0 )
    {
        status = SIGNATURE_VERIFY_INCORRECT;
		goto exit;
    }
#endif
	
#if SHA256_DBG
	show_array(hash, sizeof(hash), "final hash");
#endif

#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf 3");
#endif

#if RSA_KEY_DBG
	printf("rsa public key(word read)\n");
	for(i = 0; i < rsa.N.n; i++) {
		printf("rsa.N.p[%d] = 0x%x\n", i, rsa.N.p[i]);
    }
	show_array(rsa.N.p, rsa.len, "rsa public key(byte read)");
#endif

    if( ( ret = mbedtls_rsa_pkcs1_verify( &rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                  MBEDTLS_MD_SHA256, 20, hash, buf ) ) != 0 )
    {
        status = SIGNATURE_VERIFY_INCORRECT;
		goto exit;
    }
	
#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf 4");
#endif

	status = SIGNATURE_VERIFY_STATUS_CORRECT;
	
exit:
	mbedtls_rsa_free( &rsa );

#if defined(TCSUPPORT_STACK_DBG)
	show_array(memory_buf, sizeof(memory_buf), "RSA memory_buf end");
#endif

    return (status);
}
#endif
#else
int nand_otp_write_rsa_pub_key(u8 *data, u32 len)
{
	return -1;
}
#endif

/* End of [spi_controller.c] package */
