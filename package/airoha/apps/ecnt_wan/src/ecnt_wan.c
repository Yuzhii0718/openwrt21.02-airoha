#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#ifdef TCSUPPORT_SISM
#include <uci.h>
#include "modules/service_manager/wan_mode_sel.h"
#include "blapi_traffic_bsp.h"
#include "blapi_system.h"
#include "blapi_xdsl.h"
#endif

#define ACTIVE_ETHERNET_WAN_STATE_PATH        "/proc/tc3162/ae_wan_link_status"
#define SIP_PID_FILE        "/var/run/voip_sip.pid"

typedef enum _PON_STATUS_
{
    E_WAN_PON_NULL = 0, 
    E_WAN_PON_DOWN = 1, 
    E_WAN_PON_UP = 2, 
}E_PON_STATUS_T;

#ifdef TCSUPPORT_SISM
void get_uci_option_val(const char *package, const char *section, const char *option, char *result, size_t result_size)
{
	char uci_path[256] = {0};
	struct uci_context *ctx = NULL;
	struct uci_ptr ptr;
	
	ctx = uci_alloc_context();
	if (ctx == NULL)
	{
		fprintf(stderr, "Failed to allocate UCI context\n");
		return;
	}
	
	snprintf(uci_path, sizeof(uci_path), "%s.%s.%s", package, section, option);
	if (uci_lookup_ptr(ctx, &ptr, uci_path, true) != UCI_OK)
	{
		fprintf(stderr, "Failed to lookup UCI path:%s\n", uci_path);
		uci_free_context(ctx);
		return;
	}
	if (ptr.o != NULL && ptr.o->type == UCI_TYPE_STRING)
	{
		strncpy(result, ptr.o->v.string, result_size - 1);
		result[result_size - 1] = '\0';
	}
	else
	{
		fprintf(stderr, "Option not found or not a string:%s\n", uci_path);
	}
	uci_free_context(ctx);
}

