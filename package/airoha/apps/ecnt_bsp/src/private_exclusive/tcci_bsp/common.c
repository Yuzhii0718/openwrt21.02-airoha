#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "blapi_system.h"
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
#include "xpon_public_const.h"
#endif
#endif


#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
typedef	struct {
	char *name;		
	unsigned char  mode;
} PonModeCmds_t;

const PonModeCmds_t onu_xpon_mode[] = {
/*	name,			mode,								 */
	{"auto_mode",	XMCS_IF_WAN_DETECT_MODE_AUTO},
	{"gpon",	    XMCS_IF_WAN_DETECT_MODE_GPON},
	{"xepon_asy",	XMCS_IF_WAN_DETECT_MODE_10G_1G_EPON},
	{"xepon_sym",	XMCS_IF_WAN_DETECT_MODE_10G_10G_EPON},
	{"xepon_1g",	XMCS_IF_WAN_DETECT_MODE_1G_1G_EPON},
	{"xgpon",	    XMCS_IF_WAN_DETECT_MODE_XGPON},
	{"xgspon",	    XMCS_IF_WAN_DETECT_MODE_XGSPON},
	{"ng2_10g_sym",	XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_10G},
	{"ng2_10g_asy",	XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_2G},
	{"ng2_2g_sym",	XMCS_IF_WAN_DETECT_MODE_NGPON2_2G_2G},
	{"gpon_sym",	    XMCS_IF_WAN_DETECT_MODE_GPON_SYM},
	{"turbo_epon",	XMCS_IF_WAN_DETECT_MODE_TURBO_EPON},
	/* mode should be added before this line!  */
	{NULL, 0 }
};
#endif
#endif

/*
[1:0], 0x0--qdma_lan payload size 2k, 0x1--qdma_lan 1k, 0x2--qdma_lan 512, 0x3--qdma_lan 256
[2], rsv
[3], 0x0--qdma_lan dscp in DRAM, 0x1--qdma_lan dscp in SRAM
[5:4], 0x0--qdma_wan payload size 2k, 0x1--qdma_wan 1k, 0x2--qdma_wan 512, 0x3--qdma_wan 256
[6], rsv
[7], 0x0--qdma_wan dscp in DRAM, 0x1--qdma_wan dscp in SRAM
[8],fast wan
[9],fast lan
[10],fast xsi_pcie0
[11],fast xsi_pcie1
[12],fast xsi_usb
[13],fast xsi_ether
[14],fast xsi_pon
*/
typedef union
{
    struct
    {
        unsigned int lan_payload_size:2;
        unsigned int rsv:1;
        unsigned int lan_dscp_mode:1;
        unsigned int wan_payload_size:2;
        unsigned int rsv2:1;
        unsigned int wan_dscp_mode:1;
        unsigned int fast_wan:1;
        unsigned int fast_lan:1;
        unsigned int fast_xsi_pcie0:1;
        unsigned int fast_xsi_pcie1:1;
        unsigned int fast_xsi_usb:1;
        unsigned int fast_xsi_ether:1;
        unsigned int fast_xsi_pon:1;
		unsigned int fqos_xsi_pcie0:1;
		unsigned int fqos_xsi_pcie1:1;
		unsigned int fqos_xsi_usb:1;
		unsigned int fqos_xsi_ether:1;
		unsigned int fqos_xsi_pon:1;
		unsigned int auto_xsi_pcie0:1;
		unsigned int auto_xsi_pcie1:1;
		unsigned int auto_xsi_usb:1;
		unsigned int auto_xsi_ether:1;
		unsigned int auto_xsi_pon:1;
        unsigned int rsv3:7;
    };
    unsigned int value;
}qdma_init_value;


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


static char *str_encrypt_ext_ascii(char *src)
{
	unsigned int len = strlen(src);
	unsigned int i = 0;
	unsigned int itmp = 0;

	for (i = 0;i < len;i++)
	{
		if (!isxdigit(src[i]))
			continue;
		
		sscanf(&src[i], "%1x", &itmp);
		itmp &= 0x0f;		/* lower 4 bits store the hex digit */
		itmp |= 1 << 7;		/* extern ascii flag */
		itmp |= i << 4;		/* index of current character */
		src[i] = itmp;
	}

	return src;
}


