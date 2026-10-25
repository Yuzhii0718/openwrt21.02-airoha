/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include "lib_pt.h"
#include <sys/shm.h>
#include <sys/ipc.h>

uint8_t dest_mac[6]={0xff,0xff,0xff,0xff,0xff,0xff};

int timeout_val = 90;

#if TCSUPPORT_SISM_HOST
#define DEFAULT_IF	"nas10"
const int inic_stage = 0;
#else
#define DEFAULT_IF	"eth0.1.99"
int inic_stage = 0;
#endif
uint16_t cmd_id = 0;

static uint8_t is_first_time = 0;
//static pthread_mutex_t pt_lock;
static int inic_fd = -1;
static int src_fd = -1;
static int hb_fd = -1;
static int proc_fd = -1;
static int wan_fd = -1;
//static int first_src = 0;


#define SEND_BUF	1
#define RCV_BUF		2
#define SRC_PKT_LEN	 (sizeof(src_hdr_t)+sizeof(blapi_hdr_t)+sizeof(struct ether_header))


char ifName[IFNAMSIZ];

void htonll(uint64_t val, uint64_t * target)
{
	uint64_t val_64;
	val_64 = (((uint64_t) htonl(val)) <<32)+htonl(val>> 32);
	memcpy(target,&val_64,sizeof(uint64_t));
}

void ntohll(uint64_t val, uint64_t * target)
{
	uint64_t val_64;
	val_64 = (((uint64_t) ntohl(val)) <<32)+ntohl(val>> 32);
	memcpy(target,&val_64,sizeof(uint64_t));
}

#ifdef TCSUPPORT_SISM_HOST
int sism_log_level = LOG_INFO;
int sism_log_hdr = 0;
//int sism_log_level = LOG_DEBUG;
//int sism_log_hdr = 1;

void sismDbgMsg(int level, const char *func, int line, const char *format, ...)
{
	char cmdtmp[128] = "\0";
	char cmd[256] = "\0";

	if(level == LOG_PRINTF)
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);

		return;
	}

	if(level < sism_log_level)
		return;

	strcpy(cmd, "echo \"");

	if(sism_log_hdr)
	{
		strcat(cmd, "[SISM]");
		if(sism_log_level == LOG_DEBUG)
			strcat(cmd, "[DBG]");
		else if(sism_log_level == LOG_INFO)
			strcat(cmd, "[INFO]");
		else if(sism_log_level == LOG_ERR)
			strcat(cmd, "[ERR]");

		snprintf(cmdtmp, sizeof(cmdtmp), "[%s:L%d] ", func, line);
		strcat(cmd, cmdtmp);
	}

	va_list args;
	va_start(args, format);
	vsnprintf(cmdtmp, sizeof(cmdtmp), format, args);
	va_end(args);
	strcat(cmd, cmdtmp);

	strcat(cmd, "\" > /proc/tc3162/dbg_msg");

	system(cmd);
	return;
}

#if 0
int sism_dbg_init(void){
	struct proc_dir_entry *sismProc;
	sismProc = create_proc_entry("tc3162/sism_debug_level", 0, NULL);
	if (sismProc){
		sismProc->read_proc = sism_dbg_read_proc;
		sismProc->write_proc = sism_dbg_write_proc;
	} else {
		printk("proc_entry tc3162/sism_debug_level alloc fail\n");
	}

	return 0;
}

void sism_dbg_exit(void){
	remove_proc_entry("tc3162/sism_debug_level", NULL);
}

static int sism_dbg_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(buf+len, "SISM Debug: level=%d, header=%d", sism_log_level, sism_log_hdr);
	len += sprintf(buf+len, "\tlevel: 0:DEBUG, 1:INFO, 2: NOTIFY, 3:ERROR");
	len += sprintf(buf+len, "\theader: 0:Off, 1:On");

	return len;
}

