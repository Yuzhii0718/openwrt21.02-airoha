#include <stdlib.h>             
#include <stdio.h>             
#include <string.h>           
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <getopt.h>
#include <strings.h>
#if __GLIBC__ >= 2
#include <asm/types.h>          /* glibc 2 conflicts with linux/types.h */
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netinet/if_ether.h>
#else
#include <linux/types.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/route.h>
#include <linux/if_ether.h>
#endif

#include <modules/ra_hwnat/hwnat_ioctl.h>
#include "libcompileoption.h"
#include "hwnat_api.h"
#include "libapi_lib_ppe.h"

void show_usage(void)
{
    printf("Add Static Entry\n");
    printf("hw_nat -a\n");
    printf("    -h [SMAC]\n");
    printf("    -i [DMAC]\n");
    printf("    -j [Sip]\n");
    printf("    -k [Dip]\n");
    printf("    -l [Sp]\n");
    printf("    -m [Dp]\n");    
    printf("    -n [New_Sip]\n");
    printf("    -o [New_Dip]\n");
    printf("    -p [New_Sp]\n");
    printf("    -q [New_Dp]\n");
    printf("    -s [VLAN1_ID]\n");
    printf("    -t [STAG]\n"); 
    printf("    -u [PPPoE_ID]\n");
    printf("    -v [Tcp/Udp]\n");
    printf("    -w [OutIf:PDMA/GSW/WMAC/QDMA1_HW/QDMA/QDMA_HW/DROP]\n");
    printf("    -x [hash index offset]\n");
    printf("    -y [New_DSCP/TRFC]\n");
    printf("    -z [Info2 Value]\n"); 
    printf("    -B [tunnel:0:Insert/1:Remove]\n");
    printf("    -C [Account Group 0x3f: disable]\n");
    printf("    -D [Meter Group 0x3f: disable]\n");
    printf("    -E [tunnel:IPv6 flow label]\n");
    printf("    -F [VPM:0-sp_tag, 1-0x8100, 2-0x88a8, 3-by register]\n");
    printf("    -G [IPv4 Protocol/IPv6 next header]\n");
    printf("    -H [tunnel:DSCP/TRFC]\n");
    printf("    -I [tunnel:IPv4 TTL/IPv6 HPOL]\n");
    printf("    -J [tunnel:IPv4 flag]\n");
    printf("    -K [TSID[5:1]/TSE[bit 0], or cnt_grp1 for EN7528, or meter_idx for EN7580]\n");
    printf("    -L [QID]\n");
    printf("    -M [IN SMAC (L2B Only)]\n");    
    printf("    -P [PCP: keep ingress 1st&2st VLAN P-bit and DEI]\n");
    printf("    -S [VLAN2_ID]\n");
    printf("    -W [Vlan Layer]\n");
    printf("    -X [IN DMAC (L2B Only)]\n");
    printf("    -Y [FLOW_TYPE:0:IPV4NAPT/1:IPV4NAT/2:L2B/3:DSLite/4:IPV63T/5:IPv65T/7:6RD]\n");
    if (TCSUPPORT_CPU_EN75XX_VAL)
    {
        printf("    -1 [IN_VLAN (L2B Only)]\n");
        printf("    -2 [HSK_L2 (L2B Only)]\n");
        printf("    -3 [Channel (hex)]\n");
        printf("    -4 [HSK_MC (Mcast only)]\n");
        printf("    -5 [valid/invalid/locked/all/lineCnt] (for cache data dump)\n");
        if (TCSUPPORT_CPU_EN758X_VAL) {
            printf("    -6 [IFC Index: -1:not hit, 0~15:Lut0 Index, 16~79:Lut1 Index]\n");
        } else {
            printf("    -6 [foe_num] (clear a foe's cache. foe_num==0 means clear whole cache table)\n");
        }
        printf("    -7 [IN_GRE_Call_ID]\n");
        printf("    -8 [OUT_GRE_Call_ID]\n");
    }
    printf("    -0 [LanPort] (clear foe entries which are related to frames from LanPort)\n");
    printf("    -9 [VID] (clear foe entries which are related to frames from WAN with VID)\n");
    printf("    -^ [TTL] (0:keep ttl value/1:decrease ttl)\n");
    printf("    -# [CNT GRP-2] (0:disable/0x40~0x7f: cnt-0~cnt-63)\n");
    if(TCSUPPORT_CPU_EN758X_VAL)/*TCSUPPORT_CPU_EN7581_VAL*/
        printf("    -. [ppe1/2] (0:ppe1 1:ppe2)\n");
    printf("\n");


	printf("Show Foe Cache Entry\n");
    printf("hw_nat -b [entry_num]\n");
    printf("Ex: hw_nat -b 1234\n\n");

    printf("Show Foe Entry\n");
    printf("hw_nat -c [entry_num]\n");
    printf("Ex: hw_nat -c 1234\n\n");
    
    printf("Set Debug Level (0:disable) \n");
    printf("hw_nat -d [0~7]\n");
    printf("Ex: hw_nat -d \n\n");
    
    printf("Show All Foe Invalid Entry\n");
    printf("Ex: hw_nat -e\n\n");
    
    printf("Show All Foe Unbinded Entry\n");
    printf("Ex: hw_nat -f\n\n");
    
    printf("Show All Foe Binded Entry\n");
    printf("Ex: hw_nat -g\n\n");
    
    printf("Get ByteCNT and PktCnt of AG_IDX\n");
    printf("Ex: hw_nat -A [AG index]\n\n");

    printf("Set PPE Cofigurations:\n");
    printf("Set HNAT binding threshold per second (d=30)\n");
    printf("Ex: hw_nat -N [1~65535]\n\n");

    printf("Set HNAT Max entries allowed build when Free Entries>3/4, >1/2, <1/2 (d=100, 50, 25)\n");
    printf("Ex: hw_nat -O [1~16383][1~16383][1~16383]\n\n");


    printf("Set HNAT TCP/UDP keepalive interval (d=1, 1)(unit:4sec)\n");
    printf("Ex: hw_nat -Q [1~255][1~255]\n\n");

    printf("Set HNAT Life time of unbind entry (d=3)(unit:1Sec)\n");
    printf("Ex: hw_nat -T [1~255]\n\n");

    printf("Set HNAT Life time of Binded TCP/UDP/FIN/not TCP/UDP entry(d=15, 15, 5, 15)(unit:1Sec) \n");
    printf("Ex: hw_nat -U [1~65535][1~65535][1~65535][1~65535]\n\n");

	printf("Set multicast support\n");    
	printf("Ex: hw_nat -V [0/1]\n\n");

	printf("Set IP White List/Black List\n");    
	printf("Ex: hw_nat -r [0:White List/1:Black List]\n\n");

	printf("Clean All FOE Entries \n");
	printf("Ex: hw_nat -! [Clean All Entries]\n\n");

    printf("Remove one FOE entry or Clean All FOE Entries \n");
    printf("Ex: hw_nat -+ [0:Clean All Entries / entry_num: remove this one]\n\n");

    printf("Only Speed UP (0=Upstream, 1=Downstream, 2=Bi-Direction) flow \n");
    printf("Ex: hw_nat -Z 1\n\n");

    printf("Enable(1) or disable(0) the status of hw_nat for hardware acceleration. \n");
    printf("Ex: hw_nat -en 1|0\n\n");
   
}