static int parse_qdmainit_arg(qdma_init_value* p_val,unsigned int* p_val_mask,int argc,char *argv[])
{
    int i = 0;
    unsigned int mask = 0;

    if(0 == argc%2)
        return -1;
    
    for(i = 1;i < argc;i+=2)
    {
        if(!strcmp(argv[i],"lan_dscp_mode")){
            mask |= QDMA_MASK_LAN_DSCP;
            p_val->lan_dscp_mode = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"lan_payload_size")){
            mask |= QDMA_MASK_LAN_PAYLOAD;
            p_val->lan_payload_size = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"wan_dscp_mode")){
            mask |= QDMA_MASK_WAN_DSCP;
            p_val->wan_dscp_mode = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"wan_payload_size")){
            mask |= QDMA_MASK_WAN_PAYLOAD;
            p_val->wan_payload_size = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"wan_fastpath")){
            mask |= QDMA_MASK_FAST_WAN;
            p_val->fast_wan = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"lan_fastpath")){
            mask |= QDMA_MASK_FAST_LAN;
            p_val->fast_lan = atoi(argv[i+1]);
        }else if(!strcmp(argv[i],"xsi_pcie0_fastpath")){
        	if(atoi(argv[i+1]) == 3){
				mask |= QDMA_MASK_AUTO_XSI_PCIE0;
            	p_val->auto_xsi_pcie0 = 1;
			}else if(atoi(argv[i+1]) == 2){
				mask |= QDMA_MASK_FQOS_XSI_PCIE0;
				p_val->fqos_xsi_pcie0= 1;
				mask |= QDMA_MASK_FAST_XSI_PCIE0;
            	p_val->fast_xsi_pcie0= 1;
			}else{
				mask |= QDMA_MASK_AUTO_XSI_PCIE0;
            	p_val->auto_xsi_pcie0 = 0;
            mask |= QDMA_MASK_FAST_XSI_PCIE0;
            p_val->fast_xsi_pcie0 = atoi(argv[i+1]);
			}
        }else if(!strcmp(argv[i],"xsi_pcie1_fastpath")){
        	if(atoi(argv[i+1]) == 3){
				mask |= QDMA_MASK_AUTO_XSI_PCIE1;
            	p_val->auto_xsi_pcie1 = 1;
			}if(atoi(argv[i+1]) == 2){
				mask |= QDMA_MASK_FQOS_XSI_PCIE1;
				p_val->fqos_xsi_pcie1 = 1;
				mask |= QDMA_MASK_FAST_XSI_PCIE1;
            	p_val->fast_xsi_pcie1 = 1;
			}else{
				mask |= QDMA_MASK_AUTO_XSI_PCIE1;
            	p_val->auto_xsi_pcie1 = 0;
            mask |= QDMA_MASK_FAST_XSI_PCIE1;
            p_val->fast_xsi_pcie1 = atoi(argv[i+1]);
			}
        }else if(!strcmp(argv[i],"xsi_usb_fastpath")){
        	if(atoi(argv[i+1]) == 3){
				mask |= QDMA_MASK_AUTO_XSI_USB;
            	p_val->auto_xsi_usb = 1;
			}if(atoi(argv[i+1]) == 2){
				mask |= QDMA_MASK_FQOS_XSI_USB;
				p_val->fqos_xsi_usb = 1;
				mask |= QDMA_MASK_FAST_XSI_USB;
            	p_val->fast_xsi_usb = 1;
			}else{
				mask |= QDMA_MASK_AUTO_XSI_USB;
            	p_val->auto_xsi_usb = 0;
            mask |= QDMA_MASK_FAST_XSI_USB;
            p_val->fast_xsi_usb = atoi(argv[i+1]);
			}
        }else if(!strcmp(argv[i],"xsi_ether_fastpath")){
        	if(atoi(argv[i+1]) == 3){
				mask |= QDMA_MASK_AUTO_XSI_ETHER;
            	p_val->auto_xsi_ether = 1;
			}else if(atoi(argv[i+1]) == 2){
				mask |= QDMA_MASK_FQOS_XSI_ETHER;
				p_val->fqos_xsi_ether = 1;
				mask |= QDMA_MASK_FAST_XSI_ETHER;
            	p_val->fast_xsi_ether = 1;
			}else{
				mask |= QDMA_MASK_AUTO_XSI_ETHER;
            	p_val->auto_xsi_ether = 0;
            mask |= QDMA_MASK_FAST_XSI_ETHER;
            p_val->fast_xsi_ether = atoi(argv[i+1]);
			}
        }else if(!strcmp(argv[i],"xsi_pon_fastpath")){
        	if(atoi(argv[i+1]) == 3){
				mask |= QDMA_MASK_AUTO_XSI_PON;
            	p_val->auto_xsi_pon = 1;
			}else if(atoi(argv[i+1]) == 2){
				mask |= QDMA_MASK_FQOS_XSI_PON;
				p_val->fqos_xsi_pon = 1;
				mask |= QDMA_MASK_FAST_XSI_PON;
            	p_val->fast_xsi_pon = 1;
			}else{
				mask |= QDMA_MASK_AUTO_XSI_PON;
            	p_val->auto_xsi_pon = 0;
            mask |= QDMA_MASK_FAST_XSI_PON;
            p_val->fast_xsi_pon = atoi(argv[i+1]);
			}
        }else if(!strcmp(argv[i],"whole_value")){
            mask |= QDMA_MASK_WHOLE_VALUE;
            sscanf(argv[i+1],"%x",&(p_val->value));
        }
        else{
            printf("parameter %s invalid\n",argv[i]);
            return -1;
        }
    }

    if(0 == mask)
        return -1;

    *p_val_mask = mask;
    return 0;
}


