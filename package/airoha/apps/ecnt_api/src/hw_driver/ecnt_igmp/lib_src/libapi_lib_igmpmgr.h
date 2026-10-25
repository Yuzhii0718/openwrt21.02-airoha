/*********************************************************************************
 * decleration and function prototype for igmp module
 *
 * Copyright (C) 2015 Econet Technologies, Corp.
 * All Rights Reserved.
 *
 *********************************************************************************/
#ifndef _LIBAPI_LIB_IGMPMGR_H_
#define _LIBAPI_LIB_IGMPMGR_H_

/*********************************************************************************
 * data structure definition section
 *
 *********************************************************************************/

/* IOCTL CMD */
#define IGMP_ADD_ENTRY					1
#define IGMP_DEL_ENTRY					2
#define IGMP_UPDATE_ENTRY				3
#define IGMP_CLEAR_ENTRY_BY_PORT		4
#define IGMP_CLEAR_ENTRY_ALL			5
#define IGMP_GET_ENTRY					6
#define IGMP_SET_FILTER_MODE			7
#define IGMP_GET_FILTER_MODE			8
#define IGMP_SET_IGMP_SNOOPING_ON_OFF	9
#define IGMP_GET_IGMP_SNOOPING_ON_OFF	10

typedef enum
{
	ECNT_VLAN_TRANSPARENT = 0,
	ECNT_VLAN_REPLACE,
	ECNT_VLAN_REPLACE_AND_REMARK,
	ECNT_VLAN_STRIP,
	ECNT_VLAN_ADD,
	ECNT_VLAN_ADD_AND_REMARK
}ECNT_VLAN_MODE;

/* filter mode */
typedef enum
{
	ECNT_FWD_MODE_IP_AND_VID = 0,
	ECNT_FWD_MODE_MAC,
	ECNT_FWD_MODE_IP_AND_SRC_IP,
	ECNT_FWD_MODE_MAC_AND_VID,
	ECNT_FWD_MODE_MAC_AND_IP,
	ECNT_FWD_MODE_MAC_AND_IP_AND_VID,
}ECNT_MC_FWD_MODE;

typedef enum
{
	ECNT_IGMP_SNOOPING_OFF  = 0,
	ECNT_IGMP_SNOOPING_ON   = 1,
}ECNT_IGMP_SNOOPING_Mode;

typedef struct
{    
	unsigned char  mac_address_byte [6];
}ECNT_MAC_ADDRESS ;

typedef struct
{    
	unsigned char ip_address_array [16] ;
} ECNT_IPV6_ADDRESS ;

typedef struct
{ 
	union{        
		/* IPv4*/         
		unsigned int ipv4_address ;          
		/* IPv6*/         
		ECNT_IPV6_ADDRESS ipv6_address ;     
	}IP ;    
		
	/*True = ipv6, False = ipv4*/     
	unsigned char is_ipv6 ;
}ECNT_IP_ADDRESS ;

typedef struct
{
	ECNT_VLAN_MODE vlan_mode;
	unsigned short outer_tpid;
	unsigned short outer_tci;
	unsigned short inner_tpid;
	unsigned short inner_tci;
}ECNT_VLAN_INFO;

typedef struct
{
	unsigned char port_id;
	short int vlan_id;
	unsigned int pkt_cnt;
	unsigned long long byte_cnt;
	ECNT_IP_ADDRESS group_addr;
	ECNT_IP_ADDRESS src_addr;
	ECNT_MAC_ADDRESS group_mac;
	ECNT_VLAN_INFO vlan_action;
}ECNT_MC_PORT_INFO;

typedef struct
{
	unsigned char port_id;
	int entry_num;
	unsigned int pcnt;
	unsigned long long bcnt;
	ECNT_MC_PORT_INFO *pEntry;
}ECNT_MC_PORT_INFO_GET;

/*********************************************************************************
 * API function prototype declaration section
 *
 *********************************************************************************/
int ecnt_igmp_entry_add(ECNT_MC_PORT_INFO *entry_data);

int ecnt_igmp_entry_del(ECNT_MC_PORT_INFO *entry_data);

int ecnt_igmp_entry_update(ECNT_MC_PORT_INFO *entry_data);

int ecnt_igmp_entry_clear_by_port(unsigned char port_id);

int ecnt_igmp_entry_clear_all(void);

int ecnt_igmp_entry_get(unsigned char port_id, ECNT_MC_PORT_INFO entry[], int *array_size);

int ecnt_igmp_mode_set(ECNT_MC_FWD_MODE mode);

int ecnt_igmp_mode_get(ECNT_MC_FWD_MODE *mode);

int ecnt_igmp_snooping_on_off_set(unsigned char enable);

int ecnt_igmp_snooping_on_off_get(unsigned char *enable);

#endif
