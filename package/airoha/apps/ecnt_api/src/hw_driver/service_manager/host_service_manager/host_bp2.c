#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include "../pt/lib_pt.h"

#include "../inc/service_manager.h"
#include <netinet/ether.h>
#include <signal.h>
#include <trx/trx.h>


#define FE_SRAM_BASE 0x1fa30000
#define STATUS_REG	0x1fb00f08
#define MSG_CHANNEL_TX	0x1fb00f00
#define MSG_CHANNEL_RX	0x1fb00f04
#define CRC_DISABLE		0x1fb00f0c
#define IMAGE_LEN 0x1fb00f10
#define IMAGE_ADDR 0x1fb00f14
#define CONF_SIZE 0x1fb00f18
#define TCLINUX_START 0x1fb00f1c


#define MSG_READY 0x57414445
#define MSG_READY_INIC 0x5244494E
#define MSG_IMAGE_DONE 0x444f4E45
#define MSG_IMAGE_DONE_INIC 0x494E444E
#define MSG_IMAGE_DONE_INIC_SEG 0x494E5345
#define MSG_FINISH 0x66697368

#define MSG_SGMII2RGMII_MODE 0x53475247
#define MSG_CRC_FAIL 0x4352464c
#define MSG_CRC_SUCCESS 0x46574F4B
#define MSG_NO_CRC	0x4E4F4352
#define CONF_MAGIC 0x434F4E46


#define BP2_TIMEOUT 20
/* HSM timer */
extern int hsm_timeout;
extern void hsm_timer(int sig);


/* ROM STATUS */

#define UART_INIT_DONE (0x1<<0)
#define CRC_CHECK_SUCCESS (0x1 <<1)
#define CRC_CHECK_FAIL	(0x1<<2)
#define MSG_HANDLER_DONE (0x1 <<3)
#define CPU_STANDBY	(0x1<<4)
#define NO_CRC_CHECK_BOOT	(0x1<<5)


#define BOOTROM_MAGIC_NO	0x31623162			/* magic no */
#define	BOOTROM_SEGSIZE		1400			/* data segment size */
#define MAX_PKT_SIZE		1518
#define NET_IP_ALIGN 2
#define SEG_SIZE		0x7800
	
/*
 * Command opcode
 */
#define	BOOTROM_NOTIFY      0x01				/* notify */
#define	BOOTROM_NOTIFY_ACK  0x02				/* notify ack */
#define	BOOTROM_RRQ			0x03				/* read request */
#define	BOOTROM_WRQ			0x04				/* write request */
#define	BOOTROM_DATA		0x05				/* data packet */
#define	BOOTROM_ACK			0x06				/* ack */
#define	BOOTROM_NAK			0x07				/* nak */
#define	BOOTROM_START_BOOT	0x08				/* start boot */
#define	BOOTROM_BOOT_OK		0x09				/* boot ok notify */
#define	BOOTROM_ERROR		0x0a				/* error code */
#define	BOOTROM_RESTART		0x0b				/* restart */
#define	BOOTROM_MEMRL		0x0c				/* read memory long value */
#define	BOOTROM_MEMWL		0x0d				/* write memory long value */
#define	BOOTROM_MIIR		0x0e				/* read mii value */
#define	BOOTROM_MIIW		0x0f				/* write mii value */

/*
 * Restart opcode
 */
#define	BOOTROM_RESTART_COLD	0x01				/* cold restart */
#define	BOOTROM_RESTART_WARM	0x02				/* warm restart */
#define	BOOTROM_RESTART_RRQ		0x03				/* restart to read request */


#define UPLOADING_FILE 1
#define NOT_UPLOADING_FILE 0

static int inic_state = 0;
/* iNIC communication structure */

int last_len;
int inic_file_fd = -1;
char isUploadingFile = NOT_UPLOADING_FILE;
/*char upload_file_name[80];*/

/*uint8_t iNIC_host_mac_addr[] = {0x00, 0xaa, 0xbb, 0x01, 0x23, 0x34};*/
/*uint8_t iNIC_client_mac_addr[] = {0x00, 0x11, 0x11, 0x11, 0x11, 0x11};*/
/*uint8_t broadcast_mac_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};*/
uint16_t checksum;
uint8_t send_buf[MAX_PKT_SIZE];
extern unsigned char mac_addr[6];

/* IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 *	and FCS/CRC (frame check sequence) */
#define ETH_ALEN	6		        /* Octets in one ethernet addr	 */
#define ETH_HLEN	14		        /* Total octets in header.	 */
#define ETH_ZLEN	60		        /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */


/*******************************************************************
 * S T R U C T U R E S
 *******************************************************************/


unsigned short rom_in_csum(unsigned short *ptr, int nbytes)
{
	register int			sum;		/* assumes long == 32 bits */
	unsigned short			oddbyte;
	register unsigned short	answer; 	/* assumes u_short == 16 bits */

	/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

	sum = 0;
	//SISM_DBG_MSG(LOG_DEBUG, "[rom_in_csum] nbytes = %x",nbytes);

	while (nbytes > 1)	{
		sum += *ptr++;
		nbytes -= 2;
		//SISM_DBG_MSG(LOG_DEBUG, "%x",*ptr);
	}
	//SISM_DBG_MSG(LOG_DEBUG, "[rom_in_csum] sum = %x",sum);
	/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0;		/* make sure top half is zero */
		*((unsigned char *) &oddbyte) = *(unsigned char *)ptr;   /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */

	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16); 		/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
	return(answer);
}

void rom_send_packet(uint16_t cmd_opcode, uint16_t length, uint16_t block_no, uint8_t *data,
			uint16_t file_checksum, uint16_t restart_code, uint32_t mem_addr, 
			uint32_t mem_value)
{
	int total_len, eth_len, rom_len;
	struct romhdr *romh;
	struct ether_header *eth;

	//SISM_DBG_MSG(LOG_DEBUG, "");

	if (sizeof(*romh)+ length + ETH_HLEN> MAX_PKT_SIZE)
	{
		//SISM_DBG_MSG(LOG_ERR, "pkt exceed MAX_PKT_SIZE");
		return;
	}
	rom_len = eth_len = length + sizeof(*romh);
	total_len = eth_len + NET_IP_ALIGN;

	romh = (struct romhdr *) (send_buf + ETH_HLEN);

	memset(romh, 0, sizeof(*romh));
	romh->magic_no = htonl(BOOTROM_MAGIC_NO);
	romh->cmd_opcode = htons(cmd_opcode);
	romh->cmd_id = htons(0x0);
	romh->length = htons(length);
	romh->checksum = htons(0);
	romh->block_no = htons(block_no);
	romh->file_checksum = file_checksum;
	romh->restart_code = htons(restart_code);
	romh->mem_addr = htonl(mem_addr);
	romh->mem_value = htonl(mem_value);
	memcpy((uint8_t *)romh+sizeof(*romh),data,length);

	romh->checksum = rom_in_csum((unsigned short *) romh, rom_len);

	
	pt_pkt_send(ETH_TYPE_INIC,(uint8_t *)romh ,  total_len);

}


/*_____________________________________________________________________________
**      function name: rom_rcv_notify
**      descriptions:
**            To handle NOTIFY packet send from iNIC client.
**
**      parameters:
**            skb: socket buffer.
**
**      return:
**             none.
**
**____________________________________________________________________________
*/
static void rom_rcv_notify(uint8_t* payload, struct romhdr *romh)
{
	//uint8_t mac_addr[12]; // mac address(12) + iNIC Host mac address

	//if(FTTDP_INIC_DEBUG) {
	//	SISM_DBG_MSG(LOG_DEBUG, "");
	//}

	/* To set iNIC_client_mac_addr */
	isUploadingFile = UPLOADING_FILE;

	/*memcpy(iNIC_client_mac_addr, iNIC_host_mac_addr, 6);*/
	/*iNIC_client_mac_addr[5]+=1;*/

	/* To set iNIC_client_mac_addr */
	//memcpy(mac_addr + 6, iNIC_host_mac_addr, 6);
	//memcpy(mac_addr, iNIC_client_mac_addr, 6);

	rom_send_packet(BOOTROM_NOTIFY_ACK, 6, 0, mac_addr, 
					0, 0, 0, 0); 
}