/*
PROC command: echo <DBG level> <(opt)DBG Header>: /proc/tc3162/sism_debug_level 
*/
static int sism_dbg_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int dbgLevel;
	int dbgHdr;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	
	val_string[count] = '\0';
	
	if (sscanf(val_string, "%d %d", &dbgLevel, &dbgHdr) < 2) {
		printk("usage: <DBG level> <DBG Header>\n");
		return count;
	}

	sism_log_level = 0;
	sism_log_hdr = 0;

	SISM_DBG_MSG(LOG_INFO, "SISM Debug: level=%d, header=%d", sism_log_level, sism_log_hdr);
	SISM_DBG_MSG(LOG_INFO, "\tlevel: 0:DEBUG, 1:INFO, 2: NOTIFY, 3:ERROR");
	SISM_DBG_MSG(LOG_INFO, "\theader: 0:Off, 1:On");

	sism_log_level = dbgLevel;
	sism_log_hdr = dbgHdr;

	return count;
}
#else
int sism_dbg_init(void){return 0;}
void sism_dbg_exit(void){}
#endif
#endif

int pt_fd_open(unsigned short ethertype, int timeout)
{
	int ret = 0;
	int sockfd = -1;
	int sockopt;

	struct ifreq if_mac;
#ifdef TCSUPPORT_SISM_HOST
	struct timeval tv;
	int mark;
#endif
	share_data_t s_data;
	int shmid = 0;
	key_t key;
	char* shm;

	key = 5266;
	shmid = shmget(key, 40, IPC_CREAT | 0666);
	if (shmid <0){
		perror("shmget");
		exit(1);
	}	

	/* read common data from share mem*/
	shm = shmat(shmid, NULL,0);
	memcpy(&s_data, shm, sizeof(s_data));
	memcpy(ifName, &s_data.itf_name, s_data.len);

	if (shmdt(shm)==-1)
	{
		perror("shmdt");
		exit(1);
	}	
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));

	if ((ethertype == ETH_TYPE_SRC) && ((s_data.pt_enable & SRC_EN)!=SRC_EN))
	{
		if(is_first_time == 1)
		{
			is_first_time = 0;
			dest_mac[0]=0xff;
			dest_mac[1]=0xff;
			dest_mac[2]=0xff;
			dest_mac[3]=0xff;
			dest_mac[4]=0xff;
			dest_mac[5]=0xff;
		}
		return CLIENT_NOT_READY;
	}

	/* Open RAW socket to send on */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ethertype))) == -1) {
		//perror("socket");
		ret = SOCKET_FAIL;
		return ret;
	}

	switch (ethertype)
	{
		case ETH_TYPE_INIC:
			inic_fd = sockfd;
			break;
		case ETH_TYPE_SRC:
			src_fd = sockfd;
			break;
		case ETH_TYPE_EVENT:
			hb_fd = sockfd;
			break;
		case ETH_TYPE_PROC:
			proc_fd = sockfd;
			break;
		case ETH_TYPE_WAN:
			wan_fd = sockfd;
			break;

		default:
			break;
	}

	SISM_DBG_MSG(LOG_DEBUG,"ifName = %s, ethertype = %x, sockfd = %d", ifName, ethertype, sockfd);

	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);

		exit(EXIT_FAILURE);
	}

	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