int dummy(int argc, char *argv[]){
    printf("I am dummy!!\n");
    return 0;
}


int doSysMac(int argc, char *argv[]){
    unsigned char len, tmp[3], i;
    unsigned char mac_addr[6] = {0};
    int rebootFlag = 1;
    unsigned char mac_range = 8;

    len = strlen(argv[1]);
	if (len != 12) {
		printf("mac address must be 12 digits\n");
		return -1;
	}
	#ifdef TCSUPPORT_INIC_HOST
		/* in iNIC mode, the iNIC host mac address must be even,
		 * so we check the last digit that must be 0x30, 0x32, ... 0x38.
		 */
		if ((argv[1][11] & 0x01) != 0) {
			printf("In FTTdp, Host MAC address must be even\n");
			return -1;
		}
	#endif
	if(argc == 3){
		if (strncmp(argv[2], "-n", 2) == 0)
			rebootFlag = 0;
		else
			mac_range = atoi(argv[2]);
	}else if(argc == 4){
		mac_range = atoi(argv[2]);
		if (strncmp(argv[3], "-n", 2) == 0)
			rebootFlag = 0;
		else
			goto sysmac_exit;
	}else if(argc > 4){
		goto sysmac_exit;
	}

    /* Modify the MAC address in the bootloader */  
	tmp[2] = 0;
    for(i = 0; i < 6; i++){
		tmp[0] = argv[1][2*i];
		tmp[1] = argv[1][2*i+1];
		mac_addr[i] = (unsigned char)strtoul(tmp, NULL, 16);
	}
	
	printf("new mac addr = ");
    for(i = 0; i < 5; i++)    
        printf("%02x:", mac_addr[i]);
    printf("%02x\n", mac_addr[5]);
	
	if(!is_valid_ether_addr(mac_addr)){
		printf("MAC addr is not valid!!!!!\n");
		return 0;
	}
    
	return blapi_system_doSysMac(mac_addr, mac_range, rebootFlag);

sysmac_exit:
    printf("sys mac <mac_addr> / sys mac <mac_addr> <-n> / sys mac <mac_addr> <mac_range> / sys mac <mac_addr> <mac_range> <-n> \r\n");

	return -1;
}


/**************************************************
ENUM_SYSTEM_ONU_TYPE_SFU				=1
ENUM_SYSTEM_ONU_TYPE_HGU				=2
store onu type to flash bfc0ff9c
***************************************************/
int doSysOnuType(int argc, char *argv[]){
    unsigned char type = 0;
	if(NULL == argv[1]){
		printf("Bad command!\nUsage:sys onutype sfu|hgu \n");
		return -1;
	}
	if(!strcmp(argv[1],"sfu"))	{
		type = 1;//sfu
	}else if(!strcmp(argv[1],"hgu"))	{
		type = 2;//hgu
	}else{
		printf("Bad command!\nUsage:sys onutype sfu|hgu \n");
		return -1;
	}
		
	return blapi_system_doSysOnuType(type);
}


int doSysBobCarlibration(int argc, char *argv[]){

    int ret = -1;

	if(argc>=2){
		printf("Usage:\nwrite bob data to flash from UX: sys bobcarlibration\n");
		return -1;
	}else if(argc==1)
	{
		ret = blapi_system_doSysBobCarlibration();
	}
	return 0;
}


int doSysBootFlag(int argc, char *argv[])
{
	char boot_flag = -1;
	int ret = -1;
	
    if ((argc != 2) || (strcmp(argv[1], "read") != 0 && strcmp(argv[1], "swap") != 0 && strcmp(argv[1], "checksum") != 0)){
		printf("Usage:\nread/swap bootflag: sys bootflag read/swap/checksum\n");
		return -1;
	}

	if (!strcmp(argv[1], "swap")) {
		blapi_system_set_boot_flag_swap();
	}
	else if (!strcmp(argv[1], "checksum"))
	{
		ret = blapi_system_check_crcflag();
		if(-1 == ret){
			return 0;
		}
	}

	ret = blapi_system_get_current_boot_flag(&boot_flag);
	printf("\r\ncurrent boot_flag = %d\n", boot_flag);

	return 0;
}