static uint16_t rom_file_csum(char *f)
{
	int len;

	uint8_t buf[1024];
	unsigned short *ptr; 
	int nbytes;
	int			sum;		/* assumes long == 32 bits */
	unsigned short			oddbyte;
	unsigned short	answer; 	/* assumes u_short == 16 bits */

	if (!f)
		return 0;

	inic_file_fd = open(f,O_RDONLY);
	if (inic_file_fd <0)
		return -1;
	
		/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

	sum = 0;
	nbytes = 0;
	ptr = (unsigned short *) buf;

	do {
		len = read(inic_file_fd, buf, 1024);
		if (len <= 0)
			break;

		nbytes = len;
		ptr = (unsigned short *)buf;
		while (nbytes > 1)	{
			sum += *ptr++;
			nbytes -= 2;
		}
	} while (len != 0);

	close(inic_file_fd);
				/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0;		/* make sure top half is zero */
		*((unsigned char *) &oddbyte) = *(unsigned char *)ptr;   /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */

	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16); 		/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
	return(answer);
}

/*_____________________________________________________________________________
**      function name: rom_rcv_notify
**      descriptions:
**            To handle RRQ packet send from iNIC client.
**
**      parameters:
**            skb: socket buffer.
**
**      return:
**             none.
**
**____________________________________________________________________________
*/
static void rom_rcv_rrq(uint8_t* payload, struct romhdr *romh)
{
	char *filename = romh->data;
	char buffer[BOOTROM_SEGSIZE];
	int len;
	char file[80];
	int i;
	int byte = 0;
	//int isMtd = NOT_MTD_FILE;

	//if(FTTDP_INIC_DEBUG) {
	//}
	
	//SISM_DBG_MSG(LOG_DEBUG, "rom_rcv_rrq cmd_id=%d filename=%s", romh->cmd_id, filename);

	//if(isMtd == NOT_MTD_FILE) {
		/* get file from /tmp/ */
	//	snprintf(file, sizeof(file), "%s%s", INIC_CLIENT_FILE_DIR, filename);
	//} else {
		/* get file from /tmp/ */
		snprintf(file, sizeof(file), "%s%s", "/etc/iNIC/", filename);
	//}
	

		//strncpy(upload_file_name, filename, sizeof(upload_file_name));
		//len = rom_read_file(rrq_file, buffer, BOOTROM_SEGSIZE);
		checksum = rom_file_csum(file);
		//checksum = 0;
		inic_file_fd = open(file,O_RDONLY);
		if (inic_file_fd <0 )
		{
			SISM_DBG_MSG(LOG_ERR, "open file failed: %s!", file);
			return ;
		}

		byte = read(inic_file_fd, buffer, BOOTROM_SEGSIZE);

		if (byte < 0)
		{
			SISM_DBG_MSG(LOG_ERR, "read file failed !");
			return ;
		}

		rom_send_packet(BOOTROM_DATA, byte, 1, buffer, 
						checksum, 0, 0, 0); 
}

/*_____________________________________________________________________________
**      function name: rom_rcv_ack
**      descriptions:
**            To handle ACK packet send from iNIC client.
**
**      parameters:
**            skb: socket buffer.
**
**      return:
**             none.
**
**____________________________________________________________________________
*/
static void rom_rcv_ack(uint8_t* payload, struct romhdr *romh)
{
	char buffer[BOOTROM_SEGSIZE];
	int byte = 0;

	if (inic_file_fd<0) {
		return;
	}

	int start = (romh->block_no) * BOOTROM_SEGSIZE;
	lseek(inic_file_fd, start, SEEK_SET);
	byte = read(inic_file_fd, buffer, BOOTROM_SEGSIZE);

	if((byte > 0) || ((byte == 0) && (last_len == BOOTROM_SEGSIZE)) ) {

		rom_send_packet(BOOTROM_DATA, byte, romh->block_no + 1, buffer, 
						checksum, 0, 0, 0); 

	} else {
		/* finish send packet */
		close(inic_file_fd);
		inic_file_fd = -1;
		/* has finished uploading last file(CO romfile). */
		//if(strcmp(upload_file_name, ) == 0) {
			/* reset flag */
			isUploadingFile = NOT_UPLOADING_FILE;
			inic_state = 1;
			system("echo 2 2 0 0 0 > /proc/xsgmii");
			return 0;
		//}
	}

	/* To record send len */
	last_len = byte;

	return;
}