#ifdef TCSUPPORT_SISM_HOST
	/* set timeout value*/
	tv.tv_sec = timeout;
	tv.tv_usec= 0;
	
	if(setsockopt(sockfd, SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv))== -1){
		perror("SO_RCVTIMEO");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	/* Set mark be 0x10. 7562 uses queue 3 to send control packet for high priority. */
	mark = 0x10;

	if(setsockopt(sockfd, SOL_SOCKET,SO_MARK,&mark,sizeof(mark))== -1){
		perror("SO_MARK");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
#endif

	return ret;
}

int pt_fd_close(unsigned short ethertype)
{
	int ret = 0;
	int sockfd = -1;
	switch (ethertype)
	{
		case ETH_TYPE_INIC:
			sockfd = inic_fd;
			break;
		case ETH_TYPE_SRC:
			sockfd = src_fd;
			break;
		case ETH_TYPE_EVENT:
			sockfd = hb_fd;
			break;
		case ETH_TYPE_PROC:
			sockfd = proc_fd;
			break;
		case ETH_TYPE_WAN:
			sockfd = wan_fd;
			break;
		default:
			break;
	}

//	if (pthread_mutex_destroy(&pt_lock))
//		ret = UNHANDLE_FAIL;

	if (sockfd!= -1)
	{
		close(sockfd);
		sockfd = -1;
	}
	return ret;
}


uint8_t* req_pt_pkt_buf(int size)
{

	uint8_t *buf;
	buf = (uint8_t *)malloc(size + sizeof(struct ether_header));

	if(buf == NULL){
		SISM_DBG_MSG(LOG_ERR,"req_pt_pkt_buf alloc buffer failed");
		return NULL;
	}

	memset(buf, 0, sizeof(struct ether_header));
	return (buf + sizeof(struct ether_header));
}

void free_pt_pkt_buf(uint8_t* buf)
{
	if ((buf-sizeof(struct ether_header)) != NULL)
	{
		free((buf-sizeof(struct ether_header)));
	}
}

int pt_pkt_send(unsigned short ethertype, uint8_t* buf,int len)
{
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	int buf_len = 0;
	int sockfd = -1;
	int ret = -1;
	//uint8_t* sendbuf = (buf-sizeof(struct ether_header));
	uint8_t *sendbuf;

	if(ethertype == ETH_TYPE_PROC)
	{
		len += sizeof(src_hdr_t);
	}
	buf_len = len + sizeof(struct ether_header);

	sendbuf = (uint8_t *)malloc(buf_len);

	if (sendbuf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR,"sendbuf alloc failed");
		return SOCKET_FAIL;
	}

	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct sockaddr_ll socket_address;

	switch (ethertype)
	{
		case ETH_TYPE_INIC:
			sockfd = inic_fd;
			break;
		case ETH_TYPE_SRC:
			sockfd = src_fd;
			break;
		case ETH_TYPE_EVENT:
			sockfd = hb_fd;
			break;
		case ETH_TYPE_PROC:
			sockfd = proc_fd;
			break;
		case ETH_TYPE_WAN:
			sockfd = wan_fd;
			break;
		default:
			free(sendbuf);
			return UNHANDLE_FAIL;
			break;
	}	

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, sizeof(struct ether_header));
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = dest_mac[0];
	eh->ether_dhost[1] = dest_mac[1];
	eh->ether_dhost[2] = dest_mac[2];
	eh->ether_dhost[3] = dest_mac[3];
	eh->ether_dhost[4] = dest_mac[4];
	eh->ether_dhost[5] = dest_mac[5];

	/* Ethertype field */
	if (ethertype == ETH_TYPE_SRC)
	{
		tx_len += sizeof(blapi_hdr_t);
		tx_len += sizeof(src_hdr_t);
	}
	else
	{
		tx_len += len;
	}

	ethertype = htons(ethertype);
	memcpy(&eh->ether_type, &ethertype, sizeof(ethertype));
	tx_len += sizeof(struct ether_header);

	memcpy(sendbuf + sizeof(struct ether_header), buf, tx_len - sizeof(struct ether_header));
	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = dest_mac[0];
	socket_address.sll_addr[1] = dest_mac[1];
	socket_address.sll_addr[2] = dest_mac[2];
	socket_address.sll_addr[3] = dest_mac[3];
	socket_address.sll_addr[4] = dest_mac[4];
	socket_address.sll_addr[5] = dest_mac[5];

	if(ethertype != htons(ETH_TYPE_INIC)){
		SISM_DBG_MSG(LOG_DEBUG, "ifName = %s, sock = %d, tx_len = %d,",
					ifName, sockfd, tx_len);
		SISM_DBG_MSG(LOG_DEBUG, "send to MAC = %02X:%02X:%02X:%02X:%02X:%02X",
					dest_mac[0],dest_mac[1],dest_mac[2],dest_mac[3],dest_mac[4],dest_mac[5]);
	}


	if (tx_len > buf_len)
	{
		SISM_DBG_MSG(LOG_ERR,"tx_len: %d exceed buf_len: %d, Send failed",tx_len, buf_len);
		goto end; 	
	}


	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	{
		SISM_DBG_MSG(LOG_ERR,"Send failed");
		ret = SOCKET_FAIL;
	}
	else
	{
		ret = PT_SUCCESS;
	}

