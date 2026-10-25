#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../pt/lib_pt.h"
#include <signal.h>
#include "../inc/inic_common.h"


#define BOOT_LIMIT				25
#define RUNTIME_LIMIT			8
#define HEARTBEAT_INTERVAL		5
#define INTER_MAGIC				0x48424842

unsigned char mac_addr[6] = {0x00, 0xAA, 0xBB, 0x01, 0x23, 0x40};

int hsm_timeout = 0;
void hsm_timer(int sig)
{
	hsm_timeout = 1;
	return ;
}


void sigroutine(int sig)
{
	int shmid = 0;
	key_t key;
	char* shm;

	SISM_DBG_MSG(LOG_DEBUG,"receive sig %d",sig);
	switch(sig)
	{
		case SIGQUIT:
		case SIGKILL:
		case SIGINT:
		case SIGTERM:
			key = 5266;
			shmid = shmget(key, 40, IPC_CREAT | 0666);
			
			
			if (shmid <0){
				perror("shmget");
				exit(1);
			}	

			shm = shmat(shmid, NULL,0);
			memset(shm,0 ,sizeof(share_data_t));
			if (shmdt(shm)==-1)
			{
				perror("shmdt");
				exit(1);
			}				
			exit(1);
			break;		
		default:
			break;
	}

}

int event_recv(int interval)
{
	int ret = -1;
	int timeout_times = 0;
	int timeout_limit = BOOT_LIMIT;
	int is_request = 0;
	int link_stat = 0;
	char link_standard[60];
	unsigned int data = 0;
	unsigned int magic = htonl(INTER_MAGIC);
	char* send_buf=NULL;
	char* rcv_buf=NULL;
	event_hdr_t *event;
	char cmd[256];
	
	pt_fd_close(ETH_TYPE_INIC);

	SISM_DBG_MSG(LOG_INFO,"start HeartBeat");

	ret = pt_fd_open(ETH_TYPE_EVENT,interval);
	if (ret != 0)
	{
		return ret;
	}


	send_buf = req_pt_pkt_buf(BUF_SIZ);

	if(send_buf == NULL)
	{
		goto err;
	}
	rcv_buf = req_pt_pkt_buf(BUF_SIZ);
	if(rcv_buf == NULL)
	{
		goto err;
	}

	/* prepare interval packet for Client*/
	interval = htonl(interval);
	memcpy(send_buf, &magic, 4);
	memcpy(send_buf+4, &interval, 4);
	
	while(1)
	{
		/* send interval packet to Client*/
		if(timeout_times % 2 == 0)
			pt_pkt_send(ETH_TYPE_EVENT,send_buf,HEARTBEAT_SIZE);

		ret = pt_pkt_recv(ETH_TYPE_EVENT, rcv_buf);
		if (ret != 0)
		{
			timeout_times++;
		}
		else
		{
#if 0 /* To Do: Host should not always sent interval to client*/
			if(is_request == 0)
			{
				is_request = 1;
				timeout_limit = RUNTIME_LIMIT;
				free_pt_pkt_buf(send_buf);
				send_buf = NULL;
			}
#endif		
			/* parse event */
			event = (event_hdr_t *)rcv_buf;
			if(event->magic_no == htonl(EVENT_MAGIC)) {
				switch(event->event_type) {
					case EVENT_HEARTBEAT:
						SISM_DBG_MSG(LOG_DEBUG, "HeartBeat received");
						timeout_times = 0;
						break;

					case EVENT_WAN_CHANGE:
						SISM_DBG_MSG(LOG_INFO,"EN7517 WAN Change:%d", (int)event->event_content[0]);

						/* 0->down, 1->adsl, 2->vdsl, 3->ether, 4->SFP */
						link_stat = (int)event->event_content[0];
						sscanf(&event->event_content[1], "%[^\n]", link_standard);
						if((link_stat != 2) && (link_stat != 4)) // 2:ADSL 4:VDSL
						{
							link_stat = 0; // down
						}

						SISM_DBG_MSG(LOG_INFO,"EN7517 WAN Change:");
						SISM_DBG_MSG(LOG_INFO,"link_stat = %d\n", link_stat);
						SISM_DBG_MSG(LOG_INFO,"link_standard = %s (len=%d)\n", link_standard, strlen(link_standard));

						if(link_stat == 0)
						{
							/* Link Down */
							sprintf(cmd, "echo %d > /proc/tc3162/client_info_link_stat", link_stat);
							system(cmd);

							/* Enable filter */
#if defined(RDKB_BUILD)
							system("echo enable > /proc/tc3162/etype_filter_fwd");
#else
							system("echo enable > /proc/tc3162/ae_wan_inic_filter");
#endif
							/* Update standard */
							sprintf(cmd, "echo \"%s\" > /proc/tc3162/client_info_link_standard", link_standard);
							system(cmd);
						}
						else
						{
							/* Disable filter */
#if defined(RDKB_BUILD)
							system("echo disable > /proc/tc3162/etype_filter_fwd");
#else
							system("echo disable > /proc/tc3162/ae_wan_inic_filter");
#endif
							/* Update standard */
							sprintf(cmd, "echo \"%s\" > /proc/tc3162/client_info_link_standard", link_standard);
							system(cmd);
							
							/* Link Up */
							sprintf(cmd, "echo %d > /proc/tc3162/client_info_link_stat", link_stat);
							system(cmd);
						}
						break;

					case EVENT_OAM_PING:
						/* 0 -> no, 1 -> test over*/
						SISM_DBG_MSG(LOG_INFO,"EN7517 OAM ping status Change:%d", (int)event->event_content[0]);
						break;

					default:
						SISM_DBG_MSG(LOG_ERR,"Unknow event type.");
						break;
				}
			}
		}

		/* timeout reach the limit*/
		if(timeout_times >= timeout_limit)
		{
			SISM_DBG_MSG(LOG_ERR,"ERROR: timeout !");
			ret = RECEIVE_TIMEOUT;
			goto err;
		}
	}

err:
	SISM_DBG_MSG(LOG_ERR,"heartbeat terminate");

	if(send_buf != NULL)
	{
		free_pt_pkt_buf(send_buf);
	}
	if(rcv_buf != NULL)
	{
		free_pt_pkt_buf(rcv_buf);
	}
	pt_fd_close(ETH_TYPE_EVENT);

	return ret;
}

