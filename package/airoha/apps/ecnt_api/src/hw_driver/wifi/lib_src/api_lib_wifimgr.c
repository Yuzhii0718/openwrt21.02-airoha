#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>    /* for connect and socket*/
#include <linux/wireless.h>
#include "wifi_comm.h"
#include "wifi_file_comm.h"
#include "libapi_lib_wifimgr.h"
#include "libapi_lib_wifi_internal.h"
#include <libapi_lib_utility.h>
#include "wifi_dbg.h"
#include <flash_layout/tc_partition.h>
#include "rt_os.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

#if defined(TCSUPPORT_OPENWRT)
#define BR_NAME "br-lan"
#else
#define BR_NAME "br0"
#endif

struct PRE_WLAN_STATE pre_wlan_state;

wifi_reload_config store_global[WIFI_MAX];	

int
wifimgr_lib_get_WPS_STA_MAC(wifi_type type, unsigned char *output)
{
	struct iwreq wrq;
	int socket_id;
	int len = 0;
	unsigned char *data = NULL;
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}
	
	len = sizeof(unsigned char) * 32;
	data = (unsigned char *)malloc(len);
	if(data == NULL)
	{
		printf("malloc failed.\n");
		return ECNT_WIFIMGR_CMD_ERR;
	}

	memset(data, 0, len);
	wrq.u.data.length = len;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = OID_802_11_GET_WPS_STA_INFO;	
		
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_WPS_STA_MAC: Create socket fail\n");
	}
	
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		close(socket_id);
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_WPS_STA_MAC: ioctl open fail\n");
	}
	
	sprintf((char *)output, "%02x:%02x:%02x:%02x:%02x:%02x", data[0], data[1], data[2], data[3], data[4], data[5]);

	close(socket_id); 
	free(data);

	return ECNT_WIFIMGR_SUCCESS;
}

/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
static int is_zero_ether_addr(const unsigned char *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static int is_multicast_ether_addr(const unsigned char *addr)
{
	return (0x01 & addr[0]);
}

/**
 * is_broadcast_ether_addr - Determine if the Ethernet address is broadcast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is the broadcast address.
 */
static int is_broadcast_ether_addr(const unsigned char *addr)
{
	return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

/**
 * is_valid_ether_addr - Determine if the given Ethernet address is valid
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * Return true if the address is valid.
 */
static int is_valid_ether_addr(const unsigned char *addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr) && !is_broadcast_ether_addr(addr);
}

static unsigned char hex2digital(char ch)
{
  if ((ch >= '0') && (ch <= '9')) {
  	return (unsigned char)(ch - '0');
  }
  else if ((ch >= 'A') && (ch <= 'F')) {
    return (unsigned char)(10 + ch - 'A');
  }
  else if ((ch >= 'a') && (ch <= 'f')) {
    return (unsigned char)(10 + ch - 'a');
  }
  return 0xff;
}

void * os_zalloc(size_t size)
{
	void *n = malloc(size);
	if (n)
		memset(n, 0, size);
	return n;
}

int wifimgr_lib_ioctl(int msgType, char *data, char *ifname, int len)
{
	int ret = 0, socket_id = 0;
	struct iwreq wrq;
	
	snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s", ifname);
	wrq.u.data.length = len;
 	wrq.u.data.pointer = data;
 	wrq.u.data.flags = 0;
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == socket_id) {
		LIB_WIFI_ERROR("\n===  create socket fail ===\n"); 
		return -1;
	}
	
	ret = ioctl(socket_id, msgType, &wrq);
	if(ret != 0){
		LIB_WIFI_ERROR("\n===  ioctl open fail ===\n"); 
		close(socket_id);
		return -1;
	}
	
	close(socket_id);
	return 0;
}

int 
wifimgr_lib_set_dbg_level(char *str)
{
	WIFI_FUNC_TRACE_ENTER;
	char tmp[5];
	FILE* fp = NULL;
	fp = fopen("/tmp/wifi_lib_dbg_enable","w+");
	sprintf(tmp,"%d\n",1);
	if(fp == NULL)
		RETURN_WIFIMGR_FAIL;
	else {
		fwrite(tmp, sizeof(tmp), 1, fp);
		fclose(fp);
	}
	fp = NULL;
	if(NULL == str) {
		RETURN_WIFIMGR_FAIL;
	}
	
	fp = fopen("/tmp/ecntlib_dbg","w+");
	if(NULL != fp) {	
		fwrite(str, strlen(str), 1, fp);
		fclose(fp);
	}	
	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_wifi_path(wifi_type type, char *str) 
{
	WIFI_FUNC_TRACE_ENTER;
	if(WIFI_2_4G == type) {
		memset(dat_path_2_4g, 0, MAX_PATH_LEN);
		memcpy(dat_path_2_4g, str, MAX_PATH_LEN);
	}
	else if(WIFI_5G == type) {
		memset(dat_path_5g, 0, MAX_PATH_LEN);
		memcpy(dat_path_5g, str, MAX_PATH_LEN);
	}
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");

	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_get_wifi_calibration(wifi_type type) 
{
	WIFI_FUNC_TRACE_ENTER;
	int fd = -1;
	int len = 0;
	unsigned short id = 0;
	char tempbuffer[EEPROM_READFLASH_AC_SIZE] = {0};
	char tmp[MAX_PATH_LEN]    				  = {0};
	if(WIFI_2_4G == type) {
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])	
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)EEPROM_READFLASH_AC_SIZE,
		                  					 (long unsigned int)EEPROM_761511N_RA_OFFSET,"reservearea");
	else if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)128*1024,
											 (long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea"); 
	else if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID])
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)4096,
											 (long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
	else if(WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID])
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)204*1024,
											 (long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
	else
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)EEPROM_READFLASH_SIZE,
													(long unsigned int)EEPROM_RA_OFFSET,"reservearea");
#else
#ifdef MT7615_11N		
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)EEPROM_READFLASH_AC_SIZE,
		                  					 (long unsigned int)EEPROM_761511N_RA_OFFSET,"reservearea");
#elif (defined(MT7915N) && defined(MT7915E) && !defined(MT7915D))
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)128*1024,
											 (long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea"); 
#elif (defined(MT7916N) && defined(MT7916E) && !defined(MT7916D))
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)256*1024,
											 (long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea"); 
#elif defined(MT7915N)
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)4096,
											 (long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
#elif defined(MT7916N)
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)204*1024,
											 (long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
#else
		snprintf(tmp, sizeof(tmp),TC_FLASH_READ_CMD,CAL_FILE_TMP_PATH,(long unsigned int)EEPROM_READFLASH_SIZE,
													(long unsigned int)EEPROM_RA_OFFSET,"reservearea");
#endif
#endif
		system(tmp);

		fd=open(CAL_FILE_TMP_PATH,O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/RT30xxEEPROM.bin fail!!!\n");
		}
		len=read(fd,tempbuffer,sizeof(tempbuffer));
		close(fd);
		if((len != 0) && (len != -1)) {
			memmove(&id,tempbuffer,2);
#ifdef	TCSUPPORT_LITTLE_ENDIAN
			if(((id & 0xFFF0) != 0x3390) && ((id & 0xFFF0) != 0x3090) && 
				((id & 0xFFF0) != 0x5390) && ((id & 0xFFF0) != 0x7600) && 
				((id & 0xFFFF) != 0x7592) && ((id & 0xFFFF) != 0x7615)&& ((id & 0xFFFF) != 0x7915)&&
				((id & 0xFFFF) != 0x7916))
#else
			if(((id & 0xF0FF) != 0x9033) && ((id & 0xF0FF) != 0x9030) && 
				((id & 0xF0FF) != 0x9053) && ((id & 0xFFFF) != 0x9275) &&
				((id & 0xFFFF) != 0x1576) && ((id & 0xFFFF) != 0x1579) &&
				((id & 0xFFFF) != 0x1679))
#endif
			{
				printf("use default RT30xxEEPROM.bin\n");
			}
			else {
				#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				if((WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID]))
				{
					system("cat /tmp/RT30xxEEPROM.bin > /etc/RT30xxEEPROM.bin");
				}
				else
				{
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "cat %s > %s" ,CAL_FILE_TMP_PATH, CAL_FILE_PATH );
					system(tmp);
				}
				#else
				#if (defined(MT7915N) && defined(MT7915E) && !defined(MT7915D))
				system("cat /tmp/RT30xxEEPROM.bin > /etc/RT30xxEEPROM.bin");
				#else
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "cat %s > %s" ,CAL_FILE_TMP_PATH, CAL_FILE_PATH );
				system(tmp);
				#endif
				#endif
			}
			unlink(CAL_FILE_TMP_PATH);
		}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])
	{
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/CALDATA_default.bin",(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11N_RA_OFFSET,"reservearea");
		system(tmp);
		fd=open("/tmp/CALDATA_default.bin",O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/CALDATA_default.bin fail!!!\n");
		}		
		close(fd);
		system("cat /tmp/CALDATA_default.bin > /etc/CALDATA_default.bin"); 
		unlink("/tmp/CALDATA_default.bin"); 	
	}
#else
#ifdef MT7615_11N			
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/CALDATA_default.bin",(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11N_RA_OFFSET,"reservearea");
		system(tmp);
		fd=open("/tmp/CALDATA_default.bin",O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/CALDATA_default.bin fail!!!\n");
		}		
		close(fd);
		system("cat /tmp/CALDATA_default.bin > /etc/CALDATA_default.bin"); 
		unlink("/tmp/CALDATA_default.bin"); 	
#endif				
#endif
	}
#if defined(TCSUPPORT_WLAN_AC)
	else if(WIFI_5G == type) {	
#ifdef TCSUPPORT_EEPROM_ACEXT		
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
			snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)1024,(long unsigned int)EEPROM_RA_761511AC_OFFSET,"reservearea");
		else if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
			snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)128*1024,(long unsigned int)128*1024,"reservearea");
		else if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID])
			snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)EEPROM_791511AC_RA_SIZE,(long unsigned int)EEPROM_RA_791511AC_OFFSET,"reservearea");
		else if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
			snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)204*1024,(long unsigned int)EEPROM_RA_791611AC_OFFSET,"reservearea");
		else
			snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)1536,(long unsigned int)EEPROM_RA_761311AC_OFFSET,"reservearea");
#else
#ifdef MT7615E
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)1024,(long unsigned int)EEPROM_RA_761511AC_OFFSET,"reservearea");
#elif (defined(MT7915N) && defined(MT7915E) && !defined(MT7915D))
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)128*1024,(long unsigned int)128*1024,"reservearea");
#elif (defined(MT7916N) && defined(MT7916E) && !defined(MT7916D))
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)256*1024,(long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
#elif defined(MT7915E)
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)EEPROM_791511AC_RA_SIZE,(long unsigned int)EEPROM_RA_791511AC_OFFSET,"reservearea");
#elif defined(MT7916E)
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)204*1024,(long unsigned int)EEPROM_RA_791611AC_OFFSET,"reservearea");
#else // 7613
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)1536,(long unsigned int)EEPROM_RA_761311AC_OFFSET,"reservearea");
#endif
#endif
#else
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/MT7610EEPROM.bin",(long unsigned int)512,(long unsigned int)EEPROM_RA_AC_OFFSET,"reservearea");
#endif
		system(tmp);
		/*bin to buffer*/
		fd=open("/tmp/MT7610EEPROM.bin",O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/MT7610EEPROM.bin fail!!!\n");
		}
		len=read(fd,tempbuffer,sizeof(tempbuffer));
		close(fd);
		if((len!=0)&&(len!=-1)){
			memmove(&id,tempbuffer,2);
#ifdef	TCSUPPORT_LITTLE_ENDIAN
			if(((id&0xFFF0)!=0x7610)&& ((id&0xFFFF)!=0x7662) && ((id&0xFFFF)!=0x7615) && ((id&0xFFFF)!=0x7663)&&((id&0xFFFF)!=0x7915)&&((id&0xFFFF)!=0x7916))
#else
			if(((id&0xF0FF)!=0x1076)&& ((id&0xFFFF)!=0x6276) && ((id&0xFFFF)!=0x1576) && ((id&0xFFFF)!=0x6376)&& ((id&0xFFFF)!=0x1579)&&((id&0xFFFF)!=0x1679))
#endif
			{
				printf("use default etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin\n");
			}else{
				system("cat /tmp/MT7610EEPROM.bin > /etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin");	
			}
			unlink("/tmp/MT7610EEPROM.bin");
		}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
	{
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/CALDATA_default.bin",(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11AC_RA_OFFSET,"reservearea");
		system(tmp);
		fd=open("/tmp/CALDATA_default.bin",O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/CALDATA_default.bin fail!!!\n");
		}			
		close(fd);
		system("cat /tmp/CALDATA_default.bin > /etc/Wireless/RT2860AP_AC/CALDATA_default.bin"); 
		unlink("/tmp/CALDATA_default.bin"); 
	}	
#else
#ifdef MT7615E
		snprintf(tmp,sizeof(tmp),TC_FLASH_READ_CMD,"/tmp/CALDATA_default.bin",(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11AC_RA_OFFSET,"reservearea");
		system(tmp);
		fd=open("/tmp/CALDATA_default.bin",O_RDONLY);
		if(fd < 0){
			RETURN_WIFIMGR_FAIL_MSG("open tmp/CALDATA_default.bin fail!!!\n");
		}			
		close(fd);
		system("cat /tmp/CALDATA_default.bin > /etc/Wireless/RT2860AP_AC/CALDATA_default.bin"); 
		unlink("/tmp/CALDATA_default.bin"); 	
#endif	
#endif
	}
#endif
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");

	RETURN_WIFIMGR_SUCCESS;
}


int 
wifimgr_lib_set_wifi_calibration(wifi_type type) 
{
	WIFI_FUNC_TRACE_ENTER;
	char tempbuf[EEPROM_READFLASH_AC_SIZE] = {0};

	if(WIFI_2_4G == type){	
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])	
	{
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)1024,(long unsigned int)EEPROM_761511N_RA_OFFSET,"reservearea");
		system(tempbuf);
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/CALDATA_default.bin",
			(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11N_RA_OFFSET,"reservearea");
	}
	else if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)128*1024,(long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)4096,(long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)204*1024,(long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
	else
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)512,(long unsigned int)EEPROM_RA_OFFSET,"reservearea");
#else //TCSUPPORT_WLAN_MULTI_CHIP
#ifdef MT7615_11N
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)1024,(long unsigned int)EEPROM_761511N_RA_OFFSET,"reservearea");
		system(tempbuf);
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/CALDATA_default.bin",
			(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11N_RA_OFFSET,"reservearea");
#elif (defined(MT7915N) && defined(MT7915E) && !defined(MT7915D))
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)128*1024,(long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
#elif (defined(MT7916N) && defined(MT7916E) && !defined(MT7916D))
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)256*1024,(long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
#elif defined(MT7915N)
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)4096,(long unsigned int)EEPROM_791511N_RA_OFFSET,"reservearea");
#elif defined(MT7916N)
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)204*1024,(long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");
#else //MT7615_11N
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/RT30xxEEPROM.bin",
			(long unsigned int)512,(long unsigned int)EEPROM_RA_OFFSET,"reservearea");
#endif // MT7615_11N
#endif //TCSUPPORT_WLAN_MULTI_CHIP
		system(tempbuf);
	}
#if defined(TCSUPPORT_WLAN_AC)
	else if(WIFI_5G == type){
#ifdef TCSUPPORT_EEPROM_ACEXT
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
	{
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
				(long unsigned int)1024,(long unsigned int)EEPROM_RA_761511AC_OFFSET,"reservearea");
		system(tempbuf);
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/CALDATA_default.bin",
				(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11AC_RA_OFFSET,"reservearea");
	}
	else if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)128*1024,(long unsigned int)128*1024,"reservearea");
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)EEPROM_791511AC_RA_SIZE,(long unsigned int)EEPROM_RA_791511AC_OFFSET,"reservearea");
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)204*1024,(long unsigned int)EEPROM_RA_791611AC_OFFSET,"reservearea");
	else
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
				(long unsigned int)1536,(long unsigned int)EEPROM_RA_761311AC_OFFSET,"reservearea");
#else //TCSUPPORT_WLAN_MULTI_CHIP
#ifdef MT7615E
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
				(long unsigned int)1024,(long unsigned int)EEPROM_RA_761511AC_OFFSET,"reservearea");
		system(tempbuf);
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/CALDATA_default.bin",
				(long unsigned int)36*1024,(long unsigned int)TRX_SET1_SET2_11AC_RA_OFFSET,"reservearea");
#elif (defined(MT7915N) && defined(MT7915E) && !defined(MT7915D))
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)128*1024,(long unsigned int)128*1024,"reservearea");
#elif (defined(MT7916N) && defined(MT7916E) && !defined(MT7916D))
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)256*1024,(long unsigned int)EEPROM_791611N_RA_OFFSET,"reservearea");

#elif defined(MT7915E)
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)EEPROM_791511AC_RA_SIZE,(long unsigned int)EEPROM_RA_791511AC_OFFSET,"reservearea");
#elif defined(MT7916E)
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
			(long unsigned int)204*1024,(long unsigned int)EEPROM_RA_791611AC_OFFSET,"reservearea");

#else // 7615
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
				(long unsigned int)1536,(long unsigned int)EEPROM_RA_761311AC_OFFSET,"reservearea");
#endif //MT7615E
#endif //TCSUPPORT_WLAN_MULTI_CHIP
#else //TCSUPPORT_EEPROM_ACEXT
		snprintf(tempbuf,sizeof(tempbuf),TC_FLASH_WRITE_CMD,"/etc/Wireless/RT2860AP_AC/RT30xxEEPROM.bin",
				(long unsigned int)512,(long unsigned int)EEPROM_RA_AC_OFFSET,"reservearea");
#endif //TCSUPPORT_EEPROM_ACEXT
		system(tempbuf);
	}
#endif  //TCSUPPORT_WLAN_AC
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}

	RETURN_WIFIMGR_SUCCESS;
}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
static int cfg_get_process_name(char* path, int size)
{
	int len = 0;

	memset(path, 0, size);
	
	if ((len = readlink("/proc/self/exe", path, size - 1)) < 0){
		path[0] = '\0';
		return 0;
	}

	path[len] = '\0';
	
	return 0;
}

void __attribute__  ((constructor)) wifimgr_lib_multichip_init(void);

void wifimgr_lib_multichip_init(void)
{
	char path[64];
	int ret = init_global_shm();
	if(ret < WLAN_LIBAPI_WRITE_INFO_FAIL)
		goto Exit;
	
	return;
Exit:
	cfg_get_process_name(path, sizeof(path));
	printf("wifimgr_obj_lib_init: [%s] init fail \n",path);
	exit(0);
}

