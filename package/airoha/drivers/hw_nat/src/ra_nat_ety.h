#ifndef _RA_NAT_ETY_H
#define _RA_NAT_ETY_H

#ifndef NEXTHDR_IPIP
#define NEXTHDR_IPIP 4
#endif

#define HWNAT_OFF_UNI	1
#define HWNAT_OFF_MUL4	2
#define HWNAT_OFF_MUL6	3

#define MODE_NO_FOE	2
#define MODE_BUILD_FOE	3

#define K1_TO_K0(x) ((unsigned long)(x) & 0x9fffffff)  /* kseg1 to kseg0 */

#define CHECK_MAGIC(Magic)	    (((Magic) == FOE_MAGIC_GE)   || \
                    ((Magic) == FOE_MAGIC_PTM_LAN) || \
                    ((Magic) == FOE_MAGIC_XSI) || \
				    ((Magic) == FOE_MAGIC_WLAN_7615) || \
				    ((Magic) == FOE_MAGIC_WLAN) || \
                                    ((Magic) == FOE_MAGIC_WLAN_TX) || \
				    ((Magic) == FOE_MAGIC_ATM) || \
					((Magic) == FOE_MAGIC_PTM) || \
					((Magic) == FOE_MAGIC_EPON) || \
					((Magic) == FOE_MAGIC_GPON)|| \
					((Magic) == FOE_MAGIC_AE_WAN)|| \
					((Magic) == FOE_MAGIC_XSI_GDM4)|| \
					((Magic) == FOE_MAGIC_OLT))

#define FOEINFOSTORE 0
#define FOEINFORECOVER 1


typedef struct PpeEntryList_s
{
	struct list_head  list;
	int hwnat_type;   //ipv6 or ipv4
	int hwnat_index;   //get from skb buff	
	short int  hwnat_vid; //vlan id	
	unsigned char grp_addr[16];	
}PpeEntryList_t;

typedef struct ppeNptV6Prefix_s
{
	unsigned char used;
	unsigned char ifname[16];
	unsigned char snpt;
	unsigned short s_prefix[8];
	unsigned int s_prefix_len;
	unsigned char dnpt;
	unsigned short d_prefix[8];
	unsigned int d_prefix_len;
    unsigned int hw_prefix_len;/*set in ppe*/
}ppeNptV6Prefix_s_t;

#define PPE_NPTV6_PREFIX_MAX_NUM 16

/* for 2544 test */
#define PKT_LEN_MONITOR_NUM	5
struct LenRate
{
	__u16 len;
	__u32 rate; //Layer 2, kps	
};
struct PktLenMonitor
{
	int enable;
	__u32 setted_rate;
	struct LenRate len_rate[PKT_LEN_MONITOR_NUM];
};


#define PPP_HLEN	4
#define L2TP_MIN_HDR_LEN 6

#define L2TP_UDP_LISTEN_PORT  1701
#define TYP_BIT(ver) (ntohs(ver) & 0x8000)
#define LEN_BIT(ver) (ntohs(ver) & 0x4000)
#define SEQ_BIT(ver) (ntohs(ver) & 0x0800)
#define OFS_BIT(ver) (ntohs(ver) & 0x0200)
#define VER_BIT(ver) (ntohs(ver) & 0x000f)

#define L2TP_HDR_VER_2	   0x0002
#define L2TP_HDR_VER_3	   0x0003

/*********************************************************************************
* data type definition setction
*
*********************************************************************************/

struct l2tp_hdr
{
	unsigned short ver; 	/* Version and friends */
	unsigned short length;	/* Optional Length */
	unsigned short tid; 	/* Tunnel ID */
	unsigned short cid; 	/* Caller ID */
	unsigned short Ns;		/* Optional next sent */
	unsigned short Nr;		/* Optional next received */
	unsigned short o_size;	/* Optional offset size */
	unsigned short o_pad;	/* Optional offset padding */
};

struct ppphdr
{
	unsigned char addr;		/* address */
	unsigned char ctrl;		/* control */
	unsigned short protocol;/* protocol */
};

/*ipv4 tunnel*/

#define L2GRE_TUNNEL	0
#define VXLAN_TUNNEL	1
#define L2TP_TUNNEL	2
#define L3GRE_TUNNEL	3
#define DSLITE_TUNNEL	4
#define T6RD_TUNNEL	5

typedef enum
{
    L2TP_DOWN_TBL_IDX=0,
    VXLAN_DOWN_TBL_IDX,
    L2GRE_DOWN_TBL_IDX,
    L3GRE_DOWN_TBL_IDX,
    VXLAN_V6_DOWN_TBL_IDX,
}TUNNEL_DOWN_IDX;

typedef struct
{
	struct list_head  list;
	unsigned int foe_num;
}PpeHw_Tunnel_Session_t;

int hwnat_check_entry_is_bind(int index);

#define VXLAN_DEFAULT_UDP_PORT 4789



#endif