/*
* this command will call "sifm" command, usage bellow:
* "sys sifm flag[1/0] ...", the flag indicate whether the result
* will be encrypted, others are the same as "sifm" command
*/
int doSysSifm(int argc, char *argv[]) 
{
	FILE *proc_file = NULL;
	char buf[100] = {0};
	unsigned int i = 0;
	int loglvl = 0;
	int len = 0;

	if (argc < 9)
		goto ERROR;

	/* save and change log level */
	proc_file = fopen("/proc/sys/kernel/printk", "r");
	if (!proc_file)
		goto ERROR;

	fgets(buf, sizeof(buf), proc_file);
	fclose(proc_file);
	
	sscanf(buf, "%1d", &loglvl);
	system("echo 1 > /proc/sys/kernel/printk");

	/* call "sifm ..." command */
	proc_file = fopen("/proc/tc3162/tcci_cmd", "w");
	if (!proc_file)
		goto ERROR;

	sprintf(buf, "sifm");
	for (i = 2; i < argc; i++) {
		len = sizeof(buf) - strlen(buf) - 1;
		if(len < 0) {
			fclose(proc_file);
			goto ERROR;
		}
		strncat(buf, " ", (size_t)len);
		len = sizeof(buf) - strlen(buf) - 1;
		if(len < 0) {
			fclose(proc_file);
			goto ERROR;
		}
		strncat(buf, argv[i], (size_t)len);
	}
	
	fprintf(proc_file, "%s", buf);
	fclose(proc_file);

	/*for coverity(CID:188313), avoid the tained varible affecting system command*/
	/*sprintf(buf, "echo %d > /proc/sys/kernel/printk", loglvl);
	system(buf);*/    
	proc_file = fopen("/proc/sys/kernel/printk", "w");
	if (!proc_file)
		goto ERROR;
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", loglvl);
	fputs(buf, proc_file);
	fclose(proc_file);

	/* read result */
	proc_file = fopen("/proc/tc3162/sifm_info", "r");
	if (!proc_file)
		goto ERROR;

	memset(buf, 0, sizeof(buf));
	if (NULL == fgets(buf, sizeof(buf), proc_file)){
		fclose(proc_file);
		goto ERROR;
	}
	
	fclose(proc_file);
	
	if (atoi(argv[1]))
		str_encrypt_ext_ascii(buf);

	if (strlen(buf) > 0 && strlen(buf) <= 8)
	{
		printf("\x02");	/* start */
		printf("%s", buf);
		printf("\x03\n");	/* end */
		return 0;
	}

ERROR:	
	printf("\x17\n");	/* 00011000 indicate error */
	return -1;
}


#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
int doSysPONMode(int argc, char *argv[]){
#if 0
    unsigned char onuType = 0;
#endif
	unsigned char ponMode = 0;
    unsigned char i = 0;
#if 0
    FILE *fp = NULL; 
	size_t bytes_read;
	int ret = -1;
	long addr = 0;
	unsigned char tmp[256] = {0};
#endif
	unsigned char wanRamMode = 0, wanPayloadMode = 0;

	if(NULL == argv[1]){
		printf("Bad command!\nUsage:sys onuMode auto_mode|gpon|xepon_1g|xepon_asy|xepon_sym|xgpon|xgspon|ng2_10g_sym|ng2_10g_asy|ng2_2g_sym|gpon_sym|turbo_epon\n");
		return -1;
	}
    
    while(onu_xpon_mode[i].name != NULL){   
        if(!strcmp(argv[1],onu_xpon_mode[i].name)){
            ponMode = onu_xpon_mode[i].mode;
            break;
        }
        i++;
    }
    if(onu_xpon_mode[i].name == NULL){
        printf("Bad command!\nUsage:sys onuMode auto_mode|gpon|xepon_1g|xepon_asy|xepon_sym|xgpon|xgspon|ng2_10g_sym|ng2_10g_asy|ng2_2g_sym|gpon_sym|turbo_epon\n");
        return -1;
    }
    
	if(!strcmp(argv[1],"xepon_sym") || !strcmp(argv[1],"xepon_asy") || !strcmp(argv[1],"xgspon") || !strcmp(argv[1],"xgpon")){
			wanRamMode = 1;
			wanPayloadMode = 3;
	}else{
		wanRamMode = 0;
		wanPayloadMode = 3;
	}
	
	return blapi_system_doSysPONMode(ponMode, wanRamMode, wanPayloadMode, 0);
 
}
#endif
#endif