end:
	if(sendbuf != NULL)
	{
		free(sendbuf);
	}
	return ret;	

}

int pt_pkt_recv(unsigned short ethertype, uint8_t* payload)
{
	int ret = 0;
	int sockfd = -1;
	ssize_t numbytes;
	uint8_t *rcvbuf = payload-sizeof(struct ether_header);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) rcvbuf;
	switch (ethertype)
	{
		case ETH_TYPE_INIC:
			sockfd = inic_fd;
			break;
		case ETH_TYPE_SRC:
			sockfd = src_fd;
			break;
		case ETH_TYPE_EVENT:
			sockfd = hb_fd;
			break;
		case ETH_TYPE_PROC:
			sockfd = proc_fd;
			break;
		case ETH_TYPE_WAN:
			sockfd = wan_fd;
			break;
		default:
			return UNHANDLE_FAIL;
			break;
	}		

	numbytes = recvfrom(sockfd, rcvbuf, BUF_SIZ+sizeof(struct ether_header), 0, NULL, NULL);

	if(ethertype != ETH_TYPE_INIC)
	{
		SISM_DBG_MSG(LOG_DEBUG,"listener: Waiting to recvfrom ethertype(%x) sock = %d...", ethertype, sockfd);
		SISM_DBG_MSG(LOG_DEBUG,"listener: got packet %d bytes", numbytes);
	}

	if(numbytes <=0)
	{
		SISM_DBG_MSG(LOG_DEBUG, "RECEIVE_TIMEOUT");
		return RECEIVE_TIMEOUT;
	}
	else if ((numbytes > SRC_PKT_LEN) && ntohs(eh->ether_type)== ETH_TYPE_SRC)
	{
		SISM_DBG_MSG(LOG_ERR, "error pkt receive, len = %d", numbytes);
		return UNHANDLE_FAIL;
	}

	/* record destination MAC*/
	if (is_first_time == 0)
	{
		dest_mac[0] = eh->ether_shost[0];
		dest_mac[1] = eh->ether_shost[1];
		dest_mac[2] = eh->ether_shost[2];
		dest_mac[3] = eh->ether_shost[3];
		dest_mac[4] = eh->ether_shost[4];
		dest_mac[5] = eh->ether_shost[5];
		is_first_time = 1;

		SISM_DBG_MSG(LOG_DEBUG, "record dest MAC = %02X:%02X:%02X:%02X:%02X:%02X",
				dest_mac[0],dest_mac[1],dest_mac[2],dest_mac[3],dest_mac[4],dest_mac[5]);
	}

#if DEBUG
	int i;
	SISM_DBG_MSG(LOG_DEBUG, "\tData:");
	for (i=0; i<80; i+=8) SISM_DBG_MSG(LOG_DEBUG, "%02x:02x:02x:02x:02x:02x:02x:02x", 
		rcvbuf[i], rcvbuf[i+1], rcvbuf[i+2], rcvbuf[i+3], rcvbuf[i+4], rcvbuf[i+5], rcvbuf[i+6], rcvbuf[i+7]);
#endif

end:
	return ret;
}


