#ifndef _FE_PPE_MISC_H_
#define _FE_PPE_MISC_H_

/*TCSUPPORT_TLS*/
#if !defined(TCSUPPORT_MULTI_USER_ITF)
#define	DEV_OFFSET			28
#endif

#define ONU_TYPE_HGU 2
#define ONU_TYPE_SFU 1

extern int (*ra_sw_nat_hook_free) (struct sk_buff * skb);
extern int (*ra_sw_nat_hook_tls_vtag_handle_hook)(struct sk_buff** pskb);

#define VLAN_NOT_CARE 0xfff

#define PPE_FPORT_GDM2	2

typedef struct
{
	struct list_head  list;
	unsigned char uni_port_id;/*0~3*/
	unsigned short uni_vlan;/*2~4094,4095 means not care*/
	unsigned short gem_port;/*gemport id*/
	unsigned short ani_vlan;/*2~4094,4095 means not care*/
}FE_Tls_List_Entry_t;

typedef struct
{
	struct list_head  list;
	unsigned char port_id;/*0~3:lan port, 4:xfi*/
	unsigned short tls_vlan;/*2~4094*/
	unsigned int tls_cfg_index; /*tls cfg index 0~5*/
}FE_Tls_List_port_Vlan_t;

/*TCSUPPORT_TLS end*/

#endif