int 
wifimgr_lib_preinit(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};

	if(type == WIFI_2_4G)
		system("taskset 4 /usr/script/wlan_prepare.sh 0");
	else if(type == WIFI_5G)
		system("taskset 4 /usr/script/wlan_prepare.sh 1");
	else
		system("taskset 4 /usr/script/wlan_prepare.sh");
	RETURN_WIFIMGR_SUCCESS;
}
#endif //TCSUPPORT_WLAN_MULTI_CHIP
int 
wifimgr_lib_init(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};
	
	if(ECNT_WIFIMGR_SUCCESS != creat_shm_from_datafile(type)){
		RETURN_WIFIMGR_FAIL_MSG("Initialize the share memory failed!\n");
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
         if(type == WIFI_2_4G)
             pre_wlan_state.wlan_Bssid_num = atoi(outbuffer);
	 else if (type == WIFI_5G)
	     pre_wlan_state.wlan11ac_Bssid_num = atoi(outbuffer);
	
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_dump(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	if(ECNT_WIFIMGR_SUCCESS != print_shm(type)) {
		RETURN_WIFIMGR_FAIL_MSG("Print the share memory data failed!\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_bssidnum(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BssidNum", output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_WHNAT(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WHNAT", output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_countryRegion(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if(-1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(WIFI_2_4G != type ) {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type should be 0.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "CountryRegion", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;	
}

int 
wifimgr_lib_get_countryRegionABand(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if(-1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(WIFI_5G != type) {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type should be 1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "CountryRegionABand", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;	
}

int 
wifimgr_lib_get_countryCode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if(-1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "CountryCode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;	
}


int 
wifimgr_lib_get_channel(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "Channel", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_autoChannelSelect(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "AutoChannelSelect", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

#ifdef TCSUPPORT_WLAN_ACS
int
wifimgr_lib_get_channelScanScore(wifi_type type, int index, char* output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	int socket_id;
	int channel_idx;
	unsigned int score;
	unsigned int channel;
	PACS_CHANNEL_SCORE pchannelscore;
	int offset=0;
	int len    = sizeof(ACS_CHANNEL_SCORE);
	char *data ;
	data	= (char *) malloc(len + 1);
		
	if(data == NULL)
		RETURN_WIFIMGR_FAIL_MSG("malloc failed.\n");
		
	memset(data,0,len);
	if( -1 != index){
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
		
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return ECNT_WIFIMGR_CMD_ERR;
	}
		
	wrq.u.data.length = sizeof(ACS_CHANNEL_SCORE);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = OID_802_11_GET_ACS_CHANNEL_SCORE;	
		
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_channelScanScore: Create socket fail\n");
	}
	
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		free(data);
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_channelScanScore: ioctl open fail\n");
	}

	pchannelscore = (PACS_CHANNEL_SCORE)data;
	channel_idx = 0; 

	while(channel_idx < MAX_NUM_OF_CHANNELS){
		score	= pchannelscore->AcsChannelScore[channel_idx].Score; 
		channel = pchannelscore->AcsChannelScore[channel_idx].Channel;
		if(score>100 || channel>200 || channel==0)
			break;
		offset += sprintf(output+offset, "%u:%u,",channel,score);
		channel_idx++;
	}
	
	free(data);
	close(socket_id); 

	return ECNT_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_get_bestChannel(wifi_type type, int index, char* output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	int socket_id;
	int channel_idx;
	unsigned int score;
	unsigned int channel;
	unsigned int highest_score;
	unsigned int best_channel;
	PACS_CHANNEL_SCORE pchannelscore;
	int offset=0;
	int len    = sizeof(ACS_CHANNEL_SCORE);//
	char *data ;
	data	= (char *) malloc(len + 1);
	
	if(data == NULL)
		{
			RETURN_WIFIMGR_FAIL_MSG("malloc failed.\n");
		}
	memset(data,0,len);
	if( -1 != index) {
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
		
		}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
		}
	else{
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return ECNT_WIFIMGR_CMD_ERR;
		}
	
	wrq.u.data.length = sizeof(ACS_CHANNEL_SCORE);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = OID_802_11_GET_ACS_CHANNEL_SCORE;	

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		free(data);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_bestChannel: Create socket fail\n");
		}
	
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		free(data);
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_bestChannel: ioctl open fail\n");
	}

	highest_score= 0;
	best_channel = 0;
	
	pchannelscore = (PACS_CHANNEL_SCORE)data;
	channel_idx = 0; 

	while(channel_idx < MAX_NUM_OF_CHANNELS){
		score	= pchannelscore->AcsChannelScore[channel_idx].Score; 
		channel = pchannelscore->AcsChannelScore[channel_idx].Channel;
		if(score>100 || channel>200 || channel==0)
			break;
		if(highest_score < score){
			highest_score = score;
			best_channel = channel;
			}
		channel_idx++;
	}
	
	sprintf(output, "%u",best_channel);

	free(data);
	close(socket_id); 
	return ECNT_WIFIMGR_SUCCESS;
}
#endif //TCSUPPORT_WLAN_ACS

int 
wifimgr_lib_get_beaconPeriod(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BeaconPeriod", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_rtsThreshold(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "RTSThreshold", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_fragThreshold(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "FragThreshold", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_dtimPeriod(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DtimPeriod", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_txPower(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TxPower", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bgProtection(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BGProtection", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_disableOLBC(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DisableOLBC", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_txPreamble(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TxPreamble", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_txBurst(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TxBurst", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_pktAggregate(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PktAggregate", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_turboRate(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TurboRate", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_noForwardingBTNBSSID(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "NoForwardingBTNBSSID", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_shortSlot(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "ShortSlot", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wirelessEvent(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WirelessEvent", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_rekeyInterval(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "RekeyInterval", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_rekeyMethod(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "RekeyMethod", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_pmkCachePeriod(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PMKCachePeriod", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}



int 
wifimgr_lib_get_wirelessMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	char outbuffer[MAX_KEYWORD_LEN]= {0};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
#else
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
#endif

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WirelessMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_basicRate(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BasicRate", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_BW(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_BW", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_LDPC(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_LDPC", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_AMSDU_NUM(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "AMSDU_NUM", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_VHT_BW(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "VHT_BW", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_VHT_SGI(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "VHT_SGI", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_VHT_BW_SIGNAL(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "VHT_BW_SIGNAL", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_EXTCHA(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_EXTCHA", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_OpMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_OpMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_HT_BSSCoexistence(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_BSSCoexistence", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_TxStream(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_TxStream", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_RxStream(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_RxStream", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_GI(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_GI", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_HT_MCS(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_MCS", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_AutoBA(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_AutoBA", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_RDG(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_RDG", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_MpduDensity(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_MpduDensity", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_BAWinSize(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_BAWinSize", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_HTC(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_HTC", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_LinkAdapt(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_LinkAdapt", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HT_AMSDU(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HT_AMSDU", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_ssid(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	sprintf (keyword, "SSID%d", index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wscConfMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WscConfMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_WscConfStatus(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WscConfStatus", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_enableSsid(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "EnableSSID", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_enableSsid(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, "EnableSSID", input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_authMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "AuthMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wepAuthType(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WEPAuthType", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_txRate(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TxRate", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_noForwarding(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "NoForwarding", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_PMFMFPC(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PMFMFPC", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_PMFMFPR(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PMFMFPR", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_PMFSHA256(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PMFSHA256", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_defaultKeyID(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER; 
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DefaultKeyID", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_WPAPSK(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	sprintf (keyword, "WPAPSK%d", index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_maxStaNum(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MaxStaNum", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_mbssMaxStaNum(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MbssMaxStaNum", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_keyType(wifi_type type, int index, int num, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER; 
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	sprintf (keyword, "Key%dType", num);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_key1Type(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyType(type, index, 1, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key2Type(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyType(type, index, 2, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key3Type(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyType(type, index, 3, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key4Type(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyType(type, index, 4, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}


int 
wifimgr_lib_get_keyStr(wifi_type type, int index, int num, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	sprintf (keyword, "Key%dStr%d", num, index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_key1Str(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyStr(type, index, 1, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key2Str(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyStr(type, index, 2, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key3Str(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyStr(type, index, 3, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_key4Str(wifi_type type, int index, char *output, unsigned int size)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_keyStr(type, index, 4, output, size))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}


int 
wifimgr_lib_get_encrypType(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "EncrypType", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_preAuth(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PreAuth", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_hideSSID(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HideSSID", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wmmCapable(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WmmCapable", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_fixedTxMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "FixedTxMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_apsdCapable(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID]|| WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] ||  WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID])
	{
		char outbuffer[MAX_KEYWORD_LEN]= {0};
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}
		if(index < 1 || index > atoi(outbuffer))
				RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	}
	else
	{
		if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
		}
	}
	#else //TCSUPPORT_WLAN_MULTI_CHIP
	#if defined(MT7915E)|| defined(MT7915N) || defined(MT7613E) || defined(MT7916E)|| defined(MT7916N)
	char outbuffer[MAX_KEYWORD_LEN]= {0};
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}
		if(index < 1 || index > atoi(outbuffer))
			RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	#else //defined(MT7915E)|| defined(MT7915N) || defined(MT7613E) || defined(MT7916E)|| defined(MT7916N)
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	#endif //defined(MT7915E)|| defined(MT7915N) || defined(MT7613E) || defined(MT7916E)|| defined(MT7916N)
	#endif ////TCSUPPORT_WLAN_MULTI_CHIP
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APSDCapable", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_dlsCapable(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DLSCapable", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_apAifsn(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APAifsn", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_apCwmin(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APCwmin", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_apCwmax(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APCwmax", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_apTxop(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APTxop", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_apACM(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "APACM", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bssAifsn(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index is invalid.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BSSAifsn", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bssCwmin(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BSSCwmin", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bssCwmax(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BSSCwmax", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bssTxop(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index is should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BSSTxop", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_bssACM(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "BSSACM", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_ackPolicy(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "AckPolicy", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_macAddress(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index here should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MacAddress", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_reloadFlag(wifi_type type, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, -1, "ReloadFlag", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_radiusserver(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer,MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "RADIUS_Server", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_radiusport(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "RADIUS_Port", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_radiuskey(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	sprintf (keyword, "RADIUS_Key%d", index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_ieee8021x(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "IEEE8021X", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_session_timeout_interval(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "session_timeout_interval", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wdsenable(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WdsEnable", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wdslist(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WdsList", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wdsencryptype(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WdsEncrypType", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_wdskey(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1]	= {0};

	if(1 <= index && index <= MAX_WDS_ENTRY)
		sprintf( keyword, "Wds%dKey", index -1 );
	else 
		RETURN_WIFIMGR_FAIL_MSG("The wdskey index is invalid, please input 1 to %d.\n", MAX_WDS_ENTRY);
	
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;	
}

int 
wifimgr_lib_get_wdsphymode(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "WdsPhyMode", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_ownip(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index ) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "own_ip_addr", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_wscPinCode(wifi_type type, int index, char* output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	char data[16];
	int socket_id;
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN))
		RETURN_WIFIMGR_FAIL_MSG("get_bssidnum err.\n");
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));	
	if(WIFI_2_4G == type)
		sprintf(wrq.ifr_name, "ra%d", index-1);
	else if(WIFI_5G == type)
		sprintf(wrq.ifr_name, "rai%d", index-1);
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1)
		RETURN_WIFIMGR_FAIL_MSG("UpdatePinCodeAC: Create socket fail\n");
    wrq.u.data.length = sizeof(data);
    wrq.u.data.pointer = data;
    wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("UpdatePinCode: ioctl open fail\n");
	}
	sprintf(output, "%08u", *((unsigned int *)(data)));
	close(socket_id);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_Noise(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
#if defined(TCSUPPORT_WLAN_MT7915D)|| defined(TCSUPPORT_WLAN_MT7916D)
	unsigned char nfEnable=0;
#endif
	int data;
	int socket_id;

	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}

	if(WIFI_2_4G == type)
		sprintf(wrq.ifr_name, "ra0");
	else if(WIFI_5G == type)
		sprintf(wrq.ifr_name, "rai0");
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1)
		RETURN_WIFIMGR_FAIL_MSG("get_Noise: Create socket fail\n");


#if defined(TCSUPPORT_WLAN_MT7915D)|| defined(TCSUPPORT_WLAN_MT7916D)
	/*enable NF fun*/
	nfEnable = 1;
	wrq.u.data.length = sizeof(nfEnable);
    wrq.u.data.pointer = &nfEnable;
    wrq.u.data.flags = RT_OID_802_11_SET_NF;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("enable NF failed!: ioctl open fail\n");
	}

	sleep(1);//wait for drivers to collect NF information
#endif
	/**/
    wrq.u.data.length = sizeof(data);
    wrq.u.data.pointer = &data;
    wrq.u.data.flags = OID_802_11_GET_NF;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("get_Noise: ioctl open fail\n");
	}

#if defined(TCSUPPORT_WLAN_MT7915D)|| defined(TCSUPPORT_WLAN_MT7916D)
	/*disable NF fun for cuting cpu loading*/
	nfEnable = 0;
	wrq.u.data.length = sizeof(nfEnable);
    wrq.u.data.pointer = &nfEnable;
    wrq.u.data.flags = RT_OID_802_11_SET_NF;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("enable NF failed!: ioctl open fail\n");
	}
#endif

	sprintf(output, "%d", *((int *)(&data)));
	close(socket_id);

	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_chanLoad(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	unsigned int data;
	int socket_id;

	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}

	if(WIFI_2_4G == type)
		sprintf(wrq.ifr_name, "ra0");
	else if(WIFI_5G == type)
		sprintf(wrq.ifr_name, "rai0");
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1)
		RETURN_WIFIMGR_FAIL_MSG("chanLoad: Create socket fail\n");
    wrq.u.data.length = sizeof(data);
    wrq.u.data.pointer = &data;
    wrq.u.data.flags = RT_OID_GET_CHANLOAD;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("chanLoad: ioctl open fail\n");
	}

	sprintf(output, "%u", *((unsigned int *)(&data)));
	close(socket_id);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_accessPolicy(wifi_type type, int index, char* output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	char buf[MAX_KEYWORD_LEN]={0};  
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN))
		RETURN_WIFIMGR_FAIL_MSG("get_bssidnum err.\n");
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));	
	
	sprintf(buf, "AccessPolicy%d", index-1);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, buf, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_accessControlList(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN+1]= {0};
	char buf[MAX_KEYWORD_LEN+1]		 = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer,MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	sprintf(buf, "AccessControlList%d", index-1);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, buf, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_wscDefaultSSID(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	sprintf (keyword, "WscDefaultSSID%d", index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_get_wscV2Support(wifi_type type, int index, char* output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	sprintf (keyword, "WscV2Support");
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_EfuseBufferMode(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "EfuseBufferMode", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int
wifimgr_lib_get_BndstrgChanLoad(wifi_type type, int index, 
									char *output, unsigned int size)
{
	
	struct iwreq wrq;
	int ret=0;
	char data[16] ={0};
	int socket_id;
	WIFI_FUNC_TRACE_ENTER;
	if(index != 1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be 1 .\n");	
	if(WIFI_2_4G == type)
		RETURN_WIFIMGR_FAIL_MSG("Only 5G support BndstrgChanLoad.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		//printf("==>info_UpdateBndStrgChanload: Create socket fail\n");
		RETURN_WIFIMGR_FAIL;
	}

	memset(data, 0, sizeof(data));

	strcpy(wrq.ifr_name, "rai0");
    	wrq.u.data.length = sizeof(data);
   	 wrq.u.data.pointer = data;
   	 wrq.u.data.flags = RT_OID_BNDSTRG_SET_CHANLOAD;
   	 ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		//printf("==>info_UpdateBndStrgChanload: ioctl open fail\n");
    	close(socket_id);
		RETURN_WIFIMGR_FAIL;
	}
	snprintf(output, size,"%d",*((unsigned int *)data));
	close(socket_id);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_igmpSnEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID] && type == 0){		
		if( -1 != index) {
			RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
		}
	}else
#else
#ifdef MT7592
	if( type == 0){
		if( -1 != index) {
			RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
		}
	}else
#endif
#endif
	{
		char outbuffer[MAX_KEYWORD_LEN]= {0};
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}
		if(index < 1 || index > atoi(outbuffer))
			RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "IgmpSnEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_Sta11kCap(wifi_type type, int index, unsigned char *mac,
									unsigned char *output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	int socket_id;
	struct rrm_command_s *cmd = NULL;
	int cmd_len = 0;
	unsigned char cap[8] = {0};
	unsigned char zero_cap[8] = {0};

	if(index < 1 || index > 4)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to 4\n");	
	if(WIFI_2_4G == type)
		sprintf(wrq.ifr_name, "ra%d", index-1);
	else if(WIFI_5G == type)
		sprintf(wrq.ifr_name, "rai%d", index-1);
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1)
		RETURN_WIFIMGR_FAIL_MSG("getSta11kCap: Create socket fail\n");
	
	cmd_len = sizeof(struct rrm_command_s) + MAC_ADDR_LEN+8;
	cmd = (struct rrm_command_s *)os_zalloc(cmd_len);
	if(cmd == NULL)
	{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("getSta11kCap: malloc fail\n");
	}
	cmd->command_id = OID_802_11_RRM_CMD_QUERY_CAP;
	cmd->command_len = MAC_ADDR_LEN+8;
	memcpy(cmd->command_body, mac, MAC_ADDR_LEN);

	wrq.u.data.length = cmd_len;
	wrq.u.data.pointer = (char *)cmd;
	wrq.u.data.flags = RT_QUERY_RRM_CAPABILITY;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("getSta11kCap: ioctl open fail\n");
	}
	memcpy(cap, (cmd->command_body+MAC_ADDR_LEN), 8);
	if(memcmp(cap,zero_cap,8)) {
		*output = 1;
	}
	else{
		*output = 0;
	}
		
	close(socket_id);
	free(cmd);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_get_Sta11vCap(wifi_type type, int index, unsigned char *mac,
									unsigned char *output, unsigned int size)
{	
	WIFI_FUNC_TRACE_ENTER;
	struct iwreq wrq;
	int socket_id;
	struct wnm_command *cmd;
	int cmd_len = 0;

	if(index < 1 || index > 4)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to 4\n");	
	if(WIFI_2_4G == type)
		sprintf(wrq.ifr_name, "ra%d", index-1);
	else if(WIFI_5G == type)
		sprintf(wrq.ifr_name, "rai%d", index-1);
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1)
		RETURN_WIFIMGR_FAIL_MSG("getSta11vCap: Create socket fail\n");

	cmd_len = sizeof(struct wnm_command) + MAC_ADDR_LEN+1;
	cmd = (struct wnm_command *)os_zalloc(cmd_len);
	if(cmd == NULL)
	{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("getSta11vCap: malloc fail\n");	
	}
	cmd->command_id = OID_802_11_WNM_CMD_QUERY_BTM_CAP;
	cmd->command_len = MAC_ADDR_LEN+1;
	memcpy(cmd->command_body, mac, MAC_ADDR_LEN);

	wrq.u.data.length = cmd_len;
	wrq.u.data.pointer = (char *)cmd;
	wrq.u.data.flags = RT_QUERY_WNM_CAPABILITY;
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		close(socket_id);
		free(cmd);
		RETURN_WIFIMGR_FAIL_MSG("getSta11vCap: ioctl open fail\n");
	}
	memcpy(output, (cmd->command_body+MAC_ADDR_LEN), 1);
	close(socket_id);
	free(cmd);
	
	return ECNT_WIFIMGR_SUCCESS;
}

#if defined(TCSUPPORT_ECNT_MAP)
int 
wifimgr_lib_get_MapEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MapEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_MAP_Turnkey(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MAP_Turnkey", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_MAP_Ext(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MAP_Ext", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
#endif

#if defined(TCSUPPORT_WLAN_AX)
int 
wifimgr_lib_get_MuOfdmaDlEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MuOfdmaDlEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_MuOfdmaUlEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MuOfdmaUlEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_MuMimoDlEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MuMimoDlEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_MuMimoUlEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MuMimoUlEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_SREnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "SREnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_SRMode(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "SRMode", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_SRSDEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "SRSDEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_PPEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "PPEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_TWTSupport(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "TWTSupport", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_HeraStbcPriority(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "HeraStbcPriority", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

#if defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)
int 
wifimgr_lib_get_MaxRuOfdma(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MaxRuOfdma", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_MaxDLMuMimo(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MaxDLMuMimo", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_MaxULMuMimo(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "MaxULMuMimo", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int 
wifimgr_lib_get_Disable160RuMu(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "Disable160RuMu", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_Vht1024QamSupport(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "Vht1024QamSupport", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
#else //defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)
int wifimgr_lib_get_MaxRuOfdma(wifi_type type, int index, 
									char *output, unsigned int size)
{
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_get_MaxDLMuMimo(wifi_type type, int index, 
									char *output, unsigned int size)
{
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_get_MaxULMuMimo(wifi_type type, int index, 
									char *output, unsigned int size)
{
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_get_Disable160RuMu(wifi_type type, int index, 
									char *output, unsigned int size)
{
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_get_Vht1024QamSupport(wifi_type type, int index, 
									char *output, unsigned int size)
{
	RETURN_WIFIMGR_SUCCESS;
}
#endif //defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)
#endif //TCSUPPORT_WLAN_AX

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
void get_commonfunccap(wifi_type band ,chip_info* chipinfo)
{
	/*
		configure function 
	*/
	//below all chip support when open compile option,if any chip not support,please omit it
	chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_ACL;
	#ifdef TCSUPPORT_WLAN_IGMPSNOOP
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_IGMPSNOOP;
	#endif
	#ifdef TCSUPPORT_WLAN_ATE
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_ATE;
	#endif
	#ifdef TCSUPPORT_WLAN_WDS
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_WDS;
	#endif
	#ifdef TCSUPPORT_ECNT_MAP
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_MAP | ECNT_FUNC_CAP_SUPPORT_11K | ECNT_FUNC_CAP_SUPPORT_11V;
	#endif
	#ifdef TCSUPPORT_WLAN_8021X
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_DOT1X;
	#endif
	#if  defined(TCSUPPORT_WLAN_MULTI_WPS) || defined(TCSUPPORT_WLAN_WPS)
		chipinfo->funccapinfo.funccap |= WPSPBCPINCODE << ECNT_FUNC_CAP_SUPPORT_WPS_OFFSET;
	#endif
	#ifdef TCSUPPORT_WLAN_PMF
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_PMF;
	#endif
	#ifdef TCSUPPORT_WLAN_WPA3
		chipinfo->funccapinfo.funccap |= ECNT_FUNC_CAP_SUPPORT_WPA3;
	#endif
	#if 0//only for R2
		chipinfo->funccapinfo.funccap |= (ECNT_FUNC_CAP_SUPPORT_11R | ECNT_FUNC_CAP_SUPPORT_11U | ECNT_FUNC_CAP_SUPPORT_MBO)
	#endif
return;
}

void get_chip_cap_7592(wifi_type band,chip_info* chipinfo)
{
	chipinfo->chipcapinfo.chipcap |= ECNT_MT7592_CHIP_CAP;
	get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_7613(wifi_type band,chip_info* chipinfo)
{
	chipinfo->chipcapinfo.chipcap |= ECNT_MT7613E_CHIP_CAP;
	get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_7615(wifi_type band,chip_info* chipinfo)
{
	if(WIFI_2_4G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7615N_CHIP_CAP;
	else if(WIFI_5G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7615E_CHIP_CAP;

	get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_7915(wifi_type band,chip_info* chipinfo)
{
	if(WIFI_2_4G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7915N_CHIP_CAP;
	else if(WIFI_5G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7915E_CHIP_CAP;

		get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_7915D(wifi_type band,chip_info* chipinfo)
{
	if(WIFI_2_4G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7915D_2G_CHIP_CAP;
	else if(WIFI_5G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7915D_5G_CHIP_CAP;

	get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_7916D(wifi_type band,chip_info* chipinfo)
{
	if(WIFI_2_4G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7916D_2G_CHIP_CAP;
	else if(WIFI_5G == band)
		chipinfo->chipcapinfo.chipcap |= ECNT_MT7916D_5G_CHIP_CAP;

	get_commonfunccap(band,chipinfo);
	return;
}

void get_chip_cap_nowifi(wifi_type band,chip_info* chipinfo)
{
	chipinfo->chipcapinfo.chipcap = 0;
	chipinfo->funccapinfo.funccap = 0;
	return;
}

static const chip_cap_fun chip_cap_band0_comp_attr_tbl[] = 
{
	{WLAN_2G_NOCHIP_ID,  get_chip_cap_nowifi},
	{WLAN_2G_7592_ID,  get_chip_cap_7592},
	{WLAN_2G_7615N_ID, get_chip_cap_7615},
	{WLAN_2G_7615D_ID,  NULL},
	{WLAN_2G_7915N_ID, get_chip_cap_7915},
	{WLAN_2G_7915D_ID, get_chip_cap_7915D},
	{WLAN_2G_7916D_ID, get_chip_cap_7916D},
};
#define BAND0_ATTR_TBL_SIZE  (sizeof(chip_cap_band0_comp_attr_tbl) / sizeof(chip_cap_band0_comp_attr_tbl[0]))

static const chip_cap_fun chip_cap_band1_comp_attr_tbl[] = 
{
	{WLAN_5G_NOCHIP_ID,  get_chip_cap_nowifi},
	{WLAN_5G_7613E_ID,  get_chip_cap_7613},
	{WLAN_5G_7615E_ID, get_chip_cap_7615},
	{WLAN_5G_7615D_ID,  NULL},
	{WLAN_5G_7915E_ID, get_chip_cap_7915},
	{WLAN_5G_7915D_ID, get_chip_cap_7915D},
	{WLAN_5G_7916D_ID, get_chip_cap_7916D},
};
#define BAND1_ATTR_TBL_SIZE  (sizeof(chip_cap_band1_comp_attr_tbl) / sizeof(chip_cap_band1_comp_attr_tbl[0]))

int wifimgr_lib_get_cap(wifi_type type,chip_info* chipinfo)
{
	int i = 0;
	
	if(!chipinfo)
		RETURN_WIFIMGR_FAIL;

	memset(chipinfo,0,sizeof(chip_info));
	if(WIFI_2_4G == type)
	{
		for(i=0; i< BAND0_ATTR_TBL_SIZE; i++)
		{
			if(chip_cap_band0_comp_attr_tbl[i].chipindex == current2gindex)
			{
				if(NULL != chip_cap_band0_comp_attr_tbl[i].chipcap_api)
					chip_cap_band0_comp_attr_tbl[i].chipcap_api(type,chipinfo);
			}
		}
	}
	else if(WIFI_5G == type)
	{
		for(i=0; i< BAND1_ATTR_TBL_SIZE; i++)
		{
			if(chip_cap_band1_comp_attr_tbl[i].chipindex == current5gindex)
			{
				if(NULL != chip_cap_band1_comp_attr_tbl[i].chipcap_api)
					chip_cap_band1_comp_attr_tbl[i].chipcap_api(type,chipinfo);
			}
		}
	}

	return ECNT_WIFIMGR_SUCCESS;
}
#endif //TCSUPPORT_WLAN_MULTI_CHIP

int 
wifimgr_lib_get_DscpPriMapEnable(wifi_type type, int index, 
									char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DscpPriMapEnable", 
														output, size)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int wifimgr_lib_set_DscpPriMapEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The DscpPriMapEnable value should be number.\n");
		
	sprintf (keyword, "DscpPriMapEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_ssid(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	char outbuffer[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "SSID%d", index);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_countryRegion(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	int  tmp = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input || WIFI_2_4G != type)		
		RETURN_WIFIMGR_FAIL;
	tmp = atoi(input);
	if( doStrDecCheck(input) == 0 || tmp < 0 || (tmp > 7 && tmp <31) || tmp > 33 )
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "CountryRegion");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_countryRegionABand(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input || WIFI_5G != type)		
		RETURN_WIFIMGR_FAIL;

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 14 )
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "CountryRegionABand");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_countryCode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "CountryCode");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_channel(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 180)
		RETURN_WIFIMGR_FAIL_MSG("The input channel is invalid.Please input from 0 to 180\n");

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "Channel");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_autoChannelSelect(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 3)
		RETURN_WIFIMGR_FAIL_MSG("The input channel is invalid.Please input from 0 to 3\n");

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "AutoChannelSelect");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

#ifdef TCSUPPORT_WLAN_ACS
int 
wifimgr_lib_set_autoChannelSel(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd[MAX_KEYWORD_LEN] 	 = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 7)
		RETURN_WIFIMGR_FAIL_MSG("The input channel is invalid.Please input from 0 to 3\n");

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	/*2. iwpriv execute*/

	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra0 set AutoChannelSel=%d",  atoi(input));
		system(cmd);
		}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai0 set AutoChannelSel=%d", atoi(input));
		system(cmd);
		}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}

	
	RETURN_WIFIMGR_SUCCESS;
	
}
#endif

int
wifimgr_lib_set_beaconPeriod(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 20 || atoi(input) > 1024)
		RETURN_WIFIMGR_FAIL_MSG("The BeaconPeriod value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BeaconPeriod");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword,input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_rtsThreshold(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 1 || atoi(input) > 2347)
		RETURN_WIFIMGR_FAIL_MSG("The RTSThreshold value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RTSThreshold");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_fragThreshold(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 256 || atoi(input) > 2346)
		RETURN_WIFIMGR_FAIL_MSG("The FragThreshold value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "FragThreshold");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_dtimPeriod(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 1 || atoi(input) > 255)
		RETURN_WIFIMGR_FAIL_MSG("The DtimPeriod value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DtimPeriod");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_txPower(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 100)
		RETURN_WIFIMGR_FAIL_MSG("The TxPower value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "TxPower");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_PERCENTAGEenable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The PERCENTAGEenable value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PERCENTAGEenable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_bgProtection(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 2)
		RETURN_WIFIMGR_FAIL_MSG("The BGProtection value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BGProtection");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_disableOLBC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The DisableOLBC value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DisableOLBC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_txPreamble(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The TxPreamble value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "TxPreamble");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_txBurst(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The TxBurst value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "TxBurst");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_pktAggregate(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The PktAggregate value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PktAggregate");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_turboRate(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0)
		RETURN_WIFIMGR_FAIL_MSG("The TurboRate value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "TurboRate");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_noForwardingBTNBSSID(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The NoForwardingBTNBSSID value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "NoForwardingBTNBSSID");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_shortSlot(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The ShortSlot value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ShortSlot");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_wirelessEvent(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The WirelessEvent value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WirelessEvent");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_rekeyInterval(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  	 = {0};
	char outbuffer[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	//get the BssidNum value and check ssid index is valid or not.
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
															MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", 
																	atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 4194303)
		RETURN_WIFIMGR_FAIL_MSG("The RekeyInterval value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RekeyInterval");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_rekeyMethod(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]    = {0};
	char outbuffer[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
															MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", 
																	atoi(outbuffer));

	if( 0 != strcmp(input,"TIME") && 0 != strcmp(input,"PKT") && 0 != strcmp(input,"DISABLE"))
		RETURN_WIFIMGR_FAIL_MSG("The RekeyMethod value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RekeyMethod");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_pmkCachePeriod(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 65535)
		RETURN_WIFIMGR_FAIL_MSG("The PMKCachePeriod value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PMKCachePeriod");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_basicRate(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
		
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 4095)
		RETURN_WIFIMGR_FAIL_MSG("The BasicRate value is invalid.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BasicRate");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int wifimgr_lib_set_MuOfdmaDlEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");	

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The muofdmadl value should be between 0 to 1.\n");
	
	sprintf (keyword, "MuOfdmaDlEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int wifimgr_lib_set_MuOfdmaUlEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The muofdma value should be between 0 to 1.\n");
	
	sprintf (keyword, "MuOfdmaUlEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}
int wifimgr_lib_set_MuMimoDlEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");	
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The mumimodl value should be between 0 to 1.\n");
	
	sprintf (keyword, "MuMimoDlEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}
int wifimgr_lib_set_MuMimoUlEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The mumimoul value should be between 0 to 1.\n");
		
	sprintf (keyword, "MuMimoUlEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_SREnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The SREnable value should be between 0 to 1.\n");
		
	sprintf (keyword, "SREnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_SRMode(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The SRMode value should be between 0 to 1.\n");
		
	sprintf (keyword, "SRMode");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_SRSDEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The SRSDEnable value should be between 0 to 1.\n");
		
	sprintf (keyword, "SRSDEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_PPEnable(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The PPEnable value should be between 0 to 1.\n");
		
	sprintf (keyword, "PPEnable");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_TWTSupport(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 2)
		RETURN_WIFIMGR_FAIL_MSG("The twtsupport value should be between 0 ~2.\n");	
		
	sprintf (keyword, "TWTSupport");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_HeraStbcPriority(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The HeraStbcPriority value should be number.\n");
		
	sprintf (keyword, "HeraStbcPriority");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}
#if defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)
int wifimgr_lib_set_MaxRuOfdma(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The MaxRuOfdma value should be number.\n");
		
	sprintf (keyword, "MaxRuOfdma");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_MaxDLMuMimo(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The MaxDLMuMimo value should be number.\n");
		
	sprintf (keyword, "MaxDLMuMimo");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_MaxULMuMimo(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The MaxULMuMimo value should be number.\n");
		
	sprintf (keyword, "MaxULMuMimo");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_Disable160RuMu(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The Disable160RuMu value should be number.\n");
		
	sprintf (keyword, "Disable160RuMu");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}

int wifimgr_lib_set_Vht1024QamSupport(wifi_type type, int index, char *input){
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The Vht1024QamSupport value should be number.\n");
		
	sprintf (keyword, "Vht1024QamSupport");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;

}
#else //defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)
int wifimgr_lib_set_MaxRuOfdma(wifi_type type, int index, char *input){
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_set_MaxDLMuMimo(wifi_type type, int index, char *input){
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_set_MaxULMuMimo(wifi_type type, int index, char *input){
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_set_Disable160RuMu(wifi_type type, int index, char *input){
	RETURN_WIFIMGR_SUCCESS;
}
int wifimgr_lib_set_Vht1024QamSupport(wifi_type type, int index, char *input){
	RETURN_WIFIMGR_SUCCESS;
}
#endif //defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)|| defined(MT7916E)

int
wifimgr_lib_set_wirelessMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1;
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	char outbuffer[MAX_KEYWORD_LEN]= {0};
#endif

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
#else
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
#endif

	if(doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The WirelessMode value should be DEC num.\n");		
	tmp = atoi(input);
	if( WIFI_2_4G == type ) {
		if( tmp != 0 && tmp != 1 && tmp != 3 && tmp != 4 && tmp != 5 && tmp != 6 
			&& tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 16)
			RETURN_WIFIMGR_FAIL_MSG("The WirelessMode value for 2.4G is invalid.\n");
	}	
	else if( WIFI_5G == type ) {
		if( tmp != 2 && tmp != 3 && tmp != 5  && tmp != 8 && tmp != 10 && tmp != 11 
			&& tmp != 14 && tmp != 15 && tmp != 17)
			RETURN_WIFIMGR_FAIL_MSG("The WirelessMode value for 5G is invalid.\n");
	}
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WirelessMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_HT_BW(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]={0};
	int  tmp = -1;
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	int i = 0;
	char bssidnum[MAX_KEYWORD_LEN]= {0};
#endif
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	//check the wirelessMode
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidnum, MAX_KEYWORD_LEN))
	{
		RETURN_WIFIMGR_FAIL;
	}

	for (i = 0; i < atoi(bssidnum); i++)
	{
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, i + 1, outbuffer, MAX_KEYWORD_LEN))
		{
			tmp = atoi(outbuffer);
			if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																		&& tmp != 14 && tmp != 15&& tmp != 16&& tmp != 17 ) {
				RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_BW value.\n");
			}
		}
		else
		{
			RETURN_WIFIMGR_FAIL;
		}
	}
#else
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, -1, outbuffer, MAX_KEYWORD_LEN)) {
		tmp = atoi(outbuffer);
		if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																	&& tmp != 14 && tmp != 15 && tmp != 2 && tmp != 16 && tmp != 17) {
			RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_BW value.\n");
		}
	}
	else {
		RETURN_WIFIMGR_FAIL;
	}
#endif

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_BW value should be 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_BW");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_LDPC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The VHT_LDPC value should be between 0 to 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_LDPC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_AMSDU_NUM(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The AMSDU_NUM value should be Digital.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "AMSDU_NUM");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_VHT_BW(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};	
	char buf[MAX_KEYWORD_LEN]= {0};
	int i = 0;
	int number;

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 3)
		RETURN_WIFIMGR_FAIL_MSG("The VHT_BW value should be from 0 to 3.\n");
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
	{
		number = atoi(outbuffer);
	}
	else
	{
		number = 1;
	}
#else //TCSUPPORT_WLAN_MULTI_CHIP
#if defined(MT7915E) || defined(MT7916E) 
	number = atoi(outbuffer);
#else
	number = 1;
#endif
#endif //TCSUPPORT_WLAN_MULTI_CHIP
	for(i = 0; i < number; i++){
		if((strlen(input) != 0) && (strlen(input)+strlen(buf) < MAX_KEYWORD_LEN -1)){
			strcat(buf, input);
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
	{
		if(i < number - 1)
				strcat(buf, ";");
	}
#else
#if defined(MT7915E) || defined(MT7916E)
			if(i < number - 1)
				strcat(buf, ";");
#endif
#endif
		}		
	}
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "VHT_BW");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, buf)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_VHT_SGI(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char buf[MAX_KEYWORD_LEN]= {0};
	int i = 0;
#if defined(MT7615E) || defined(MT7915E) || defined(MT7613E)  || defined(MT7916E) || defined(TCSUPPORT_WLAN_MULTI_CHIP)
	char buf_bak[MAX_KEYWORD_LEN]= {0};
	int number;
#endif

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The VHT_SGI value should be between 0 to 1.\n");

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g)
	{
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
																MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}

		number = atoi(outbuffer);

		for(i = 0; i < number; i++){
			if(0 == i){
				snprintf(buf, sizeof(buf), input);
				continue;
			}
			snprintf(buf_bak, sizeof(buf_bak), buf);
			snprintf(buf, sizeof(buf), "%s;%s", buf_bak, input);	
		}
	}
	else
	{
		snprintf(buf, sizeof(buf), input);
	}
#else
#if defined(MT7615E) || defined(MT7915E) || defined(MT7613E) || defined(MT7916E)
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
															MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}

	number = atoi(outbuffer);
	
	for(i = 0; i < number; i++){
		if(0 == i){
			snprintf(buf, sizeof(buf), "%s", input);
			continue;
		}
		snprintf(buf_bak, sizeof(buf_bak), "%s", buf);
		snprintf(buf, sizeof(buf), "%s;%s", buf_bak, input);	
	}
#else
	snprintf(buf, sizeof(buf), input);
#endif
#endif

	/*2. Construct suitable parameters*/
	sprintf (keyword, "VHT_SGI");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, buf)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_VHT_LDPC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The VHT_LDPC value should be between 0 to 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "VHT_LDPC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_VHT_STBC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The VHT_STBC value should be between 0 to 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "VHT_STBC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_VHT_BW_SIGNAL(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)  
		RETURN_WIFIMGR_FAIL_MSG("The VHT_BW_SIGNAL value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "VHT_BW_SIGNAL");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_EXTCHA(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0}; 
	int  tmp = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	tmp = atoi(input);  //0 or 1
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_EXTCHA value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_EXTCHA");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_OpMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0}; 
	int  tmp = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	tmp = atoi(input);  //0 or 1
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_OpMode value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_OpMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_HT_BSSCoexistence(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0}; 
	int  tmp = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	tmp = atoi(input);  //0 or 1
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_BSSCoexistence value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_BSSCoexistence");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_HT_GI(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]={0};
	int  tmp = -1;
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
		int i = 0;
		char bssidnum[MAX_KEYWORD_LEN]= {0};
#endif
		/*1. Check the vaildity of input parameters*/
		if(NULL == input)		
			RETURN_WIFIMGR_FAIL;
		//check the wirelessMode
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidnum, MAX_KEYWORD_LEN))
		{
			RETURN_WIFIMGR_FAIL;
		}
	
		for (i = 0; i < atoi(bssidnum); i++)
		{
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, i + 1, outbuffer, MAX_KEYWORD_LEN))
			{
				tmp = atoi(outbuffer);
				if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																			&& tmp != 14 && tmp != 15 && tmp != 16&& tmp != 17 ) {
					RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_BW value.\n");
				}
			}
			else
			{
				RETURN_WIFIMGR_FAIL;
			}
		}
#else
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, -1, outbuffer, MAX_KEYWORD_LEN)) {
		tmp = atoi(outbuffer);
		if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																	&& tmp != 14 && tmp != 15 && tmp != 16&& tmp != 17) {
			RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_GI value.\n");
		}
	}
	else {
		RETURN_WIFIMGR_FAIL;
	}
#endif

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	tmp = atoi(input);  // 0 or 1
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_GI value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_GI");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_MCS(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	int  tmp = -1;
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	int i = 0;
	char bssidnum[MAX_KEYWORD_LEN]= {0};
#endif

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	//check the wirelessMode
#if defined(TCSUPPORT_WIRELESS_MODE_ENTRY)
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidnum, MAX_KEYWORD_LEN))
	{
		RETURN_WIFIMGR_FAIL;
	}

	for (i = 0; i < atoi(bssidnum); i++)
	{
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, i + 1, outbuffer, MAX_KEYWORD_LEN))
		{
			tmp = atoi(outbuffer);
			if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																		&& tmp != 14 && tmp != 15&& tmp != 16&& tmp != 17 ) {
				RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_BW value.\n");
			}
		}
		else
		{
			RETURN_WIFIMGR_FAIL;
		}
	}
#else
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type, -1, outbuffer, MAX_KEYWORD_LEN)) {
		tmp = atoi(outbuffer);
		if ( tmp != 5 && tmp != 6 && tmp != 7 && tmp != 8 && tmp != 9 && tmp != 10 && tmp != 11 
																	&& tmp != 14 && tmp != 15 && tmp != 16&& tmp != 17) {
			RETURN_WIFIMGR_FAIL_MSG("The wirelessMode do not support to set the HT_BW value.\n");
		}
	}
	else {
		RETURN_WIFIMGR_FAIL;
	}
#endif
	memset(outbuffer, 0, MAX_KEYWORD_LEN);
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	tmp = atoi(input);  // 0-15 or 32-33
	if( doStrDecCheck(input) == 0 || tmp < 0 || ( tmp > 15 && tmp <32) || tmp > 33)
		RETURN_WIFIMGR_FAIL_MSG("The HT_MCS value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_MCS");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_AutoBA(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_AutoBA value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_AutoBA");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_RDG(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_RDG value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_RDG");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_MpduDensity(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_MpduDensity value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_MpduDensity");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_BAWinSize(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 256)
		RETURN_WIFIMGR_FAIL_MSG("The HT_BAWinSize value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_BAWinSize");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_HTC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_HTC value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_HTC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_LinkAdapt(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_LinkAdapt value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_LinkAdapt");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_AMSDU(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_AMSDU value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_AMSDU");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_HT_STBC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The HT_STBC value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_STBC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_IEEE80211H(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The IEEE80211H value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "IEEE80211H");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_G_BAND_256QAM(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The G_BAND_256QAM value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "G_BAND_256QAM");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_WHNAT(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The WHNAT value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WHNAT");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_WOEOFF(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The WOEOFF value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WOEOFF");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ProbeHideSSID(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The ProbeHideSSID value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ProbeHideSSID");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_SHORTGIFLAG(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The SHORTGIFLAG value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "SHORTGIFLAG");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_StaNumDetect(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	
	if(doStrDecCheck(input) == 0 || tmp < 0 )
		RETURN_WIFIMGR_FAIL_MSG("The StaNumDetect value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "StaNumDetect");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_AutoChannelSkipList(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "AutoChannelSkipList");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_TxStream(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 1-4
	if( tmp < 0 || tmp > 4)
		RETURN_WIFIMGR_FAIL_MSG("The HT_TxStream value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_TxStream");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_HT_RxStream(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 1-4
	if( tmp < 0 || tmp > 4)
		RETURN_WIFIMGR_FAIL_MSG("The HT_RxStream value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HT_RxStream");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ITxBfEn(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The ITxBfEn value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ITxBfEn");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ETxBfEnCond(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The ETxBfEnCond value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ETxBfEnCond");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ETxBfIncapable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The ETxBfIncapable value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ETxBfIncapable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_MUTxRxEnable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);  // 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The MUTxRxEnable value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MUTxRxEnable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_reloadFlag(wifi_type type, char *str)
{
	WIFI_FUNC_TRACE_ENTER;
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, -1, "ReloadFlag", str)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_bssidNum(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char output[MAX_KEYWORD_LEN] = {0};
#if 0 
	int  tmp = -1;
#endif
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
#if 0	
	tmp = atoi(input);  // 1/2/4/8
	if( doStrDecCheck(input) == 0 || ((tmp != 1) && (tmp != 2) && (tmp != 4) && (tmp != 8)) )
		RETURN_WIFIMGR_FAIL_MSG("The BssidNum value should be chosed from 1,2,4,8.\n");
#endif
	/*Verify BssidNum changed or not */
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssidnum(type,-1,output,
													MAX_KEYWORD_LEN)) {
													
		if(type == WIFI_2_4G)
		   pre_wlan_state.wlan_Bssid_num = atoi(output);
		else if (type == WIFI_5G)
		   pre_wlan_state.wlan11ac_Bssid_num = atoi(output);
												
		if(0 != strncmp(output, input, strlen(input))) {
			if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(type,"1"))
				RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");
		}
		else {
			if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(type,"0"))
				RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");
		}
	}
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BssidNum");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_authMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	char outbuffer[MAX_KEYWORD_LEN]={0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	/*2. Construct suitable parameters*/
	sprintf (keyword, "AuthMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_wepAuthType(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};

	char outbuffer[MAX_KEYWORD_LEN]={0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	/*2. Construct suitable parameters*/
	sprintf (keyword, "WEPAuthType");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_txRate(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 )
		RETURN_WIFIMGR_FAIL_MSG("The TxRate value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "TxRate");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_noForwarding(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )
		RETURN_WIFIMGR_FAIL_MSG("The NoForwarding value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "NoForwarding");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_PMFMFPC(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )
		RETURN_WIFIMGR_FAIL_MSG("The PMFMFPC value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PMFMFPC");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_PMFMFPR(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )
		RETURN_WIFIMGR_FAIL_MSG("The PMFMFPR value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PMFMFPR");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_PMFSHA256(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )
		RETURN_WIFIMGR_FAIL_MSG("The PMFSHA256 value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PMFSHA256");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_defaultKeyID(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	int tmp   = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	tmp = atoi(input);  // the range is between 1 and 4
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 4 )
		RETURN_WIFIMGR_FAIL_MSG("The DefaultKeyID value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DefaultKeyID");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_WPAPSK(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	/*get the BssidNum value and check ssid index is valid or not.*/
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( strlen(input) > 64 )
		RETURN_WIFIMGR_FAIL_MSG("The WPAPSK value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WPAPSK%d",index);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_maxStaNum(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	int tmp   = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	tmp = atoi(input);  // the range is between 1 and 35
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 45 )
		RETURN_WIFIMGR_FAIL_MSG("The MaxStaNum value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MaxStaNum");
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_mbssMaxStaNum(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	int tmp   = -1;
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	tmp = atoi(input);  // the range is between 1 and 35
	if( doStrDecCheck(input) == 0 || tmp < 0 || tmp > 64 )
		RETURN_WIFIMGR_FAIL_MSG("The MbssMaxStaNum value is invalid,%d.\n",tmp);
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MbssMaxStaNum");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_keyType(wifi_type type, int index, int num, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
															MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( strlen(input) != 1 || doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The KeyType value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "Key%dType", num);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_key1Type(wifi_type type, int index, char *input)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyType(type, index, 1, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;		
}

int
wifimgr_lib_set_key2Type(wifi_type type, int index, char *input)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyType(type, index, 2, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;		
}

int
wifimgr_lib_set_key3Type(wifi_type type, int index, char *input)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyType(type, index, 3, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;		
}

int
wifimgr_lib_set_key4Type(wifi_type type, int index, char *input)
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyType(type, index, 4, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;		
}

int
wifimgr_lib_set_keyStr(wifi_type type, int index, int num, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer,
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	if(0 == strcmp(input, "0")){
		sprintf (keyword, "Key%dStr%d", num, index);
		if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
			RETURN_WIFIMGR_SUCCESS;
		}else{
			RETURN_WIFIMGR_FAIL;;
		}
	}

	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "Key%dStr%d", num, index);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_key1Str(wifi_type type, int index, char *input) 
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyStr(type, index, 1, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_key2Str(wifi_type type, int index, char *input) 
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyStr(type, index, 2, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_key3Str(wifi_type type, int index, char *input) 
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyStr(type, index, 3, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_key4Str(wifi_type type, int index, char *input) 
{
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_keyStr(type, index, 4, input))
		RETURN_WIFIMGR_FAIL;
	else
		RETURN_WIFIMGR_SUCCESS;
}


int
wifimgr_lib_set_encrypType(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
#ifndef TCSUPPORT_WLAN_MULTI_CHIP
	char encryp_type[ENCRYP_TYPE_NUM][MAX_KEYWORD_LEN] ={"NONE","WEP",
												"TKIP","AES","TKIPAES","GCMP256"};

	int i = 0;
#endif
	char outbuffer[MAX_KEYWORD_LEN]={0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer,
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
#ifndef TCSUPPORT_WLAN_MULTI_CHIP
	//check EncrypType is valid or not
	for (i = 0; i < ENCRYP_TYPE_NUM; i++) {
		if( 0 == strcmp(input, encryp_type[i]))
			break;
	}
	if(i == ENCRYP_TYPE_NUM)
		RETURN_WIFIMGR_FAIL_MSG("The EncrypType value should be chosed from NONE,WEP,TKIP,AES,TKIPAES.\n");
#endif
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "EncrypType");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_preAuth(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														 MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The PreAuth value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "PreAuth");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_hideSSID(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The HideSSID value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "HideSSID");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_ftSupport(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The FtSupport value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "FtSupport");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_ftotd(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The FtOtd value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "FtOtd");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ApCliMac_Manual_Flag(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The ApCliMac_Manual_Flag value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "ApCliMacManual_Flag");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_ApCliMac_Manual(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if(MacCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("Please input correct MacAddress, such as 00:1a:22:aa:dc:a1.\n");

	/*2. Construct suitable parameters*/
	sprintf (keyword, "ApCliMacManual");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
int
wifimgr_lib_set_rrmEnable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The rrmEnable value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RRMEnable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_wnmEnable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The rrmEnable value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WNMEnable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_wmmCapable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1 )// the range is between 0 and 1
		RETURN_WIFIMGR_FAIL_MSG("The WmmCapable value should be 0 or 1.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WmmCapable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_fixedTxMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 2 )// the range is between 0 and 2
		RETURN_WIFIMGR_FAIL_MSG("The FixedTxMode value should be between 0 and 2.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "FixedTxMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_apsdCapable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g || (chipflag2g & (~(BIT(WLAN_2G_7592_ID)))))
	{
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
															MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}

		if(index < 1 || index > atoi(outbuffer))
			RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	}
	else
	{
		if( -1 != index )
			RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	}
	#else
	#if defined(MT7915N) || defined(MT7915E) || defined(MT7615_11N) || defined(MT7615E) || defined(MT7613E) || defined(MT7916E)|| defined(MT7916N)
	//get the BssidNum value and check ssid index is valid or not.
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, 
														MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	#else
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	#endif
	#endif
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The APSDCapable value should be 0 or 1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APSDCapable");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_dlsCapable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
		
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)
		RETURN_WIFIMGR_FAIL_MSG("The DLSCapable value should be 0 or 1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DLSCapable");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_apAifsn(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APAifsn");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_apCwmin(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APCwmin");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_apCwmax(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APCwmax");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_apTxop(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APTxop");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_apACM(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "APACM");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_bssAifsn(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BSSAifsn");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_bssCwmin(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");	
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BSSCwmin");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_bssCwmax(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BSSCwmax");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_bssTxop(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
			
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BSSTxop");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_bssACM(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
			
	/*2. Construct suitable parameters*/
	sprintf (keyword, "BSSACM");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_ackPolicy(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char str1[MAX_KEYWORD_LEN]={0};
	char str2[MAX_KEYWORD_LEN]={0};
	char str3[MAX_KEYWORD_LEN]={0};
	char str4[MAX_KEYWORD_LEN]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_SUCCESS != check_para_valid(input, str1, str2, str3, str4))
		RETURN_WIFIMGR_FAIL;

	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
		
	/*2. Construct suitable parameters*/
	sprintf (keyword, "AckPolicy");
		
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_wscGetConf(wifi_type type, int index)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd[MAX_KEYWORD_LEN] 	 = {0};
		
	/*1. Check the vaildity of input parameters*/
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscGetConf=1", index-1);
		system(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscGetConf=1", index-1);
		system(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wscStop(wifi_type type, int index)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd[MAX_KEYWORD_LEN] 	 = {0};
	/*1. Check the vaildity of input parameters*/
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscStop=1", index-1);
		system(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscStop=1", index-1);
		system(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wscConfMode(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char keyword[MAX_KEYWORD_LEN] 	 = {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 0 || tmp > 7)
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. Write parameters to shm*/
	sprintf(keyword, "WscConfMode");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscConfMode failed.\n");
	}
	
	/*3. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscConfMode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscConfMode=%s",index-1, sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wscConfStatus(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char keyword[MAX_KEYWORD_LEN]= {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 1 || tmp > 2)//1-2
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");

	/*2. Write parameters to shm*/
	sprintf(keyword, "WscConfStatus");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscConfStatus failed.\n");
	}
	
	/*3. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscConfStatus=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscConfStatus=%s",index-1, sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wscV2Support(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char keyword[MAX_KEYWORD_LEN]= {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 0 || tmp > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");

	/*2. Write parameters to shm*/
	sprintf(keyword, "WscV2Support");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscV2Support failed.\n");
	}
	
	/*3. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscV2Support=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscV2Support=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_wscMaxPinAttack(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char keyword[MAX_KEYWORD_LEN]= {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 0 || tmp > 10)//0-10
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");

	/*2. Write parameters to shm*/
	sprintf(keyword, "WscMaxPinAttack");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscMaxPinAttack failed.\n");
	}
	
	/*3. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscMaxPinAttack=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscMaxPinAttack=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_wscKeyASCII(wifi_type type, int index, char *input)
{	
	WIFI_FUNC_TRACE_ENTER;	
	int  tmp = -1;	
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};	
	char keyword[MAX_KEYWORD_LEN]= {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/	
	if(NULL == input)				
		RETURN_WIFIMGR_FAIL;	
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))		
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");	
	tmp = atoi(input);	
	if(tmp < 0 || (tmp > 1 && tmp < 8) || tmp > 63)//0-1	or 8-63	
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");	
	if(index < 1 ||index > 8)		
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");		

	/*2. Write parameters to shm*/
	sprintf(keyword, "WscKeyASCII");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscKeyASCII failed.\n");
	}
	
	/*3. parameters execute*/	
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {		
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscKeyASCII=%s",index-1,sf_input);		
		API_SYSTEM_RAW(cmd);	
	}	
	else if(WIFI_5G == type) {		
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscKeyASCII=%s",index-1,sf_input);		
		API_SYSTEM_RAW(cmd);	
	}	
	else {		
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");	
	}	
	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_WscUUIDE(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;	

	if(index < 1 ||index > 8){
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
		}
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WSC_UUID_E%d",index);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_WscUUIDStr(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;	

	if(index < 1 ||index > 8){
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
		}
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WSC_UUID_Str%d",index);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}



int
wifimgr_lib_set_wscSetupLockTime(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char keyword[MAX_KEYWORD_LEN]= {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 0)//>=0
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");

	/*2. Write parameters to shm*/
	sprintf(keyword, "WscSetupLockTime");
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_FAIL_MSG("Set the WscSetupLockTime failed.\n");
	}
	
	/*3. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscSetupLockTime=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscSetupLockTime=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}


int
wifimgr_lib_set_wscMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 1 || tmp > 2)//1-2
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscMode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscMode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_set_wscPinCode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscPinCode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscPinCode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}


int
wifimgr_lib_set_wscGenPinCode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int  tmp = -1;
	char cmd[MAX_KEYWORD_LEN*2] 	 = {0};
	char sf_input[MAX_KEYWORD_LEN]   = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	if(ECNT_WIFIMGR_FALSE == check_is_num(input))
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	tmp = atoi(input);
	if(tmp < 0 || tmp > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	api_escape_parameter(input, sf_input, sizeof(sf_input));
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscGenPinCode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscGenPinCode=%s",index-1,sf_input);
		API_SYSTEM_RAW(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wscOOB(wifi_type type, int index)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd[MAX_KEYWORD_LEN] = {0};
	/*1. Check the vaildity of input parameters*/
	if(index < 1 ||index > 8)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	/*2. parameters execute*/
	if(WIFI_2_4G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv ra%d set WscOOB=1", index-1);
		system(cmd);
	}
	else if(WIFI_5G == type) {
		sprintf(cmd,"/userfs/bin/iwpriv rai%d set WscOOB=1", index-1);
		system(cmd);
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The wifi type is invalid.\n");
	}
	RETURN_WIFIMGR_SUCCESS;	
}

int
wifimgr_lib_set_radiusserver(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if(IPV4check(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The RADIUS_Server IP value is invalid.Please input from 0.0.0.0 to 255.255.255.255\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RADIUS_Server");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_radiusport(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	if(doStrDecCheck(input) == 0 || !(0 < atoi(input) && atoi(input) <= 65535) )  /*1  to 65535*/
		RETURN_WIFIMGR_FAIL_MSG("The RADIUS_Port value is invalid.\nPlease input DecNum from 1 - 65535\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "RADIUS_Port");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_radiuskey(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;		
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	int  len;
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)	
		RETURN_WIFIMGR_FAIL;
		
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	len=strlen(input);
	if(8 <= len && len <= 63){
	    ;
	}
	else if(64 == len){
		if(doStrHexCheck(input) == 0)
			RETURN_WIFIMGR_FAIL_MSG("The RADIUS_Key is invalid.\nPlease input 8-63 characters or 64 Hex string.\n");
	}
	else{ 
		RETURN_WIFIMGR_FAIL_MSG("The RADIUS_Key is invalid.\nPlease input 8-63 characters or 64 Hex string.\n");
	}
	/*2. Construct suitable parameters*/
	sprintf(keyword, "RADIUS_Key%d", index);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_ieee8021x(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if( strcmp(input, "0") != 0 && strcmp(input, "1") != 0 ) 
		RETURN_WIFIMGR_FAIL_MSG("The IEEE8021X value is invalid.\nPlease input 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "IEEE8021X");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_session_timeout_interval(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char  keyword[MAX_KEYWORD_LEN+1] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

    if(doStrDecCheck(input) == 0 || strlen(input) > 10 || (strlen(input) == 10 && strcmp(input, "2147483647") > 0)){
		RETURN_WIFIMGR_FAIL_MSG("The timeout value is invalid.\nPlease input from 0 to 2147483647.\n");
	}
	/*2. Construct suitable parameters*/
	sprintf (keyword, "session_timeout_interval");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_wdsenable(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	if (strlen(input) != 1 || doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 4)	
		RETURN_WIFIMGR_FAIL_MSG("Please input value from 0 to 4(0:disable;1-4:diffrent modes,recommend 1).\n");

	/*2. Construct suitable parameters*/
	sprintf (keyword, "WdsEnable");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_wdslist(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	int	 i,j = 0;
	char *macptr;
	char inputTmp[128] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	else
		strncpy(inputTmp, input, sizeof(inputTmp) - 1);
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	for (i = 0, macptr = rstrtok(inputTmp,";"); (macptr); macptr = rstrtok(NULL,";"), i++) {				
		if(strlen(macptr) != 17)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 */
			RETURN_WIFIMGR_FAIL_MSG("Please input acceptable Mac address.\n");
		if(strcmp(macptr,"00:00:00:00:00:00") == 0)
			continue; 

		for (j = 0; (macptr && j < MAC_ADDR_LEN); j++) {
			if ((doHexCheck(*macptr) < 0) || (doHexCheck(*(macptr+1)) < 0)) {
   				RETURN_WIFIMGR_FAIL_MSG("Mac address check error!\n");
			}		
			macptr = macptr+3;
		}
	}
	
	if (i != MAX_WDS_ENTRY || charCnt(input, ';') > MAX_WDS_ENTRY -1){
		RETURN_WIFIMGR_FAIL_MSG("Please input %d Mac addresses\n", MAX_WDS_ENTRY);
	}
	/*2. Construct suitable parameters*/
	sprintf (keyword, "WdsList");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_set_wdsencryptype(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	int	  i = 0;
	char *macptr;
	char inputTmp[MAX_KEYWORD_LEN+1] = {0};
	char buf[MAX_KEYWORD_LEN+1]		 = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	else
		strncpy(inputTmp, input, sizeof(inputTmp) - 1);
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
				
	for (i = 0, macptr = rstrtok(inputTmp,";"); (macptr && i < MAX_WDS_ENTRY); macptr = rstrtok(NULL,";"), i++) {		
	        if (rtstrcasecmp(macptr, "NONE") == 1)
	            continue;
	        else if (rtstrcasecmp(macptr, "WEP") == 1)
	            continue;
	        else if (rtstrcasecmp(macptr, "TKIP") ==1)
	            continue;
	        else if (rtstrcasecmp(macptr, "AES") == 1)
	            continue;
	        else
	            RETURN_WIFIMGR_FAIL_MSG("Please input value from NONE,WEP,TKIP,AES.\n");
	}
					
	if (i != MAX_WDS_ENTRY)
		RETURN_WIFIMGR_FAIL_MSG("Please input %d wdsencryptypes\n", MAX_WDS_ENTRY);

	/*2. Construct suitable parameters*/
	sprintf (keyword, "WdsEncrypType");
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		if(strlen(input) != 0){
			strcat(buf, input);
			strcat(buf, ";");
		}	
	}
	else
	{
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			if(strlen(input) != 0){
				strcat(buf, input);
				strcat(buf, ";");
			}		
		}
	}
#else		
#if defined(MT7915D) || defined(MT7915E) || defined(MT7916D)|| defined(MT7916E)
	/*3. Construct suitable input parameters*/
	if(strlen(input) != 0){
		strcat(buf, input);
		strcat(buf, ";");
	}	
#else
	/*3. Construct suitable input parameters*/
	for(i = 0; i < MAX_WDS_ENTRY; i++){
		if(strlen(input) != 0){
			strcat(buf, input);
			strcat(buf, ";");
		}		
	}
#endif
#endif
	buf[strlen(buf)-1] = '\0';
	/*4. Write parameters to shm*/
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, buf)){
			RETURN_WIFIMGR_SUCCESS;
		}
	}
	else
	{
		if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
			RETURN_WIFIMGR_SUCCESS;
		}
	}
#else
#if defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, buf)){
		RETURN_WIFIMGR_SUCCESS;
	}
#else
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
#endif
#endif
	RETURN_WIFIMGR_FAIL;
}



int 
wifimgr_lib_set_wdskey(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1] = {0};
#if defined(TCSUPPORT_WLAN_MULTI_CHIP) || defined(MT7615E)
	char wdsencrypTmp[MAX_KEYWORD_LEN+1] = {0};
#endif
	char WdsencrypStr[4][MAX_KEYWORD_LEN+1] = {0};
	int  IPcont = 0;

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( index < 1 || MAX_WDS_ENTRY < index )
		RETURN_WIFIMGR_FAIL_MSG("The wdskey index should be from 1 to %d.\n", MAX_WDS_ENTRY);
	
	if (ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_wdsencryptype(type, -1, outbuffer, MAX_KEYWORD_LEN)) 
		RETURN_WIFIMGR_FAIL_MSG("get_wdsencryptype err.\n");

		
	IPcont = charCnt(outbuffer, ';');    //cnt ';'
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
	{
		if (IPcont != (MAX_WDS_ENTRY*MAX_WDS_ENTRY-1))
			RETURN_WIFIMGR_FAIL_MSG("The number of WdsEncrypType is should be %d.\n", MAX_WDS_ENTRY*MAX_WDS_ENTRY);
		
		strextract(wdsencrypTmp, outbuffer);
		sscanf(wdsencrypTmp, "%[^;];%[^;];%[^;];%[^;]", WdsencrypStr[0], WdsencrypStr[1], WdsencrypStr[2], WdsencrypStr[3]);
	}
	else
	{
		if (IPcont != (MAX_WDS_ENTRY-1))
			RETURN_WIFIMGR_FAIL_MSG("The number of WdsEncrypType is should be %d.\n", MAX_WDS_ENTRY);
		sscanf(outbuffer, "%[^;];%[^;];%[^;];%[^;]", WdsencrypStr[0], WdsencrypStr[1], WdsencrypStr[2], WdsencrypStr[3]);
	}
#else
#if defined(MT7615E)
	if (IPcont != (MAX_WDS_ENTRY*MAX_WDS_ENTRY-1))
		RETURN_WIFIMGR_FAIL_MSG("The number of WdsEncrypType is should be %d.\n", MAX_WDS_ENTRY*MAX_WDS_ENTRY);
	
	strextract(wdsencrypTmp, outbuffer);
	sscanf(wdsencrypTmp, "%[^;];%[^;];%[^;];%[^;]", WdsencrypStr[0], WdsencrypStr[1], WdsencrypStr[2], WdsencrypStr[3]);	
#else 	
	if (IPcont != (MAX_WDS_ENTRY-1))
		RETURN_WIFIMGR_FAIL_MSG("The number of WdsEncrypType is should be %d.\n", MAX_WDS_ENTRY);
	sscanf(outbuffer, "%[^;];%[^;];%[^;];%[^;]", WdsencrypStr[0], WdsencrypStr[1], WdsencrypStr[2], WdsencrypStr[3]);
#endif	
#endif
		if(strcmp(WdsencrypStr[index-1], "WEP") == 0){          //check wdskey based on wdsencryptype
			if (0 != wdskey_check_for_wep(strlen(input), input))
			RETURN_WIFIMGR_FAIL_MSG("Please input 5 or 13 ASCII characters / 10 or 26 hexadecimal characters!\n");
	}
		else if(strcmp(WdsencrypStr[index-1], "TKIP") == 0 || strcmp(WdsencrypStr[index-1], "AES") == 0){
			if (0 != wdskey_check(strlen(input), input))
			RETURN_WIFIMGR_FAIL_MSG("Please input 8 to 64 hexadecimal characters!\n");
	}
	else 
		RETURN_WIFIMGR_FAIL_MSG("WdsEntryType in datfile is ERR.\n");
	

	/*2. Construct suitable parameters and Write parameters to shm*/
	sprintf (keyword, "Wds%dKey", index - 1);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS != update_attr(type, -1, keyword, input))
		return ECNT_WIFIMGR_FAILURE;
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
wifimgr_lib_set_wdsphymode(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	int	  i = 0;
	char *macptr;
	char inputTmp[MAX_KEYWORD_LEN+1] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	else
		strncpy(inputTmp, input, sizeof(inputTmp) - 1);
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	for (i=0, macptr = rstrtok(inputTmp,";"); (macptr && i < MAX_WDS_ENTRY); macptr = rstrtok(NULL,";"), i++) {
	        if (rtstrcasecmp(macptr, "CCK") == 1)
	            continue;
	        else if (rtstrcasecmp(macptr, "OFDM") == 1)
	            continue;
	        else if (rtstrcasecmp(macptr, "HTMIX") == 1)
	            continue;
	        else if (rtstrcasecmp(macptr, "GREENFIELD") == 1)
	            continue;
	        else
	            RETURN_WIFIMGR_FAIL_MSG("Please input value from CCK,OFDM,HTMIX,GREENFIELD.\n");
	}
	
	if (i != MAX_WDS_ENTRY)
		RETURN_WIFIMGR_FAIL_MSG("Please input %d wdsphyModes\n", MAX_WDS_ENTRY);

	/*2. Construct suitable parameters*/
	sprintf (keyword, "WdsPhyMode");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;			
}

int 
wifimgr_lib_set_ownip(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if(IPV4check(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("The own IP is invalid.\nPlease input from 0.0.0.0 to 255.255.255.255\n");

	/*2. Construct suitable parameters*/
	sprintf (keyword, "own_ip_addr");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_MacAddress(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};

	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");

	if(MacCheck(input) == 0)
		RETURN_WIFIMGR_FAIL_MSG("Please input correct MacAddress, such as 00:1a:22:aa:dc:a1.\n");

	/*2. Construct suitable parameters*/
	sprintf (keyword, "MacAddress");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_accessPolicy(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1] = {0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if( doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 2 ) /*0-2*/
		RETURN_WIFIMGR_FAIL_MSG("The AccessPolicy value is invalid.\n");

	/*2. Construct suitable parameters*/
	sprintf (keyword, "AccessPolicy%d", index-1);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_accessControlList(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0};
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "AccessControlList%d", index-1);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_wscDefaultSSID(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;		
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	char outbuffer[MAX_KEYWORD_LEN+1]={0}; 
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)	
		RETURN_WIFIMGR_FAIL;
		
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN+1))
		RETURN_WIFIMGR_FAIL;

	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	if(strlen(input) < 1 || strlen(input) > 32)
		RETURN_WIFIMGR_FAIL_MSG("The input value is invalid.\n");

	/*2. Construct suitable parameters*/
	sprintf(keyword, "WscDefaultSSID%d", index);

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int
wifimgr_lib_set_EfuseBufferMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	if(doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The EfuseBufferMode value should be 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "EfuseBufferMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_igmpSnEnable(wifi_type type, int index, char *input)
{
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID] && type == 0){ 	
		if( -1 != index) {
			RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
		}
	}else
#else
#ifdef MT7592
	if( type == 0){
		if( -1 != index) {
			RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
		}
	}else
#endif
#endif
	{
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
			RETURN_WIFIMGR_FAIL;
		}
		if(index < 1 || index > atoi(outbuffer))
			RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	}
	if(doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The IgmpSnEnable value should be 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "IgmpSnEnable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

#if defined(TCSUPPORT_ECNT_MAP)
int
wifimgr_lib_set_MapEnable(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	if(doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The MapEnable value should be 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MapEnable");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_MAP_Turnkey(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	if(doStrDecCheck(input) == 0 || atoi(input) < 0 || atoi(input) > 1)//0-1
		RETURN_WIFIMGR_FAIL_MSG("The MAP_Turnkey value should be 0 or 1.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MAP_Turnkey");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_set_MAP_Ext(wifi_type type, int index, char* input)
{
	WIFI_FUNC_TRACE_ENTER;	
	char keyword[MAX_KEYWORD_LEN+1] = {0};
	int	 i = 0;
	char *macptr;
	char inputTmp[128] = {0};
	int  tmp = 0;
	
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	else
		strncpy(inputTmp, input, sizeof(inputTmp) - 1);
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	for (i = 0, macptr = rstrtok(inputTmp,";"); (macptr); macptr = rstrtok(NULL,";"), i++) {				
		tmp = strtol(macptr, 0, 10);
		if(tmp < 0)
			RETURN_WIFIMGR_FAIL_MSG("The MAP_Ext value error.\n"); 
	}
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MAP_Ext");

	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)){
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}
#endif

int
wifimgr_lib_save_dataFile(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	int count 					= -1;
	char buffer[MAX_KEYWORD_LEN]= {0};
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssidnum(type, -1, buffer, MAX_KEYWORD_LEN)){
		count = atoi(buffer);
	}else{
		RETURN_WIFIMGR_FAIL;
	}
	
	if(ECNT_WIFIMGR_SUCCESS == update_datafile_from_shm(type, count)){
		RETURN_WIFIMGR_SUCCESS;
	}else{
		RETURN_WIFIMGR_FAIL;
	}
}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int
wifimgr_lib_restartInterface(WLan_info wlan_info,int nodeid, int resetFlag)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd_down[MAX_KEYWORD_LEN] 	 = {0};
	char cmd_up[MAX_KEYWORD_LEN] 	 = {0};
	char flag[7]					 = {'0','0','0','0','0','0','0'};
	int i 		= 0;
	char EnableSSID[MAX_KEYWORD_LEN]	= {0};
	char node_name[128] = {0};
	char tmp[32]		= {0};	
	
	if(WIFI_2_4G == wlan_info.type ) {		
		/*check need to rmmod and insmod .ko or not*/
		if(wlan_info.oldNum != wlan_info.newNum) {
			/*need to reload .ko*/
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down),"ifconfig wds%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s wds%d",BR_NAME, i);
				system(cmd_down);
			}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			for(i = 0; i < MAX_APCLIENT_NUM; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_2G_NAME_FORMAT " down",i);
				system(cmd_down);	
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_2G_NAME_FORMAT,BR_NAME,i);
				system(cmd_down);		
			}
				
				
#endif

			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s ra%d", BR_NAME, i);
				system(cmd_down);
			}
			if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag, sizeof(flag))) {
#ifdef MT7592
				/*MT7603*/
				if('1' == flag[0]) {	
					system("rmmod mt7603eap.ko");
					system(INSMOD_MT7603_MODULE_PATH);
				}
#elif MT7615_11N
				/*MT7615_11N*/
				if('1' == flag[2]) {	
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif		
				}	
#elif MT7915N
				/*MT7915N*/
				if('1' == flag[4] || '1' == flag[5] || '1' == flag[6]) {	
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko");*/
				}	
#elif MT7916N
				/*MT7916N*/
				if('1' == flag[4] || '1' == flag[5] || '1' == flag[6]) {	
					/*system("rmmod mt7916_ap");*/
					/*system("insmod /lib/modules/mt7916_ap.ko");*/
				}
#endif // MT7592
				if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(WIFI_2_4G, "0"))
					RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");	
			}
			
			for( i = 0; i < MAX_SSID_NUM; i++){
				if( 0 != ((wlan_info.index) & (1 << i))){
					snprintf(cmd_up, sizeof(cmd_up), "ifconfig ra%d up", i);
					system(cmd_up);
					memset(cmd_up, 0, sizeof(cmd_up));
					snprintf(cmd_up, sizeof(cmd_up), "brctl addif br0 ra%d", i);
					system(cmd_up);
				}			
			}
		} 
		else {
			if(resetFlag ==1){
				
				for(i = 0; i < MAX_SSID_NUM; ++i ) {
					memset(cmd_down,0,sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
					system(cmd_down);
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "brctl delif br0 ra%d", i);
					system(cmd_down);
				}
#ifdef TCSUPPORT_WLAN_WDS
				for(i = 0; i < MAX_WDS_ENTRY; i++){
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"ifconfig wds%d down",i);
					system(cmd_down);
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"brctl delif br0 wds%d",i);
					system(cmd_down);		
				}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
				for(i = 0; i < MAX_APCLIENT_NUM; i++){
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_2G_NAME_FORMAT " down",i);
					system(cmd_down);	
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_2G_NAME_FORMAT,BR_NAME,i);
					system(cmd_down);		
				}
										
#endif
				for( i = 0; i < MAX_SSID_NUM; i++){
					if( 0 != ((wlan_info.index) & (1 << i))){
						snprintf(cmd_up, sizeof(cmd_up), "ifconfig ra%d up", i);
						system(cmd_up);
						memset(cmd_up, 0, sizeof(cmd_up));
						snprintf(cmd_up, sizeof(cmd_up), "brctl addif br0 ra%d", i);
						system(cmd_up);
					}			
				}
			}
			else {
				
				//memset(node_name, 0, sizeof(node_name));
				//snprintf(node_name, sizeof(node_name), "root.wlan.entry.%d", nodeid+1);
				if(wifimgr_lib_get_enableSsid(0, nodeid+1, EnableSSID, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE)
				{
					
					if(atoi(EnableSSID)==0){
						
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "ifconfig ra%d down", nodeid);
						system(tmp);
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "brctl delif br0 ra%d", nodeid);
						system(tmp);		
					}else{
						
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "ifconfig ra%d up", nodeid);
						system(tmp);
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "brctl addif br0 ra%d", nodeid);
						system(tmp);
					}
				}
			}
		}

#ifdef TCSUPPORT_WLAN_WDS
		if(wlan_info.wdsActive != 0){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig wds%d up", i);
				system(cmd_up);
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s wds%d", BR_NAME, i);
				system(cmd_up);
				printf("addif wds%d to br0\n",i);
			}
		}
#endif

	} 
	else if (WIFI_5G == wlan_info.type) {
#if defined(TCSUPPORT_WLAN_MT7612_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7613_BUILD_IN) 
		if (resetFlag ==1){
			
			for(i = 0; i < MAX_SSID_NUM; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif br0 rai%d", i);
				system(cmd_down);
			}
#if defined(TCSUPPORT_ECNT_MAP)
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down, sizeof(cmd_down), "ifconfig apclii0 down");
			system(cmd_down);
#endif	
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig wdsi%d down",i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif br0 wdsi%d",i);
				system(cmd_down);		
			}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			for(i = 0; i < MAX_APCLIENT_NUM; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_5G_NAME_FORMAT " down",i);
				system(cmd_down);	
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_5G_NAME_FORMAT,BR_NAME,i);
				system(cmd_down);		
			}
#endif
			for( i = 0; i < MAX_SSID_NUM; i++){
				if( 0 != ((wlan_info.index) & (1 << i))){
					snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
					system(cmd_up);
					memset(cmd_up, 0, sizeof(cmd_up));
					snprintf(cmd_up, sizeof(cmd_up), "brctl addif br0 rai%d", i);
					system(cmd_up);
				}			
			}
		}
		else {
			
			if(wifimgr_lib_get_enableSsid(1, nodeid+1, EnableSSID, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE)
			{
				if(atoi(EnableSSID)==0){
					
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "ifconfig rai%d down", nodeid);
					system(tmp);
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "brctl delif br0 rai%d", nodeid);
					system(tmp);		
				}else{
					
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "ifconfig rai%d up", nodeid);
					system(tmp);
					memset(tmp, 0, sizeof(tmp));
					snprintf(tmp, sizeof(tmp), "brctl addif br0 rai%d", nodeid);
					system(tmp);
				}
			}
		}
#else
		/*check need to rmmod and insmod .ko or not*/
		if(wlan_info.oldNum != wlan_info.newNum) {
			/*need to reload .ko*/
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
				memset(cmd_down,0,MAX_KEYWORD_LEN);
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig wdsi%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s wdsi%d",BR_NAME, i);
				system(cmd_down);
			}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			for(i = 0; i < MAX_APCLIENT_NUM; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_5G_NAME_FORMAT " down",i);
				system(cmd_down);	
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_5G_NAME_FORMAT,BR_NAME,i);
				system(cmd_down);		
			}
		
#endif

			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,MAX_KEYWORD_LEN);
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s rai%d",BR_NAME, i);
				system(cmd_down);
			}
			if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag, sizeof(flag))) {
#ifdef MT7612E
				/*MT7612*/					
				if('1' == flag[1]) {
					system("rmmod mt7662e_ap");
					system(INSMOD_MT7612_MODULE_PATH);
				}
#elif MT7613E
				/*MT7613*/					
				if('1' == flag[3]) {
					system("rmmod mt7663_ap");
					system(INSMOD_MT7613_MODULE_PATH);
				}
#elif MT7615E
				/*MT7615E*/
				if('1' == flag[2]) {
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif
				}	
#elif MT7915E
				/*MT7915E*/
				if('1' == flag[4] || '1' == flag[6]) {
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko"); */
				}	
#elif MT7916E
				/*MT7916E*/
				if('1' == flag[4] || '1' == flag[6]) {
					/*system("rmmod mt7916_ap");*/
					/*system("insmod /lib/modules/mt7916_ap.ko"); */
				}
#endif
				if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(WIFI_5G, "0"))
					RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");
			}
			for(i = 0; i < MAX_SSID_NUM; i++){
				if( 0 != ((wlan_info.index) & (1 << i))){
					snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
					system(cmd_up);
					memset(cmd_up, 0, sizeof(cmd_up));
					snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s rai%d",BR_NAME, i);
					system(cmd_up);
				}			
			}
		} 
		else {
			if(resetFlag ==1){
				
				for(i = 0; i < MAX_SSID_NUM; ++i ) {
					memset(cmd_down,0,sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
					system(cmd_down);
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "brctl delif br0 rai%d", i);
					system(cmd_down);
				}
		
#ifdef TCSUPPORT_WLAN_WDS
				for(i = 0; i < MAX_WDS_ENTRY; i++){
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"ifconfig wdsi%d down",i);
					system(cmd_down);
					memset(cmd_down, 0, sizeof(cmd_down));
					snprintf(cmd_down,sizeof(cmd_down),"brctl delif br0 wdsi%d",i);
					system(cmd_down);		
				}
#endif
				for( i = 0; i < MAX_SSID_NUM; i++){
					if( 0 != ((wlan_info.index) & (1 << i))){
						snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
						system(cmd_up);
						memset(cmd_up, 0, sizeof(cmd_up));
						snprintf(cmd_up, sizeof(cmd_up), "brctl addif br0 rai%d", i);
						system(cmd_up);
					}			
				}
			}

			else {
			
				if(wifimgr_lib_get_enableSsid(1, nodeid+1, EnableSSID, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE)
				{
					if(atoi(EnableSSID)==0){
						
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "ifconfig rai%d down", nodeid);
						system(tmp);
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "brctl delif br0 rai%d", nodeid);
						system(tmp);		
					}else{

						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "ifconfig rai%d up", nodeid);
						system(tmp);
						memset(tmp, 0, sizeof(tmp));
						snprintf(tmp, sizeof(tmp), "brctl addif br0 rai%d", nodeid);
						system(tmp);
					}
				}
			}
		}
#endif

#ifdef TCSUPPORT_WLAN_WDS
		if(wlan_info.wdsActive != 0){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig wdsi%d up", i);
				system(cmd_up);
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s wdsi%d",BR_NAME, i);
				system(cmd_up);
				printf("addif wdsi%d to br0\n",i);
			}
		}
#endif

	}
	else {
		RETURN_WIFIMGR_FAIL;
	}

	RETURN_WIFIMGR_SUCCESS;
}

#else //defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int
wifimgr_lib_restartInterface(WLan_info wlan_info)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd_down[MAX_KEYWORD_LEN] 	 = {0};
	char cmd_up[MAX_KEYWORD_LEN] 	 = {0};
	char flag[7] 					 = {'0','0','0','0','0','0','0'};
	int i 		= 0;
	
	if(WIFI_2_4G == wlan_info.type ) {		
		/*check need to rmmod and insmod .ko or not*/
		if(wlan_info.oldNum != wlan_info.newNum) {
			/*need to reload .ko*/
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down),"ifconfig wds%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s wds%d",BR_NAME, i);
				system(cmd_down);
			}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			for(i = 0; i < MAX_APCLIENT_NUM; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_2G_NAME_FORMAT " down",i);
				system(cmd_down);	
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_2G_NAME_FORMAT,BR_NAME,i);
				system(cmd_down);		
			}
				
				