#ifdef TCSUPPORT_SISM_HOST
int SRC_blapi(blapi_hdr_t* blapi_var)
{

	uint8_t *s_buf = NULL;
	uint8_t *r_buf = NULL;
	unsigned int req_id = 0;
	int ret = -1;
	int i = 0;

	ret = pt_fd_open(ETH_TYPE_SRC,SRC_TIMEOUT) ;
	if(ret != PT_SUCCESS)
	{
		SISM_DBG_MSG(LOG_ERR, "pt_fd_open failed!");
		return ret;
	}


	/*	request buf for send/recv packet*/
	s_buf =  req_pt_pkt_buf(BUF_SIZ);

	if (s_buf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR, "request send pkt fail");
		pt_fd_close(ETH_TYPE_SRC);
		return -1;
	}
	
	src_hdr_t *src_h = (src_hdr_t *)s_buf;
	r_buf = req_pt_pkt_buf(BUF_SIZ);

	if (r_buf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR, "request recv pkt fail");
		free_pt_pkt_buf(s_buf);
		pt_fd_close(ETH_TYPE_SRC);
		return -1;
	}

	/* fill packet payload*/
	blapi_hdr_t *blapi_h = (blapi_hdr_t*)(s_buf + sizeof(src_hdr_t)) ;
	blapi_h->blapi_id = blapi_var->blapi_id;
	blapi_h->argc = blapi_var->argc;
	memcpy(&blapi_h->argv1, &blapi_var->argv1,sizeof(blapi_var->argv1));
	memcpy(&blapi_h->argv2, &blapi_var->argv2,sizeof(blapi_var->argv2));
	memcpy(&blapi_h->argv3, &blapi_var->argv3,sizeof(blapi_var->argv3));
	memcpy(&blapi_h->argv4, &blapi_var->argv4,sizeof(blapi_var->argv4));
	memcpy(&blapi_h->argv5, &blapi_var->argv5,sizeof(blapi_var->argv5));
	src_h->magic_no = htonl(0x75177517);

	src_h->cmd_opcode = htons(OP_BLAPI);

	/* record request blapi id*/
	req_id = blapi_h->blapi_id;

	/* send packet*/
	SISM_DBG_MSG(LOG_DEBUG, "send SRC pkt (id = %x)", htonl(blapi_h->blapi_id));
	ret = pt_pkt_send(ETH_TYPE_SRC,s_buf,BUF_SIZ);
	if (ret)
	{
		goto end;
	}

	/* receive packet*/
	ret = pt_pkt_recv(ETH_TYPE_SRC,r_buf);
	if (ret)
	{
		goto end;
	}

#if DEBUG
	SISM_DBG_MSG(LOG_DEBUG, "\tData:");
	for (i=0; i<80; i+=8) SISM_DBG_MSG(LOG_DEBUG, "%02x:02x:02x:02x:02x:02x:02x:02x", 
		r_buf[i], r_buf[i+1], r_buf[i+2], r_buf[i+3], r_buf[i+4], r_buf[i+5], r_buf[i+6], r_buf[i+7]);
#endif

	/* fill the blapi information from received packet*/
	blapi_h = (blapi_hdr_t*)(r_buf + sizeof(src_hdr_t)) ;

	if (req_id != blapi_var->blapi_id)
	{
		SISM_DBG_MSG(LOG_INFO, "recv wrong blapi id packet!!");
		goto end;
	}

	/* extract blapi result from receive packet*/
	blapi_var->blapi_id = blapi_h->blapi_id;
	blapi_var->argc     = blapi_h->argc;
	memcpy(&blapi_var->argv1, &blapi_h->argv1,sizeof(blapi_var->argv1));
	memcpy(&blapi_var->argv2, &blapi_h->argv2,sizeof(blapi_var->argv2));
	memcpy(&blapi_var->argv3, &blapi_h->argv3,sizeof(blapi_var->argv3));
	memcpy(&blapi_var->argv4, &blapi_h->argv4,sizeof(blapi_var->argv4));
	memcpy(&blapi_var->argv5, &blapi_h->argv5,sizeof(blapi_var->argv5));
	blapi_var->ret  = ntohl(blapi_h->ret);

end:
	free_pt_pkt_buf(s_buf);
	free_pt_pkt_buf(r_buf);

	pt_fd_close(ETH_TYPE_SRC);
	return ret;
}
#endif

