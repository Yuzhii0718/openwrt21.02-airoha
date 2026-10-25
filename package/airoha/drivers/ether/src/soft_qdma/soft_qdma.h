/*
** $Id: //BBN_Linux/Branch/Branch_for_Rel_SDK_ASB_20160112/tclinux_phoenix/modules/private/net/soft_qdma.h#1 $
*/
/************************************************************************
 *
 *	Copyright (C) 2008 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
/*

 */
#ifndef _SOFTQDMA_H
#define _SOFTQDMA_H
#include <linux/version.h>

#ifndef uint32
#define uint32	unsigned long
#endif

#define TRUE	1
#define FALSE	0

#define RTSP_OFFLOAD	0xff
#define RTSP_OFFLOAD_WLAN	0xfe
#define RTSP 			(0x01<<16)

int soft_qdma_rtsp_data_handle(struct sk_buff * skb, char *rb_ptr, unsigned int datalen);

#define PDMA_TXRING_0	0
#define PDMA_TXRING_1	1
#define PDMA_TXRING_2	2
#define PDMA_TXRING_3	3
#define DOWN 0
#define UP 1


int two_port_flag_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data);
int two_port_flag_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data);

void soft_qdma_pkt_proc(struct sk_buff *skb, struct net_device *dev, uint32 foe_ds_offload);

#endif /* _SOFTQDMA_H */