#endif

			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s ra%d", BR_NAME, i);
				system(cmd_down);
			}
			if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag, sizeof(flag))) {

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				/*MT7603*/
				if('1' == flag[0]) {	
					system("rmmod mt7603eap.ko");
					system(INSMOD_MT7603_MODULE_PATH);
				}

				/*MT7615_11N*/
				if('1' == flag[2]) {	
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif		
				}	

				/*MT7915N*/
				if('1' == flag[4] || '1' == flag[5] || '1' == flag[6]) {	
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko");	*/	
				}	
#else
#ifdef MT7592
				/*MT7603*/
				if('1' == flag[0]) {	
					system("rmmod mt7603eap.ko");
					system(INSMOD_MT7603_MODULE_PATH);
				}
#elif MT7615_11N
				/*MT7615_11N*/
				if('1' == flag[2]) {	
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif		
				}	
#elif MT7915N
				/*MT7915N*/
				if('1' == flag[4] || '1' == flag[5] || '1' == flag[6]) {	
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko");	*/	
				}	
#elif MT7916N
				/*MT7916N*/
				if('1' == flag[4] || '1' == flag[5] || '1' == flag[6]) {	
					/*system("rmmod mt7916_ap");*/
					/*system("insmod /lib/modules/mt7916_ap.ko");	*/	
				}
#endif
#endif //MULTI_CHIP
				if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(WIFI_2_4G, "0"))
					RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");	
			}
		} 
		else {
			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s ra%d", BR_NAME, i);
				system(cmd_down);
			}