/*_____________________________________________________________________________
**      function name: rom_rcv_ack
**      descriptions:
**            To handle packet send from iNIC client.
**
**      parameters:
**            work: 
**
**      return:
**             none.
**
**____________________________________________________________________________
*/
static int rom_process(void)
{
	struct romhdr *romh;
	int timeout_limit = 10;
	int timeout_count = 0;
	char * buf = NULL;

	/* initial iNIC state*/
	pt_fd_open(ETH_TYPE_INIC,INIC_TIMEOUT);
	inic_state = 0;
	buf = req_pt_pkt_buf(BOOTROM_SEGSIZE);

	while ((inic_state == 0)) {
		
		if ((pt_pkt_recv(ETH_TYPE_INIC,buf)==RECEIVE_TIMEOUT) && (timeout_count < timeout_limit))
		{
			timeout_count++;
			continue;
		}
		else if (timeout_count >= timeout_limit)
		{
			SISM_DBG_MSG(LOG_ERR, "iNIC transmit fail!");
			return RECEIVE_TIMEOUT;
		}
		else
		{
			timeout_count = 0;
		}
		romh = (struct romhdr*)buf;
#if 0
		SISM_DBG_MSG(LOG_DEBUG, "rom->magic_no      = %x ", romh->magic_no );
		SISM_DBG_MSG(LOG_DEBUG, "rom->cmd_opcode    = %x ", romh->cmd_opcode);
		SISM_DBG_MSG(LOG_DEBUG, "rom->cmd_id =      = %x ", romh->cmd_id);
		SISM_DBG_MSG(LOG_DEBUG, "rom->length        = %x ", romh->length );
		SISM_DBG_MSG(LOG_DEBUG, "rom->checksum      = %x ", romh->checksum );
		SISM_DBG_MSG(LOG_DEBUG, "rom->block_no      = %x ", romh->block_no);
		SISM_DBG_MSG(LOG_DEBUG, "rom->file_checksum = %x ", romh->file_checksum );
		SISM_DBG_MSG(LOG_DEBUG, "rom->checksum      = %x ", rom_in_csum((unsigned short *) romh, sizeof(*romh) + ntohs(romh->length)));
		SISM_DBG_MSG(LOG_DEBUG, "*romh size = %d", sizeof(*romh));
#endif
		if (romh->checksum && rom_in_csum((unsigned short *) romh, sizeof(*romh)+ ntohs(romh->length)))
		{
			SISM_DBG_MSG(LOG_ERR, "checksum mismatch");
			free_pt_pkt_buf(buf);
			return -1;
		}
		else
		{
			romh->magic_no = ntohl(romh->magic_no);
			romh->cmd_opcode = ntohs(romh->cmd_opcode);
			romh->cmd_id = ntohs(romh->cmd_id);
			romh->length = ntohs(romh->length);
			romh->block_no = ntohs(romh->block_no);			
		}

		switch (romh->cmd_opcode) {
			case BOOTROM_NOTIFY:
				rom_rcv_notify(buf, romh);
				break;

			case BOOTROM_RRQ:
				rom_rcv_rrq(buf, romh);
				break;

			case BOOTROM_ACK:
				rom_rcv_ack(buf, romh);
				break;
			case BOOTROM_BOOT_OK:
			case BOOTROM_DATA:
			case BOOTROM_NOTIFY_ACK:
			case BOOTROM_WRQ:
			case BOOTROM_NAK:
			case BOOTROM_START_BOOT:
			case BOOTROM_ERROR:
			case BOOTROM_RESTART:
			case BOOTROM_MEMRL:
			case BOOTROM_MEMWL:
			case BOOTROM_MIIR:
			case BOOTROM_MIIW:
				break;
			default:
				SISM_DBG_MSG(LOG_ERR, "unknow");
				break;
		}

	}
	free_pt_pkt_buf(buf);
	return 0;
}

#define CONFIG_TP_IMAGE