void create_xdsl_device()
{
	FILE *fp = NULL;
	char BR_LAN_MAC[32] = {0};
	char CLIENT_MAC[32] = {0};
	char xdsl_hardware[8] = {0};
	char cmd[128] = {0};
	unsigned char mac[6] = {0};	
	int ret = 0, pvcactive_flag = 0;
	char xdsl_vpi[8] = {0}, xdsl_vci[8] = {0};
	char xdsl_wan_link_mode[8] = {0};
	blapi_atm_interface_obj_t interface_obj = {0};
	blapi_atm_device_obj_t device_obj = {0};

	interface_obj.idx = 0;
	strcpy(interface_obj.transMode, "ATM");
	strcpy(interface_obj.str_qos, "ubr");
	strcpy(interface_obj.str_encap, "PPPoE LLC");
	get_uci_option_val("xdsl", "xdsl", "vpi", xdsl_vpi, sizeof(xdsl_vpi));
	get_uci_option_val("xdsl", "xdsl", "vci", xdsl_vci, sizeof(xdsl_vci));
	interface_obj.vpi = atoi(xdsl_vpi);
	interface_obj.vci = atoi(xdsl_vci);
	strcpy(interface_obj.str_pcr, "0");
	strcpy(interface_obj.str_scr, "0");
	strcpy(interface_obj.str_mbs, "0");
	get_uci_option_val("xdsl", "xdsl", "wan_link_mode", xdsl_wan_link_mode, sizeof(xdsl_wan_link_mode));
	if ( 0 == strcmp(xdsl_wan_link_mode, "PPPoE") )
	{
		strcpy(interface_obj.str_encap, "PPPoE LLC");
		strcpy(interface_obj.isp, "2");
	}
	else
	{
		strcpy(interface_obj.str_encap, "1483 Bridged IP LLC");
		strcpy(interface_obj.isp, "0");
	}
	ret = blapi_xdsl_atm_create_interface(&interface_obj, &pvcactive_flag);
	if ( 0 != ret )
	{
		fprintf(stderr, "blapi_xdsl_atm_create_interface failed, ret=%d\n", ret);
	}

	fp = popen("ifconfig br-lan | sed -n '1p' | sed 's/^.*HWaddr //g' | sed 's/[ \t]*$//g'", "r");
	if ( NULL != fp )
	{
		fgets(BR_LAN_MAC, sizeof(BR_LAN_MAC), fp);
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "BR_LAN_MAC is NULL");
		return;
	}
	sscanf(BR_LAN_MAC, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	
	device_obj.idx = 0;
	strcpy(device_obj.transMode, "ATM");
	strcpy(device_obj.dev, "nas0");
	get_uci_option_val("xdsl", "xdsl", "hardware", xdsl_hardware, sizeof(xdsl_hardware));
	if ( 0 == strcmp(xdsl_hardware, "1") )
	{
		strcpy(device_obj.mac, "00:AA:BB:01:23:40");
	}
	else
	{
#if 0
		mac[0] = (mac[0] << 1) & 0xff;
		mac[1] = (mac[1] << 2) & 0xff;
		mac[2] = (mac[2] << 3) & 0xff;
		mac[3] = (mac[3] << 4) & 0xff;
		snprintf(device_obj.mac, sizeof(device_obj.mac), "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#else
		fp = popen("ps | grep -v 'grep' | grep -e '/userfs/bin/hsm' | awk '{print $9}'", "r");
		if(!(fp && fgets(CLIENT_MAC, sizeof(CLIENT_MAC), fp)))
		{
			strcpy(CLIENT_MAC, "00AABB012340");
		}
		sscanf(CLIENT_MAC, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		snprintf(device_obj.mac, sizeof(device_obj.mac), "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		if ( fp )
		{
			pclose(fp);
		}
#endif
	}
	ret = blapi_xdsl_create_device(&device_obj);
	if ( 0 != ret )
	{
		fprintf(stderr, "blapi_xdsl_create_device failed, ret=[%d]\n", ret);
		return;
	}
	system("ifconfig nas0 down");
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ifconfig nas0 hw ether %s", BR_LAN_MAC);
	system(cmd);
	system("ifconfig nas0 up");
}
#endif

int ecnt_get_wan_state()
{
    FILE *fp = NULL;
	char string[80] = {0};
	char etherState[8] = {0};
	int etherUp = E_WAN_PON_NULL;
	char state[3]={0};
#ifdef TCSUPPORT_SISM
	char cmd[128] = {0};
	char xdsl_state[XDSL_STR_BUFF_SIZE + 1] = {0};
	int type = 0, ret = 0, xdsl_isbsp_result = 0;
	char xdsl_isbsp[8] = {0};
	get_uci_option_val("xdsl", "xdsl", "isbsp", xdsl_isbsp, sizeof(xdsl_isbsp));
	xdsl_isbsp_result = atoi(xdsl_isbsp);
	static int atm_itf_create = 0;
#endif
	
	fp = fopen(ACTIVE_ETHERNET_WAN_STATE_PATH, "r");        
	if(fp != NULL)
	{
		while(fgets(string, sizeof(string), fp) != NULL)
		{     
            if(sscanf(string, "ae_wan_phy_link_status = %s\n" , etherState) == 1)
			{
                if (strcmp(etherState, "up") == 0)
				{
					etherUp = E_WAN_PON_UP;
#ifdef TCSUPPORT_SISM
					/*check dsl enable/disable*/
					if ( 1 == blapi_traffic_check_dsl_enable() )
					{
						if ( 1 == xdsl_isbsp_result )
						{
							break;
						}
						/*check xdsl state*/
						blapi_xdsl_get_xdsl_linkstatus(xdsl_state, &type);

						/*ATM UP*/
						if ( ( 1 == type ) && ( 0 == atm_itf_create ) )
						{
							atm_itf_create = 1;
							create_xdsl_device();
						}
					}
					if((1 != type) && (2 != type))
					{
						etherUp = E_WAN_PON_DOWN;
					}
#endif
					break;
				}
				else
				{
					etherUp=E_WAN_PON_DOWN;
					break;
				}
			}

            memset(string, 0,sizeof(string));
		}
		fclose(fp);
	}
	else
	{
		fp = popen("/userfs/bin/ponmgr gpon get info | grep 'ONU State' | awk '{print $3}'", "r");
		if(fp != NULL)
		{
			fgets(state, sizeof(state), fp);
			pclose(fp);
		}
		if(strncmp(state, "O1", 2)  == 0) {
			etherUp=E_WAN_PON_DOWN;
		} else {
			etherUp=E_WAN_PON_UP;
		}
	}

	return etherUp;
}

int ecnt_wan_handle_xpon_event(int event){
	FILE *fp = NULL;
	char buf[128] = {0};
	char interface[16] = {0};
	char cmd[64]={0};

	system("ubus -S list 'network.interface.*' > /tmp/wanInfo");	
	fp = fopen("/tmp/wanInfo", "r");	
	if(fp != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {	
			if(sscanf(buf, "network.interface.%s\n" , interface) == 1)
			{
                if (strcmp(interface, "lan") != 0)
				{
					snprintf(cmd, sizeof(cmd), "ubus call network.interface %s \'{ \"interface\" : \"%s\" }\'",E_WAN_PON_UP == event ? "up" : "down" ,interface);
					system(cmd);
				}
			}

			memset(interface, 0,sizeof(interface));
			memset(cmd, 0,sizeof(cmd));
            memset(buf, 0,sizeof(buf));
		}

		fclose(fp);
	}
	return 0;
}

struct list_head {
	int pid;
	int count;
	int ischanged;
	struct list_head *next;
	struct list_head *prev;
};

void list_del(struct list_head *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	entry->next = entry->prev = NULL;
	free(entry);
}

void list_add(struct list_head *new, struct list_head *head)
{
	head->next->prev = new;
	new->next = head->next;
	new->prev = head;
	head->next = new;
}

int ecnt_check_pid(struct list_head *list){
	FILE *fp = NULL;
	char buf[32] = {0};
	int pid = 0;
	int found = 0;
	char cmd[64]={0};
	struct list_head *tmp = NULL;
	struct list_head *head = NULL;

	system("ps | grep odhcpd-update | awk '{ print $1 }' > /tmp/pidInfo");
	system("ps | grep 'jshn -p procd -w' | awk '{ print $1 }' >> /tmp/pidInfo");
	fp = fopen("/tmp/pidInfo", "r");	
	if(fp != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {	
			if(sscanf(buf, "%d" , &pid) == 1)
			{
				//find pid in list
				head = list;
				while (head->next != list)
				{
					if(head->next->pid == pid)
					{
						head->next->count++;
						head->next->ischanged = 1;
						found = 1;
						break;
					}
					head = head->next;
				}
				
				//if pid not found, add it into list
				if(found == 0)
				{
					tmp = (struct list_head *)malloc(sizeof(struct list_head));
					if(NULL != tmp)
					{
						memset(tmp,0,sizeof(struct list_head));
						tmp->pid = pid;
						tmp->count = 1;
						tmp->ischanged = 1;
						list_add(tmp,list);
					}
				}
			}
			found = 0;
			pid = 0;
            memset(buf, 0,sizeof(buf));
		}

		head = list;
		while (head->next != list)
		{
			//if pid has found more than five times, kill it
			if(head->next->count > 5)
			{
				kill(head->next->pid, SIGTERM);
				list_del(head->next);
				continue;
			}
			//if pid not found, del it from list
			if(head->next->ischanged == 0)
			{
				list_del(head->next);
				continue;
			}
			head->next->ischanged = 0;
			head = head->next;
		}

		fclose(fp);
	}
	return 0;
}

int chkSipProcess(void)
{
	FILE *fp = NULL;
	char pid_buf[32] = {0};
	char cmd_line_path[64] = {0};
	char cmd_line[128] = {0};
	char *cmd_chk = NULL;
	int status = 0;
	int pid = 0;
	int count = 0;
	
	if(NULL == (fp = fopen(SIP_PID_FILE, "r")))
	{
		return -1;
	}

	count = fread(pid_buf, sizeof(char), sizeof(pid_buf) - 1, fp);
	if ( count < 0 )
	{
		printf("fread error.\n");
	}

	fclose(fp);

	pid = atoi(pid_buf);
	if ( pid > 0 )
	{
		snprintf(cmd_line_path, sizeof(cmd_line_path), "/proc/%d/cmdline", pid);
		fp = fopen(cmd_line_path, "r");
		if(fp)
		{
			count = fread(cmd_line, sizeof(char), sizeof(cmd_line) - 1, fp);
			if ( count < 0 )
			{
				printf("fread error.\n");
			}
			fclose(fp);
		}
		cmd_line[sizeof(cmd_line) - 1] = '\0';
		cmd_chk = strstr(cmd_line, "sip");
		if ( cmd_chk )
			status = 1;
	}
	else
	{
		return -1;
	}

	/*if status == 0, that's mean need restart sipclient*/
	if (0 == status)
	{
		printf("\nrestart sip app for voip.\n");
		system("/usr/bin/killall -9 sipclient");
		unlink(SIP_PID_FILE);
		system("/userfs/bin/sipclient &");
	}

	return 0;
}

int
main(int argc, char **argv)
{
	E_PON_STATUS_T ponStatus = E_WAN_PON_NULL;
	static E_PON_STATUS_T old_ponStatus = E_WAN_PON_NULL;
	static int ponFirstUp = 1;
	int gWait = 0;
	struct list_head *list = NULL;
	list = (struct list_head *)malloc(sizeof(struct list_head));
	if(NULL == list)
	{
		return -1;
	}
	list->next=list;
	list->prev=list;
	
	
	while(1)
    {    
    	sleep(5);
		//get ae_wan status
		ponStatus = ecnt_get_wan_state();
		
		//check if pon status has changed
        if(E_WAN_PON_NULL != ponStatus && old_ponStatus != ponStatus){
			//if pon is first down or up, do nothing
			if(ponStatus == E_WAN_PON_DOWN && old_ponStatus == E_WAN_PON_NULL){
			}else{		
				old_ponStatus = ponStatus;
				if(ponFirstUp == 1 && E_WAN_PON_UP == ponStatus){
					ponFirstUp = 0;
				}else{
					//if pon status has changed, disable or enable interface
					ecnt_wan_handle_xpon_event(ponStatus);
				}
			}
    	}
		
		//monitor process, if live long time kill it
		ecnt_check_pid(list);
		
		//monitor sipclient, restart it if it not exits
		chkSipProcess();
    }
	free(list);
	return 0;
}