#ifdef TCSUPPORT_WLAN_WDS
					for(i = 0; i < MAX_WDS_ENTRY; i++){
						memset(cmd_down, 0, sizeof(cmd_down));
						snprintf(cmd_down,sizeof(cmd_down),"ifconfig wds%d down",i);
						system(cmd_down);
						memset(cmd_down, 0, sizeof(cmd_down));
						snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s wds%d",BR_NAME,i);
						system(cmd_down);		
					}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
					for(i = 0; i < MAX_APCLIENT_NUM; i++){
						memset(cmd_down, 0, sizeof(cmd_down));
						snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_2G_NAME_FORMAT " down",i);
						system(cmd_down);	
						memset(cmd_down, 0, sizeof(cmd_down));
						snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_2G_NAME_FORMAT,BR_NAME,i);
						system(cmd_down);		
					}
						
#endif


		}

		for( i = 0; i < MAX_SSID_NUM; i++){
			if( 0 != ((wlan_info.index) & (1 << i))){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig ra%d up", i);
				system(cmd_up);
				memset(cmd_up, 0, sizeof(cmd_up));
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s ra%d",BR_NAME, i);
				system(cmd_up);
			} 			
		}
#ifdef TCSUPPORT_WLAN_WDS
		if(wlan_info.wdsActive != 0){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig wds%d up", i);
				system(cmd_up);
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s wds%d", BR_NAME, i);
				system(cmd_up);
				printf("addif wds%d to br0\n",i);
			}
		}
#endif

	} 
	else if (WIFI_5G == wlan_info.type) {
#if defined(TCSUPPORT_WLAN_MT7612_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7615_BUILD_IN)  || defined(TCSUPPORT_WLAN_MT7613_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7916_BUILD_IN)
		for(i = 0; i < wlan_info.oldNum; ++i ) {
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
			system(cmd_down);
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s rai%d", BR_NAME, i);
			system(cmd_down);
		}
#if defined(TCSUPPORT_ECNT_MAP)
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down, sizeof(cmd_down), "ifconfig apclii0 down");
			system(cmd_down);
#endif			
#ifdef TCSUPPORT_WLAN_WDS
		for(i = 0; i < MAX_WDS_ENTRY; i++){
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down,sizeof(cmd_down),"ifconfig wdsi%d down",i);
			system(cmd_down);
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s wdsi%d",BR_NAME,i);
			system(cmd_down);		
		}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
		for(i = 0; i < MAX_APCLIENT_NUM; i++){
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_5G_NAME_FORMAT " down",i);
			system(cmd_down);	
			memset(cmd_down, 0, sizeof(cmd_down));
			snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_5G_NAME_FORMAT,BR_NAME,i);
			system(cmd_down);		
		}

#endif
		for(i = 0; i < MAX_SSID_NUM; i++){
			if(0 != ((wlan_info.index) & (1 << i))){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
				system(cmd_up);
				memset(cmd_up, 0, sizeof(cmd_up));
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s rai%d",BR_NAME, i);
				system(cmd_up);
			}			
		}
#else //defined(TCSUPPORT_WLAN_MT7612_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7615_BUILD_IN)  || defined(TCSUPPORT_WLAN_MT7613_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7916_BUILD_IN)
		/*check need to rmmod and insmod .ko or not*/
		if(wlan_info.oldNum != wlan_info.newNum) {
			/*need to reload .ko*/
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
				memset(cmd_down,0,MAX_KEYWORD_LEN);
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig wdsi%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s wdsi%d",BR_NAME, i);
				system(cmd_down);
			}
#endif
#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			for(i = 0; i < MAX_APCLIENT_NUM; i++){
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_5G_NAME_FORMAT " down",i);
				system(cmd_down);	
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_5G_NAME_FORMAT,BR_NAME,i);
				system(cmd_down);		
			}
		
#endif

			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,MAX_KEYWORD_LEN);
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s rai%d",BR_NAME, i);
				system(cmd_down);
			}
			if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag, sizeof(flag))) {
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				/*MT7612*/					
				if('1' == flag[1]) {
					system("rmmod mt7662e_ap");
					system(INSMOD_MT7612_MODULE_PATH);
				}

				/*MT7613*/					
				if('1' == flag[3]) {
					system("rmmod mt7663_ap");
					system(INSMOD_MT7613_MODULE_PATH);
				}

				/*MT7615E*/
				if('1' == flag[2]) {
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif
				}	

				/*MT7915E*/
				if('1' == flag[4]|| '1' == flag[6]) {
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko"); */
				}	

#else //TCSUPPORT_WLAN_MULTI_CHIP
#ifdef MT7612E
				/*MT7612*/					
				if('1' == flag[1]) {
					system("rmmod mt7662e_ap");
					system(INSMOD_MT7612_MODULE_PATH);
				}
#elif MT7613E
				/*MT7613*/					
				if('1' == flag[3]) {
					system("rmmod mt7663_ap");
					system(INSMOD_MT7613_MODULE_PATH);
				}
#elif MT7615E
				/*MT7615E*/
				if('1' == flag[2]) {
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("rmmod mt_whnat");
#endif
					system("rmmod mt7615_ap");
					system(INSMOD_MT7615_MODULE_PATH); 
#ifdef TCSUPPORT_WLAN_MT7615_TXOFFLOAD
					system("insmod /lib/modules/mt_whnat.ko");
#endif
				}	
#elif MT7915E
				/*MT7915E*/
				if('1' == flag[4]|| '1' == flag[6]) {
					/*system("rmmod mt7915_ap");*/
					/*system("insmod /lib/modules/mt7915_ap.ko"); */
				}	
#elif MT7916E
				/*MT7916E*/
				if('1' == flag[4]|| '1' == flag[6]) {
					/*system("rmmod mt7916_ap");*/
					/*system("insmod /lib/modules/mt7916_ap.ko"); */
				}
#endif //MT7612E
#endif //TCSUPPORT_WLAN_MULTI_CHIP
				if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(WIFI_5G, "0"))
					RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");
			}
		} 
		else {
			for(i = 0; i < wlan_info.oldNum; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
				system(cmd_down);
				memset(cmd_down, 0, sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s rai%d",BR_NAME, i);
				system(cmd_down);
			}
		}
			
		for(i = 0; i < MAX_SSID_NUM; i++){
			if( 0 != ((wlan_info.index) & (1 << i))){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
				system(cmd_up);
				memset(cmd_up, 0, sizeof(cmd_up));
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s rai%d",BR_NAME, i);
				system(cmd_up);
			}			
		}
//#endif //TCSUPPORT_WLAN_MULTI_CHIP
#endif //defined(TCSUPPORT_WLAN_MT7612_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7615_BUILD_IN)  || defined(TCSUPPORT_WLAN_MT7613_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7916_BUILD_IN)

#ifdef TCSUPPORT_WLAN_WDS
		if(wlan_info.wdsActive != 0){
			for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(cmd_up, sizeof(cmd_up), "ifconfig wdsi%d up", i);
				system(cmd_up);
				snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s wdsi%d",BR_NAME, i);
				system(cmd_up);
				printf("addif wdsi%d to br0\n",i);
			}
		}
#endif

	}
	else {
		RETURN_WIFIMGR_FAIL;
	}

	RETURN_WIFIMGR_SUCCESS;
}
#endif //defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int 
wifimgr_lib_commit(wifi_type type,int index)
{
	WIFI_FUNC_TRACE_ENTER;
	char cmd_down[MAX_KEYWORD_LEN] 	 = {0};
	char cmd_up[MAX_KEYWORD_LEN] 	 = {0};
	char outbuffer[MAX_KEYWORD_LEN]  = {0};
	char output[MAX_KEYWORD_LEN]     = {0};
	char wdsenable[MAX_KEYWORD_LEN]  = {0};
	int  count = -1;
	char flag[7] = {'0','0','0','0','0','0','0'};
	int i = 0;
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssidnum(type, -1, outbuffer,
													MAX_KEYWORD_LEN))
		count = atoi(outbuffer);
	
	else RETURN_WIFIMGR_FAIL;

	
	if(index < 1 || index > count)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index parameter is invalid.\n ");

	
    #ifdef TCSUPPORT_WLAN_WDS
		memset(wdsenable,0,MAX_KEYWORD_LEN);
		wifimgr_lib_get_wdsenable(type,-1,wdsenable,MAX_KEYWORD_LEN);
    #endif

	wifimgr_lib_get_reloadFlag(type, output, MAX_KEYWORD_LEN);
	if(0 == strncmp(output,"1",1)) 
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(type,"0"))
			RETURN_WIFIMGR_FAIL_MSG("Set the ReloadFlag failed.\n");
	
	if(WIFI_2_4G == type ) {	
		if( ECNT_WIFIMGR_SUCCESS == update_datafile_from_shm(type, count)) {		

		#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
					memset(cmd_down,0,MAX_KEYWORD_LEN);
					sprintf(cmd_down,"ifconfig wds%d down", i);
					system(cmd_down);
					sprintf(cmd_down,"brctl delif %s wds%d",BR_NAME, i);
					system(cmd_down);
					LIB_WIFI_DEBUG("\n=== wds%d has been down ===\n", i); 
				}
         #endif 

		#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			 for(i = 0; i < MAX_APCLIENT_NUM; i++){
				 memset(cmd_down, 0, sizeof(cmd_down));
				 snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_2G_NAME_FORMAT " down",i);
				 system(cmd_down);	 
				 memset(cmd_down, 0, sizeof(cmd_down));
				 snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_2G_NAME_FORMAT,BR_NAME,i);
				 system(cmd_down);
				 LIB_WIFI_DEBUG("\n=== apcli%d has been down ===\n", i); 
			 }				 
		#endif


		for(i = 0; i <  pre_wlan_state.wlan_Bssid_num; ++i ) {
				memset(cmd_down,0,sizeof(cmd_down));
				snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
				system(cmd_down);
				snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s ra%d",BR_NAME, i);
				system(cmd_down);
			}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID]){			
			for(i = 0; i <	pre_wlan_state.wlan11ac_Bssid_num; ++i ) {
					memset(cmd_down,0,sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "ifconfig rai%d down", i);
					system(cmd_down);
					snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s rai%d",BR_NAME, i);
					system(cmd_down);
				}
		}
#endif

		/*need to reload .ko*/	
		if(0 == strncmp(output,"1",1)) {

			#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) && defined(TCSUPPORT_WLAN_MT7615D)
				system("reboot");			
			#else
				if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag,sizeof(flag))) {
				/*MT7603*/
					if('1' == flag[0]) {	
					    system("rmmod mt7603eap.ko");
					    system(INSMOD_MT7603_MODULE_PATH);
				     }
				/*MT7615E*/
					#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN)
					#else
					else if('1' == flag[2]) {	
						system("rmmod mt7615_ap");
						system(INSMOD_MT7615_MODULE_PATH);	
					 }		
					#endif
				}
			#endif
		}


		for(i = 0; i <  count; ++i ) {
			if(wifimgr_lib_get_enableSsid(0, i+1, outbuffer, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE){
				if(atoi(outbuffer)==1){
					memset(cmd_up,0,sizeof(cmd_up));
					snprintf(cmd_up, sizeof(cmd_up), "ifconfig ra%d up", i);
					system(cmd_up);
					snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s ra%d",BR_NAME, i);
					system(cmd_up);
				}
			}
		}		
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID]){
			for(i = 0; i <	pre_wlan_state.wlan11ac_Bssid_num; ++i ) {
				if(wifimgr_lib_get_enableSsid(1, i+1, outbuffer, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE){
					if(atoi(outbuffer)==1){
						memset(cmd_up,0,sizeof(cmd_up));
						snprintf(cmd_up, sizeof(cmd_up), "ifconfig rai%d up", i);
						system(cmd_up);
						snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s rai%d",BR_NAME, i);
						system(cmd_up);
					}
				}
			}
		}
#endif
		#ifdef TCSUPPORT_WLAN_WDS
			if (0 == strncmp(wdsenable,"1",1))
			  for(i = 0;i<MAX_WDS_ENTRY;i++){
				memset(cmd_up,0,MAX_KEYWORD_LEN);
				sprintf(cmd_up,"ifconfig wds%d up", i);
				system(cmd_up);
				sprintf(cmd_up,"brctl addif %s wds%d",BR_NAME, i);
			    system(cmd_up);
			}
	    #endif

  }		
  else RETURN_WIFIMGR_FAIL_MSG("WIFI_2_4G updtate datefile from shm failed.\n ");
 }
 else if (WIFI_5G == type) {
		if( ECNT_WIFIMGR_SUCCESS == update_datafile_from_shm(type, count))
		  {
            #ifdef TCSUPPORT_WLAN_WDS
				for(i = 0; i < MAX_WDS_ENTRY; ++i ) {
						memset(cmd_down,0,MAX_KEYWORD_LEN);
						sprintf(cmd_down,"ifconfig wdsi%d down", i);
						system(cmd_down);
						sprintf(cmd_down,"brctl delif %s wdsi%d",BR_NAME, i);
						system(cmd_down);
						LIB_WIFI_DEBUG("\n=== wdsi%d has been down ===\n", i); 
					}
            #endif 
			#ifdef TCSUPPORT_ECNT_MAP_ENHANCE
			 for(i = 0; i < MAX_APCLIENT_NUM; i++){
				 memset(cmd_down, 0, sizeof(cmd_down));
				 snprintf(cmd_down,sizeof(cmd_down),"ifconfig " M_APCLIENT_5G_NAME_FORMAT " down",i);
				 system(cmd_down);	 
				 memset(cmd_down, 0, sizeof(cmd_down));
				 snprintf(cmd_down,sizeof(cmd_down),"brctl delif %s " M_APCLIENT_5G_NAME_FORMAT,BR_NAME,i);
				 system(cmd_down);
				 LIB_WIFI_DEBUG("\n=== apclii%d has been down ===\n", i); 
			 }				 
			#endif

					
			for(i = 0; i < pre_wlan_state.wlan11ac_Bssid_num; ++i ) {
					memset(cmd_down,0,MAX_KEYWORD_LEN);
					sprintf(cmd_down,"ifconfig rai%d down", i);
					system(cmd_down);
					sprintf(cmd_down,"brctl delif %s rai%d",BR_NAME, i);
					system(cmd_down);
				}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP			
			if(WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]){
				for(i = 0; i <  pre_wlan_state.wlan_Bssid_num; ++i ) {
					memset(cmd_down,0,sizeof(cmd_down));
					snprintf(cmd_down, sizeof(cmd_down), "ifconfig ra%d down", i);
					system(cmd_down);
					snprintf(cmd_down, sizeof(cmd_down), "brctl delif %s ra%d",BR_NAME, i);
					system(cmd_down);
				}
			}
#endif		
			/*check need to rmmod and insmod .ko or not*/
			/* need to reload ko */
			if(0 == strncmp(output,"1",1)) {
				LIB_WIFI_DEBUG("\n===  reloadFlag ==1 ===\n"); 

				#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) && defined(TCSUPPORT_WLAN_MT7615D)
				system("reboot");
				#elif defined(TCSUPPORT_WLAN_MT7612_BUILD_IN)
				/* when 7612 wifi driver build in , do not need reload ko*/
				#else
					if(ECNT_WIFIMGR_SUCCESS == parse_driver(flag,sizeof(flag))) {
						/*MT7612*/					
						if('1' == flag[1]) {
							system("rmmod mt7662e_ap");
							system(INSMOD_MT7612_MODULE_PATH);
						}
						/*MT7613*/
						else if('1' == flag[3]) {
							system("rmmod mt7663_ap");
							system(INSMOD_MT7613_MODULE_PATH);	
						}	
						
						/*MT7615*/
						#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN)
						#else
						else if('1' == flag[2]) {
							system("rmmod mt7615_ap");
							system(INSMOD_MT7615_MODULE_PATH);	
						}		
						#endif		
					LIB_WIFI_DEBUG("\n\n\n===  reload ko success  ===\n\n\n"); 
				}
				#endif
			}

#ifdef TCSUPPORT_WLAN_MULTI_CHIP	
			if(WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]){
				for(i = 0; i <	pre_wlan_state.wlan_Bssid_num; ++i ) {
					if(wifimgr_lib_get_enableSsid(0, i+1, outbuffer, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE){
						if(atoi(outbuffer)==1){
							memset(cmd_up,0,sizeof(cmd_up));
							snprintf(cmd_up, sizeof(cmd_up), "ifconfig ra%d up", i);
							system(cmd_up);
							snprintf(cmd_up, sizeof(cmd_up), "brctl addif %s ra%d",BR_NAME, i);
							system(cmd_up);
						}
					}
				}
			}	
#endif	

			for(i = 0; i < count; ++i ){
				if(wifimgr_lib_get_enableSsid(1, i+1, outbuffer, MAX_KEYWORD_LEN) != ECNT_WIFIMGR_FAILURE){
					if(atoi(outbuffer)==1){		
				sprintf(cmd_up,"ifconfig rai%d up", i);
				system(cmd_up);
				sprintf(cmd_up,"brctl addif %s rai%d",BR_NAME, i);
				system(cmd_up);
			}
				}
			}
		  #ifdef TCSUPPORT_WLAN_WDS
			if (0 == strncmp(wdsenable,"1",1))
			  for(i = 0;i<MAX_WDS_ENTRY;i++){
				memset(cmd_up,0,MAX_KEYWORD_LEN);
				sprintf(cmd_up,"ifconfig wdsi%d up", i);
				system(cmd_up);
				sprintf(cmd_up,"brctl addif %s wdsi%d", BR_NAME,i);
			    system(cmd_up);
			}
		  #endif
		
		}
	  else RETURN_WIFIMGR_FAIL_MSG("WIFI_5G updtate datefile from shm failed.\n ");
	}
	else RETURN_WIFIMGR_FAIL_MSG("type should be 2.4G or 5G .\n ");

	RETURN_WIFIMGR_SUCCESS;
}