int main (int argc, char **argv)
{
	int interval = HEARTBEAT_INTERVAL;
	int shmid = 0;
	key_t key;
	char* shm;
	int testsocket = 0;
	int len = 0;
	int mac_len =0 ;
	int gpio = 0;
	int ret = -1;
	int p_en = 0;
	int i = 0;
	int skip = 0;
	unsigned int mdio_phy_addr = 0;
	unsigned int inic_start_addr = 0x1000000;
	share_data_t s_data;
	unsigned char tmp[3];
	
	if (argc < 5)
	{
		SISM_DBG_MSG(LOG_ERR,"Usage: hsm [interface] [gpio] [mdio_phy_addr] [mac_addr] [skip_inic]");
		return -1;
	}

	sism_dbg_init();

	memset(&s_data, 0, sizeof(s_data));
	
	len = strlen(argv[1]);

	gpio = atoi(argv[2]);
	mdio_phy_addr = atoi(argv[3]);
	mac_len = strlen(argv[4]);
	if (mac_len != 12) {
		SISM_DBG_MSG(LOG_ERR,"mac address must be 12 digits");
		return -1;
	}
	tmp[2] = 0;
	for(i = 0; i < 6; i++){
		tmp[0] = argv[4][2*i];
		tmp[1] = argv[4][2*i+1];
		mac_addr[i] = (unsigned char)strtoul(tmp, NULL, 16);
	}

	skip = atoi(argv[5]);

	SISM_DBG_MSG(LOG_DEBUG,"gpio=%d, mdio_phy_addr=%d, skip=%d",gpio,mdio_phy_addr,skip);
	SISM_DBG_MSG(LOG_DEBUG,"mac addr = %02x:%02x:%02x:%02x:%02x:%02x",
		mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);

	/* register signal for clear shm*/
	signal(SIGQUIT, sigroutine);
	signal(SIGKILL, sigroutine);
	signal(SIGINT, sigroutine);
	signal(SIGTERM, sigroutine);
	signal(SIGALRM,hsm_timer);

	key = 5266;
	shmid = shmget(key, 40, IPC_CREAT | 0666);

	
	if (shmid <0){
		perror("shmget");
		exit(1);
	}	

	shm = shmat(shmid, NULL,0);
	memcpy(&s_data.itf_name, argv[1], len);
	s_data.len= len;

	if(skip == 1)
	{
		SISM_DBG_MSG(LOG_INFO,"Skip iNIC boot client !");

		s_data.pt_enable = SRC_EN | HEARTBEAT_EN;
		memcpy(shm, &s_data, sizeof(s_data));		

		while(1)
		{
			ret = event_recv(interval);

			if (ret != 0)
				SISM_DBG_MSG(LOG_ERR,"heartbeat timeout !");
		}
		return 0;
	}

	while(1)
	{
		s_data.pt_enable = INIC_EN;
		memcpy(shm, &s_data, sizeof(s_data));	

		SISM_DBG_MSG(LOG_INFO,"hsm: boot phase 1!");
		ret = host_bp1(gpio, mdio_phy_addr);
		if (ret!= 0)
		{
			SISM_DBG_MSG(LOG_ERR,"host_bp1 failed!");
			continue;
		}

		SISM_DBG_MSG(LOG_INFO,"hsm: boot phase 2!");
		ret = host_bp2(mdio_phy_addr, inic_start_addr);
		if (ret!= 0)
		{
			SISM_DBG_MSG(LOG_ERR,"host_bp2 failed!");
			continue;
		}

		s_data.pt_enable = SRC_EN | HEARTBEAT_EN;
		memcpy(shm, &s_data, sizeof(s_data));

		ret = event_recv(interval);

		if (ret != 0)
			SISM_DBG_MSG(LOG_ERR,"heartbeat timeout !");

	}

	sism_dbg_exit();

	return 0;
}