int getnext(char *src, int separator, char *dest)
{
    char *c;
    int len;

    if ((src == NULL) || (dest == NULL)) 
    {
        return -1;
    }

    c = strchr(src, separator);
    if (c == NULL) 
    {
        strcpy(dest, src);
        return -1;
    }
    
    len = c - src;
    strncpy(dest, src, len);
    dest[len] = '\0';
    return len + 1;
}

int str_to_mac(unsigned char *mac, char *str)
{
    int len;
    char *ptr = str;
    char buf[128];
    int i;

    for (i = 0; i < 5; i++)
    {
        if ((len = getnext(ptr, ':', buf)) == -1) 
        {
            return 1; /* parse error */
        }
        mac[i] = strtol(buf, NULL, 16);
        ptr += len;
    }
    mac[5] = strtol(ptr, NULL, 16);

    return 0;
}

int str_to_ip (unsigned int *ip, char *str)
{
    int len;
    char *ptr = str;
    char buf[128];
    unsigned char c[4];
    int i;

    for (i = 0; i < 3; ++i)
    {
        if ((len = getnext(ptr, '.', buf)) == -1) 
        {
            return 1; /* parse error */
        }
        c[i] = atoi(buf);
        ptr += len;
    }
    c[3] = atoi(ptr);
    *ip = (c[0]<<24) + (c[1]<<16) + (c[2]<<8) + c[3];
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    char options[256] = {0};
    char tmp[129];
    struct sockaddr_in6 ip6;
    int fd, method;
    int i=0;
    unsigned int entry_num = 0;
    unsigned int debug;
    unsigned int dir = 0;
    struct hwnat_args *args;
	struct hwnat_tuple args2;
    struct hwnat_ac_args args3;
    struct hwnat_config_args args4;
    int	   result;
    unsigned char cache_type = 0;
	struct hwnat_debug_args args5;

	init_compileoption_val();

    if (TCSUPPORT_CPU_EN75XX_VAL)
    {
        strcpy(options,"ab:gefg?!Rc:d:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:S:T:U:V:W:X:Y:Z:+:$:@:0:1:2:3:4:5:6:7:8:9:^:#:%:*:.:");
	}
    else
    {
        strcpy(options,"abgefg?!Rc:d:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:S:T:U:V:W:X:Y:Z:+:$:@:#:0:1:2:3:4:5:6:7:8:9:^:");
	}

    fd = open("/dev/"HW_NAT_DEVNAME, O_RDONLY);
    if (fd < 0)
    {
		printf("Open %s pseudo device failed\n","/dev/"HW_NAT_DEVNAME);
		return 0;
    }
    close(fd);

    if (argc < 2) 
	{
		show_usage();
		return 0;
    }

    /* Max table size is 32K */
    args=malloc(sizeof(struct hwnat_args)+sizeof(struct hwnat_tuple)*1024*32);
	if (args == NULL)
	{
		return 0;
	}

	memset(args, 0, (sizeof(struct hwnat_args)+sizeof(struct hwnat_tuple)*1024*32));
	memset(&args2, 0, sizeof(args2));
	
	/* ifc not hit, ifc index default value */
	args2.ifc_idx = IFC_MAX_IDX;
	memset(&args3, 0, sizeof(args3));
	memset(&args4, 0, sizeof(args4));


	args2.ag = 0x3f;//default disable
	args2.mg = 0x3f;//default disable
	args2.info2 = 0xffffffff;//default disable
	args2.sta = 0;
	args2.vlan_layer = 0xff;
	args2.ttl = 1;//default TTL-1
	if (TCSUPPORT_CPU_EN758X_VAL) {
		args2.tsid = 0x7f;
	}

	//printf("main 0 %c\n");
    while ((opt = getopt (argc, argv, options)) != -1) 
	{
		switch (opt) {		
		case '!':
			method = HW_NAT_TAB_CLEAN;		
			printf("method = HW_NAT_TAB_CLEAN\n");
			break;
	    case '1':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.in_vlan= strtoll(optarg, NULL, 16) & 0xffff;
	        break;
	    case '2':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.hsk_l2 = strtoll(optarg, NULL, 16) & 0xffffffff;
	        break;
	    case '3':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.channel = strtoll(optarg, NULL, 16) & 0xff;
	        break;
	    case '4':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.hsk_mc = strtoll(optarg, NULL, 16) & 0xffffffff;
	        break;
	    case '5':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	        {
	            method = HW_NAT_DUMP_CACHE_DATA;
	            if(strcasecmp(optarg,"valid")==0)            
	                cache_type = PPE_CACHE_TYPE_VALID;
	            else if(strcasecmp(optarg,"invalid")==0)             
	                cache_type = PPE_CACHE_TYPE_INVALID;
	            else if(strcasecmp(optarg,"locked")==0)      
	                cache_type = PPE_CACHE_TYPE_LOCKED;
	            else if(strcasecmp(optarg,"all")==0)
	                cache_type = PPE_CACHE_TYPE_ALL;
	            else if(strcasecmp(optarg,"lineCnt")==0)
	                cache_type = PPE_CACHE_TYPE_LINECNT;
	            else
	            {            
	                printf("Error: -5 valid/invalid/locked/all/lineCnt\n");          
	                return 0;        
	            }
	        }
	        break;
	    case '6':
	        if (TCSUPPORT_CPU_EN758X_VAL) {            
				result = strtoll(optarg, NULL, 10);            
				if(result == -1)                
					args2.ifc_idx = IFC_MAX_IDX;            
				else                
					args2.ifc_idx = result + 0x80;        
			} else if (TCSUPPORT_CPU_EN75XX_VAL) {
	            method = HW_NAT_CLEAR_CACHE;
	            entry_num = strtoll(optarg, NULL, 10);
	        } else {
	        }
	        break;
	    case '7':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.in_gre_call_id = strtoll(optarg, NULL, 16) & 0xffff;
	        break;
	    case '8':
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            args2.out_gre_call_id = strtoll(optarg, NULL, 16) & 0xffff;
	        break;

		case 'b':
			method = HW_NAT_DUMP_CACHE_ENTRY;
	        if (TCSUPPORT_CPU_EN75XX_VAL)
	            entry_num = strtoll(optarg, NULL, 10);
			break;
		case 'B':
			args2.RMT = strtoll(optarg, NULL, 10);
			break;	
		case 'C':
			args2.ag = strtoll(optarg, NULL, 16);
			break;	
		case 'D':
			args2.mg = strtoll(optarg, NULL, 16);
			break;
		case 'E':
			args2.ipv6_flowlabel = strtoll(optarg, NULL, 16);
			break;	
		case 'F':
			args2.vpm = strtoll(optarg, NULL, 16) & 0x3;
			break;	
		case 'G':
			args2.prot = strtoll(optarg, NULL, 16);
			break;	
		case 'H':
			args2.tunnel_dscp = strtoll(optarg, NULL, 16);
			break;	
		case 'I':
			args2.tunnel_TTL = strtoll(optarg, NULL, 16);
			break;	
		case 'J':
			args2.tunnel_IPv4_flag = strtoll(optarg, NULL, 16);
			break;
		case 'K':
			args2.tsid = strtoll(optarg, NULL, 16) & 0xff;
			break;
		case 'L':
			args2.qid = strtoll(optarg, NULL, 10) & 0xf;
			break;	
		case 'M':
			str_to_mac(args2.in_smac, optarg);		 
			break;
		case 'P':
			args2.pcp = strtoll(optarg, NULL, 10) & 0x1;
			break;		
		case 'W':
			args2.vlan_layer = strtoll(optarg, NULL, 10) & 0xf;
			break;
		case 'X':
			str_to_mac(args2.in_dmac, optarg);		 
			break;	
		case 'c':
			method = HW_NAT_DUMP_ENTRY;
			entry_num = strtoll(optarg, NULL, 10);
			break;
		case 'd':
			method = HW_NAT_DEBUG;
			 if(strcasecmp(optarg,"bindfail")==0)
			 {
			 	args5.id = HWNAT_DBG_ID_SET_BINDFAILIDX;
			 	args5.bind_fail_idx = strtoll(argv[3],NULL,10);
			 }
			 else if(strcasecmp(optarg,"free")==0)
			 {
				args5.id = HWNAT_DBG_ID_SET_FREEIDX;
				args5.free_idx = strtoll(argv[3],NULL,10);
			 }
			 else if(strcasecmp(optarg,"drop")==0)
			 {
				args5.id = HWNAT_DBG_ID_SET_DROPIDX;
				args5.drop_idx = strtoll(argv[3],NULL,10);
			 }
			else if(strcasecmp(optarg,"parse")==0)
			 {
				args5.id = HWNAT_DBG_ID_SET_DUMP_PARSEINFO;
				args5.parse_dump_enable = strtoll(argv[3],NULL,10);
			 }
			 else
			{			
			 	args5.id = HWNAT_DBG_ID_SET_DBGLVL;
				args5.dbg_lvl = strtoll(optarg, NULL, 10);
			 }
			break;
		case 'e':
            if(strncmp(argv[1], "-en", 3) == 0){
                method = HW_NAT_ENABLE_STATUS;
                if(argc > 2){
                    args->hwnat_enable_status = atoi(argv[2]);
                }else{
                    args->hwnat_enable_status = 2; /*show enable status*/
                }
            }
            else{
                method = HW_NAT_GET_ALL_ENTRIES;
                args->entry_state=0; /* invalid entry */
            }
            break;
		case 'f':
			method = HW_NAT_GET_ALL_ENTRIES;
			args->entry_state=1; /* unbinded entry */
			break;
		case 'g':
			method = HW_NAT_GET_ALL_ENTRIES;
			args->entry_state=2; /* binded entry */
			break;
		case 'A':
			method = HW_NAT_GET_AC_CNT;
			args3.ag_index = strtoll(optarg, NULL, 10);
			break;
		case 'N':
			method = HW_NAT_BIND_THRESHOLD;
			args4.bind_threshold = strtoll(argv[2], NULL, 10);
			break;
		case 'O':
			method = HW_NAT_MAX_ENTRY_LMT;
			args4.foe_qut_lmt = strtoll(argv[2], NULL, 10);
			args4.foe_half_lmt = strtoll(argv[3], NULL, 10);
			args4.foe_full_lmt  = strtoll(argv[4], NULL, 10);
			break;
		case 'Q':
			method = HW_NAT_KA_INTERVAL;
			args4.foe_tcp_ka = strtoll(argv[2], NULL, 10);
			args4.foe_udp_ka = strtoll(argv[3], NULL, 10);
			break;
		case 'T':
			method = HW_NAT_UB_LIFETIME;
			args4.foe_unb_dlta = strtoll(argv[2], NULL, 10);
			break;
		case 'U':
			method = HW_NAT_BIND_LIFETIME;
			args4.foe_tcp_dlta = strtoll(argv[2], NULL, 10);
			args4.foe_udp_dlta = strtoll(argv[3], NULL, 10);
			args4.foe_fin_dlta = strtoll(argv[4], NULL, 10);
			args4.foe_ntu_dlta = strtoll(argv[5], NULL, 10);
			break;
		case 'Z':
			method = HW_NAT_BIND_DIRECTION;
			dir = strtoll(optarg, NULL, 10);
			break;
		case 'a':
			method= HW_NAT_ADD_ENTRY;
			break;
		case 'h':		 
			str_to_mac(args2.smac, optarg);		 
			break;	
		case 'i':		 
			str_to_mac(args2.dmac, optarg);		 
			break;	
		case 'j':
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, optarg, (sizeof(tmp)-1));
			//printf("Old  SIP %s!!!!!!\n",tmp);
			if(inet_pton(AF_INET6, tmp, &ip6.sin6_addr) > 0){
				args2.ing_sipv6_3 = swab32(*((unsigned int *)ip6.sin6_addr.s6_addr));
				args2.ing_sipv6_2 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
				args2.ing_sipv6_1 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
				args2.ing_sipv6_0 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			}else{	
				str_to_ip(&args2.ing_sipv4, optarg);
				//printf("OLD SIP 2%x!!!!!!\n",args2.ing_sipv4);
			}	
			break;	
		case 'k':
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, optarg, (sizeof(tmp)-1));
			//printf("Old  DIP %s!!!!!!\n",tmp);
			if(inet_pton(AF_INET6, tmp, &ip6.sin6_addr) > 0){
				args2.ing_dipv6_3 = swab32(*((unsigned int *)ip6.sin6_addr.s6_addr));
				args2.ing_dipv6_2 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
				args2.ing_dipv6_1 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
				args2.ing_dipv6_0 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			}else{
				str_to_ip(&args2.ing_dipv4, optarg);
				//printf("OLD DIP 2%x!!!!!!\n",args2.ing_dipv4);
			}	
			break;	
		case 'l':		 
			args2.ing_sp = strtoll(optarg, NULL, 10);		 
			break;	
		case 'm':		 
			args2.ing_dp = strtoll(optarg, NULL, 10);		 
			break;	
		case 'n':
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, optarg, (sizeof(tmp)-1));
			//printf("New  SIP %s!!!!!!\n",tmp);
			if(inet_pton(AF_INET6, tmp, &ip6.sin6_addr) > 0){
				args2.eg_sipv6_3 = swab32(*((unsigned int *)ip6.sin6_addr.s6_addr));
				args2.eg_sipv6_2 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
				args2.eg_sipv6_1 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
				args2.eg_sipv6_0 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			}else{	
				str_to_ip(&args2.eg_sipv4, optarg);
				//printf("New  SIP 2%x!!!!!!\n",args2.eg_sipv4);
			}	
			break;	
		case 'o':
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, optarg, (sizeof(tmp)-1));
			//printf("New  DIP %s!!!!!!\n",tmp);
			if(inet_pton(AF_INET6, tmp, &ip6.sin6_addr) > 0){
				args2.eg_dipv6_3 = swab32(*((unsigned int *)ip6.sin6_addr.s6_addr));
				args2.eg_dipv6_2 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
				args2.eg_dipv6_1 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
				args2.eg_dipv6_0 = swab32(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			}else{	
				str_to_ip(&args2.eg_dipv4, optarg);	
				//printf("New  DIP 2%x!!!!!!\n",args2.eg_dipv4);
			}	
			break;	
		case 'p':		 
			args2.eg_sp= strtoll(optarg, NULL, 10);		 
			break;	
		case 'q':		 
			args2.eg_dp= strtoll(optarg, NULL, 10);		 
			break;			
		case 'r':
			method = HW_NAT_SET_IP_CHK;
			args4.ip_chk_type = (strtoll(optarg, NULL, 16) & 0x1);	
			break;	
		case 's':		 
			args2.vlan1 = strtoll(optarg, NULL, 16);                 
			break;		
		case 'S':		 
			args2.vlan2 = strtoll(optarg, NULL, 16);
			break;
		case 't':		 
				args2.stag= strtoll(optarg, NULL, 16); 	 
				break;
		case 'u':		 
			args2.pppoe_id = strtoll(optarg, NULL, 10);		 
			break;	
		case 'v':		 
			if(strcasecmp(optarg,"Tcp")==0){			 
				args2.is_udp=0;		 
			}else if(strcasecmp(optarg,"Udp")==0){			 
				args2.is_udp=1;		 
			}else {			 
				printf("Error: -v Tcp/Udp\n");			 
				return 0;		 
			}		 
			break;	
		case 'w':		
			if(strcasecmp(optarg,"PDMA")==0){			 
				args2.dst_port=0; 		 
			}else if(strcasecmp(optarg,"GSW")==0){			 
				args2.dst_port=1; 		 
			}else if(strcasecmp(optarg,"WMAC")==0){			 
				args2.dst_port=2;		 
			}else if(strcasecmp(optarg,"QDMA1_HW")==0){			 
				args2.dst_port=3; 
			}else if(strcasecmp(optarg,"QDMA")==0){			 
				args2.dst_port=5; 
			}else if(strcasecmp(optarg,"QDMA_HW")==0){			 
				args2.dst_port=6; 
			}else if(strcasecmp(optarg,"DROP")==0){			 
				args2.dst_port=7; 		 
			}else {			 
				printf("Error: -w PDMA/GSW/WMAC/QDMA1_HW/QDMA/QDMA_HW/DROP\n");			 
				return 0;		 
			}		 
			break;
		case 'x':
			//hash_index_shift	
			args2.hash_index_shift = strtoll(optarg, NULL, 16)&0xf;	
			break;
		case 'y':		 
			args2.eg_dscp = strtoll(optarg, NULL, 16)&0xff;	
			break;
		case 'z':		 
			args2.info2= strtoll(optarg, NULL, 16);		
			break;
		case 'R':
			method = HW_NAT_REG_DUMP;
			break;
		case 'Y':		 
			args2.pkt_type = strtoll(optarg, NULL, 10);
			break;
		case 'V':		
			method = HW_NAT_MULTICAST_EN;		
			args4.enable = strtoll(optarg, NULL, 10);		
			break;	
		case '+':
			method = HW_NAT_FOE_REMOVE;
			entry_num = strtoll(optarg, NULL, 10);
			break;
		case '$':
			method = HW_NAT_L2BR_ETHTYPE_EN;
			args4.L2ethtypeEN = strtoll(optarg, NULL, 10);
			break;
		case '@'://tony add
			if(TCSUPPORT_CT_SIMCARD_SEPARATION_VAL && TCSUPPORT_CT_2PORTS_VAL){
				method = HW_NAT_SNOOPING_EN;
				args4.snoopingEn = strtoll(optarg, NULL, 10);
			}
			break;
		case '%':	
			if(TCSUPPORT_CPU_EN758X_VAL)
			{
				args2.dpi = strtoll(optarg, NULL, 10);
			}
			break;
	    case '0':
			if (TCSUPPORT_XPON_HAL_API_EXT_VAL)
			{
				method = HW_NAT_CLEAN_LAN;
				entry_num = strtoll(optarg, NULL, 10);
			}
			break;
	    case '9':
			if (TCSUPPORT_XPON_HAL_API_EXT_VAL)
			{
				method = HW_NAT_CLEAN_WAN;	
				entry_num = strtoll(optarg, NULL, 10);
			}
			break;
	    case '^':
			args2.ttl = strtoll(optarg, NULL, 10);	
			break;

		case '#':
			args2.cnt_grp2 = strtoll(optarg, NULL, 16) & 0xff;	
			break;
        case '.':
            args2.ppe2 = strtoll(optarg, NULL, 10);
            break;

		case '?':
			show_usage();
			break;
		}
    } 

	//printf("main 2 %d\n",method);
    switch(method){		
	case HW_NAT_ADD_ENTRY:
	    HwNatAddEntry(&args2);
	    result = args2.result;
	    break;
	case HW_NAT_INVALID_ENTRY:	
		result = HwNatInvalidEntry(entry_num);
		break;
	case HW_NAT_MULTICAST_EN:
	    HwNatSetMulticastEn(&args4);
	    result = args4.result;
	    break;
	case HW_NAT_SET_IP_CHK:
	    HwNatSetIPChk(&args4);
	    result = args4.result;
	    break;
	case HW_NAT_REG_DUMP:
	    HwNatREGDUMP(&args4);
	    result = args4.result;
	    break;
    case HW_NAT_GET_ALL_ENTRIES:
	    HwNatGetAllEntries(args);

	#ifndef TCSUPPORT_HWNAT_V3
	    printf("Total Entry Count = %d\n",args->num_of_entries);	
        if(debug==1) {
            result = args->result;
            break;
        }
	    for(i=0;i<args->num_of_entries;i++){
		if(args->entries[i].pkt_type==0) { //IPV4_NAPT
            if ((TCSUPPORT_CPU_EN75XX_VAL) && args->entries[i].hsk_mc)
            {
            printf("IPv4_NAPT=%d : %u.%u.%u.%u->%u.%u.%u.%u => %u.%u.%u.%u->%u.%u.%u.%u (HSK_MC:0x%08x) (INFO2:0x%08lx)\n", \
                args->entries[i].hash_index, \
                NIPQUAD(args->entries[i].ing_sipv4), \
                NIPQUAD(args->entries[i].ing_dipv4), \
                NIPQUAD(args->entries[i].eg_sipv4), \
                    NIPQUAD(args->entries[i].eg_dipv4), \
                    args->entries[i].hsk_mc, args->entries[i].info2);
            }
            else
            {
		    printf("IPv4_NAPT=%d : %u.%u.%u.%u:%d->%u.%u.%u.%u:%d => %u.%u.%u.%u:%d->%u.%u.%u.%u:%d (INFO2:0x%08lx)\n", \
			    args->entries[i].hash_index, \
			    NIPQUAD(args->entries[i].ing_sipv4), \
			    args->entries[i].ing_sp, \
			    NIPQUAD(args->entries[i].ing_dipv4), \
			    args->entries[i].ing_dp, \
			    NIPQUAD(args->entries[i].eg_sipv4), \
		            args->entries[i].eg_sp, \
		            NIPQUAD(args->entries[i].eg_dipv4), \
		            args->entries[i].eg_dp, args->entries[i].info2);
            }
		} else if(args->entries[i].pkt_type==1) { //IPV4_NAT
		    printf("IPv4_NAT=%d : %u.%u.%u.%u->%u.%u.%u.%u => %u.%u.%u.%u->%u.%u.%u.%u (INFO2:0x%08lx)\n", \
			    args->entries[i].hash_index, \
			    NIPQUAD(args->entries[i].ing_sipv4), \
			    NIPQUAD(args->entries[i].ing_dipv4), \
			    NIPQUAD(args->entries[i].eg_sipv4), \
			    NIPQUAD(args->entries[i].eg_dipv4), args->entries[i].info2); 
		} else if(args->entries[i].pkt_type==2) { //L2 Bridge
            if (TCSUPPORT_CPU_EN75XX_VAL && args->entries[i].new_l2b)
            {
                if( args->entries[i].new_l2b == 2) {
                    printf("L2 BRIDGE= %d /IN SMAC %02x:%02x:%02x:%02x:%02x:%02x  IN DMAC: %02x:%02x:%02x:%02x:%02x:%02x  IN_VLAN %x  HSK_L2 %x (INFO2:0x%08lx)\n", \
            			args->entries[i].hash_index, \
            		    NIPQUAD(*(unsigned int *)(&args->entries[i].in_smac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_smac[4])),\
                        NIPQUAD(*(unsigned int *)(&args->entries[i].in_dmac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_dmac[4])),\
            		    args->entries[i].in_vlan, args->entries[i].hsk_l2, args->entries[i].info2);
                } else {
                    printf("L2 BRIDGE= %d /IN_DMAC %02x:%02x:%02x:%02x:%02x:%02x  IN_VLAN %x  HSK_L2 %x (INFO2:0x%08lx)\n", \
            			args->entries[i].hash_index, \
            		    NIPQUAD(*(unsigned int *)(&args->entries[i].in_dmac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_dmac[4])),\
            		    args->entries[i].in_vlan, args->entries[i].hsk_l2, args->entries[i].info2);
                }
            }
            else if(TCSUPPORT_CPU_EN75XX_VAL)
            {
                printf("L2 BRIDGE= %d /IN SMAC: %02x:%02x:%02x:%02x:%02x:%02x DMAC %02x:%02x:%02x:%02x:%02x:%02x\n", \
        			args->entries[i].hash_index, \
        		    NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_smac[0])), NIPQUAD(*(unsigned int *)(&args->entries[i].in_smac[2])),\
        		    NIPQUAD(*(unsigned int* )(&args->entries[i].in_dmac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_dmac[4])));
            }
            else
            {
     			printf("L2 BRIDGE= %d /IN SMAC: %02x:%02x:%02x:%02x:%02x:%02x DMAC %02x:%02x:%02x:%02x:%02x:%02x (INFO2:0x%08lx)\n", \
        			args->entries[i].hash_index, \
        		    NIPQUAD(*(unsigned int *)(&args->entries[i].in_smac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_smac[4])),\
        		    NIPQUAD(*(unsigned int* )(&args->entries[i].in_dmac[0])), NIPTWOBYTE(*(unsigned short *)(&args->entries[i].in_dmac[4])), args->entries[i].info2);
            }
		} else if(args->entries[i].pkt_type==3) { //IPV4_DSLITE
            if ((TCSUPPORT_CPU_EN75XX_VAL) && args->entries[i].hsk_mc)
            {
            printf("DS-Lite= %d : %u.%u.%u.%u->%u.%u.%u.%u (HSK_MC:0x%08x) (%x:%x:%x:%x:%x:%x:%x:%x -> %x:%x:%x:%x:%x:%x:%x:%x) (INFO2:0x%08lx)\n", \
                args->entries[i].hash_index, \
                NIPQUAD(args->entries[i].ing_sipv4),  \
                NIPQUAD(args->entries[i].ing_dipv4),  \
                args->entries[i].hsk_mc, \
                NIPHALF(args->entries[i].eg_sipv6_0), \
                NIPHALF(args->entries[i].eg_sipv6_1), \
                NIPHALF(args->entries[i].eg_sipv6_2), \
                NIPHALF(args->entries[i].eg_sipv6_3), \
                NIPHALF(args->entries[i].eg_dipv6_0), \
                NIPHALF(args->entries[i].eg_dipv6_1), \
                NIPHALF(args->entries[i].eg_dipv6_2), \
                NIPHALF(args->entries[i].eg_dipv6_3), args->entries[i].info2);
            }
            else
            {
			printf("DS-Lite= %d : %u.%u.%u.%u:%d->%u.%u.%u.%u:%d (%x:%x:%x:%x:%x:%x:%x:%x -> %x:%x:%x:%x:%x:%x:%x:%x) (INFO2:0x%08lx) \n", \
			    args->entries[i].hash_index, \
			    NIPQUAD(args->entries[i].ing_sipv4),  \
			    args->entries[i].ing_sp,     \
			    NIPQUAD(args->entries[i].ing_dipv4),  \
			    args->entries[i].ing_dp, \
				NIPHALF(args->entries[i].eg_sipv6_0), \
			    NIPHALF(args->entries[i].eg_sipv6_1), \
			    NIPHALF(args->entries[i].eg_sipv6_2), \
			    NIPHALF(args->entries[i].eg_sipv6_3), \
			    NIPHALF(args->entries[i].eg_dipv6_0), \
			    NIPHALF(args->entries[i].eg_dipv6_1), \
			    NIPHALF(args->entries[i].eg_dipv6_2), \
			    NIPHALF(args->entries[i].eg_dipv6_3), args->entries[i].info2);
            }
		} else if(args->entries[i].pkt_type==4) { //IPV6_3T_ROUTE
			printf("IPv6_3T= %d SIP: %x:%x:%x:%x:%x:%x:%x:%x DIP: %x:%x:%x:%x:%x:%x:%x:%x (INFO2:0x%08lx)\n", \
			    args->entries[i].hash_index, \
				NIPHALF(args->entries[i].ing_sipv6_0), \
			    NIPHALF(args->entries[i].ing_sipv6_1), \
			    NIPHALF(args->entries[i].ing_sipv6_2), \
			    NIPHALF(args->entries[i].ing_sipv6_3), \
			    NIPHALF(args->entries[i].ing_dipv6_0), \
			    NIPHALF(args->entries[i].ing_dipv6_1), \
			    NIPHALF(args->entries[i].ing_dipv6_2), \
			    NIPHALF(args->entries[i].ing_dipv6_3), args->entries[i].info2);

		} else if(args->entries[i].pkt_type==5) { //IPV6_5T_ROUTE
		    if(args->entries[i].ipv6_flowlabel==1) {
			printf("IPv6_5T= %d SIP: %x:%x:%x:%x:%x:%x:%x:%x DIP: %x:%x:%x:%x:%x:%x:%x:%x (Flow Label=%x) (INFO2:0x%08lx)\n", \
				args->entries[i].hash_index, \
				NIPHALF(args->entries[i].ing_sipv6_0), \
				NIPHALF(args->entries[i].ing_sipv6_1), \
				NIPHALF(args->entries[i].ing_sipv6_2), \
				NIPHALF(args->entries[i].ing_sipv6_3), \
				NIPHALF(args->entries[i].ing_dipv6_0), \
				NIPHALF(args->entries[i].ing_dipv6_1), \
				NIPHALF(args->entries[i].ing_dipv6_2), \
				NIPHALF(args->entries[i].ing_dipv6_3), \
				((args->entries[i].ing_sp << 16) | (args->entries[i].ing_dp))&0xFFFFF, args->entries[i].info2);		
		    }else {
            if ((TCSUPPORT_CPU_EN75XX_VAL) && args->entries[i].hsk_mc)
            {
            printf("IPv6_5T= %d SIP: %x:%x:%x:%x:%x:%x:%x:%x DIP: %x:%x:%x:%x:%x:%x:%x:%x (HSK_MC:0x%08x) (INFO2:0x%08lx)\n", \
                args->entries[i].hash_index, \
                NIPHALF(args->entries[i].ing_sipv6_0), \
                NIPHALF(args->entries[i].ing_sipv6_1), \
                NIPHALF(args->entries[i].ing_sipv6_2), \
                NIPHALF(args->entries[i].ing_sipv6_3), \
                NIPHALF(args->entries[i].ing_dipv6_0), \
                NIPHALF(args->entries[i].ing_dipv6_1), \
                NIPHALF(args->entries[i].ing_dipv6_2), \
                NIPHALF(args->entries[i].ing_dipv6_3), \
                args->entries[i].hsk_mc, args->entries[i].info2);
            }
            else
            {
			printf("IPv6_5T= %d SIP: %x:%x:%x:%x:%x:%x:%x:%x (SP:%d) DIP: %x:%x:%x:%x:%x:%x:%x:%x (DP=%d) (INFO2:0x%08lx)\n", \
				args->entries[i].hash_index, \
				NIPHALF(args->entries[i].ing_sipv6_0), \
				NIPHALF(args->entries[i].ing_sipv6_1), \
				NIPHALF(args->entries[i].ing_sipv6_2), \
				NIPHALF(args->entries[i].ing_sipv6_3), \
				args->entries[i].ing_sp, \
				NIPHALF(args->entries[i].ing_dipv6_0), \
				NIPHALF(args->entries[i].ing_dipv6_1), \
				NIPHALF(args->entries[i].ing_dipv6_2), \
				NIPHALF(args->entries[i].ing_dipv6_3), \
				args->entries[i].ing_dp, args->entries[i].info2);
            }
		    }

		} else if(args->entries[i].pkt_type==7) { //IPV6_6RD
		    if(args->entries[i].ipv6_flowlabel==1) {
			printf("6RD= %d %x:%x:%x:%x:%x:%x:%x:%x->%x:%x:%x:%x:%x:%x:%x:%x [Flow Label=%x] (INFO2:0x%08lx)\n", \
				args->entries[i].hash_index, \
				NIPHALF(args->entries[i].ing_sipv6_0), \
				NIPHALF(args->entries[i].ing_sipv6_1), \
				NIPHALF(args->entries[i].ing_sipv6_2), \
				NIPHALF(args->entries[i].ing_sipv6_3), \
				NIPHALF(args->entries[i].ing_dipv6_0), \
				NIPHALF(args->entries[i].ing_dipv6_1), \
				NIPHALF(args->entries[i].ing_dipv6_2), \
				NIPHALF(args->entries[i].ing_dipv6_3), \
				((args->entries[i].ing_sp << 16) | (args->entries[i].ing_dp))&0xFFFFF, args->entries[i].info2);
				printf("(%u.%u.%u.%u->%u.%u.%u.%u)\n", NIPQUAD(args->entries[i].eg_sipv4), NIPQUAD(args->entries[i].eg_dipv4));
		    }else {
            if ((TCSUPPORT_CPU_EN75XX_VAL) && args->entries[i].hsk_mc)
            {
            printf("6RD= %d /SIP: %x:%x:%x:%x:%x:%x:%x:%x /DIP: %x:%x:%x:%x:%x:%x:%x:%x (HSK_MC:0x%08x) (INFO2:0x%08lx)", \
                args->entries[i].hash_index, \
                NIPHALF(args->entries[i].ing_sipv6_0), \
                NIPHALF(args->entries[i].ing_sipv6_1), \
                NIPHALF(args->entries[i].ing_sipv6_2), \
                NIPHALF(args->entries[i].ing_sipv6_3), \
                NIPHALF(args->entries[i].ing_dipv6_0), \
                NIPHALF(args->entries[i].ing_dipv6_1), \
                NIPHALF(args->entries[i].ing_dipv6_2), \
                NIPHALF(args->entries[i].ing_dipv6_3), \
                args->entries[i].hsk_mc, args->entries[i].info2);
            }
            else
            {
			printf("6RD= %d /SIP: %x:%x:%x:%x:%x:%x:%x:%x [SP:%d] /DIP: %x:%x:%x:%x:%x:%x:%x:%x [DP=%d] (INFO2:0x%08lx)", \
				args->entries[i].hash_index, \
				NIPHALF(args->entries[i].ing_sipv6_0), \
				NIPHALF(args->entries[i].ing_sipv6_1), \
				NIPHALF(args->entries[i].ing_sipv6_2), \
				NIPHALF(args->entries[i].ing_sipv6_3), \
				args->entries[i].ing_sp, \
				NIPHALF(args->entries[i].ing_dipv6_0), \
				NIPHALF(args->entries[i].ing_dipv6_1), \
				NIPHALF(args->entries[i].ing_dipv6_2), \
				NIPHALF(args->entries[i].ing_dipv6_3), \
				args->entries[i].ing_dp, args->entries[i].info2);
            }
			printf("(%u.%u.%u.%u->%u.%u.%u.%u)\n", NIPQUAD(args->entries[i].eg_sipv4), NIPQUAD(args->entries[i].eg_dipv4));
		    }
		} else{
		    printf("unknown packet type! (pkt_type=%d) \n", args->entries[i].pkt_type);
		}
	    }
		#endif
	    result = args->result;
	    break;
    case HW_NAT_DUMP_CACHE_ENTRY:
        if (TCSUPPORT_CPU_EN75XX_VAL)
            result = HwNatCacheDumpEntry(entry_num);
        else
            result = HwNatCacheDumpEntry(0);
	    break;
    case HW_NAT_DUMP_CACHE_DATA:
        if (TCSUPPORT_CPU_EN75XX_VAL)
            result = HwNatCacheDumpData(cache_type);
        break;
    case HW_NAT_CLEAR_CACHE:
        if (TCSUPPORT_CPU_EN75XX_VAL)
            result = HwNatCacheClear(entry_num);
	    break;
    case HW_NAT_DUMP_ENTRY:
	    result = HwNatDumpEntry(entry_num);
	    break;
    case HW_NAT_DEBUG:
	    result = HwNatDebug(&args5);
	    break;
    case HW_NAT_GET_AC_CNT:
	    HwNatGetAGCnt(&args3);
	    printf("Byte cnt=%d\n", args3.ag_byte_cnt);
	    printf("Pkt cnt=%d\n", args3.ag_pkt_cnt);
	    result = args3.result;
	    break;
    case HW_NAT_BIND_THRESHOLD:
	    HwNatSetBindThreshold(&args4);
	    result = args4.result;
	    break;
    case HW_NAT_MAX_ENTRY_LMT:
	    HwNatSetMaxEntryRateLimit(&args4);
	    result = args4.result;
	    break;		
    case HW_NAT_KA_INTERVAL:
	    HwNatSetKaInterval(&args4);
	    result = args4.result;
	    break;
    case HW_NAT_UB_LIFETIME:
	    HwNatSetUnbindLifeTime(&args4);
	    result = args4.result;
	    break;
    case HW_NAT_BIND_LIFETIME:
	    HwNatSetBindLifeTime(&args4);
	    result = args4.result;
	    break;	
    case HW_NAT_BIND_DIRECTION:
	    result = HwNatSetBindDir(dir);
	    break;	
	case HW_NAT_TAB_CLEAN:
		result = HwNatTblClean(&args2);;
		break;
    case HW_NAT_FOE_REMOVE:
	    result = HwNatRemoveFoe(entry_num);
	    break;
	case HW_NAT_L2BR_ETHTYPE_EN:
	    result = HwNatL2br_ethtypeEN(&args4);
	    break;	
    case HW_NAT_SNOOPING_EN:
		if(TCSUPPORT_CT_SIMCARD_SEPARATION_VAL && TCSUPPORT_CT_2PORTS_VAL)
	    	result = HwNatSnoopEN(&args4);
	    break;
	case HW_NAT_CLEAN_LAN:
		if (TCSUPPORT_XPON_HAL_API_EXT_VAL)
		{
			result = HwNatCleanLanWan(HW_NAT_CLEAN_LAN,entry_num);
		}
		break;
	case HW_NAT_CLEAN_WAN:
		if (TCSUPPORT_XPON_HAL_API_EXT_VAL)
		{
			result = HwNatCleanLanWan(HW_NAT_CLEAN_WAN,entry_num);
		}
		break;
    case HW_NAT_ENABLE_STATUS:
        result = HwNatGetEnableStatus(args->hwnat_enable_status);
        break;
    }

    if(result==HWNAT_SUCCESS){
	printf("done\n");
    }else if(result==HWNAT_ENTRY_NOT_FOUND) {
	printf("entry not found\n");
    }else {
	printf("fail\n");
    }

    free(args);
    return 0;
}