#if 0
int 
wifimgr_lib_exit(void)
{
	WIFI_FUNC_TRACE_ENTER;
	if(ECNT_WIFIMGR_SUCCESS != free_shm())
	{
		RETURN_WIFIMGR_FAIL_MSG("Fail to free the share memory.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}
#endif

#if defined(TCSUPPORT_WLAN_APCLIENT)
int wifimgr_lib_get_apcli_status(wifi_type type)
{
	int ret = 0, connStatus = 0;
	char data[512] = {0};
	char ifname[16] = {0};
	
	memset(data, 0, sizeof(data));
	if( type )
		snprintf(ifname, sizeof(ifname), "apclii0");
	else
		snprintf(ifname, sizeof(ifname), "apcli0");
	
	ret = wifimgr_lib_ioctl(RTPRIV_IOCTL_GET_APCLI_STATUS, data, ifname, sizeof(data));
	
	if( strcmp(data, "Disconnect") )
		connStatus = 1;
	
	return connStatus;
}

int wifimgr_lib_set_SiteSurvery(wifi_type type, char *SSID)
{
	int ret = 0;
	char data[512] = {0};
	char ifname[16] = {0};
	
	memset(data, 0, sizeof(data));
	snprintf(data, sizeof(data), "SiteSurvey=%s", SSID);
	
	if( type )
		snprintf(ifname, sizeof(ifname), "apclii0");
	else
		snprintf(ifname, sizeof(ifname), "apcli0");
	
	ret = wifimgr_lib_ioctl(RTPRIV_IOCTL_SET, data, ifname, sizeof(data));
	
	return ret; 
}

int wifimgr_lib_get_SiteSurvery(wifi_type type, char *SSID, char *data, int len) 
{	
	int ret = 0;
	char ifname[16] = {0};
	
	if( type )
		snprintf(ifname, sizeof(ifname), "apclii0");
	else
		snprintf(ifname, sizeof(ifname), "apcli0");

	ret = wifimgr_lib_ioctl(RTPRIV_IOCTL_GSITESURVEY, data, ifname, len);  /* length should less than 32 when need get assign SSID info*/
	
	return ret;
}

int wifimgr_lib_get_Statistics(wifi_type type, char *data, int len) 
{	
	int ret = 0;
	char ifname[16] = {0};
	
	if( type )
		snprintf(ifname, sizeof(ifname), "apclii0");
	else
		snprintf(ifname, sizeof(ifname), "apcli0");

	ret = wifimgr_lib_ioctl(RTPRIV_IOCTL_STATISTICS, data, ifname, len);
	
	return ret;
}
#endif

int 
wdskey_check(int wdskeylen, char* input)
{
	int i;
	char keychar;
	
   	if (wdskeylen < 8) {
		return -1;
	} else if (64 == wdskeylen) {
		for (i = 0; i < 64; i++) {
			keychar = *(input+i);
			if (doHexCheck(keychar) < 0) {
				return -1;
			}
		}
	} else if (wdskeylen > 64) {
		return -1;
	}	
	return 0;
}

int 
wdskey_check_for_wep(int wdskeylen, char* input)
{
	int i;
	char keychar;
	
   	if ((10 == wdskeylen) || (26 == wdskeylen)) {
    	for (i = 0; i < wdskeylen; i++) {
			keychar = *(input+i);
			if (doHexCheck(keychar) < 0) {
				return -1;
			}
		}
	} else if ((5 != wdskeylen) && (13 != wdskeylen)) {
		return -1;
	} 
	return 0;
}

int 
doStrHexCheck(char* input)
{
	char* temp = input;
	if(temp == NULL)
		return 0;

	while(*temp != '\0'){
		if( (doHexCheck(*temp++)) == -1 )
			return 0;
	}
	return 1;
	
}


int 
doHexCheck(char ch)
{
  if ((ch >= '0') && (ch <= '9')) {
  	return 1;
  }
  else if ((ch >= 'A') && (ch <= 'F')) {
    return 1;
  }
  else if ((ch >= 'a') && (ch <= 'f')) {
    return 1;
  }
  return -1;
}

int 
doStrDecCheck(char *input)
{
	char *tmp = input;
	if(NULL == tmp)
		return 0;

	while(*tmp != '\0'){
		if( !(*tmp <= '9' && *tmp++ >= '0') )
			return 0;
	}

	return 1;
}

int
checkV4(char* p)
{
    char* tmp = p; 
	int val;
	if(NULL == tmp)
		return 0;
	else
	{
		while( *tmp != '\0'){
			if( !(*tmp <= '9' && *tmp++ >= '0') )
				return 0;
		}
	}
	val = atoi(p);
	return (int)(0 <= val && val <= 255);	
}

int IPV4check(char *input)
{
	int   IPcont = 0, IPflag = 0;
	char  IPstr[4][4], bak[16];

    if(input == NULL || strlen(input)>16)
		return 0;
	strncpy(bak, input, sizeof(bak) - 1);

	IPcont = charCnt(bak, '.');
	IPflag = sscanf(bak, "%[^.].%[^.].%[^.].%[^.]", IPstr[0], IPstr[1], IPstr[2], IPstr[3]);
	if(IPcont != 3 || IPflag != 4 || !(checkV4(IPstr[0]) && checkV4(IPstr[1]) && checkV4(IPstr[2]) && checkV4(IPstr[3])))
		return 0;
	
	return 1;
}

int charCnt(char* input, char character)
{
	char* temp = input;
	int cnt = 0;
	if(input == NULL)
		return 0;
	while(*temp != '\0'){
		if(*temp++ == character)
			cnt++;
	}
	return cnt;
	
}

int MacCheck(char* input)
{
	int   IPcont = 0, IPflag = 0, i;
	char  IPstr[6][18] = {0}, bak[20] = {0};
	/*char* temp;*/
	unsigned char IPhex[6] = {0};

    if(input == NULL || strlen(input) != 17)
		return 0;
	/*temp = strncpy(bak, input, sizeof(bak) - 1);*/

	IPcont = charCnt(bak, ':');
	IPflag = sscanf(bak, "%[^:]:%[^:]:%[^:]:%[^:]:%[^:]:%[^:]", IPstr[0], IPstr[1], IPstr[2], IPstr[3], IPstr[4], IPstr[5]);
	if(IPcont != 5 || IPflag != 6)
		return 0;
	
	for(i = 0; i < 6; i++){
	    if(doStrHexCheck(IPstr[i]) == 0 || strlen(IPstr[i]) != 2)
			return 0;
		IPhex[i] = (hex2digital(IPstr[i][0]) << 4) | hex2digital(IPstr[i][1]);
	}
	
	return is_valid_ether_addr(IPhex);	
}

/**
 * This function is extract encrypType from input to output
 * for example, input is AES;TKIP;TKIP;AES;AES;TKIP;TKIP;AES;AES;TKIP;TKIP;AES;AES;TKIP;TKIP;AES
 * output is AES;TKIP;TKIP;AES
**/
char * strextract(char *output, char * input)
{
   char * input_tmp = input;
   char * output_tmp = output;
   int i = 0;
   int index = 0;
   
   for(i = 0; i < MAX_WDS_ENTRY; i++){
   	while(*input_tmp != ';') {
	  output_tmp[index++] = *input_tmp++ ;
    }
	output_tmp[index++] = ';';
    input_tmp++;
   }
   output_tmp[index-1]='\0';

   return output;
}
	

/**
 * rstrtok - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 * * WARNING: strtok is deprecated, use strsep instead. However strsep is not compatible with old architecture.
 */
char * __rstrtok;
char * rstrtok(char * s,const char * ct)
{
	char *sbegin, *send;

	sbegin  = s ? s : __rstrtok;
	if (!sbegin)
	{
		return NULL;
	}

	sbegin += strspn(sbegin,ct);
	if (*sbegin == '\0')
	{
		__rstrtok = NULL;
		return( NULL );
	}

	send = strpbrk( sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';

	__rstrtok = send;

	return (sbegin);
}

#define ASC_LOWER(_x)	((((_x) >= 0x41) && ((_x) <= 0x5a)) ? (_x) + 0x20 : (_x))
/* we assume the s1 and s2 both are strings.*/
char 
rtstrcasecmp(char *s1, char *s2)
{
	char *p1 = s1, *p2 = s2;
	char c1, c2;
	
	if (strlen(s1) != strlen(s2))
		return 0;
	
	while(*p1 != '\0')
	{
		c1 = ASC_LOWER(*p1);
		c2 = ASC_LOWER(*p2);
		if(c1 != c2)
			return 0;
		p1++;
		p2++;
	}
	
	return 1;
}


 

void 
UpdateChannel(char* outchannel)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultchannel=6;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateChannel: Create socket fail\n");
		sprintf(outchannel,"%d",defaultchannel);
		return;
	}

	strncpy(wrq.ifr_name, "ra0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = 0;
       ret=ioctl(socket_id, SIOCGIWFREQ , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateChannel: ioctl open fail\n");
    		close(socket_id);
		sprintf(outchannel,"%d",defaultchannel);
		return;
	}
	sprintf(outchannel,"%d",wrq.u.freq.m);
	close(socket_id);
	return;

}

#ifdef WSC_AP_SUPPORT
void 
UpdatePinCode(char* outPinCode)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultPinCode=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdatePinCode: Create socket fail\n");
		sprintf(outPinCode,"%d",defaultPinCode);
		return;
	}

	strncpy(wrq.ifr_name, "ra0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdatePinCode: ioctl open fail\n");
    		close(socket_id);
		sprintf(outPinCode,"%d",defaultPinCode);
		return;
	}
	sprintf(outPinCode,"%08u",*((unsigned int *)(data)));
	close(socket_id);
	return;
	
}

#ifdef TCSUPPORT_WLAN_MULTI_WPS

void 
UpdateWpsStatus(int* outWpsStatus, char*outWpsMac, int i)
{
	struct iwreq wrq;
	int ret=0;
	char data[20];
	WSC_PEER_DEV_INFO datainfo = {0};
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateWpsStatus: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return;
	}

	sprintf(wrq.ifr_name, "ra%d",i);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateWpsStatus: ioctl open fail\n");
    		close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return;
	}
	*outWpsStatus=*((int *)data);

	/* get the mac when in progress or configured*/
	if(*outWpsStatus > 2){
		wrq.u.data.length = sizeof(datainfo);
		wrq.u.data.pointer = &datainfo;
		wrq.u.data.flags = RT_OID_WSC_QUERY_PEER_INFO_ON_RUNNING;
		ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

		if ( ret != 0 ) {
			printf("==>UpdateWpsMac: ioctl open fail:%d\n",ret);
	    	close(socket_id);
			outWpsMac[0] = '\0';
			return ;
		}
		sprintf(data,"%02X:%02X:%02X:%02X:%02X:%02X",
			datainfo.WscPeerMAC[0],datainfo.WscPeerMAC[1],datainfo.WscPeerMAC[2],datainfo.WscPeerMAC[3],datainfo.WscPeerMAC[4],datainfo.WscPeerMAC[5]);
		memcpy(outWpsMac,data, 18);
	}
	close(socket_id);
	return;
}
#else
void 
UpdateWpsStatus(int* outWpsStatus)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateWpsStatus: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		return;
	}

	strncpy(wrq.ifr_name, "ra0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateWpsStatus: ioctl open fail\n");
    		close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		return;
	}
	*outWpsStatus=*((int *)data);

	close(socket_id);
	return;
	
}

#endif
#endif

#if defined(TCSUPPORT_WLAN_AC)
void UpdateChannelAC(char * outchannel){

	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultchannel=6;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateChannelAC: Create socket fail\n");
		sprintf(outchannel,"%d",defaultchannel);
		return;
	}

	strncpy(wrq.ifr_name, "rai0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = 0;
       ret=ioctl(socket_id, SIOCGIWFREQ , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateChannelAC: ioctl open fail\n");
    		close(socket_id);
		sprintf(outchannel,"%d",defaultchannel);
		return;
	}
	sprintf(outchannel,"%d",wrq.u.freq.m);
	close(socket_id);
	return;

}
#ifdef WSC_AP_SUPPORT
void UpdatePinCodeAC(char * outPinCode)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultPinCode=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdatePinCodeAC: Create socket fail\n");
		sprintf(outPinCode,"%d",defaultPinCode);
		return;
	}

	strncpy(wrq.ifr_name, "rai0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdatePinCodeAC: ioctl open fail\n");
    		close(socket_id);
		sprintf(outPinCode,"%d",defaultPinCode);
		return;
	}
	sprintf(outPinCode,"%08u",*((unsigned int *)(data)));
	close(socket_id);
	return;
	
}

#ifdef TCSUPPORT_WLAN_MULTI_WPS
void UpdateWpsStatusAC(int * outWpsStatus, char *outWpsMac, int i)
{
	struct iwreq wrq;
	int ret=0;
	char data[20];
	WSC_PEER_DEV_INFO datainfo = {0};
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(socket_id==-1){
		printf("==>UpdateWpsStatus: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return;
	}

	sprintf(wrq.ifr_name, "rai%d",i);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);
	
	if ( ret != 0 ) {
		printf("==>UpdateWpsStatus: ioctl open fail\n");
    	close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return;
	}
	*outWpsStatus=*((int *)data);

	/* get the mac when in progress or configured*/
	if(*outWpsStatus > 2){
		wrq.u.data.length = sizeof(datainfo);
		wrq.u.data.pointer = &datainfo;
		wrq.u.data.flags = RT_OID_WSC_QUERY_PEER_INFO_ON_RUNNING;
		ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

		if ( ret != 0 ) {
			printf("==>UpdateWpsMac: ioctl open fail:%d\n",ret);
	    	close(socket_id);
			outWpsMac[0] = '\0';
			return ;
		}
		
		sprintf(data,"%02X:%02X:%02X:%02X:%02X:%02X",
			datainfo.WscPeerMAC[0],datainfo.WscPeerMAC[1],datainfo.WscPeerMAC[2],datainfo.WscPeerMAC[3],datainfo.WscPeerMAC[4],datainfo.WscPeerMAC[5]);
		memcpy(outWpsMac,data, 18);
	}
	close(socket_id);
	return;
}
#else
void UpdateWpsStatusAC(int * outWpsStatus)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateWpsStatusAC: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		return;
	}

	strncpy(wrq.ifr_name, "rai0", sizeof(wrq.ifr_name) - 1);
       wrq.u.data.length = sizeof(data);
       wrq.u.data.pointer = data;
       wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
       ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateWpsStatusAC: ioctl open fail\n");
    		close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		return;
	}
	*outWpsStatus=*((int *)data);

	close(socket_id);
	return;
	
}
#endif
#endif /* WSC_AP_SUPPORT */
#endif /* TCSUPPORT_WLAN_AC */

int getAPCount(int rfband, RT_AP_COUNT *apcnt_out)
{
	struct iwreq wrq;
	char *data = NULL;
	RT_AP_COUNT *apcnt = NULL;
	int socket_id = 0, ret = 0;

	/* alloc data */
	data = (char*)malloc(1024);
	if (data == NULL)
		return -1;
	memset(data, 0, sizeof(1024));

	/* init wrq structure */
	wrq.u.data.length = 1024;
 	wrq.u.data.pointer = data;
 	wrq.u.data.flags = 0;
	if (rfband == 0) { /* 2.4G */ 
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s", "ra0");
	}
	else {
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s", "rai0");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == socket_id) {
		printf("==>getAPCount(): XXXXXX socket open fail\n");
		free(data);
		return -1;
	}
	ret = ioctl(socket_id, RTPRIV_IOCTL_AP_COUNT, &wrq);
	if (ret != 0) {
		printf("==>getAPCount(): XXXXXX ioctl open fail\n");
		free(data);
    	close(socket_id);
		return -1;
	}
	apcnt = (RT_AP_COUNT *)wrq.u.data.pointer;

	/* copy out */
	memcpy(apcnt_out, apcnt, sizeof(RT_AP_COUNT));

	apcnt_out->rxDropPkts = 0;
	apcnt_out->txDropPkts = 0;
	
	free(data);
	close(socket_id);

	return 0;
}

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
void getMacEntryByIndex(mac_table_t *pMacTable, int index,int maxnum)
{
	int bindinfo = 0;
	int chipindex = 0;
		if(pMacTable == NULL || (index > (MAX_DUALBAND_BSSID_NUM - 1) || index < 0)){
		return;
		}
		struct iwreq wrq;
		RT_802_11_MAC_TABLE *mp = NULL;
		int i, j, ret, socket_id;
		char *data = NULL;
		int tmpindex;
		
		data = (char*)malloc(4096);
		if (data == NULL)
			return;
		memset(data, 0, 4096);
		if(index < MAX_ECNT_WALN_PORT_NUM && index >= 0){
			snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s%d", "ra", index);
			bindinfo = 0;
		}
		else if(index < MAX_DUALBAND_BSSID_NUM && index >= MAX_ECNT_WALN_PORT_NUM){
			snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s%d", "rai", index-MAX_ECNT_WALN_PORT_NUM);
			bindinfo = 1;
		}
		wrq.u.data.length = sizeof(data);
		wrq.u.data.pointer = data;
		wrq.u.data.flags = 0;
	
		socket_id = socket(AF_INET, SOCK_DGRAM, 0);
		if (-1 == socket_id) {
			printf( "==>getMacEntryByIndex(): create socket fail.\n" );
			free(data);
			return;
		}
		ret = ioctl(socket_id, RTPRIV_IOCTL_GET_MAC_TABLE, &wrq);
		if (ret != 0) {
			printf("==>getMacEntryByIndex(): XXXXXX ioctl open fail\n");
			free(data);
			close(socket_id);
			return;
		}
		mp = (RT_802_11_MAC_TABLE *)wrq.u.data.pointer;
		//num is same,using one table to get num
		if (mp->table_7592.Num == 0){
			printf("==>getMacEntryByIndex(): no sta existance\n");
			free(data);
			close(socket_id);	
			return;
		}

		for (i = 0, j = 0; j < MAX_LEN_OF_MAC_TABLE; j++){
			switch(bindinfo)
			{
				case WIFI_2_4G:
					if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])
					{
						if (index == (int)mp->table_7615.Entry[j].ApIdx)
						{
							if (mp->table_7615.Entry[j].Aid == 0)
							{	
							 	break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7615.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
						pMacTable->macEntry[i].AvgRssi0 = mp->table_7615.Entry[j].AvgRssi0;
						pMacTable->macEntry[i].AvgRssi1 = mp->table_7615.Entry[j].AvgRssi1;
						pMacTable->macEntry[i].AvgRssi2 = mp->table_7615.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)	
						pMacTable->macEntry[i].AvgRssi = mp->table_7615.Entry[j].AvgRssi;
						pMacTable->macEntry[i].DataRate = mp->table_7615.Entry[j].DataRate;
						pMacTable->macEntry[i].PortSecured = mp->table_7615.Entry[j].PortSecured;
#endif
#endif
							if(chipflag2g)
								i++;
						}
					}

					if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID])
					{	
						if (index == (int)mp->table_7915.Entry[j].ApIdx)
						{
							if (mp->table_7915.Entry[j].Aid == 0)
							{	
								 break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7915.Entry[j].Addr, MAC_ADDR_LENGTH);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
							pMacTable->macEntry[i].AvgRssi0 = mp->table_7915.Entry[j].AvgRssi0;
							pMacTable->macEntry[i].AvgRssi1 = mp->table_7915.Entry[j].AvgRssi1;
							pMacTable->macEntry[i].AvgRssi2 = mp->table_7915.Entry[j].AvgRssi2;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2) || 	defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
							pMacTable->macEntry[i].AvgRssi = mp->table_7915.Entry[j].AvgRssi;
							pMacTable->macEntry[i].DataRate = mp->table_7915.Entry[j].DataRate;
							pMacTable->macEntry[i].PortSecured = mp->table_7915.Entry[j].PortSecured;
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CT_UBUS)
							pMacTable->macEntry[i].EntryType =	mp->table_7915.Entry[j].EntryType;
#endif
							if(chipflag2g)
								i++;
						}
					}

					if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
					{
						if (index == (int)mp->table_7592.Entry[j].ApIdx)
						{
							if (mp->table_7592.Entry[j].Aid == 0)
							{	
								break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7592.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
							pMacTable->macEntry[i].AvgRssi0 = mp->table_7592.Entry[j].AvgRssi0;
							pMacTable->macEntry[i].AvgRssi1 = mp->table_7592.Entry[j].AvgRssi1;
							pMacTable->macEntry[i].AvgRssi2 = mp->table_7592.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
							pMacTable->macEntry[i].AvgRssi = mp->table_7592.Entry[j].AvgRssi;
							pMacTable->macEntry[i].DataRate = mp->table_7592.Entry[j].DataRate;
							pMacTable->macEntry[i].PortSecured = mp->table_7592.Entry[j].PortSecured;
#endif
#endif
							if(chipflag2g)
								i++;
						}
					}

				break;

				case WIFI_5G:
					if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
					{
						if ( (index - MAX_ECNT_WALN_PORT_NUM )== (int)mp->table_7615.Entry[j].ApIdx ) {
							if ( mp->table_7615.Entry[j].Aid == 0 ) {	
								 break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7615.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
							pMacTable->macEntry[i].AvgRssi0 = mp->table_7615.Entry[j].AvgRssi0;
							pMacTable->macEntry[i].AvgRssi1 = mp->table_7615.Entry[j].AvgRssi1;
							pMacTable->macEntry[i].AvgRssi2 = mp->table_7615.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2)	 || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)		
							pMacTable->macEntry[i].AvgRssi = mp->table_7615.Entry[j].AvgRssi;
							pMacTable->macEntry[i].DataRate = mp->table_7615.Entry[j].DataRate;
							pMacTable->macEntry[i].PortSecured = mp->table_7615.Entry[j].PortSecured;
#endif
#endif
							if(chipflag5g)
								i++;
						}
					}
					
					if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]  || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
					{
						if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
						{
							/*because 7915d 5g index according to 2.4g bssidnum in driver,but the app use MAX_ECNT_WALN_PORT_NUM,so need mapping here*/
							tmpindex = index - MAX_ECNT_WALN_PORT_NUM + maxnum;
						}
						else
						{
							tmpindex = index - MAX_ECNT_WALN_PORT_NUM;
						}

						if (tmpindex == (int)mp->table_7915.Entry[j].ApIdx ) {
							if ( mp->table_7915.Entry[j].Aid == 0 ) {	
								 break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7915.Entry[j].Addr, MAC_ADDR_LENGTH);
							#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
							pMacTable->macEntry[i].AvgRssi0 = mp->table_7915.Entry[j].AvgRssi0;
							pMacTable->macEntry[i].AvgRssi1 = mp->table_7915.Entry[j].AvgRssi1;
							pMacTable->macEntry[i].AvgRssi2 = mp->table_7915.Entry[j].AvgRssi2;
							#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2)	 || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)		
							pMacTable->macEntry[i].AvgRssi = mp->table_7915.Entry[j].AvgRssi;
							pMacTable->macEntry[i].DataRate = mp->table_7915.Entry[j].DataRate;
							pMacTable->macEntry[i].PortSecured = mp->table_7915.Entry[j].PortSecured;
							#endif/*TCSUPPORT_COMPILE*/
							#endif/*TCSUPPORT_COMPILE*/			
							#if defined(TCSUPPORT_CT_UBUS)
							pMacTable->macEntry[i].EntryType =	mp->table_7915.Entry[j].EntryType;
							#endif
							if(chipflag5g)
								i++;
						}
					}

					if(WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID])
					{
						if ((index - MAX_ECNT_WALN_PORT_NUM )== (int)mp->table_7613.Entry[j].ApIdx )
						{
							if (mp->table_7613.Entry[j].Aid == 0)
							{	
								break;
							}
							memcpy(pMacTable->macEntry[i].Addr, mp->table_7613.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
						pMacTable->macEntry[i].AvgRssi0 = mp->table_7613.Entry[j].AvgRssi0;
						pMacTable->macEntry[i].AvgRssi1 = mp->table_7613.Entry[j].AvgRssi1;
						pMacTable->macEntry[i].AvgRssi2 = mp->table_7613.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2)	|| defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
						pMacTable->macEntry[i].AvgRssi = mp->table_7613.Entry[j].AvgRssi;
						pMacTable->macEntry[i].DataRate = mp->table_7613.Entry[j].DataRate;
						pMacTable->macEntry[i].PortSecured = mp->table_7613.Entry[j].PortSecured;
#endif
#endif
							if(chipflag5g)
								i++;

						}
					}	
					break;	

					default:
					break;
			}
		}
		
	pMacTable->Num = i;
	free(data);
	close(socket_id);			
	return;
}
#else
void getMacEntryByIndex(mac_table_t *pMacTable, int index,int maxnum)
{
	if(pMacTable == NULL || (index > (MAX_DUALBAND_BSSID_NUM - 1) || index < 0)){
		return;
	}

	struct iwreq wrq;
	RT_802_11_MAC_TABLE *mp = NULL;
	int i, j, ret, socket_id;
	char *data = NULL;
	int tmpindex;
	
	data = (char*)malloc(4096);
	if (data == NULL)
		return;
	memset(data, 0, 4096);
	if(index < MAX_ECNT_WALN_PORT_NUM && index >= 0){
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s%d", "ra", index);
	}
	else if(index < MAX_DUALBAND_BSSID_NUM && index >= MAX_ECNT_WALN_PORT_NUM){
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s%d", "rai", index-MAX_ECNT_WALN_PORT_NUM);
	}
	wrq.u.data.length = sizeof(data);
 	wrq.u.data.pointer = data;
 	wrq.u.data.flags = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == socket_id) {
		printf( "==>getMacEntryByIndex(): create socket fail.\n" );
		free(data);
		return;
	}
	ret = ioctl(socket_id, RTPRIV_IOCTL_GET_MAC_TABLE, &wrq);
	if (ret != 0) {
		printf("==>getMacEntryByIndex(): XXXXXX ioctl open fail\n");
		free(data);
    	close(socket_id);
		return;
	}
	mp = (RT_802_11_MAC_TABLE *)wrq.u.data.pointer;
	if(index < MAX_ECNT_WALN_PORT_NUM && index >= 0){
		#if defined(MT7915D) || defined(MT7915N) || defined(MT7916D) || defined(MT7916N)
		if (mp->table_7915.Num == 0){
			printf("==>getMacEntryByIndex(): 7915 no sta existance\n");
			free(data);
			close(socket_id);	
			return;
		}
		#else
		if (mp->table_7592.Num == 0){
			printf("==>getMacEntryByIndex(): 7592 no sta existance\n");
			free(data);
			close(socket_id);	
			return;
		}
		#endif
	}
	else if(index < MAX_DUALBAND_BSSID_NUM && index >= MAX_ECNT_WALN_PORT_NUM){
		#if defined(MT7915D) || defined(MT7915E) || defined(MT7916D)|| defined(MT7916E)
		if ( mp->table_7915.Num == 0 ){
			printf("==>getMacEntryByIndex(): 7915 no sta existance\n");
			free(data);
			close(socket_id);	
			return;
		}
		#else
		if ( mp->table_7612.Num == 0 ){
			printf("==>getMacEntryByIndex(): 7612 no sta existance\n");
			free(data);
			close(socket_id);	
			return;
		}
		#endif
	}
	if(index <  MAX_ECNT_WALN_PORT_NUM && index >= 0){
		for (i = 0, j = 0; j < MAX_LEN_OF_MAC_TABLE; j++){
#if defined(MT7615E)
			if (index == (int)mp->table_7615.Entry[j].ApIdx){
				if (mp->table_7615.Entry[j].Aid == 0){	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7615.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7615.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7615.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7615.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2) || 	defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)	
				pMacTable->macEntry[i].AvgRssi = mp->table_7615.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7615.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7615.Entry[j].PortSecured;
#endif
#endif
				i++;
			}
#endif
#if defined(MT7915D) || defined(MT7915N) ||defined(MT7916D) || defined(MT7916N)
			if (index == (int)mp->table_7915.Entry[j].ApIdx){
				if (mp->table_7915.Entry[j].Aid == 0){	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7915.Entry[j].Addr, MAC_ADDR_LENGTH);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7915.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7915.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7915.Entry[j].AvgRssi2;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2) || 	defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
				pMacTable->macEntry[i].AvgRssi = mp->table_7915.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7915.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7915.Entry[j].PortSecured;
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CT_UBUS)
				pMacTable->macEntry[i].EntryType =  mp->table_7915.Entry[j].EntryType;
