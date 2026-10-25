/*********************************************************************************
 * decleration and function prototype for PPTP VPN offload module
 *
 * Copyright (C) 2018 Econet Technologies, Corp.
 * All Rights Reserved.
 *
 *********************************************************************************/
#ifndef PPTP_HW_OFFLOAD_H_
#define	PPTP_HW_OFFLOAD_H_
#include <net/gre.h>
//#include <net/ip.h>

/*********************************************************************************
 * mirco definition setction
 *
 *********************************************************************************/
#ifndef uint8
#define uint8	unsigned char
#endif
	
#ifndef uint16
#define uint16	unsigned short
#endif
	
#ifndef uint32
#define uint32	unsigned long
#endif

#define IP_HEADER_LEN	20
#define PPTP_HLEN 16
#define PPP_HLEN 4


#define PPTP_GRE_PROTO  0x880B
#define PPTP_GRE_VER    0x1

#define PPTP_GRE_FLAG_C	0x80
#define PPTP_GRE_FLAG_R	0x40
#define PPTP_GRE_FLAG_K	0x20
#define PPTP_GRE_FLAG_S	0x10
#define PPTP_GRE_FLAG_A	0x80

#define PPTP_GRE_IS_C(f) ((f)&PPTP_GRE_FLAG_C)
#define PPTP_GRE_IS_R(f) ((f)&PPTP_GRE_FLAG_R)
#define PPTP_GRE_IS_K(f) ((f)&PPTP_GRE_FLAG_K)
#define PPTP_GRE_IS_S(f) ((f)&PPTP_GRE_FLAG_S)
#define PPTP_GRE_IS_A(f) ((f)&PPTP_GRE_FLAG_A)


/*********************************************************************************
* data type definition setction
*
*********************************************************************************/
typedef enum
{
	PPTP_PKT_NONE = 0,
	PPTP_PKT_CONTROL = 1,
	PPTP_PKT_DATA,
}PPTP_PKT_TYPE;

struct ppphdr
{
	unsigned char addr; 	/* address */
	unsigned char ctrl; 	/* control */
	unsigned short protocol;/* protocol */
};
struct pptp_gre_header {
	u8  flags;
	u8  ver;
	__be16 protocol;
	__be16 payload_len;
	__be16 call_id;
	__be32 seq;
	__be32 ack;
} __packed;

struct PptpHeadEntry 
{
	
	unsigned short call_id; 	/* Call ID */
	/* hw_nat entry index */
	int entry_index;

	/* IP header */
	unsigned char iph[sizeof(struct iphdr)];

	/* gre_ppp header */
	unsigned char pptp_greh[sizeof(struct pptp_gre_header)];
	int pptp_hlen;

	/* ppp header */
	unsigned char ppph[sizeof(struct ppphdr)];
	
	struct list_head list;
};



#endif