int doSysQdmaInit(int argc, char *argv[]){
    unsigned int qdmaValue =0, qdmaMask = 0;
    qdma_init_value val;
    unsigned int val_mask = 0;

    memset(&val,0,sizeof(qdma_init_value));
	if(parse_qdmainit_arg(&val,&val_mask,argc,argv)){
		goto qdma_init_error;
	}
    
    qdmaValue = val.value;
	qdmaMask = val_mask;

    return blapi_system_doSysQdmaInit_int32(qdmaValue, qdmaMask);

qdma_init_error:
	printf("Usage:sys qdmainit [para1] [value1] [para2] [value2]...[paraN] [valueN]\n");
    printf("parameter:\nlan_dscp_mode [0-dram 1-sram]\nlan_payload_size [0-2048 1-1024 2-512 3-256]\n");
    printf("wan_dscp_mode [0-dram 1-sram]\nwan_payload_size [0-2048 1-1024 2-512 3-256]\n");
    printf("support blow parameters in arm:\n");
    printf("wan_fastpath [0/1 - hwnat fastpath to GDM2(ATM/PTM/PON..)]\n");
    printf("lan_fastpath [0/1 - hwnat fastpath to GDM1(GSW)]\n");
    printf("xsi_pcie0_fastpath [0/1 - hwnat fastpath to HSGMII PCIE0; 2 - direct to GDM; 3 - auto mode]\n");
    printf("xsi_pcie1_fastpath [0/1 - hwnat fastpath to HSGMII PCIE1; 2 - direct to GDM; 3 - auto mode]\n");
    printf("xsi_usb_fastpath [0/1 - hwnat fastpath to HSGMII USB; 2 - direct to GDM; 3 - auto mode]\n");
    printf("xsi_ether_fastpath [0/1 - hwnat fastpath to HSGMII/XFI ETHER; 2 - direct to GDM; 3 - auto mode]\n");
    printf("xsi_pon_fastpath [0/1 - hwnat fastpath to HSGMII/XFI PON; 2 - direct to GDM; 3 - auto mode]\n");
    printf("whole_value [32bit whole value in hex]\n");
	
	return -1;
}


int doSysSerdesSel(int argc, char *argv[])
{
	return blapi_system_doSysSerdesSel(argc, argv);
}

#ifdef TCSUPPORT_BOARD_SELECT
int doSysBoardInfo(int argc, char *argv[])
{
	if(strncmp(argv[1], "show", 4) == 0) {
		return blapi_system_show_board_info(argc, argv);
	} else if(strncmp(argv[1], "cfg", 3) == 0) {
		return blapi_system_cfg_board_info(argc, argv);
	} else {
		printf("Bad command!\nUsage:sys boardinfo show|cfg \n");
		return -1;
	}
}
#endif

int doSysBBF247(int argc, char *argv[]){
    unsigned char enable = 0;
	if(NULL == argv[1]){
		printf("Bad command!\nUsage:sys bbf247 enable|disable \n");
		return -1;
	}
	if(!strcmp(argv[1],"enable"))	{
		enable = 1;
	}else if(!strcmp(argv[1],"disable"))	{
		enable = 0;
	}else{
		printf("Bad command!\nUsage:sys bbf247 enable|disable \n");
		return -1;
	}
		
	return blapi_system_doSysBBF247(enable);
}

int doSysComboPon(int argc, char *argv[]){
    unsigned char enable = 0;
	if(NULL == argv[1]){
		printf("Bad command!\nUsage:sys Combo Pon enable|disable \n");
		return -1;
	}
	if(!strcmp(argv[1],"enable"))	{
		enable = 1;
	}else if(!strcmp(argv[1],"disable"))	{
		enable = 0;
	}else{
		printf("Bad command!\nUsage:sys Combo Pon enable|disable \n");
		return -1;
	}
		
	return blapi_system_doSysComboPon(enable);
}

int doSysHybrid(int argc, char *argv[]){
    unsigned char enable = 0;
	if(NULL == argv[1]){
		printf("Bad command!\nUsage:sys hybrid enable|disable \n");
		return -1;
	}
	if(!strcmp(argv[1],"enable"))	{
		enable = 1;
	}else if(!strcmp(argv[1],"disable"))	{
		enable = 0;
	}else{
		printf("Bad command!\nUsage:sys hybrid enable|disable \n");
		return -1;
	}
		
	return blapi_system_doSysHybrid(enable);
}