#endif
				i++;
			}
#endif
#if defined(MT7592)
			if (index == (int)mp->table_7592.Entry[j].ApIdx){
#else
			if ( index == (int)mp->table_7592.Entry[j].Psm ) {
#endif
				if (mp->table_7592.Entry[j].Aid == 0){	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7592.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7592.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7592.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7592.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2) || 	defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi = mp->table_7592.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7592.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7592.Entry[j].PortSecured;
#endif
#endif	
#if defined(TCSUPPORT_CT_UBUS)
				pMacTable->macEntry[i].EntryType =  mp->table_7592.Entry[j].EntryType;
#endif
			i++;
		}
		}
	}
	else if(index < MAX_DUALBAND_BSSID_NUM && index >= MAX_ECNT_WALN_PORT_NUM){
		for (i = 0, j = 0; j < MAX_LEN_OF_MAC_TABLE; j++){
#if defined(MT7612E) || defined(MT7613E)
			if ((index - MAX_ECNT_WALN_PORT_NUM )== (int)mp->table_7612.Entry[j].ApIdx 
#ifdef TCSUPPORT_CMCC_WLAN_FORCEROAM
				 || (16 == (int)mp->table_7612.Entry[j].ApIdx)
#endif
			){
				if (mp->table_7612.Entry[j].Aid == 0){	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7612.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7612.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7612.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7612.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2)	|| defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi = mp->table_7612.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7612.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7612.Entry[j].PortSecured;
#endif
#endif
#if defined(TCSUPPORT_CT_UBUS)
				pMacTable->macEntry[i].EntryType =  mp->table_7612.Entry[j].EntryType;
#endif
				i++;
			}
		
#endif
#if defined(MT7915D) || defined(MT7915E) || defined(MT7916D) || defined(MT7916E)
#if defined(MT7915D) || defined(MT7916D)
			/*because 7915d 5g index according to 2.4g bssidnum in driver,but the app use MAX_ECNT_WALN_PORT_NUM,so need mapping here*/
			tmpindex = index - MAX_ECNT_WALN_PORT_NUM + maxnum;
#else
			tmpindex = index - MAX_ECNT_WALN_PORT_NUM;
#endif
			if (tmpindex == (int)mp->table_7915.Entry[j].ApIdx ) {
				if ( mp->table_7915.Entry[j].Aid == 0 ) {	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7915.Entry[j].Addr, MAC_ADDR_LENGTH);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7915.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7915.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7915.Entry[j].AvgRssi2;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2)	 || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)		
				pMacTable->macEntry[i].AvgRssi = mp->table_7915.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7915.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7915.Entry[j].PortSecured;
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/			
#if defined(TCSUPPORT_CT_UBUS)
				pMacTable->macEntry[i].EntryType =	mp->table_7915.Entry[j].EntryType;
#endif
				i++;
			}
#endif
#if defined(MT7615E)
			if ( (index - MAX_ECNT_WALN_PORT_NUM )== (int)mp->table_7615.Entry[j].ApIdx ) {
				if ( mp->table_7615.Entry[j].Aid == 0 ) {	
					 break;
				}
				memcpy(pMacTable->macEntry[i].Addr, mp->table_7615.Entry[j].Addr, MAC_ADDR_LENGTH);
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
				pMacTable->macEntry[i].AvgRssi0 = mp->table_7615.Entry[j].AvgRssi0;
				pMacTable->macEntry[i].AvgRssi1 = mp->table_7615.Entry[j].AvgRssi1;
				pMacTable->macEntry[i].AvgRssi2 = mp->table_7615.Entry[j].AvgRssi2;
#if defined(TCSUPPORT_CMCCV2)	 || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)		
				pMacTable->macEntry[i].AvgRssi = mp->table_7615.Entry[j].AvgRssi;
				pMacTable->macEntry[i].DataRate = mp->table_7615.Entry[j].DataRate;
				pMacTable->macEntry[i].PortSecured = mp->table_7615.Entry[j].PortSecured;
#endif
#endif
				i++;
			}
#endif
		}	
	}
	pMacTable->Num = i;
	free(data);
	close(socket_id);
	
	return;
}
#endif

void reload_WlanAC_Driver(void)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID])
	{
		system("rmmod mt7663_ap");
		system(INSMOD_MT7613_MODULE_PATH);
	}
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
	{
		system("rmmod mt7615_ap");
		system(INSMOD_MT7615_MODULE_PATH);
	}
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
	{
		system("rmmod mt7915_ap");
		system(INSMOD_MT7915A_MODULE_PATH);
	}
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID])
	{
		system("rmmod mt7915_ap");
		system(INSMOD_MT7915D_MODULE_PATH);
	}
	else if(WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		system("rmmod mt7916_ap");
		system(INSMOD_MT7916D_MODULE_PATH);
	}
	else
	{
		WIFIMGR_FAIL_MSG("No support!\n");
	}
#else
#ifdef MT7612E
	system("rmmod mt7662e_ap");
	system(INSMOD_MT7612_MODULE_PATH);
#elif MT7613E
	system("rmmod mt7663_ap");
	system(INSMOD_MT7613_MODULE_PATH);
#elif MT7615E
	system("rmmod mt7615_ap");
	system(INSMOD_MT7615_MODULE_PATH);
#elif MT7915E
	system("rmmod mt7915_ap");
	system("insmod /lib/modules/mt7915_ap.ko");
#elif MT7916E
	system("rmmod mt7916_ap");
	system("insmod /lib/modules/mt7916_ap.ko");
#endif
#endif
	return;
}

int
wifimgr_lib_set_EAPifname(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	sprintf (keyword, "EAPifname");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_PreAuthifname(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( -1 != index )
		RETURN_WIFIMGR_FAIL_MSG("The ssid index here should be -1.\n");
	
	sprintf (keyword, "PreAuthifname");
	
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

static void specialCharacterHandle(char *oriStr)
{
	int i;
	int j = 0;
	char handle[160] = {0};
	for(i = 0;i < strlen(oriStr);i++)
	{
		if(oriStr[i] == '"')
		{
			handle[j++]='\\';
			handle[j++]='"';
		}
		else if(oriStr[i] == '\\')
		{
			handle[j++]='\\';
			handle[j++]='\\';
		}
#if 0
		else if(oriStr[i] == '\$')
		{
			handle[j++]='\\';
			handle[j++]='\$';
		}
		else if(oriStr[i] == '\`')
		{
			handle[j++]='\\';
			handle[j++]='\`';
		}
#endif
		else
		{
			handle[j] = oriStr[i];
			j++;
		}
	}
	handle[j] = '\0';
	strcpy(oriStr,handle);

	return;
}

/**
	delete /r /n
*/
static void update_wifi_data(char *tmp)
{
	int len = strlen(tmp);

	if (len >= 2 && ('\n' == tmp[len - 2] || '\r' == tmp[len - 2]))
	{
		tmp[len - 2] = '\0';
		return;
	}

	if (len >= 1 && ('\n' == tmp[len - 1] || '\r' == tmp[len - 1]))
	{
		tmp[len - 1] = '\0';
	}

	return;
}


void doWifiDownUp(wifi_type type, int downup)
{
	char tmp[160] = {0};
	int up = 0;
	int i = 0;
	char tmp_svc_wlan_name[64] = {0};
	char tmp_svc_wlan_ra_name[64] = {0};
	char tmp_svc_wlan_wds_name[64] = {0};
#if defined(TCSUPPORT_ECNT_MAP)
	char tmp_svc_wlan_apcli_name[64] = {0};
#endif
	char bssidnum[160] = {0};
	char enableSsid[160] = {0};
	int ismasterra = 0;
	
	if(wifimgr_lib_get_bssidnum(type, -1, bssidnum, sizeof(bssidnum)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_bssidnum fail.\n");
	}

	if (WIFI_2_4G == type)
	{
		snprintf(tmp_svc_wlan_name, sizeof(tmp_svc_wlan_name), "WLan");	
		snprintf(tmp_svc_wlan_ra_name, sizeof(tmp_svc_wlan_ra_name), "ra");
		snprintf(tmp_svc_wlan_wds_name, sizeof(tmp_svc_wlan_wds_name), "wds");
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp_svc_wlan_apcli_name, sizeof(tmp_svc_wlan_apcli_name), "apcli");
#endif
	}	
	else if(WIFI_5G == type)
	{	
		snprintf(tmp_svc_wlan_name, sizeof(tmp_svc_wlan_name), "WLan11ac");
		snprintf(tmp_svc_wlan_ra_name, sizeof(tmp_svc_wlan_ra_name), "rai");
		snprintf(tmp_svc_wlan_wds_name, sizeof(tmp_svc_wlan_wds_name), "wdsi");
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp_svc_wlan_apcli_name, sizeof(tmp_svc_wlan_apcli_name), "apclii");
#endif
	}

	if (0 == downup)
	{
		for(i = 0; i < atoi(bssidnum); i++)
		{
			snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
			system(tmp);			
		}
#ifdef TCSUPPORT_WLAN_WDS
		for(i = 0; i < MAX_WDS_ENTRY; i++)
		{
			snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_wds_name, i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_wds_name, i);
			system(tmp);		
		}
#endif				
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_apcli_name);
		system(tmp);
#endif
	}
	else if (1 == downup)
	{
		for(i = 0; i < atoi(bssidnum); i++)
		{				
			up = 1;
#if defined(TCSUPPORT_FON)
			if (1 == i)
				continue;
#endif

			ismasterra = 0;
			wifimgr_lib_get_enableSsid(type, i + 1, enableSsid, sizeof(enableSsid));
			if (0 == atoi(enableSsid))
			{
				up = 0;
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				if((WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7615D_ID])
					|| (WIFI_5G == type && WLAN_5G_CHIPYPTE[WLAN_5G_7615D_ID]))
				{
					if (0 == i && 0 == strcmp(tmp_svc_wlan_ra_name, "rai"))
					{
						ismasterra = 1;
					}
				}
				else
				{
					if (0 == i && 0 == strcmp(tmp_svc_wlan_ra_name, "ra"))
					{
						ismasterra = 1;
					}
				}
#else

				if((0 == i)
#if defined(MT7615D)
					&&(!strcmp(tmp_svc_wlan_ra_name,"rai"))
#else/*7915 use ra0 as master*/
					&&(!strcmp(tmp_svc_wlan_ra_name,"ra"))
#endif
					)
				{
					ismasterra = 1;
				}
#endif//TCSUPPORT_WLAN_MULTI_CHIP
			}

			if(up)
			{
	            snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_ra_name, i);
				system(tmp);					
				snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
				printf("add %s%d to br0\n", tmp_svc_wlan_ra_name, i);
			}
			else
			{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				if((WIFI_2_4G == type && (WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]))
					|| (WIFI_5G == type && (WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])))
				{
					/*wlan_load.sh do ifconfig ra0 up/down,so not do anymore in buildin mode*/
					snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
					system(tmp);
					snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
					system(tmp);
				}
				else
				{
					if(ismasterra)
					{					
						/*need do up and down operation when disable rai0,enable other raix*/	
						snprintf(tmp, sizeof(tmp), "ifconfig %s0 up", tmp_svc_wlan_ra_name);
						system(tmp);
						snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_ra_name);
						system(tmp);
					}
				}
#else
#if (defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) && defined(MT7915D)) || (defined(TCSUPPORT_WLAN_MT7916_BUILD_IN) && defined(MT7916D))
				/*wlan_load.sh do ifconfig ra0 up/down,so not do anymore in buildin mode*/
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
#else
				if(ismasterra)
				{					
					/*need do up and down operation when disable rai0,enable other raix*/	
					snprintf(tmp, sizeof(tmp), "ifconfig %s0 up", tmp_svc_wlan_ra_name);
					system(tmp);
					snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_ra_name);
					system(tmp);
				}
#endif
#endif
			}
		}

		memset(tmp, 0, sizeof(tmp));
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdsenable(type, -1, tmp, sizeof(tmp)) && 1 == atoi(tmp))
		{
			for(i = 0; i < MAX_WDS_ENTRY; i++)
			{
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_wds_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_wds_name, i);
				system(tmp);
				printf("addif %s%d to br0\n", tmp_svc_wlan_wds_name, i);						
			}
		}
	}

	return;
}

void update_auth_mode(wifi_type type, int index)
{
	char authMode[160] = {0};
#if 0
	char defaultKeyID[160] = {0};
#endif
	char keyStr[160] = {0};
	int i = 0;
#if defined(TCSUPPORT_CT_WLAN_NODE)
	char wepAuthType[160] = {0};
#endif

	if (0 == wifimgr_lib_get_authMode(type, index, authMode, sizeof(authMode)))
	{
		/*WEP64-bits or WEP128-bits*/
		if (strstr(authMode, "WEP"))
		{
#if defined(TCSUPPORT_WLAN_8021X)
			/*Radius-WEP64, Radius-WEP128*/
			if (strstr(authMode, "Radius"))
			{
				if (wifimgr_lib_set_ieee8021x(type, index, "1") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
				}
			}
			else
			{
				if (wifimgr_lib_set_ieee8021x(type, index, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
				}
			}
#endif	
 
#if defined(TCSUPPORT_CT_WLAN_NODE)
			if (0 == wifimgr_lib_get_wepAuthType(type, index, wepAuthType, sizeof(wepAuthType))){
 
				if(!strcmp(wepAuthType, "OpenSystem")){
					if(wifimgr_lib_set_authMode(type, index, "OPEN") != 0){
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}else if(!strcmp(wepAuthType, "SharedKey")){
					if(wifimgr_lib_set_authMode(type, index, "SHARED") != 0){
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}else{
					if(wifimgr_lib_set_authMode(type, index, "WEPAUTO") != 0){
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}
			}
			else{
				if(wifimgr_lib_set_authMode(type, index, "WEPAUTO") != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}
			}					
#else 
			if(wifimgr_lib_set_authMode(type, index, "WEPAUTO") != 0){
				printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
			}
#endif


			if (wifimgr_lib_set_encrypType(type, index, "WEP") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_encrypType fail.\n");
			}

#if 0
			if (0 == wifimgr_lib_get_defaultKeyID(type, index, defaultKeyID, sizeof(defaultKeyID))){
				if(wifimgr_lib_set_defaultKeyID(type, index, defaultKeyID) != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
				}
			}
#endif
			/*4 key string*/
			for(i = 1; i <= 4; i++)
			{
				if(0 == wifimgr_lib_get_keyStr(type, index, i, keyStr, sizeof(keyStr)))
				{	
					if(10 == strlen(keyStr) || 26 == strlen(keyStr) || 0 == strlen(keyStr))
					{
						if(wifimgr_lib_set_keyType(type, index, i, "0") != 0)
						{
							printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
						}
					}
					else
					{
						if(wifimgr_lib_set_keyType(type, index, i, "1") != 0)
						{
							printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
						}
					}					
				}
			}
		}
		else if(0 == strcmp(authMode, "WPAPSK") || 0 == strcmp(authMode, "WPA2PSK")
			|| 0 == strcmp(authMode, "WPAPSKWPA2PSK") /*WPAPSK or WPA2PSK or Mixed mode*/
#if defined(TCSUPPORT_WLAN_8021X)
			|| 0 == strcmp(authMode, "WPA") || 0 == strcmp(authMode, "WPA2")
			|| 0 == strcmp(authMode, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
#if defined(TCSUPPORT_WLAN_8021X)
			|| !strcmp(authMode, "WPA3") || !strcmp(authMode, "WPA3-192Bit")
#endif

			|| 0 == strcmp(authMode, "WPA3PSK") || 0 == strcmp(authMode, "WPA2PSKWPA3PSK")
#endif
			)
		{/*WPAPSK or WPA2PSK or Mixed mode*/	
#if defined(TCSUPPORT_WLAN_8021X)
			if(0 == strcmp(authMode, "WPA") || 0 == strcmp(authMode, "WPA2") 
				|| 0 == strcmp(authMode, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
				|| !strcmp(authMode, "WPA3") || !strcmp(authMode, "WPA3-192Bit")
#endif
			)
			{/*Radius-WEP64, Radius-WEP128*/
				if (0 == strcmp(authMode, "WPA3-192Bit"))
				{
					if(wifimgr_lib_set_authMode(type, index, "WPA3-192"))
					{
						printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
					}
				}

				if(wifimgr_lib_set_ieee8021x(type, index, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_ieee8021x fail.\n");
				}
			}
#endif
#if 0
			if(wifimgr_lib_set_authMode(type, index, authMode) != 0){
				printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
			}
			if(cfg_get_object_attr(node_name, "WPAPSK", tmp, sizeof(tmp)) > 0){
				if(wifimgr_lib_set_WPAPSK(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_WPAPSK fail.\n");
				}
			}
			if(cfg_get_object_attr(node_name, "EncrypType", tmp, sizeof(tmp)) > 0){
				if(wifimgr_lib_set_encrypType(0, j+1, tmp) != 0){
					printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
				}
			}
#endif
			if(wifimgr_lib_set_defaultKeyID(type, index, "2") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
			}

			for(i = 1; i <= 4; i++)
			{
				if(wifimgr_lib_set_keyStr(type, index, i, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_keyStr fail.\n");
				}

				if(wifimgr_lib_set_keyType(type, index, i, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
				}
			}				
		}
		else
		{/*OPEN*/
			if(wifimgr_lib_set_authMode(type, index, "OPEN") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
			}
			if(wifimgr_lib_set_encrypType(type, index, "NONE") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_authMode fail.\n");
			}				
			if(wifimgr_lib_set_defaultKeyID(type, index, "0") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_defaultKeyID fail.\n");
			}
			if(wifimgr_lib_set_WPAPSK(type, index, "0") != 0)
			{
				printf("write_wlan_config:wifimgr_lib_set_WPAPSK fail.\n");
			}
			
			for(i = 1; i <= 4; i++)
			{	
				if(wifimgr_lib_set_keyType(type, index, i, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_keyType fail.\n");
				}

				if(wifimgr_lib_set_keyStr(type, index, i, "0") != 0)
				{
					printf("write_wlan_config:wifimgr_lib_set_keyStr fail.\n");
				}
			}
		}
	}

	return;
}

/*
{"1", 	0},
{"2", 	1},
{"5.5",	2},
{"11",	3},
{"6",	0},
{"9",	1},
{"12",	2},
{"18",	3},
{"24",	4},
{"36",	5},
{"48",	6},
{"54",	7},
{"0",	33}
*/
void update_fixedTxMode(wifi_type type, int index, int strWirelessMode)
{
	char fixedTxMode[160] = {0};

	if (0 == strWirelessMode)
	{
		if (0 == wifimgr_lib_get_fixedTxMode(type, index, fixedTxMode, sizeof(fixedTxMode)))
		{
			if (0 == strcmp(fixedTxMode, "1") || 0 == strcmp(fixedTxMode, "2")
				|| 0 == strcmp(fixedTxMode, "5.5") || 0 == strcmp(fixedTxMode, "11"))
			{
				wifimgr_lib_set_fixedTxMode(type, index, "1");
			}
		}
		else
		{
			wifimgr_lib_set_fixedTxMode(type, index, "2");
		}
		
	}
	else if (1 == strWirelessMode)
	{
		wifimgr_lib_set_fixedTxMode(type, index, "1");
	}
	else if (4 == strWirelessMode)
	{
		wifimgr_lib_set_fixedTxMode(type, index, "2");
	}
	else
	{
		wifimgr_lib_set_fixedTxMode(type, index, "0");
	}

	return;
}

int
wifimgr_lib_check_wifi_value_befor_write_dat(wifi_type type)
{
	char bssidnum[160] = {0};
	char ht_bw[160] = {0};
	char strWirelessMode[160] = {0};
#if defined(MT7915D) || defined(TCSUPPORT_WLAN_MULTI_CHIP) || defined(MT7916D)
	char authMode[160] = {0};
	char htMCS[160] = {0};
#endif
	char buf[160] = {0};
	int i = 0, txBurst = 1;

	if(wifimgr_lib_get_bssidnum(type, -1, bssidnum, sizeof(bssidnum)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_bssidnum fail.\n");
	}

	if(wifimgr_lib_get_wirelessMode(type, -1, strWirelessMode, sizeof(strWirelessMode)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_wirelessMode fail.\n");
	}

	if (WIFI_2_4G == type)
	{
		/*update auth mode*/
		for (i = 0; i < atoi(bssidnum); i++)
		{
			update_auth_mode(type, i + 1);
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
			if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID])
			{
				/*HT MCS*/ 
				if(wifimgr_lib_get_authMode(type, i + 1, authMode, sizeof(authMode)) != 0)
				{
					printf("write_wlan_config:wifimgr_lib_get_authMode fail.\n");
				}
				if(wifimgr_lib_get_HT_MCS(type, i + 1, htMCS, sizeof(htMCS)) != 0)
				{
					printf("write_wlan_config:wifimgr_lib_get_HT_MCS fail.\n");
				}
				if(0 == strcmp(authMode,"WPAPSK") == 0 && 9 == atoi(strWirelessMode)
					&& atoi(htMCS) > 7)
				{
					if(wifimgr_lib_set_HT_MCS(type, i + 1, "33"))
					{
						printf("write_wlan_config:wifimgr_lib_set_HT_MCS fail.\n");
					}
				}
			}
#else
#if defined(MT7915D)
			/*HT MCS*/ 
			if(wifimgr_lib_get_authMode(type, i + 1, authMode, sizeof(authMode)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_authMode fail.\n");
			}
			if(wifimgr_lib_get_HT_MCS(type, i + 1, htMCS, sizeof(htMCS)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_HT_MCS fail.\n");
			}
			if(0 == strcmp(authMode,"WPAPSK") == 0 && 9 == atoi(strWirelessMode)
				&& atoi(htMCS) > 7)
			{
				if(wifimgr_lib_set_HT_MCS(type, i + 1, "33"))
				{
					printf("write_wlan_config:wifimgr_lib_set_HT_MCS fail.\n");
				}
			}
#endif
#if defined(MT7916D)
			/*HT MCS*/ 
			if(wifimgr_lib_get_authMode(type, i + 1, authMode, sizeof(authMode)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_authMode fail.\n");
			}
			if(wifimgr_lib_get_HT_MCS(type, i + 1, htMCS, sizeof(htMCS)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_HT_MCS fail.\n");
			}
			if(0 == strcmp(authMode,"WPAPSK") == 0 && 9 == atoi(strWirelessMode)
				&& atoi(htMCS) > 7)
			{
				if(wifimgr_lib_set_HT_MCS(type, i + 1, "33"))
				{
					printf("write_wlan_config:wifimgr_lib_set_HT_MCS fail.\n");
				}
			}
#endif
#endif
			if (6 == atoi(strWirelessMode)
				|| 9 == atoi(strWirelessMode)
				|| 16 == atoi(strWirelessMode))
			{
				wifimgr_lib_set_wmmCapable(type, i + 1, "0");
			}
			update_fixedTxMode(type, i + 1, atoi(strWirelessMode));
		}

		/*check if wmm enabled or not, txburst should disable when wmm enable.*/
		for (i = 0; i < atoi(bssidnum); i++)
		{
			memset(buf, 0, sizeof(buf));
			if(wifimgr_lib_get_wmmCapable(type, i + 1, buf, sizeof(buf)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_wmmCapable fail.\n");
			}
//			if (1 == atoi(buf))
//			{
//				txBurst = 0;
//				break;
//			}
		}

		if (1 == txBurst)
		{
			wifimgr_lib_set_txBurst(type, -1, "1");
		}
		else
		{
			wifimgr_lib_set_txBurst(type, -1, "0");
		}
		/*HT BW*/
		if (6 != atoi(strWirelessMode)
			&& 7 != atoi(strWirelessMode)
			&& 9 != atoi(strWirelessMode)
			&& 16 != atoi(strWirelessMode))
		{
			wifimgr_lib_set_HT_BW(type, -1, "0");
		}

		/*Auto Channel*/ 
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_get_channel(type, -1, buf, sizeof(buf));
		if (0 == atoi(buf))
		{
			wifimgr_lib_set_autoChannelSelect(type, -1, "3");
		}
		else
		{
			wifimgr_lib_set_autoChannelSelect(type, -1, "0");
		}
		
		/*basic Rate*/ 
		if (0 == atoi(strWirelessMode)
			|| 6 == atoi(strWirelessMode)
			|| 9 == atoi(strWirelessMode)
			|| 16 == atoi(strWirelessMode))
		{
			wifimgr_lib_set_basicRate(type, -1, "15");
		}
		else if (1 == atoi(strWirelessMode))
		{
			wifimgr_lib_set_basicRate(type, -1, "3");
		}
		else if (4 == atoi(strWirelessMode)
			|| 7 == atoi(strWirelessMode))
		{
			wifimgr_lib_set_basicRate(type, -1, "351");
		}

		/*HT OpMode*/ 
		if (0 != atoi(strWirelessMode)
			&& 1 != atoi(strWirelessMode)
			&& 7 != atoi(strWirelessMode)
			&& 9 != atoi(strWirelessMode)
			&& 16 != atoi(strWirelessMode))
		{
			wifimgr_lib_set_HT_OpMode(type, -1, "0");
		}
		else if (4 != atoi(strWirelessMode)
			&& 6 != atoi(strWirelessMode))
		{
			wifimgr_lib_set_HT_OpMode(type, -1, "1");
		}

		/*txPower*/ 
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_get_txPower(type, -1, buf, sizeof(buf));
		if( 1 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "75");
		}
		else if( 2 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "50");
		}
		else if( 3 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "25");
		}
		else if( 4 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "15");
		}
		else if( 5 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "9");
		}
		else if( 6 == atoi(buf) )
		{
			wifimgr_lib_set_txPower(type, -1, "100");
		}		
		
	}
	else if (WIFI_5G == type)
	{
		/*update auth mode*/
		for (i = 0; i < atoi(bssidnum); i++)
		{
			update_auth_mode(type, i + 1);
			if (8 == atoi(strWirelessMode)
				|| 14 == atoi(strWirelessMode)
				|| 15 == atoi(strWirelessMode)
				|| 17 == atoi(strWirelessMode))
			{
				wifimgr_lib_set_wmmCapable(type, i + 1, "1");
			}
			wifimgr_lib_set_fixedTxMode(type, i + 1, "0");
		}

		/*check if wmm enabled or not, txburst should disable when wmm enable.*/
		for (i = 0; i < atoi(bssidnum); i++)
		{
			memset(buf, 0, sizeof(buf));
			wifimgr_lib_get_wmmCapable(type, i + 1, buf, sizeof(buf));
//			if (1 == atoi(buf))
//			{
//				txBurst = 0;
//				break;
//			}
		}

		if (1 == txBurst)
		{
			wifimgr_lib_set_txBurst(type, -1, "1");
		}
		else
		{
			wifimgr_lib_set_txBurst(type, -1, "0");
		}
		
		/*HT BW*/
		if (2 == atoi(strWirelessMode))
		{
			wifimgr_lib_set_HT_BW(type, -1, "0");
		}
		
		/*VHT BW*/
		if (14 != atoi(strWirelessMode)
			&& 15 != atoi(strWirelessMode)
			&& 17 != atoi(strWirelessMode))
		{
			wifimgr_lib_set_VHT_BW(type, -1, "0");
			wifimgr_lib_set_VHT_SGI(type, -1, "1");
		}
		else
		{
			wifimgr_lib_get_HT_BW(type, -1, ht_bw, sizeof(ht_bw));
			if (1 != atoi(ht_bw))
			{
				wifimgr_lib_set_VHT_SGI(type, -1, "1");
			}
		}
		/*HT OpMode*/
		wifimgr_lib_set_HT_OpMode(type, -1, "0");

		/*Auto Channel*/ 
		memset(buf, 0, sizeof(buf));
		wifimgr_lib_get_channel(type, -1, buf, sizeof(buf));
		if (0 == atoi(buf))
		{
			wifimgr_lib_set_autoChannelSelect(type, -1, "3");
		}
		else
		{
			wifimgr_lib_set_autoChannelSelect(type, -1, "0");
		}

		/*HT OpMode*/ 
		wifimgr_lib_set_HT_OpMode(type, -1, "0");	
	}

	return 0;
}


int
wifimgr_lib_wifi_execute(wifi_type type, int downup, int downup_other)
{
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D) || defined(TCSUPPORT_WLAN_MULTI_CHIP)
	int type_other = 0;

	if (0 == type)
		type_other = 1;
#endif
	if (0 == downup)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if((WIFI_2_4G == type && (WLAN_2G_CHIPYPTE[WLAN_2G_7615D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID]|| WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]))
			|| (WIFI_5G == type && (WLAN_5G_CHIPYPTE[WLAN_5G_7615D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])))
		{
			doWifiDownUp(type, downup);
			if (1 == downup_other)
				doWifiDownUp(type_other, 1);
		}
		else
		{
			doWifiDownUp(type, downup);
		}
#else
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
		doWifiDownUp(type, downup);
		if (1 == downup_other)
			doWifiDownUp(type_other, 1);
#else
		doWifiDownUp(type, downup);
#endif
#endif
	}
	else
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if((WIFI_2_4G == type && (WLAN_2G_CHIPYPTE[WLAN_2G_7615D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID]|| WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]))
			|| (WIFI_5G == type && (WLAN_5G_CHIPYPTE[WLAN_5G_7615D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])))
		{
			doWifiDownUp(type, 0);
			doWifiDownUp(type_other, 0);

			doWifiDownUp(type, 1);
			doWifiDownUp(type_other, 1);
			if (0 == downup_other)
				doWifiDownUp(type_other, 0);
		}
		else
		{
			doWifiDownUp(type, 0);
			doWifiDownUp(type, 1);
		}
#else
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
		doWifiDownUp(type, 0);
		doWifiDownUp(type_other, 0);

		doWifiDownUp(type, 1);
		doWifiDownUp(type_other, 1);
		if (0 == downup_other)
			doWifiDownUp(type_other, 0);
#else
		doWifiDownUp(type, 0);
		doWifiDownUp(type, 1);
#endif
#endif
	}

	return 0;
}

void wifimgr_lib_wifi_onoff(wifi_type type, int downup)
{
	char tmp[160] = {0};
	int i = 0;
	char tmp_svc_wlan_name[64] = {0};
	char tmp_svc_wlan_ra_name[64] = {0};
	char tmp_svc_wlan_wds_name[64] = {0};
#if defined(TCSUPPORT_ECNT_MAP)
	char tmp_svc_wlan_apcli_name[64] = {0};
#endif
	char bssidnum[160] = {0};
	char enableSsid[160] = {0};
	
	if(wifimgr_lib_get_bssidnum(type, -1, bssidnum, sizeof(bssidnum)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_bssidnum fail.\n");
	}

	if (WIFI_2_4G == type)
	{
		snprintf(tmp_svc_wlan_name, sizeof(tmp_svc_wlan_name), "WLan");	
		snprintf(tmp_svc_wlan_ra_name, sizeof(tmp_svc_wlan_ra_name), "ra");
		snprintf(tmp_svc_wlan_wds_name, sizeof(tmp_svc_wlan_wds_name), "wds");
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp_svc_wlan_apcli_name, sizeof(tmp_svc_wlan_apcli_name), "apcli");
#endif
	}	
	else if(WIFI_5G == type)
	{	
		snprintf(tmp_svc_wlan_name, sizeof(tmp_svc_wlan_name), "WLan11ac");
		snprintf(tmp_svc_wlan_ra_name, sizeof(tmp_svc_wlan_ra_name), "rai");
		snprintf(tmp_svc_wlan_wds_name, sizeof(tmp_svc_wlan_wds_name), "wdsi");
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp_svc_wlan_apcli_name, sizeof(tmp_svc_wlan_apcli_name), "apclii");
#endif
	}

	if (0 == downup)
	{
		for(i = 0; i < atoi(bssidnum); i++)
		{
			snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
			system(tmp);			
		}
#ifdef TCSUPPORT_WLAN_WDS
		for(i = 0; i < MAX_WDS_ENTRY; i++)
		{
			snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_wds_name, i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_wds_name, i);
			system(tmp);		
		}
#endif				
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_apcli_name);
		system(tmp);
#endif
	}
	else if (1 == downup)
	{
		for(i = 0; i < atoi(bssidnum); i++)
		{				
#if defined(TCSUPPORT_FON)
			if (1 == i)
				continue;
#endif
			wifimgr_lib_get_enableSsid(type, i + 1, enableSsid, sizeof(enableSsid));
			if (1 == atoi(enableSsid))
			{
	            snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_ra_name, i);
				system(tmp);					
				snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
				printf("add %s%d to br0\n", tmp_svc_wlan_ra_name, i);
			}
			else
			{
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
			}
		}

		memset(tmp, 0, sizeof(tmp));
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdsenable(type, -1, tmp, sizeof(tmp)) && 1 == atoi(tmp))
		{
			for(i = 0; i < MAX_WDS_ENTRY; i++)
			{
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_wds_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_wds_name, i);
				system(tmp);
				printf("addif %s%d to br0\n", tmp_svc_wlan_wds_name, i);						
			}
		}
	}

	return;
}