int mdio_msg_handler_bp2 (unsigned int phy_addr, unsigned int start_addr)
{
	FILE *fd;
	/*
	int i = 0;
	int fd = -1;
	int fd_config = -1;
	int fd_new = -1;
	int config_len =0 ;
	int is_default_conf = 0;
	*/
	unsigned int data = 0;
	int byte = 0;
	int file_len = 0;
	int inic_len = 0;
	int seg_len = 0;
	unsigned int rcv_result = 0;
	unsigned int temp_addr = 0x1fa40000;
	unsigned int conf_magic = CONF_MAGIC;
	int ret = UNHANDLE_FAIL;

	/* init bp2 timer*/
	hsm_timeout = 0;
	alarm(0);
	alarm(BP2_TIMEOUT);
	

	//inic_hdr inic_h = NULL;
#ifdef CONFIG_TP_IMAGE
	fd = fopen("/etc/iNIC/iNIC.bin","rb");
#else
	fd = fopen("/etc/iNIC/iNIC.bin","r+");
#endif
	if (!fd)
	{
		SISM_DBG_MSG(LOG_ERR, "open iNIC.bin failed ! errno=%d reason=%s", errno, strerror(errno));
		return SOCKET_FAIL;
	}
#ifndef CONFIG_TP_IMAGE
	/* check if the image has been set*/
	if (fseek(fd, -10, SEEK_END))
	{
		ret = SOCKET_FAIL;
		goto exit_handle;
	}

	fread(&rcv_result, 1, 4, fd);

	if (rcv_result != CONF_MAGIC)
	{
		fseek(fd, 0, SEEK_END);

		byte = fwrite(&conf_magic, 1, 4, fd);
		if (byte < 4)
		{
			SISM_DBG_MSG(LOG_ERR, "write magic error !");
			ret = UNHANDLE_FAIL;
			goto exit_handle;
		}
	}

	/* add mac addr to the image*/
	byte = fwrite(mac_addr, 1, 6, fd);

	if (byte < 6)
	{
		SISM_DBG_MSG(LOG_ERR, "padding MAC error!");
	}
	/*
	else
	{
		SISM_DBG_MSG(LOG_DEBUG, "Add MAC addr to image: %02x:%02x:%02x:%02x:%02x:%02x",
			mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	}
	*/
#endif
	/* restore the pointer to the file head*/
	fseek(fd, 0, SEEK_SET);

	if (mdio_msg_init(phy_addr)!=0)
	{
		SISM_DBG_MSG(LOG_ERR, "create socket fail!");
		ret = SOCKET_FAIL;
		goto exit_handle;
	}
	//mdio_msg_send(phy_addr, IMAGE_ADDR, start_addr);
	do
	{
		while(1)
		{
			if (mdio_msg_rcv(phy_addr, MSG_CHANNEL_TX, &rcv_result)!= 0)
			{
				ret = SOCKET_FAIL;
				goto exit_handle;
			}

			if (rcv_result == MSG_READY_INIC)
				break;
			else if (hsm_timeout)
			{
				SISM_DBG_MSG(LOG_ERR, "host bp2 message handler timeout !");
				ret = RECEIVE_TIMEOUT;
				goto exit_handle;
			}
			else
				continue;
		}

		do
		{
			byte = fread(&data, 1, 4, fd);
			if(byte == 0)
				break;
			mdio_msg_send(phy_addr, temp_addr, data);
			temp_addr +=4;
			file_len += byte;
			seg_len  += byte;
		}while((byte == 4) && (seg_len < SEG_SIZE));

		if(seg_len == 0) /*No data to transmit*/
			break;
#ifdef CONFIG_TP_IMAGE
		if (byte < 4)
		{
			mdio_msg_send(phy_addr, temp_addr, conf_magic);
			temp_addr +=4;
			data = *((unsigned int *)mac_addr);
			mdio_msg_send(phy_addr, temp_addr, data);
			temp_addr +=4;
			data = *((unsigned short *)(mac_addr + 4));
			mdio_msg_send(phy_addr, temp_addr, data);
			temp_addr +=4;
			file_len += 10;
		}
#endif

		SISM_DBG_MSG(LOG_INFO, "Writing iNIC.bin to client!");
		mdio_msg_send(phy_addr, IMAGE_ADDR, start_addr);
		/*SISM_DBG_MSG(LOG_DEBUG, "\tstart_addr = %x",start_addr);*/
		/*mdio_msg_rcv(phy_addr, IMAGE_ADDR, &rcv_result);*/
		/*SISM_DBG_MSG(LOG_DEBUG, "\tIMAGE_ADDR = %x",rcv_result);*/

		mdio_msg_send(phy_addr, MSG_CHANNEL_TX, 0       );
		mdio_msg_send(phy_addr, IMAGE_LEN     , file_len);

		SISM_DBG_MSG(LOG_INFO,"iNIC.bin len=%x", seg_len);
		if (seg_len==SEG_SIZE)
		{
			SISM_DBG_MSG(LOG_INFO," SEG full");
			mdio_msg_send(phy_addr, MSG_CHANNEL_RX, MSG_IMAGE_DONE_INIC_SEG);
		}
		temp_addr = 0x1fa40000;
		seg_len = 0;

	}while(byte >= 4);

	SISM_DBG_MSG(LOG_INFO,"write iNIC.bin finished! len=%x", file_len);

#if 1
	SISM_DBG_MSG(LOG_INFO,"Disable CRC check.");
	mdio_msg_send(phy_addr, CRC_DISABLE, 1);
#else
	SISM_DBG_MSG(LOG_INFO, "Enable CRC check.");
	mdio_msg_send(phy_addr, CRC_DISABLE, 0);
#endif
	mdio_msg_send(phy_addr, MSG_CHANNEL_RX, MSG_IMAGE_DONE_INIC);
	
	while(1)
	{
		if (mdio_msg_rcv(phy_addr, MSG_CHANNEL_TX, &rcv_result)!= 0)
		{
			ret = SOCKET_FAIL;
			goto exit_handle;
		}
		if ((rcv_result == MSG_CRC_SUCCESS) ||(rcv_result == MSG_NO_CRC))
		{
			ret = PT_SUCCESS;
			SISM_DBG_MSG(LOG_INFO,"client boot into iNIC.bin!");
			break;
		}
		else if(rcv_result == MSG_CRC_FAIL)
		{
			ret = CRC_FAIL;
			SISM_DBG_MSG(LOG_ERR,"CRC check failed!");
			break;
		}
		else if (hsm_timeout)
		{
			SISM_DBG_MSG(LOG_ERR,"host bp2 message handler timeout !");
			ret = RECEIVE_TIMEOUT;
			goto exit_handle;
		}
		else
			continue;
	}
	
	/* notify client that host has received result*/
	mdio_msg_send(phy_addr, MSG_CHANNEL_RX, MSG_FINISH);

	/* Disable EN7517 MDIO read path */
	mdio_msg_send(phy_addr, 0x1fa20160, 0x800);

exit_handle:
	alarm(0);
	if (fd >= 0)
		fclose(fd);
	return ret;
}

