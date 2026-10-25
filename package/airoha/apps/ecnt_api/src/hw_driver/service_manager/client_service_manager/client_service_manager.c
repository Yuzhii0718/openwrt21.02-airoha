#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#include <stdint.h>
#include <sys/mman.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <netinet/ether.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "../inc/inic_common.h"


#define BUF_SIZ		1400
#define INTER_MAGIC 0x9999
#define HEARTBEAT_SIZE	20

char ifName[IFNAMSIZ];


int call_blapi(blapi_hdr_t *blapi_h )
{
	int ret = -1;
	switch(blapi_h->blapi_id)
	{
		case BLAPI_XDSL_RELOAD_KO					 :
			ret = blapi_xdsl_reload_ko(ntohl(blapi_h->argv1.i));
			break;
		case BLAPI_XDSL_GET_FWVER					 :
			ret = blapi_xdsl_get_fwver(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		case BLAPI_XDSL_GET_TSARM					 :
			ret = blapi_xdsl_get_tsarm(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		case BLAPI_XDSL_PTM_DO_RESET_SEQUENCE		 :
			ret = blapi_xdsl_ptm_do_reset_sequence(ntohl(blapi_h->argv1.i), ntohl(blapi_h->argv2.i));
			break;
		case BLAPI_XDSL_GET_INTERFACE_CONFIG_FILE	 :
			ret = blapi_xdsl_get_interface_config_file(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		case BLAPI_XDSL_ATM_CREATE_INTERFACE		 :
			blapi_h->argv1.atm_i.vpi = ntohl(blapi_h->argv1.atm_i.vpi);
			blapi_h->argv1.atm_i.vci = ntohl(blapi_h->argv1.atm_i.vci);
			ret = blapi_xdsl_atm_create_interface(&blapi_h->argv1.atm_i, &blapi_h->argv2.i);
			break;
		case BLAPI_XDSL_ATM_DELETE_INTERFACE		 :
			blapi_h->argv1.i = ntohl(blapi_h->argv1.i);
			ret = blapi_xdsl_atm_delete_interface(blapi_h->argv1.i);
			break;
		case BLAPI_XDSL_CREATE_DEVICE				 :
			ret = blapi_xdsl_create_device(&blapi_h->argv1.atm_d);
			break;
		case BLAPI_TRAFFIC_GET_ETHER_WAN_STATE		 :
			ret =  blapi_traffic_get_ether_wan_state(&blapi_h->argv1.i);
			blapi_h->argv1.i = htonl(blapi_h->argv1.i);
			break;
		case BLAPI_TRAFFIC_GET_WAN_TX_TRAFFIC		 :
			ret =  blapi_traffic_get_wan_tx_traffic(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_RX_TRAFFIC		 :
			ret =  blapi_traffic_get_wan_rx_traffic(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_TX_RATE			 :
			ret =  blapi_traffic_get_wan_tx_rate(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_RX_RATE			 :
			ret =  blapi_traffic_get_wan_rx_rate(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_TX_OCTETS		 :
			ret =  blapi_traffic_get_wan_tx_octets(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_RX_OCTETS		 :
			ret = blapi_traffic_get_wan_rx_octets(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_TX_DISCARD_COUNTER:
			ret =  blapi_traffic_get_wan_tx_discard_counter(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_RX_DISCARD_COUNTER:
			ret = blapi_traffic_get_wan_rx_discard_counter(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;

		case BLAPI_TRAFFIC_GET_WAN_TX_ERROR_COUNTER  :
			ret =  blapi_traffic_get_wan_tx_error_counter(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_RX_ERROR_COUNTER  :
			ret =  blapi_traffic_get_wan_rx_error_counter(&blapi_h->argv1.iu);
			blapi_h->argv1.iu = htonl(blapi_h->argv1.iu);
			break;
		case BLAPI_TRAFFIC_GET_WAN_TX_STATISTICS	 :
		{
			uint64_t temp_value;
			ret =  blapi_traffic_get_wan_tx_statistics(&blapi_h->argv1.tx_s);
			htonll(blapi_h->argv1.tx_s.frame_cnt,&temp_value); 
			blapi_h->argv1.tx_s.frame_cnt = temp_value;
			htonll(blapi_h->argv1.tx_s.frame_len,&temp_value); 
			blapi_h->argv1.tx_s.frame_len = temp_value;
			htonll(blapi_h->argv1.tx_s.drop_cnt,&temp_value); 
			blapi_h->argv1.tx_s.drop_cnt = temp_value;
			htonll(blapi_h->argv1.tx_s.broadcast,&temp_value); 
			blapi_h->argv1.tx_s.broadcast = temp_value;
			htonll(blapi_h->argv1.tx_s.multicast,&temp_value); 
			blapi_h->argv1.tx_s.multicast = temp_value;
			htonll(blapi_h->argv1.tx_s.less_64,&temp_value); 
			blapi_h->argv1.tx_s.less_64 = temp_value;
			htonll(blapi_h->argv1.tx_s.more_1518,&temp_value); 
			blapi_h->argv1.tx_s.more_1518 = temp_value;
			htonll(blapi_h->argv1.tx_s.eq_64,&temp_value); 
			blapi_h->argv1.tx_s.eq_64 = temp_value;
			htonll(blapi_h->argv1.tx_s.from_65_to_127,&temp_value); 
			blapi_h->argv1.tx_s.from_65_to_127 = temp_value;
			htonll(blapi_h->argv1.tx_s.from_128_to_255,&temp_value); 
			blapi_h->argv1.tx_s.from_128_to_255 = temp_value;
			htonll(blapi_h->argv1.tx_s.from_256_to_511,&temp_value); 
			blapi_h->argv1.tx_s.from_256_to_511 = temp_value;
			htonll(blapi_h->argv1.tx_s.from_512_to_1023,&temp_value); 
			blapi_h->argv1.tx_s.from_512_to_1023 = temp_value;
			htonll(blapi_h->argv1.tx_s.from_1024_to_1518,&temp_value); 
			blapi_h->argv1.tx_s.from_1024_to_1518 = temp_value;
			break;
		}
		case BLAPI_TRAFFIC_GET_WAN_RX_STATISTICS	 :
		{
			uint64_t temp_value;
			ret = blapi_traffic_get_wan_rx_statistics(&blapi_h->argv1.rx_s);
			htonll(blapi_h->argv1.rx_s.oversize,&temp_value); 
			blapi_h->argv1.rx_s.oversize = temp_value;
			htonll(blapi_h->argv1.rx_s.undersize,&temp_value); 
			blapi_h->argv1.rx_s.undersize = temp_value;
			htonll(blapi_h->argv1.rx_s.frame_cnt,&temp_value); 
			blapi_h->argv1.rx_s.frame_cnt = temp_value;
			htonll(blapi_h->argv1.rx_s.frame_len,&temp_value); 
			blapi_h->argv1.rx_s.frame_len = temp_value;
			htonll(blapi_h->argv1.rx_s.drop_cnt,&temp_value); 
			blapi_h->argv1.rx_s.drop_cnt = temp_value;
			htonll(blapi_h->argv1.rx_s.broadcast,&temp_value); 
			blapi_h->argv1.rx_s.broadcast = temp_value;
			htonll(blapi_h->argv1.rx_s.multicast,&temp_value); 
			blapi_h->argv1.rx_s.multicast = temp_value;
			htonll(blapi_h->argv1.rx_s.crc,&temp_value); 
			blapi_h->argv1.rx_s.crc = temp_value;
			htonll(blapi_h->argv1.rx_s.fragment,&temp_value); 
			blapi_h->argv1.rx_s.fragment = temp_value;
			htonll(blapi_h->argv1.rx_s.jabber,&temp_value); 
			blapi_h->argv1.rx_s.jabber = temp_value;
			htonll(blapi_h->argv1.rx_s.less_64,&temp_value); 
			blapi_h->argv1.rx_s.less_64 = temp_value;
			htonll(blapi_h->argv1.rx_s.more_1518,&temp_value); 
			blapi_h->argv1.rx_s.more_1518 = temp_value;
			htonll(blapi_h->argv1.rx_s.eq_64,&temp_value); 
			blapi_h->argv1.rx_s.eq_64 = temp_value;
			htonll(blapi_h->argv1.rx_s.from_65_to_127,&temp_value); 
			blapi_h->argv1.rx_s.from_65_to_127 = temp_value;
			htonll(blapi_h->argv1.rx_s.from_128_to_255,&temp_value); 
			blapi_h->argv1.rx_s.from_128_to_255 = temp_value;
			htonll(blapi_h->argv1.rx_s.from_256_to_511,&temp_value); 
			blapi_h->argv1.rx_s.from_256_to_511 = temp_value;
			htonll(blapi_h->argv1.rx_s.from_512_to_1023,&temp_value); 
			blapi_h->argv1.rx_s.from_512_to_1023 = temp_value;
			htonll(blapi_h->argv1.rx_s.from_1024_to_1518,&temp_value); 
			blapi_h->argv1.rx_s.from_1024_to_1518 = temp_value;
			break; 
		}
		case BLAPI_GET_WAN_LINK_MODE				 :
			ret = blapi_system_get_spf_linkup(&blapi_h->argv1.i);
			blapi_h->argv1.i = htonl(blapi_h->argv1.i);
			break;
		case BLAPI_SRC_CMD						 :
			ret = blapi_system_client_command(blapi_h->argv1.ch);
			break;
		case BLAPI_CONSOLE_AND_PRINT_SWITCH						 :
			break;
		case BLAPI_TRAFFIC_GET_WAN_PORT_STATISTICS:
		{
			#if defined(TCSUPPORT_CPU_EN7517)
			uint64_t temp_value;
			ret =  blapi_traffic_get_wan_port_statistics(&blapi_h->argv1.port_s, ntohl(blapi_h->argv2.i));
	
			htonll(blapi_h->argv1.port_s.inBytes,&temp_value); 
			blapi_h->argv1.port_s.inBytes = temp_value;
			htonll(blapi_h->argv1.port_s.inPkts,&temp_value); 
			blapi_h->argv1.port_s.inPkts = temp_value;
			htonll(blapi_h->argv1.port_s.inUnicast,&temp_value); 
			blapi_h->argv1.port_s.inUnicast = temp_value;
			htonll(blapi_h->argv1.port_s.inMulticast,&temp_value); 
			blapi_h->argv1.port_s.inMulticast = temp_value;
			htonll(blapi_h->argv1.port_s.inBroadcast,&temp_value); 
			blapi_h->argv1.port_s.inBroadcast = temp_value;
			htonll(blapi_h->argv1.port_s.inError,&temp_value); 
			blapi_h->argv1.port_s.inError = temp_value;
			htonll(blapi_h->argv1.port_s.inDiscard,&temp_value); 
			blapi_h->argv1.port_s.inDiscard = temp_value;
			htonll(blapi_h->argv1.port_s.outBytes,&temp_value); 
			blapi_h->argv1.port_s.outBytes = temp_value;
			htonll(blapi_h->argv1.port_s.outPkts,&temp_value); 
			blapi_h->argv1.port_s.outPkts = temp_value;
			htonll(blapi_h->argv1.port_s.outUnicast,&temp_value); 
			blapi_h->argv1.port_s.outUnicast = temp_value;
			htonll(blapi_h->argv1.port_s.outMulticast,&temp_value); 
			blapi_h->argv1.port_s.outMulticast = temp_value;
			htonll(blapi_h->argv1.port_s.outBroadcast,&temp_value); 
			blapi_h->argv1.port_s.outBroadcast = temp_value;
			htonll(blapi_h->argv1.port_s.outError,&temp_value); 
			blapi_h->argv1.port_s.outError = temp_value;
			htonll(blapi_h->argv1.port_s.outDiscard,&temp_value); 
			blapi_h->argv1.port_s.outDiscard = temp_value;
			SISM_DBG_MSG(LOG_INFO,"blapi_h->argv1.port_s.inBytes 0x%016llx\n",blapi_h->argv1.port_s.inBytes);
			SISM_DBG_MSG(LOG_INFO,"blapi_h->argv1.port_s.outDiscard 0x%016llx\n",blapi_h->argv1.port_s.outDiscard);
			break;
			#endif
		}
		case BLAPI_TRAFFIC_GET_WAN_PORT_LINK_STATISTICS:
		{
			#if defined(TCSUPPORT_CPU_EN7517)
			ret =  blapi_traffic_get_wan_port_link_state(&blapi_h->argv1.link_s, ntohl(blapi_h->argv2.i));
			SISM_DBG_MSG(LOG_INFO,"link_status    %s\n", ((blapi_h->argv1.link_s.link_status) ? "up" : "down"));
			SISM_DBG_MSG(LOG_INFO,"auto_ng_status %s\n", ((blapi_h->argv1.link_s.auto_ng_status) ? "force" : "auto"));
			SISM_DBG_MSG(LOG_INFO,"duplex_status  %s\n", ((blapi_h->argv1.link_s.duplex_status) ? "full-duplex" : "half-duplex"));
			SISM_DBG_MSG(LOG_INFO,"speed status %d\n", blapi_h->argv1.link_s.speed_status);
			break;
			#endif
		}
		case BLAPI_TRAFFIC_SET_WAN_PORT_LINK_MODE:
		{
			#if defined(TCSUPPORT_CPU_EN7517)
			ret = blapi_traffic_set_wan_port_link_mode(ntohl(blapi_h->argv3.i), ntohl(blapi_h->argv2.i));
			break;
			#endif
		}	
		case BLAPI_XDSL_GET_ADSL_MODE:
		{
			ret = blapi_xdsl_get_adsl_mode(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_ANNEX:
		{
			ret = blapi_xdsl_get_adsl_annex(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_RX_BEAR_TPSTC_TYPE:
		{
			ret = blapi_xdsl_get_adsl_rx_bear_tpstc_type(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_ATTAIN_RATE:
		{
			ret = blapi_xdsl_get_adsl_attain_rate(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_NEAR_OP_DATA:
		{
			ret = blapi_xdsl_get_adsl_near_op_data(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_NEAR_ITUID:
		{
			ret = blapi_xdsl_get_adsl_near_ituid(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_STATISTIC_2:
		{
			ret = blapi_xdsl_get_adsl_statistic_2(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_LAST_DROP_REASON:
		{
			ret = blapi_xdsl_get_adsl_last_drop_reason(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_GET_ADSL_FAR_ITUID:
		{
			ret = blapi_xdsl_get_adsl_far_ituid(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		case BLAPI_XDSL_SET_POWER_UP_DOWN:
		{
			ret = blapi_xdsl_set_power_up_down(ntohl(blapi_h->argv1.i));
			break;
		}
		case BLAPI_SFP_SET_POWER_UP_DOWN:
		{
			ret = blapi_sfp_set_power_up_down(ntohl(blapi_h->argv1.i));
			break;
		}
		
		case BLAPI_XDSL_SET_ADSL_ANNEX:
        {
            ret = blapi_xdsl_set_adsl_annex(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_LOW_POWER:
        {
            ret = blapi_xdsl_set_adsl_low_power(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_AELEM:
        {
            ret = blapi_xdsl_set_adsl_aelem(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_TESTLAB:
        {
            ret = blapi_xdsl_set_adsl_testlab(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_SYSVID:
        {
            ret = blapi_xdsl_set_adsl_sysvid(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_VERSION:
        {
            ret = blapi_xdsl_set_adsl_version(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_GINP:
        {
            ret = blapi_xdsl_set_adsl_ginp(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_SRA:
        {
            ret = blapi_xdsl_set_adsl_sra(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_GVECTOR:
        {
            ret = blapi_xdsl_set_adsl_gvector(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_BITSWAP:
        {
            ret = blapi_xdsl_set_adsl_bitswap(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_SOS_ROC:
        {
            ret = blapi_xdsl_set_adsl_sos_roc(ntohl(blapi_h->argv1.i), ntohl(blapi_h->argv2.i));
			break;
        }
        case BLAPI_XDSL_SET_ADSL_US0:
        {
            ret = blapi_xdsl_set_adsl_us0(blapi_h->argv1.ch);
			break;
        }
		case BLAPI_XDSL_SET_ADSL_DYING_GASP:
	    {
            ret = blapi_xdsl_set_adsl_dying_gasp(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_VIRTUAL_NOISE:
        {
            ret = blapi_xdsl_set_adsl_virtual_noise(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_MODE:
        {
            ret = blapi_xdsl_set_adsl_mode(blapi_h->argv1.ch);
			break;
        }
        case BLAPI_XDSL_SET_ADSL_PROFILE:
        {
            ret = blapi_xdsl_set_adsl_profile(blapi_h->argv1.ch);
			break;
        }
		case BLAPI_XDSL_GET_OAM_PING_STATUS:
		{
			ret = blapi_xdsl_get_oam_ping_status(blapi_h->argv1.ch, ntohl(blapi_h->argv2.i));
			break;
		}
		default:
			break;
		
	}
	return ret;

}


int receive_blapi(uint8_t* payload)
{
	int var_num = 0;
	int ret = 0;
	uint8_t* buf;
#if DEBUG
	int i;
#endif

	//blapi_hdr_t *blapi_h = (blapi_hdr_t*)(payload + sizeof(src_hdr_t));
	blapi_hdr_t *blapi_send;
	//var_num = blapi_h->argc;
	blapi_hdr_t blapi_h;
	src_hdr_t src_h;

	memcpy(&blapi_h, payload+sizeof(src_hdr_t), sizeof(blapi_hdr_t));
	
	SISM_DBG_MSG(LOG_INFO,"blapi_id = %d\n",ntohl(blapi_h.blapi_id));

#if DEBUG
	SISM_DBG_MSG(LOG_DEBUG, "\tData:");
	for (i=0; i<80; i+=8)
	{
		SISM_DBG_MSG(LOG_DEBUG, "\t%02x:02x:02x:02x:02x:02x:02x:02x", 
			payload[i], payload[i+1], payload[i+2], payload[i+3], payload[i+4],payload[i+5], payload[i+6], payload[i+7]);
	}

	SISM_DBG_MSG(LOG_DEBUG, "-------id is %d , argv1 is %x size of src_hdr is %d",ntohl(blapi_h->blapi_id), blapi_h->argv1,sizeof(src_hdr_t));
#endif

	blapi_h.blapi_id = ntohl(blapi_h.blapi_id);
	/* exec blapi*/
	ret = call_blapi(&blapi_h);
	
	buf = req_pt_pkt_buf(BUF_SIZ);
	if (buf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR,"%s: alloc failed!",__FUNCTION__);
		return SOCKET_FAIL;
	}


	/* prepare reply packet*/
	//src_hdr_t *src_h = (src_hdr_t *)buf;

	memset(&src_h, 0, sizeof(src_hdr_t));
	blapi_send = (blapi_hdr_t*)(buf + sizeof(src_hdr_t));
	src_h.magic_no = htonl(0x75177517);
	src_h.cmd_opcode = htonl(OP_BLAPI_REPLY);

	memcpy(buf, &src_h, sizeof(src_hdr_t));
	
	//blapi_send->blapi_id = htonl(blapi_h.blapi_id);
	//blapi_send->argc =     htonl(blapi_h.argc);
	blapi_h.blapi_id = htonl(blapi_h.blapi_id);
	blapi_h.argc = htonl(blapi_h.argc);
	blapi_h.ret = htonl(ret);
	memcpy(blapi_send, &blapi_h,sizeof(blapi_hdr_t));
	ret = pt_pkt_send(ETH_TYPE_SRC, buf,BUF_SIZ);
	free_pt_pkt_buf(buf);
	if (ret)
	{
		return 1;
	}
	

}

int receive_proc(uint8_t* payload)
{
	int var_num = 0;
	int ret = 0;
	int i = 0;
	uint8_t* buf;
	char* proc_name =NULL;
	src_hdr_t src_h;
	src_proc_hdr_t proc_h;
	int  fd=0, byte=0;
	int size = BUF_SIZ - sizeof(src_h)-sizeof(proc_h);
	int len = 0;
	SISM_DBG_MSG(LOG_DEBUG,"Client_manager: receive proc pkt");

	memcpy(&src_h, payload, sizeof(src_h));
	memcpy(&proc_h, payload+sizeof(src_h), sizeof(proc_h));

	len = ntohs(src_h.length)+1;
	proc_name = (uint8_t *)malloc(len);
	if (proc_name == NULL)
	{
		SISM_DBG_MSG(LOG_ERR,"%s: alloc failed!",__FUNCTION__);
		return SOCKET_FAIL;
	}
	SISM_DBG_MSG(LOG_DEBUG,"%s: src_h.length = %d",__FUNCTION__,len);
	memcpy(proc_name, payload+sizeof(src_h)+sizeof(proc_h),len);
	proc_name[len-1]='\0';
	
	SISM_DBG_MSG(LOG_DEBUG,"%s: proc_name = %s, strlen = %d",__FUNCTION__, proc_name, strlen(proc_name));
	buf = req_pt_pkt_buf(BUF_SIZ);
	if (buf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR,"%s: alloc failed!",__FUNCTION__);
		free(proc_name);
		return SOCKET_FAIL;
	}

	pt_fd_open(ETH_TYPE_PROC);
	if (ntohs(proc_h.rw_op) == READ_PROC_OP)
	{
		
		fd = open(proc_name,O_RDONLY);
		if(fd == -1){
			SISM_DBG_MSG(LOG_ERR,"%s: open proc fail", __FUNCTION__);
			proc_h.ret = htonl(NO_PROC);
			goto reply_err;
		}

		do{
			byte = read(fd, buf+sizeof(src_h)+sizeof(proc_h), size-1);
			SISM_DBG_MSG(LOG_DEBUG,"%s: receive %d bytes", __FUNCTION__,byte);
			if(byte < 0){
				SISM_DBG_MSG(LOG_ERR,"%s: read file fail", __FUNCTION__);
				close(fd);
				proc_h.ret = htonl(NO_PROC);
				goto reply_err;
			}else
			{
				src_h.length = htons(byte);
				proc_h.ret = htonl(PT_SUCCESS);
				memcpy(buf, &src_h, sizeof(src_h));
				memcpy(buf+sizeof(src_h), &proc_h, sizeof(proc_h));
				SISM_DBG_MSG(LOG_DEBUG,"%s: send %d bytes",__FUNCTION__, byte);
				ret = pt_pkt_send(ETH_TYPE_PROC, buf,byte+sizeof(proc_h));

			}
		}while(byte == (size-1));	
	}
	else
	{
		ret = system(proc_name);
		if (ret != 0)
		{
			proc_h.ret = htonl(UNHANDLE_FAIL);
		}
		else
		{
			proc_h.ret = htonl(PT_SUCCESS);
		}
			memcpy(buf+sizeof(src_h), &proc_h, sizeof(proc_h));
			ret = pt_pkt_send(ETH_TYPE_PROC, buf,byte+sizeof(proc_h));		
	}

done:
	free_pt_pkt_buf(buf);
	pt_fd_close(ETH_TYPE_PROC);
	free(proc_name);
	return 0;

reply_err:
	memcpy(buf+sizeof(src_h), &proc_h, sizeof(proc_h));
	ret = pt_pkt_send(ETH_TYPE_PROC, buf,sizeof(proc_h));
	goto done;
	return 0;

}

int receive_src(uint8_t* payload)
{

	int var_num = 0;
	int ret = 0;
	int i = 0;
	src_hdr_t src_h;

	SISM_DBG_MSG(LOG_INFO,"Client_manager: receive src pkt");
	memcpy(&src_h, payload, sizeof(src_hdr_t));

	if (ntohs(src_h.cmd_opcode) == OP_PROC)
	{
		receive_proc(payload);
	}
	else if(ntohs(src_h.cmd_opcode) == OP_BLAPI)
	{
		receive_blapi(payload);
	}
	return 0;

}

void *heartbeat(void* input)
{
	unsigned int interval = 3;
	unsigned char* buf = NULL;
	unsigned int magic = 0x48424842;
	unsigned int data = 0;
	int ret = 0;
	share_data_t s_data;
	int shmid = 0;
	key_t key;
	char* shm;

	SISM_DBG_MSG(LOG_INFO,"Client Heartbeat start!");
	pt_fd_open(ETH_TYPE_EVENT,5);

	key = 5266;
	shmid = shmget(key, 40, IPC_CREAT | 0666);

	if (shmid <0){
		perror("shmget");
		exit(1);
	}	
	shm = shmat(shmid, NULL,0);
	memcpy(&s_data, shm, sizeof(s_data));
	
	buf = req_pt_pkt_buf(BUF_SIZ);
	if (buf == NULL)
		return SOCKET_FAIL;

	if(s_data.inter_magic != INTER_MAGIC)
	{

		ret = pt_pkt_recv(ETH_TYPE_EVENT, buf);
		if(ret != PT_SUCCESS)
		{
			goto err;
		}

		memcpy(&data, buf,4);
		if (ntohl(data) == magic)
		{
			memcpy(&data, buf+4, 4);
			interval = ntohl(data);
			memcpy(&s_data, shm, sizeof(s_data));
			s_data.inter_magic = INTER_MAGIC;
			s_data.interval = interval;
			memcpy(shm, &s_data,sizeof(s_data));
		}
		else
		{
			SISM_DBG_MSG(LOG_ERR, "receive error control packet!");
			goto err;
		}
	}
	else
	{

		interval = s_data.interval;
	}
	while(1)
	{

		pt_pkt_send(ETH_TYPE_EVENT,buf,HEARTBEAT_SIZE);
		sleep(interval);
	}

	err:

		free_pt_pkt_buf(buf);
		pthread_exit(0);
		return UNHANDLE_FAIL;
}

int main(int argc, char *argv[])
{
	uint8_t *r_buf;
	int ret = 0;
	int shmid = 0;
	key_t key;
	char* shm, *s;
	int testsocket = 0;
	int len = 0;
	share_data_t s_data;
	pthread_t t;
	

	len = 10;
	key = 5266;

	if (argc <2)
	{
		SISM_DBG_MSG(LOG_ERR,"Usage: csm [interface]");
		return 0;
	}
	shmid = shmget(key, 40, IPC_CREAT | 0666);

	if (shmid <0){
		perror("shmget");
		exit(1);
	}	
	shm = shmat(shmid, NULL,0);
	memcpy(&s_data.itf_name, argv[1], len);
	s_data.len = len;

	s_data.pt_enable = SRC_EN | HEARTBEAT_EN;

	memcpy(shm, &s_data,sizeof(s_data));

	if (pthread_create(&t, NULL, heartbeat, (void*)shm) <0 )
	{
		SISM_DBG_MSG(LOG_ERR,"heartbeat: create thread fail");
		return -1;
	}

	pt_fd_open(ETH_TYPE_SRC, 5);
	r_buf = req_pt_pkt_buf(BUF_SIZ);
	if (r_buf == NULL)
		return SOCKET_FAIL;
	while(1)
	{
		ret = pt_pkt_recv(ETH_TYPE_SRC, r_buf);
		if(ret == 0)
			{
				receive_src(r_buf);
			}
		else
		{
			SISM_DBG_MSG(LOG_ERR,"unhandle packet!!");
		}
	}
	
	
	
	return 0;
}