int wifimgr_lib_service_stop_wps(unsigned char *filename, wifi_type type, int index)
	{
	char wpsConfMode[MAX_KEYWORD_LEN] = {0};
	int ret = ECNT_WIFIMGR_SUCCESS;
	wps_para tmppara;
	interface_para interpara;
	memset(&tmppara,0,sizeof(tmppara));
	memset(&interpara,0,sizeof(interpara));
	wifimgr_lib_get_wscConfMode(type, index + 1, wpsConfMode, sizeof(wpsConfMode));	
	if (1 == atoi(wpsConfMode) || 2 == atoi(wpsConfMode) || 7 == atoi(wpsConfMode))
	{
		interpara.band = type;
		interpara.index = index;
		ret = wps_service_action_internal(filename,WPS_STOP,&interpara,&tmppara);
	}

	return ret;

	}

int wifimgr_lib_service_start_wps(unsigned char *filename, wifi_type type, int index,wps_info* wpsinfo)
{
	char wpsConfMode[MAX_KEYWORD_LEN] = {0}, wpsConfStatus[MAX_KEYWORD_LEN] = {0};
	int ret = ECNT_WIFIMGR_SUCCESS;
	wps_para tmppara;
	interface_para interpara;
	memset(&tmppara,0,sizeof(tmppara));
	memset(&interpara,0,sizeof(interpara));
	wifimgr_lib_get_wscConfMode(type, index + 1, wpsConfMode, sizeof(wpsConfMode));
	if (1 == atoi(wpsConfMode) || 2 == atoi(wpsConfMode) || 7 == atoi(wpsConfMode))
	{
		interpara.band = type;
		interpara.index = index;
		
		wifimgr_lib_get_WscConfStatus(type, index + 1, wpsConfStatus, sizeof(wpsConfStatus));
		
		tmppara.wpsConfMode = atoi(wpsConfMode);
		tmppara.wpsConfStatus = atoi(wpsConfStatus);
		tmppara.wpsMode = wpsinfo->wpsMode;
		if(0 == wpsinfo->wpsMode)
		{
			#if 1
			tmppara.wpsPinCode = wpsinfo->wpsPinCode;
#else
			wifimgr_lib_get_wscPinCode(type, index, wscPinCode, sizeof(wscPinCode));
			tmppara.wpsPinCode = (unsigned int)strtoul(wscPinCode, NULL, 10);
#endif
		
}
		ret = wps_service_action_internal(filename,WPS_START,&interpara,&tmppara);
				}

	return ret;
			}

int wifimgr_lib_service_genpincode(unsigned char *filename, wifi_type type, int index)
			{
	char wpsConfMode[MAX_KEYWORD_LEN] = {0};
	int ret = ECNT_WIFIMGR_SUCCESS;
	wps_para tmppara;
	interface_para interpara;
	memset(&tmppara,0,sizeof(tmppara));
	memset(&interpara,0,sizeof(interpara));
	wifimgr_lib_get_wscConfMode(type, index + 1, wpsConfMode, sizeof(wpsConfMode));	
	if (1 == atoi(wpsConfMode) || 2 == atoi(wpsConfMode) || 7 == atoi(wpsConfMode))
				{
		interpara.band = type;
		interpara.index = index;
		ret = wps_service_action_internal(filename,WPS_GENPINCODE,&interpara,&tmppara);
	}

	return ret;
					}

int wifimgr_lib_service_resetoob(unsigned char *filename, wifi_type type, int index)
					{
	char wpsConfMode[MAX_KEYWORD_LEN] = {0};
	int ret = ECNT_WIFIMGR_SUCCESS;
	wps_para tmppara;
	interface_para interpara;
	memset(&tmppara,0,sizeof(tmppara));
	memset(&interpara,0,sizeof(interpara));
	wifimgr_lib_get_wscConfMode(type, index + 1, wpsConfMode, sizeof(wpsConfMode));	
	if (1 == atoi(wpsConfMode) || 2 == atoi(wpsConfMode) || 7 == atoi(wpsConfMode))
						{
		interpara.band = type;
		interpara.index = index;
		ret = wps_service_action_internal(filename,WPS_RESETOOB,&interpara,&tmppara);
						}

	return ret;
					}


int wps_service_action_internal(unsigned char *filename, int action,interface_para*interconfig,wps_para*config)
					{
	char tmpbuf[128] ={0};
	int ret = ECNT_WIFIMGR_SUCCESS;
	char runningname[32] = {0};
	FILE *fp = NULL;
	wifi_reload_config *configptr = NULL;
	
	if(!config || !interconfig)
	{
		ret = ECNT_WIFIMGR_FAILURE;
		goto retvalue; 
	}
	
	if (WIFI_2_4G == interconfig->band)
	{
		configptr = &store_global[WIFI_2_4G];
					}
	else if (WIFI_5G == interconfig->band)
	{
		configptr = &store_global[WIFI_5G];
				}
	else
				{
		goto retvalue; 
				}

	snprintf(runningname,sizeof(runningname),"%s%d",configptr->itf, interconfig->index);
	switch(action)
	{
		case WPS_RESETOOB:
			sprintf(tmpbuf,"/userfs/bin/iwpriv %s set WscOOB=1\n",runningname);
			if(doCmdWrite((char *)filename,tmpbuf) < 0)
				ret = ECNT_WIFIMGR_FAILURE;
		break;

		case WPS_GENPINCODE:
			sprintf(tmpbuf,"/userfs/bin/iwpriv %s set WscGenPinCode=1\n",runningname);
			if(doCmdWrite((char *)filename,tmpbuf) < 0)
				ret = ECNT_WIFIMGR_FAILURE;
		break;

		case WPS_STOP:
			sprintf(tmpbuf,"/userfs/bin/iwpriv %s set WscStop\n",runningname);
			if(doCmdWrite((char *)filename,tmpbuf) < 0)
				ret = ECNT_WIFIMGR_FAILURE;
		break;

		case WPS_START:
			fp = fopen((const char *)filename, "w");
			if (NULL != fp)
			{
				if (1 != config->wpsConfStatus && 2 != config->wpsConfStatus)
				{
					fclose(fp);
					ret = ECNT_WIFIMGR_FAILURE;
					goto retvalue; 
				}
				fprintf(fp, "/userfs/bin/iwpriv %s set WscConfMode=%d\n", runningname, config->wpsConfMode);
				fprintf(fp, "/userfs/bin/iwpriv %s set WscConfStatus=%d\n", runningname, config->wpsConfStatus);
				if (0 == config->wpsMode)/* Pin code */
				{
					fprintf(fp, "/userfs/bin/iwpriv %s set WscMode=1\n", runningname);
					if(config->wpsPinCode != 0)
						fprintf(fp, "/userfs/bin/iwpriv %s set WscPinCode=%d\n", runningname, config->wpsPinCode);
			}
				else if (1 == config->wpsMode)/*  PBC  */
				{
					fprintf(fp, "/userfs/bin/iwpriv %s set WscMode=2\n", runningname);
				}
				fprintf(fp, "/userfs/bin/iwpriv %s set WscGetConf=1\n", runningname);/* start WPS */

				if(chmod((const char *)filename, S_IRUSR|S_IWUSR|S_IXUSR) < 0)
					ret = ECNT_WIFIMGR_FAILURE;
				
			fclose(fp);
		}
		break;

		default:
			break;
	}

retvalue:
	return ret;
}

void get_onu_ipaddr(char *ip)
{
	FILE *fp = NULL;
    char *line = NULL;
	char *tmp= NULL;
	size_t size = 0;

	system("ifconfig br0 > /tmp/br0_info.txt");
	fp = fopen("/tmp/br0_info.txt", "r");
	if (NULL != fp)
	{
		while(getline(&line, &size, fp) != -1)
		{
			if ((tmp = strstr(line, "inet addr:")) != NULL)
			{
				sscanf(tmp, "inet addr:%s ", ip);
				break;
			}
		}
		fclose(fp);

		if(line != NULL)
	    {
	        free(line);
	    }
	}

	return;
}

int
wifimgr_lib_service_wpsstart_wifiupnp(wifi_type type, int index, char *active)
{
	char lan_ip[32] 	= {0};
	char wscdpid[128]	= {0};
	char wpsConfMode[160] = {0};
	char name[16] = {0};
	if (WIFI_2_4G == type)
	{
		strcpy(name,MTKINTERFACEPREFIX);
	}
	else if (WIFI_5G == type)
	{
		strcpy(name,MTKINTERFACE5GPREFIX);
	}
	else
	{
		return -1;
	}

	system("killall -9 wscd");
	//system("/sbin/route del -net 239.0.0.0 netmask 255.0.0.0 dev br0");
	if(0 != strcmp(active, "Yes"))
	{
		return 0;
	}
	
	if(wifimgr_lib_get_wscConfMode(type, index, wpsConfMode, sizeof(wpsConfMode)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_wscConfMode fail.\n");
	}

	if(0 != atoi(wpsConfMode))
	{
		//system("/sbin/route add -net 239.0.0.0 netmask 255.0.0.0 dev br0");
		get_onu_ipaddr(lan_ip);
		/*mark this route, because when switch upnp on page, it will cause the lan pc can not ping to the CPE. evan.jiang_20120517*/
    	memset(wscdpid, 0, sizeof(wscdpid));
		snprintf(wscdpid, sizeof(wscdpid), "/usr/bin/wscd -i %s%d -a %s -m 1 &", name, index - 1, lan_ip);
		system(wscdpid);	    	
	}

	return 0;	
}

/**
	action 0:init
	action 1:check is change?
*/
void chipspecial_store(wifi_reload_config*paraptr,wifi_type type, int index)
{
	return;
}

int chipspecial_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	return NO_NEED_RELOAD;
}

void systemreboot_store(wifi_reload_config*paraptr,wifi_type type, int index)
{	
	/*
	wifi_get_config_fun config[16] = 
{
		{"BssidNum", 		wifimgr_lib_get_bssidnum,		&(paraptr->common.BssidNum)},
		{"WHNAT", 			wifimgr_lib_get_WHNAT,			&(paraptr->common.WHNAT)},
		{"",				NULL,								NULL},
	};
	*/

	char systemreboot[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, systemreboot, MAX_KEYWORD_LEN))
		return;
	paraptr->common.BssidNum = atoi(systemreboot);

	memset(systemreboot,0,MAX_KEYWORD_LEN);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_WHNAT(type, -1, systemreboot, MAX_KEYWORD_LEN))
		return;
	paraptr->common.WHNAT = atoi(systemreboot);
	return;
}

int systemreboot_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	/*
	wifi_get_config_fun config[16] = 
	{
		{"BssidNum",		wifimgr_lib_get_bssidnum,		&(paraptr->common.BssidNum)},
		{"WHNAT",			wifimgr_lib_get_WHNAT,			&(paraptr->common.WHNAT)},
		{"",				NULL,								NULL},
	};
	*/

	char systemreboot[MAX_KEYWORD_LEN]= {0};
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssidnum(type, -1, systemreboot, MAX_KEYWORD_LEN))
	{
	if(paraptr->common.BssidNum != atoi(systemreboot))
		return NEED_REBOOT;
	}

	memset(systemreboot,0,MAX_KEYWORD_LEN);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WHNAT(type, -1, systemreboot, MAX_KEYWORD_LEN))
	{
	if(paraptr->common.WHNAT != atoi(systemreboot))
		return NEED_REBOOT;
	}
	
	return NO_NEED_RELOAD;
}

void commoncap_store(wifi_reload_config*paraptr,wifi_type type, int index)
{
	char commoncap[MAX_KEYWORD_LEN]= {0};
	int i			= 0;
	wifi_get_config_fun config[16] = 
	{
		{"channel", 		wifimgr_lib_get_channel,		&(paraptr->common.channel)},
		{"BeaconPeriod", 	wifimgr_lib_get_beaconPeriod,	&(paraptr->common.BeaconPeriod)},
		{"DtimPeriod",		wifimgr_lib_get_dtimPeriod, 	&(paraptr->common.DtimPeriod)},
		{"HtTxstream",		wifimgr_lib_get_HT_TxStream, 	&(paraptr->common.HtTxstream)},
		{"HtRxstream",		wifimgr_lib_get_HT_RxStream, 	&(paraptr->common.HtRxstream)},
		{"WirelessMode",	wifimgr_lib_get_wirelessMode,	&(paraptr->common.WirelessMode)}, 
		{"TxPower",			wifimgr_lib_get_txPower,		&(paraptr->common.txPower)}, 
		{"FragThreshold",	wifimgr_lib_get_fragThreshold,	&(paraptr->common.FragThreshold)}, 
		{"",				NULL,								NULL},
	};

	while(0 != strlen(config[i].attr_name))
	{
		memset(commoncap, 0, sizeof(commoncap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, commoncap, sizeof(commoncap)))
		{
			*(config[i].buf) = atoi(commoncap);
		}
		i++;
	}
	
	//special case:need to remove from table and do here
	if (WIFI_2_4G == type)
	{
		memset(commoncap,0,MAX_KEYWORD_LEN);
		wifimgr_lib_get_countryRegion(type, -1, commoncap, MAX_KEYWORD_LEN);
		paraptr->common.CountryRegion = atoi(commoncap);
	}
	else if (WIFI_5G == type)
		{
		memset(commoncap,0,MAX_KEYWORD_LEN);
		wifimgr_lib_get_countryRegionABand(type, -1, commoncap, MAX_KEYWORD_LEN);
		paraptr->common.CountryRegionABand = atoi(commoncap);
		}

	return;
	}

int commoncap_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	char commoncap[MAX_KEYWORD_LEN]= {0};
	int i = 0;
	wifi_get_config_fun config[16] = 
	{
		{"channel", 		wifimgr_lib_get_channel,		&(paraptr->common.channel)},
		{"BeaconPeriod", 	wifimgr_lib_get_beaconPeriod,	&(paraptr->common.BeaconPeriod)},
		{"DtimPeriod",		wifimgr_lib_get_dtimPeriod, 	&(paraptr->common.DtimPeriod)},
		{"HtTxstream",		wifimgr_lib_get_HT_TxStream, 	&(paraptr->common.HtTxstream)},
		{"HtRxstream",		wifimgr_lib_get_HT_RxStream, 	&(paraptr->common.HtRxstream)},
		{"WirelessMode",	wifimgr_lib_get_wirelessMode,	&(paraptr->common.WirelessMode)}, 
		{"TxPower",			wifimgr_lib_get_txPower,		&(paraptr->common.txPower)}, 
		{"FragThreshold",	wifimgr_lib_get_fragThreshold,	&(paraptr->common.FragThreshold)}, 
		{"",				NULL,								NULL},
	};

	while(0 != strlen(config[i].attr_name))
	{
		memset(commoncap, 0, sizeof(commoncap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, commoncap, sizeof(commoncap)))
	{
			if (*(config[i].buf) != atoi(commoncap))
		{
					return  NEED_RELOAD;
		}
		}
		i++;
	}

	//special case:need to remove from table and do here
	if (WIFI_2_4G == type)
	{
		memset(commoncap,0,MAX_KEYWORD_LEN);
		wifimgr_lib_get_countryRegion(type, -1, commoncap, MAX_KEYWORD_LEN);
		if(paraptr->common.CountryRegion != atoi(commoncap))
			return NEED_RELOAD;
	}
	else if(WIFI_5G == type)
		{
		memset(commoncap,0,MAX_KEYWORD_LEN);
		wifimgr_lib_get_countryRegionABand(type, -1, commoncap, MAX_KEYWORD_LEN);
		if(paraptr->common.CountryRegionABand != atoi(commoncap))
			return NEED_RELOAD;
		}

	return NO_NEED_RELOAD;
	}

void entrycap_store(wifi_reload_config*paraptr,wifi_type type, int index)
{
	char entrycap[MAX_KEYWORD_LEN] = {0};
	char bssidNum[MAX_KEYWORD_LEN] = {0};
	int i = 0;
	int j = 0;
	wifi_get_config_fun config[16] = 
	{
		{"APSDCapable", 	wifimgr_lib_get_apsdCapable,		(unsigned int *)&paraptr->entry.APSDCapable},
		{"WscV2Support", 	wifimgr_lib_get_wscV2Support,		(unsigned int *)&paraptr->entry.WscV2Support},
		{"EnableSSID", 		wifimgr_lib_get_enableSsid,			(unsigned int *)&paraptr->entry.EnableSSID},
		{"",				NULL,								NULL},
	};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidNum, sizeof(bssidNum))){
			return;
	}
	
	for (j = 1; j <= atoi(bssidNum); j++)
	{
		while(0 != strlen(config[i].attr_name))
		{
			memset(entrycap, 0, sizeof(entrycap));
			if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, j, entrycap, sizeof(entrycap)))
			{
				config[i].buf[j-1] = atoi(entrycap);
			}
			i++;
		}
	}

	return;
}

int entrycap_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	char entrycap[MAX_KEYWORD_LEN] = {0};
	char bssidNum[MAX_KEYWORD_LEN] = {0};
	int i = 0;
	int j = 0;
	wifi_get_config_fun config[16] = 
	{
		{"APSDCapable", 	wifimgr_lib_get_apsdCapable,		(unsigned int *)&(paraptr->entry.APSDCapable)},
		{"WscV2Support", 	wifimgr_lib_get_wscV2Support,		(unsigned int *)&(paraptr->entry.WscV2Support)},
		{"EnableSSID", 		wifimgr_lib_get_enableSsid,			(unsigned int *)&(paraptr->entry.EnableSSID)},
		{"",				NULL,								NULL},
	};

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidNum, sizeof(bssidNum))){
			RETURN_WIFIMGR_FAIL;
	}

	for (j = 1; j <= atoi(bssidNum); j++)
	{
		while(0 != strlen(config[i].attr_name))
		{
			memset(entrycap, 0, sizeof(entrycap));
			if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, j, entrycap, sizeof(entrycap)))
			{
				if (config[i].buf[j-1] != atoi(entrycap))
				{
						return  NEED_RELOAD;
				}
			}
			i++;
		}
	}

	return NO_NEED_RELOAD;
}

void htcap_store(wifi_reload_config*paraptr,wifi_type type, int index)
{
	char htcap[MAX_KEYWORD_LEN]= {0};
	int i = 0;
	wifi_get_config_fun config[16] = 
	{
		{"HtBssCoex", 	wifimgr_lib_get_HT_BSSCoexistence,		&(paraptr->common.HtBssCoex)},
		{"HtExchar", 	wifimgr_lib_get_HT_EXTCHA,				&(paraptr->common.HtExchar)},
		{"HTBw",		wifimgr_lib_get_HT_BW, 					&(paraptr->common.HTBw)},
		{"HtGi",		wifimgr_lib_get_HT_GI, 					&(paraptr->common.HtGi)},
		{"",			NULL,									NULL},
	};

	while(0 != strlen(config[i].attr_name))
	{
		memset(htcap, 0, sizeof(htcap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, htcap, sizeof(htcap)))
		{
			*(config[i].buf) = atoi(htcap);
		}
		i++;
	}
	return;
		}

int htcap_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	char htcap[MAX_KEYWORD_LEN] = {0};
	int i = 0;
	wifi_get_config_fun config[16] = 
		{
		{"HtBssCoex", 	wifimgr_lib_get_HT_BSSCoexistence,		&(paraptr->common.HtBssCoex)},
		{"HtExchar", 	wifimgr_lib_get_HT_EXTCHA,				&(paraptr->common.HtExchar)},
		{"HTBw",		wifimgr_lib_get_HT_BW, 					&(paraptr->common.HTBw)},
		{"HtGi",		wifimgr_lib_get_HT_GI, 					&(paraptr->common.HtGi)},
		{"",			NULL,									NULL},
	};

	while(0 != strlen(config[i].attr_name))
			{
		memset(htcap, 0, sizeof(htcap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, htcap, sizeof(htcap)))
				{
			if (*(config[i].buf) != atoi(htcap))
					{
					return  NEED_RELOAD;
					}
				}	
		i++;
			}

	return NO_NEED_RELOAD;
		}

void vhtcap_store(wifi_reload_config*paraptr,wifi_type type, int index)
{
	/*
	wifi_get_config_fun config[16] = 
	{
		{"VHTBw", 	wifimgr_lib_get_VHT_BW,		&(paraptr->common.VHTBw)},
		{"VHtGi", 	wifimgr_lib_get_VHT_SGI,				&(paraptr->common.VHtGi)},
		{"",			NULL,									NULL},
	};
	*/
	
	char vhtcap[MAX_KEYWORD_LEN]= {0};
	char vhtcap1[MAX_KEYWORD_LEN]= {0};
	char bssidnum[32] = {0};
	int j = 0;
	if(WIFI_2_4G == type)
		return;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidnum, MAX_KEYWORD_LEN))
		return;
	
	wifimgr_lib_get_VHT_BW(type, -1, vhtcap, MAX_KEYWORD_LEN);
	snprintf(vhtcap1, sizeof(vhtcap1), "%s", vhtcap);
	for (j = 1; j < atoi(bssidnum); j++)
	{
		snprintf(vhtcap1 + strlen(vhtcap1), sizeof(vhtcap1) - strlen(vhtcap1), ";%s", vhtcap);
	}
	snprintf(vhtcap, sizeof(vhtcap), "%s", vhtcap1);
	paraptr->common.VHTBw = atoi(vhtcap);


	memset(vhtcap,0,MAX_KEYWORD_LEN);
	wifimgr_lib_get_VHT_SGI(type, -1, vhtcap, MAX_KEYWORD_LEN);
	snprintf(vhtcap1, sizeof(vhtcap1), "%s", vhtcap);
	for (j = 1; j < atoi(bssidnum); j++)
	{
		snprintf(vhtcap1 + strlen(vhtcap1), sizeof(vhtcap1) - strlen(vhtcap1), ";%s", vhtcap);
	}
	snprintf(vhtcap, sizeof(vhtcap), "%s", vhtcap1);
	paraptr->common.VHtGi = atoi(vhtcap);
	return;
}

int vhtcap_compare(wifi_reload_config*paraptr, wifi_type type, int index)
{
	/*
	wifi_get_config_fun config[16] = 
	{
		{"VHTBw", 	wifimgr_lib_get_VHT_BW,		&(paraptr->common.VHTBw)},
		{"VHtGi", 	wifimgr_lib_get_VHT_SGI,				&(paraptr->common.VHtGi)},
		{"",			NULL,									NULL},
	};
	*/

	char vhtcap[MAX_KEYWORD_LEN]= {0};
	char vhtcap1[MAX_KEYWORD_LEN]= {0};
	char bssidnum[32] = {0};
	int j = 0;
	if(WIFI_2_4G == type)
		return NO_NEED_RELOAD;
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, bssidnum, MAX_KEYWORD_LEN))
		return NO_NEED_RELOAD;

	wifimgr_lib_get_VHT_BW(type, -1, vhtcap, MAX_KEYWORD_LEN);
	snprintf(vhtcap1, sizeof(vhtcap1), "%s", vhtcap);
	for (j = 1; j < atoi(bssidnum); j++)
	{
		snprintf(vhtcap1 + strlen(vhtcap1), sizeof(vhtcap1) - strlen(vhtcap1), ";%s", vhtcap);
	}
	snprintf(vhtcap, sizeof(vhtcap), "%s", vhtcap1);
	if(paraptr->common.VHTBw != atoi(vhtcap))
		return NEED_RELOAD;

	
	memset(vhtcap,0,MAX_KEYWORD_LEN);
	wifimgr_lib_get_VHT_SGI(type, -1, vhtcap, MAX_KEYWORD_LEN);
	snprintf(vhtcap1, sizeof(vhtcap1), "%s", vhtcap);
	for (j = 1; j < atoi(bssidnum); j++)
	{
		snprintf(vhtcap1 + strlen(vhtcap1), sizeof(vhtcap1) - strlen(vhtcap1), ";%s", vhtcap);
	}
	snprintf(vhtcap, sizeof(vhtcap), "%s", vhtcap1);
	if(paraptr->common.VHtGi != atoi(vhtcap))
		return NEED_RELOAD;

	return NO_NEED_RELOAD;
	}
		