int set_ram_size(unsigned int phy_addr)
{
	unsigned int ram_size = 0;
	int fd = -1;
	struct trx_header trx;
	unsigned root_offset = 0;
	int ret = -1;
	int byte = 0;
	fd = open("/etc/iNIC/tclinux.bin",O_RDONLY);
	if (fd <0)
	{
		SISM_DBG_MSG(LOG_ERR, "open tclinux.bin failed !");
		return SOCKET_FAIL;
	}
	if (lseek(fd, 0 ,SEEK_SET)== -1)
	{
		ret = SOCKET_FAIL;
		goto exit_handle;
	}
	//trx = (struct trx_header *)malloc(sizeof(struct trx_header*));
	byte = read(fd, &trx, sizeof(struct trx_header));
	if (byte <= 0)
	{
		ret = UNHANDLE_FAIL;
		goto exit_handle;
	}
	root_offset = trx.linux_7z_pad  + trx.header_len;
	mdio_msg_send(phy_addr,TCLINUX_START,INIC_RAM_BASE-root_offset);
	if (lseek(fd, 0 ,SEEK_SET)== -1)
	{
		ret = SOCKET_FAIL;
		goto exit_handle;
	}
	ret = PT_SUCCESS;
	
	exit_handle:
	if (fd >= 0)
		close(fd);
	return ret;
}


int iNIC_server(unsigned int phy_addr)
{
	int ret = -1;
	
	SISM_DBG_MSG(LOG_INFO, "start iNIC server!");

	ret = set_ram_size(phy_addr);
	if (ret != PT_SUCCESS)
	{
		SISM_DBG_MSG(LOG_ERR, "set_ram_size error !");
		return ret;
	}	
	ret = rom_process();
	return ret;
}

int host_bp2(unsigned int phy_addr, unsigned start_addr)
{
	int ret = -1;
	ret = mdio_msg_handler_bp2(phy_addr, start_addr);
	if (ret != PT_SUCCESS)
		return ret;
	ret = iNIC_server(phy_addr);
	
	return ret;
}