void hecap_store(wifi_reload_config* paraptr,wifi_type type, int index)
{
#if defined(TCSUPPORT_WLAN_MULTI_CHIP) 
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID])	
	{
		char hecap[MAX_KEYWORD_LEN] = {0}; 
		int i = 0;
		wifi_get_config_fun config[16] = 
		{
			{"MuOfdmaDlEnable", 	wifimgr_lib_get_MuOfdmaDlEnable,	&(paraptr->common.MuOfdmaDlEnable)},
			{"MuOfdmaUlEnable", 	wifimgr_lib_get_MuOfdmaUlEnable,	&(paraptr->common.MuOfdmaUlEnable)},
			{"MuMimoDlEnable",		wifimgr_lib_get_MuMimoDlEnable, 	&(paraptr->common.MuMimoDlEnable)},
			{"MuMimoUlEnable",		wifimgr_lib_get_MuMimoUlEnable, 	&(paraptr->common.MuMimoUlEnable)},
			{"TWTSupport",			wifimgr_lib_get_TWTSupport, 		&(paraptr->common.TWTSupport)},
			{"SREnable",			wifimgr_lib_get_SREnable,			&(paraptr->common.SREnable)},
			{"SRMode",				wifimgr_lib_get_SRMode, 			&(paraptr->common.SRMode)},
			{"SRSDEnable",			wifimgr_lib_get_SRSDEnable, 		&(paraptr->common.SRSDEnable)},
			{"",					NULL,								NULL},
		};
	
		while(0 != strlen(config[i].attr_name))
	{
			memset(hecap, 0, sizeof(hecap));
			if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, hecap, sizeof(hecap)))
		{
				*(config[i].buf) = atoi(hecap);
			}
			i++;
		}
		}
#else
#if defined(MT7915N) || defined(MT7915E) || defined(MT7916N) || defined(MT7916E) 
	char hecap[MAX_KEYWORD_LEN] = {0};
	int i = 0;
	wifi_get_config_fun config[16] = 
	{
		{"MuOfdmaDlEnable", 	wifimgr_lib_get_MuOfdmaDlEnable,	&(paraptr->common.MuOfdmaDlEnable)},
		{"MuOfdmaUlEnable", 	wifimgr_lib_get_MuOfdmaUlEnable,	&(paraptr->common.MuOfdmaUlEnable)},
		{"MuMimoDlEnable",		wifimgr_lib_get_MuMimoDlEnable, 	&(paraptr->common.MuMimoDlEnable)},
		{"MuMimoUlEnable",		wifimgr_lib_get_MuMimoUlEnable, 	&(paraptr->common.MuMimoUlEnable)},
		{"TWTSupport",			wifimgr_lib_get_TWTSupport, 		&(paraptr->common.TWTSupport)},
		{"SREnable",			wifimgr_lib_get_SREnable,			&(paraptr->common.SREnable)},
		{"SRMode",				wifimgr_lib_get_SRMode, 			&(paraptr->common.SRMode)},
		{"SRSDEnable",			wifimgr_lib_get_SRSDEnable, 		&(paraptr->common.SRSDEnable)},
		{"",					NULL,								NULL},
	};

	
	while(0 != strlen(config[i].attr_name))
		{
		memset(hecap, 0, sizeof(hecap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, hecap, sizeof(hecap)))
			{
			*(config[i].buf) = atoi(hecap);
			}
		i++;
		}
	
#endif
#endif
return;
	}

int hecap_compare(wifi_reload_config*paraptr,wifi_type type, int index)
{
#if defined(TCSUPPORT_WLAN_MULTI_CHIP) 
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID])	
	{
		char hecap[MAX_KEYWORD_LEN] = {0};
		int i = 0;
		wifi_get_config_fun config[16] = 
	{
			{"MuOfdmaDlEnable", 	wifimgr_lib_get_MuOfdmaDlEnable,	&(paraptr->common.MuOfdmaDlEnable)},
			{"MuOfdmaUlEnable", 	wifimgr_lib_get_MuOfdmaUlEnable,	&(paraptr->common.MuOfdmaUlEnable)},
			{"MuMimoDlEnable",		wifimgr_lib_get_MuMimoDlEnable, 	&(paraptr->common.MuMimoDlEnable)},
			{"MuMimoUlEnable",		wifimgr_lib_get_MuMimoUlEnable, 	&(paraptr->common.MuMimoUlEnable)},
			{"TWTSupport",			wifimgr_lib_get_TWTSupport, 		&(paraptr->common.TWTSupport)},
			{"SREnable",			wifimgr_lib_get_SREnable,			&(paraptr->common.SREnable)},
			{"SRMode",				wifimgr_lib_get_SRMode, 			&(paraptr->common.SRMode)},
			{"SRSDEnable",			wifimgr_lib_get_SRSDEnable, 		&(paraptr->common.SRSDEnable)},
			{"",					NULL,								NULL},
		};


		while(0 != strlen(config[i].attr_name))
		{
			memset(hecap, 0, sizeof(hecap));
			if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, hecap, sizeof(hecap)))
		{
				if (*(config[i].buf) != atoi(hecap))
				{
					return  NEED_RELOAD;
		}
			}
			i++;
		}
	}
#else
#if defined(MT7915N) || defined(MT7915E) || defined(MT7916N) || defined(MT7916E)
	char hecap[MAX_KEYWORD_LEN] = {0};
	int i = 0;
	wifi_get_config_fun config[16] = 
		{
		{"MuOfdmaDlEnable", 	wifimgr_lib_get_MuOfdmaDlEnable,	&(paraptr->common.MuOfdmaDlEnable)},
		{"MuOfdmaUlEnable", 	wifimgr_lib_get_MuOfdmaUlEnable,	&(paraptr->common.MuOfdmaUlEnable)},
		{"MuMimoDlEnable",		wifimgr_lib_get_MuMimoDlEnable, 	&(paraptr->common.MuMimoDlEnable)},
		{"MuMimoUlEnable",		wifimgr_lib_get_MuMimoUlEnable, 	&(paraptr->common.MuMimoUlEnable)},
		{"TWTSupport",			wifimgr_lib_get_TWTSupport, 		&(paraptr->common.TWTSupport)},
		{"SREnable",			wifimgr_lib_get_SREnable,			&(paraptr->common.SREnable)},
		{"SRMode",				wifimgr_lib_get_SRMode, 			&(paraptr->common.SRMode)},
		{"SRSDEnable",			wifimgr_lib_get_SRSDEnable, 		&(paraptr->common.SRSDEnable)},
		{"",					NULL,								NULL},
	};

	
	while(0 != strlen(config[i].attr_name))
	{
		memset(hecap, 0, sizeof(hecap));
		if (ECNT_WIFIMGR_SUCCESS == config[i].wifi_api(type, -1, hecap, sizeof(hecap)))
			{
			if (*(config[i].buf) != atoi(hecap))
				return  NEED_RELOAD;
			}		
		i++;
		}
#endif	
#endif
	return NO_NEED_RELOAD;
	}


void wds_store(wifi_reload_config*para,wifi_type type, int index)
{
	int i			= 0;
	char tmp[160]	= {0};
	char tmp2[160]	= {0};

	/*WdsEnable */
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdsenable(type, -1, tmp, sizeof(tmp));
	para->wds.WdsEnable = atoi(tmp);
	
	/*WdsEncrypType */
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdsencryptype(type, -1, tmp, sizeof(tmp));
	update_wifi_data(tmp);	
	strncpy((char *)para->wds.WdsEncrypType, tmp, sizeof(para->wds.WdsEncrypType) - 1);

	/*WdsKey */
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdskey(type, 1, tmp, sizeof(tmp));
	update_wifi_data(tmp);
	strncpy((char *)para->wds.WdsKey, tmp, sizeof(para->wds.WdsKey) - 1);

	/*Wds_MAC*/
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdslist(type, -1, tmp, sizeof(tmp));
	update_wifi_data(tmp);
	strncpy((char *)para->wds.Wds_MAC, tmp, sizeof(para->wds.Wds_MAC) - 1);

	/*main ssid wep key change
	if wds is not enable ,main ssid wep key change ,not reload wifi module
	if wds enable,main ssid wep key change, reload wifi module*/
	/*wds enable*/
	if(para->wds.WdsEnable != 0)
	{
		memset(tmp, 0, sizeof(tmp));
		wifimgr_lib_get_authMode(type, 1, tmp, sizeof(tmp));
		if(NULL != strstr(tmp, "WEP"))
		{
			for(i = 0; i < 4; i++)
	{
				memset(tmp, 0, sizeof(tmp));
				memset(tmp2, 0, sizeof(tmp2));
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				wifimgr_lib_get_keyStr(type, 1, i + 1, tmp, sizeof(tmp));
				update_wifi_data(tmp);
				strncpy((char *)para->wds.WepKeyStr[i], tmp, sizeof(para->wds.WepKeyStr[i]) - 1);
			}
		}
	}

	return;
	}

int wds_compare(wifi_reload_config*para, wifi_type type, int index)
	{
	int i			= 0;
	char tmp[MAX_KEYWORD_LEN]	= {0};
	char tmp2[MAX_KEYWORD_LEN]	= {0};

	/*WdsEnable */
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdsenable(type, -1, tmp, sizeof(tmp));
	if(atoi(tmp) != para->wds.WdsEnable)
		return NEED_RELOAD;
	
	/*WdsEncrypType */
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdsencryptype(type, -1, tmp, sizeof(tmp));
	update_wifi_data(tmp);
	if (0 != strcmp(tmp,(const char *)para->wds.WdsEncrypType) && para->wds.WdsEnable != 0)
		return NEED_RELOAD;

	/*WdsKey */
		memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdskey(type, 1, tmp, sizeof(tmp));
	update_wifi_data(tmp);
	if (0 != strcmp(tmp,(const char *)para->wds.WdsKey) && para->wds.WdsEnable != 0)
		return NEED_RELOAD;

	/*Wds_MAC*/
	memset(tmp, 0, sizeof(tmp));
	wifimgr_lib_get_wdslist(type, -1, tmp, sizeof(tmp));
	update_wifi_data(tmp);
	if (0 != strcmp(tmp,(const char *)para->wds.Wds_MAC) && para->wds.WdsEnable != 0)
		return NEED_RELOAD;

	/*main ssid wep key change
	if wds is not enable ,main ssid wep key change ,not reload wifi module
	if wds enable,main ssid wep key change, reload wifi module*/
	/*wds enable*/
	if(para->wds.WdsEnable != 0)
		{
		memset(tmp, 0, sizeof(tmp));
		wifimgr_lib_get_authMode(type, 1, tmp, sizeof(tmp));
		if(NULL != strstr(tmp, "WEP"))
			{
			for(i = 0; i < 4; i++)
				{
				memset(tmp, 0, sizeof(tmp));
				memset(tmp2, 0, sizeof(tmp2));
				snprintf(tmp2, sizeof(tmp2), "Key%dStr", i);
				wifimgr_lib_get_keyStr(type, 1, i + 1, tmp, sizeof(tmp));
				update_wifi_data(tmp);
				if (0 != strcmp(tmp,(const char *)para->wds.WepKeyStr[i]) && para->wds.WdsEnable != 0)
					return NEED_RELOAD;
			}
		}
			}

	return NO_NEED_RELOAD;
}

int wifimgr_lib_wifi_config_oper(wifi_type type, int action)
{
	char tmp[MAX_KEYWORD_LEN] = {0};
	int i = 0,reload = 0;
	wifi_reload_config *configptr = NULL;
	wifi_check_config_fun config[16] = 
	{
		{"systemreoot", 		systemreboot_store,			systemreboot_compare},
		{"commoncap", 			commoncap_store,			commoncap_compare},
		{"entrycap", 			entrycap_store,				entrycap_compare},
		{"htcap", 				htcap_store,				htcap_compare},
		{"vhtcap", 				vhtcap_store,				vhtcap_compare},
		{"hecap", 				hecap_store,				hecap_compare},
		{"wds", 				wds_store,					wds_compare},
		{"chipspecial", 		chipspecial_store,			chipspecial_compare},
		{"",					NULL,						NULL},
	};

	
	if (WIFI_2_4G == type)
			{
		configptr = &store_global[WIFI_2_4G];
	}
	else if (WIFI_5G == type)
				{
		configptr = &store_global[WIFI_5G];
				}
	else
	{
		goto retvalue; 
			}

	switch(action)
	{
		case INIT_PARA:
			memset(configptr, 0, sizeof(wifi_reload_config));
			if (WIFI_2_4G == type)
			{
				snprintf(configptr->itf, sizeof(configptr->itf),"%s", MTKINTERFACEPREFIX);
			}
			else if(WIFI_5G == type)
				{
				snprintf(configptr->itf, sizeof(configptr->itf),"%s", MTKINTERFACE5GPREFIX);
				}
			while(0 != strlen(config[i].cap_name))
			{
				memset(tmp, 0, sizeof(tmp));
				config[i].wifi_store(configptr,type, -1);
				i++;
			}
		break;

		case COMPARE_PARA:
		while(0 != strlen(config[i].cap_name))
		{
			memset(tmp, 0, sizeof(tmp));
			reload = config[i].wifi_compare(configptr,type, -1);
			//if any para need to reload,return directly
			if(reload)
			{
				goto retvalue;
		}
		i++;
	}
		break;
	
		default:
		break;
	}

retvalue:
	return reload;
}

int
wifimgr_lib_write_wifi_execute_sh(wifi_type type, int index)
{
	char tmp[MAX_KEYWORD_LEN]			= {0};
	char tmp2[MAX_KEYWORD_LEN]			= {0};
	char tmp3[MAX_KEYWORD_LEN]			= {0};
	char wirelessmode[MAX_KEYWORD_LEN]	= {0};
  	FILE *fp				= NULL;
	int i					= 0;
	int res 				= 0;
	char bssidnum[MAX_KEYWORD_LEN]		= {0};
	int txBurst				= 1;
	wifi_reload_config *configptr = NULL;
#if defined(TCSUPPORT_CT_WLAN_NODE)
	char tmpEncType[MAX_KEYWORD_LEN]	= {0};
#endif

	if (WIFI_2_4G == type)
	{
		fp = fopen(SVC_WLAN_SCRIPT_PATH, "w");
		configptr = &store_global[WIFI_2_4G];
	}
	else if (WIFI_5G == type)
	{
		fp = fopen(SVC_WLAN11AC_SCRIPT_PATH, "w");
		configptr = &store_global[WIFI_5G];
	}
	else
	{
		return -1;
	}

	if(NULL == fp)
	{
		return -1;
	}

#ifndef TCSUPPORT_ECNT_MAP
	/*AccessPolicy=....*/
	if (ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_accessPolicy(type, index + 1, tmp, sizeof(tmp)))
	{
		fprintf(fp, "/userfs/bin/iwpriv %s%d set AccessPolicy=%s\n", configptr->itf, index, tmp);
	}

	/*AccessControlList=MAC1;MAC2;.......*/
	if (ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_accessControlList(type, index + 1, tmp, sizeof(tmp)))
	{
		fprintf(fp, "/userfs/bin/iwpriv %s%d set ACLClearAll=1\n", configptr->itf, index);
		fprintf(fp, "/userfs/bin/iwpriv %s%d set ACLAddEntry=%s\n", configptr->itf, index, tmp);
	}
#endif

	if (ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_hideSSID(type, index + 1, tmp, sizeof(tmp)))
	{
		fprintf(fp, "/userfs/bin/iwpriv %s%d set HideSSID=%s\n", configptr->itf, index, tmp);
	}

	/*AuthMode*/
 	if (ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_authMode(type, index + 1, tmp, sizeof(tmp)))
	{
#if defined(TCSUPPORT_CT_WLAN_NODE)
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_encrypType(type, index + 1, tmpEncType, sizeof(tmpEncType))) {			
			fclose(fp);
			RETURN_WIFIMGR_FAIL;
		}	
			if ((strstr(tmp, "WEP")) || (strstr(tmp, "OPEN") && strstr(tmpEncType, "WEP")) ||(strstr(tmp, "SHARED") && strstr(tmpEncType, "WEP")))
			{
#if defined(TCSUPPORT_WLAN_8021X)
				/*Radius-WEP64, Radius-WEP128*/
				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ieee8021x(type, index + 1, tmp3, sizeof(tmp3)) && !strcmp(tmp3, "1"))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=1\n", configptr->itf, index);
					
					if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusserver(type, index + 1, tmp2, sizeof(tmp2)))
					{
						fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Server=%s\n", configptr->itf, index, tmp2);
					}

					if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusport(type, index + 1, tmp2, sizeof(tmp2)))
					{
						fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Port=%s\n", configptr->itf, index, tmp2);
					}

					if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiuskey(type, index + 1, tmp2, sizeof(tmp2)))
					{
						fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Key=%s\n", configptr->itf, index, tmp2);
					}
			
					if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rekeyInterval(type, index + 1, tmp2, sizeof(tmp2)))
					{
						fprintf(fp, "/userfs/bin/iwpriv %s%d set RekeyInterval=%s\n", configptr->itf, index, tmp2);
					}
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=0\n", configptr->itf, index);
				}
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=%s\n", configptr->itf, index, tmp);
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
				}
#else
#if defined(MT7592)
				if (WIFI_2_4G == type)
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=%s\n", configptr->itf, index, tmp);
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
				}
#else
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
#endif
#endif
				fprintf(fp, "/userfs/bin/iwpriv %s%d set EncrypType=WEP\n", configptr->itf, index);
				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_defaultKeyID(type, index + 1, tmp, sizeof(tmp)))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set DefaultKeyID=%s\n", configptr->itf, index, tmp);
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set DefaultKeyID=1\n", configptr->itf, index);
				}
				/*key1~key4*/
				for(i = 1; i <= 4; i++)
				{
					if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_keyStr(type, index + 1, i, tmp, sizeof(tmp)))
					{
						/*check strlen because the key maybe empty*/
						if(strlen(tmp) > 0)
						{
							update_wifi_data(tmp);
							specialCharacterHandle(tmp);
							snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
							snprintf(tmp, sizeof(tmp), "Key%d", i);
							fprintf(fp, "/userfs/bin/iwpriv %s%d set %s=%s\n", configptr->itf, index, tmp, tmp2);
						}
					}
				}
			}
#else	

		/*WEP64-bits or WEP128-bits*/
		if (strstr(tmp, "WEP"))
		{
#if defined(TCSUPPORT_WLAN_8021X)
			/*Radius-WEP64, Radius-WEP128*/
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ieee8021x(type, index + 1, tmp3, sizeof(tmp3)) && !strcmp(tmp3, "1"))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=1\n", configptr->itf, index);
				
				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusserver(type, index + 1, tmp2, sizeof(tmp2)))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Server=%s\n", configptr->itf, index, tmp2);
				}

				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusport(type, index + 1, tmp2, sizeof(tmp2)))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Port=%s\n", configptr->itf, index, tmp2);
				}

				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiuskey(type, index + 1, tmp2, sizeof(tmp2)))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Key=%s\n", configptr->itf, index, tmp2);
				}
			
				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rekeyInterval(type, index + 1, tmp2, sizeof(tmp2)))
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set RekeyInterval=%s\n", configptr->itf, index, tmp2);
				}
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=0\n", configptr->itf, index);
			}
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
			if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=WEPAUTO\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=WEPAUTO\n", configptr->itf, index);
			}
#else
#if defined(MT7592)
			if (WIFI_2_4G == type)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=WEPAUTO\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=WEPAUTO\n", configptr->itf, index);
			}
#else
			fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=WEPAUTO\n", configptr->itf, index);
#endif
#endif
			fprintf(fp, "/userfs/bin/iwpriv %s%d set EncrypType=WEP\n", configptr->itf, index);
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_defaultKeyID(type, index + 1, tmp, sizeof(tmp)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set DefaultKeyID=%s\n", configptr->itf, index, tmp);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set DefaultKeyID=1\n", configptr->itf, index);
			}
			/*key1~key4*/
			for(i = 1; i <= 4; i++)
			{
				if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_keyStr(type, index + 1, i, tmp, sizeof(tmp)))
				{
					/*check strlen because the key maybe empty*/
					if(strlen(tmp) > 0)
					{
						update_wifi_data(tmp);
						specialCharacterHandle(tmp);
						snprintf(tmp2, sizeof(tmp2), "\"%s\"", tmp);/*need "..."*/
						snprintf(tmp, sizeof(tmp), "Key%d", i);
						fprintf(fp, "/userfs/bin/iwpriv %s%d set %s=%s\n", configptr->itf, index, tmp, tmp2);
					}
				}
			}
    	}
#endif
		else if(!strcmp(tmp, "WPAPSK") || !strcmp(tmp, "WPA2PSK") || !strcmp(tmp, "WPAPSKWPA2PSK") /*WPAPSK or WPA2PSK or Mixed mode*/
#if defined(TCSUPPORT_WLAN_8021X)
						|| !strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#endif
#if defined(TCSUPPORT_WLAN_WPA3)
#if defined(TCSUPPORT_WLAN_8021X)
						|| !strcmp(tmp, "WPA3")	|| !strcmp(tmp, "WPA3-192")
#endif
						||!strcmp(tmp, "WPA3PSK")||!strcmp(tmp, "WPA2PSKWPA3PSK")
#endif
			)
		{/*WPAPSK or WPA2PSK or Mixed mode*/
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
			if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=%s\n",configptr->itf, index, tmp);
			}
			else
			{
			fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
			}
#else
#if defined(MT7592)
			if (WIFI_2_4G == type)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=%s\n", configptr->itf, index, tmp);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
			}
#else
			fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=%s\n", configptr->itf, index, tmp);
#endif
#endif
#if defined(TCSUPPORT_WLAN_8021X)
			wifimgr_lib_get_IEEE8021X_VAL(tmp2, sizeof(tmp2));
			if(!strcmp(tmp, "WPA") || !strcmp(tmp, "WPA2") || !strcmp(tmp, "WPA1WPA2")
#if defined(TCSUPPORT_WLAN_WPA3)
					|| !strcmp(tmp, "WPA3")	|| !strcmp(tmp, "WPA3-192")
#endif
			)
			{/*Radius-WEP64, Radius-WEP128*/
				fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=%s\n", configptr->itf, index, tmp2);
			}
			else if (0 == strcmp(tmp2, "1"))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=0\n", configptr->itf, index);
			}

			
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusserver(type, index + 1, tmp2, sizeof(tmp2)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Server=%s\n", configptr->itf, index, tmp2);
			}

			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusport(type, index + 1, tmp2, sizeof(tmp2)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Port=%s\n", configptr->itf, index, tmp2);
			}

			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiuskey(type, index + 1, tmp2, sizeof(tmp2)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set RADIUS_Key=%s\n", configptr->itf, index, tmp2);
			}
			
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rekeyInterval(type, index + 1, tmp2, sizeof(tmp2)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set RekeyInterval=%s\n", configptr->itf, index, tmp2);
			}
#endif
			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_encrypType(type, index + 1, tmp, sizeof(tmp)))
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set EncrypType=%s\n", configptr->itf, index, tmp);
			}

			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ssid(type, index + 1, tmp, sizeof(tmp)))
			{
				update_wifi_data(tmp);
				specialCharacterHandle(tmp);
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
				if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set SSIDSingle=\"%s\"\n", configptr->itf, index, tmp);
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n", configptr->itf, index, tmp);
				}
#else
#if defined(MT7592)
				if (WIFI_2_4G == type)
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set SSIDSingle=\"%s\"\n", configptr->itf, index, tmp);
				}
				else
				{
					fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n", configptr->itf, index, tmp);
				}
					
#else
				fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n", configptr->itf, index, tmp);
#endif
#endif
			}

			if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WPAPSK(type, index + 1, tmp, sizeof(tmp)))
			{
				update_wifi_data(tmp);
				specialCharacterHandle(tmp);
				fprintf(fp, "/userfs/bin/iwpriv %s%d set WPAPSK=\"%s\"\n", configptr->itf, index, tmp);
			}
			
			fprintf(fp, "/userfs/bin/iwpriv %s%d set DefaultKeyID=2\n", configptr->itf, index);
		}
		else
		{/*OPEN*/
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
			if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=OPEN\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=OPEN\n", configptr->itf, index);
			}
#else
#if defined(MT7592)
			if (WIFI_2_4G == type)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthModeSingle=OPEN\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=OPEN\n", configptr->itf, index);
			}
#else
			fprintf(fp, "/userfs/bin/iwpriv %s%d set AuthMode=OPEN\n", configptr->itf, index);
#endif
#endif
			fprintf(fp, "/userfs/bin/iwpriv %s%d set EncrypType=NONE\n", configptr->itf, index);
#if defined(TCSUPPORT_WLAN_8021X)
			fprintf(fp, "/userfs/bin/iwpriv %s%d set IEEE8021X=0\n", configptr->itf, index);
#endif
		}
 	}

#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID]){
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_maxStaNum(type, index + 1, tmp, sizeof(tmp)))
		{
			wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(type, tmp2, sizeof(tmp2));
			fprintf(fp, "/userfs/bin/iwpriv %s%d set %s=%s\n", configptr->itf, index, tmp2, tmp);
		}
	}
	else{
		if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_mbssMaxStaNum(type, index + 1, tmp, sizeof(tmp)))
		{
			wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(type, tmp2, sizeof(tmp2));
			fprintf(fp, "/userfs/bin/iwpriv %s%d set %s=%s\n", configptr->itf, index, tmp2, tmp);
		}
	}
#else
#if defined(MT7592)
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_maxStaNum(type, index + 1, tmp, sizeof(tmp)))
#else
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_mbssMaxStaNum(type, index + 1, tmp, sizeof(tmp)))
#endif
	{
		wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(type, tmp2, sizeof(tmp2));
		fprintf(fp, "/userfs/bin/iwpriv %s%d set %s=%s\n", configptr->itf, index, tmp2, tmp);
	}
#endif
#endif
	if(wifimgr_lib_get_wirelessMode(type, -1, wirelessmode, sizeof(wirelessmode)) != 0)
	{
		printf("write_wlan_config:wifimgr_lib_get_wirelessMode fail.\n");
	}
	if (WIFI_2_4G == type)
	{
		if (atoi(wirelessmode) != 6 && atoi(wirelessmode) != 7
			&& atoi(wirelessmode) != 9 && atoi(wirelessmode) != 16)
		{
			if(wifimgr_lib_get_bssidnum(type, -1, bssidnum, sizeof(bssidnum)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_bssidnum fail.\n");
			}
			if(wifimgr_lib_get_wmmCapable(type, index + 1, tmp, sizeof(tmp)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_wmmCapable fail.\n");
			}
			fprintf(fp, "/userfs/bin/iwpriv %s%d set WmmCapable=%s\n", configptr->itf, index, tmp);
			for (i = 0; i < atoi(bssidnum); i++)
			{
				wifimgr_lib_get_wmmCapable(type, i + 1, tmp, sizeof(tmp));
//				if (1 == atoi(tmp))
//				{
//					txBurst = 0;
//					break;
//				}
			}

			if (1 == txBurst)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=1\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=0\n", configptr->itf, index);
			}
		}
	}
	else if (WIFI_5G == type)
	{
		if (atoi(wirelessmode) != 8 && atoi(wirelessmode) != 14
			&& atoi(wirelessmode) != 15 && atoi(wirelessmode) != 17)
		{
			if(wifimgr_lib_get_bssidnum(type, -1, bssidnum, sizeof(bssidnum)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_bssidnum fail.\n");
			}
			if(wifimgr_lib_get_wmmCapable(type, index + 1, tmp, sizeof(tmp)) != 0)
			{
				printf("write_wlan_config:wifimgr_lib_get_wmmCapable fail.\n");
			}
			fprintf(fp, "/userfs/bin/iwpriv %s%d set WmmCapable=%s\n", configptr->itf, index, tmp);
			for (i = 0; i < atoi(bssidnum); i++)
			{
				wifimgr_lib_get_wmmCapable(type, i + 1, tmp, sizeof(tmp));
//				if (1 == atoi(tmp))
//				{
//					txBurst = 0;
//					break;
//				}
			}

#ifndef MT7612E /*don't need to set TxBurst=0 when test WMM for 7612 3.0.4.0*/
			if (1 == txBurst)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=1\n", configptr->itf, index);
			}
			else
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=0\n",  configptr->itf, index);
			}
#else
			if (1 == txBurst)
			{
				fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=1\n",  configptr->itf, index);
			}
#endif
		}
		else
		{
			fprintf(fp, "/userfs/bin/iwpriv %s%d set WmmCapable=1\n",  configptr->itf, index);
			fprintf(fp, "/userfs/bin/iwpriv %s%d set TxBurst=1\n",  configptr->itf, index);
		}
	}

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ssid(type, index + 1, tmp, sizeof(tmp)))
	{
		update_wifi_data(tmp);
		specialCharacterHandle(tmp);
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WIFI_2_4G == type && WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
		{
			fprintf(fp, "/userfs/bin/iwpriv %s%d set SSIDSingle=\"%s\"\n",  configptr->itf, index, tmp);
		}
		else
		{
		fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n",  configptr->itf, index, tmp);
		}
#else
#if defined(MT7592)
		if (WIFI_2_4G == type)
		{
			fprintf(fp, "/userfs/bin/iwpriv %s%d set SSIDSingle=\"%s\"\n", configptr->itf, index, tmp);
		}
		else
		{
			fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n", configptr->itf, index, tmp);
		}

#else
		fprintf(fp, "/userfs/bin/iwpriv %s%d set SSID=\"%s\"\n",  configptr->itf, index, tmp);
#endif
#endif		
	}

	fclose(fp);
	if (WIFI_2_4G == type)
	{
		res = chmod(SVC_WLAN_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);
	}
	else if (WIFI_5G == type)
	{
		res = chmod(SVC_WLAN11AC_SCRIPT_PATH, S_IRUSR|S_IWUSR|S_IXUSR);
	}

	return res;

}

