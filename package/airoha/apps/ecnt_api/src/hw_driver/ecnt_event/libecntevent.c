/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdarg.h>
#include <pthread.h>
#include <linux/netlink.h>
#include <sys/msg.h>

#include <ecnt_event_global/ecnt_event_global.h>
#include "libecntevent.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/
typedef struct __pthreadArgType{
		int 		sock_fd;
		int 		(*hook)(struct ecnt_data *in_data);
}pthreadArgType;


/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/



/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Function name: 	ecnt_netlink_create()
 Descriptor:		Used to create netlink socket.
 Input Args:		nl_saddr, pid.
 Ret Value:		success: sock_fd
                    	fail: -1
******************************************************************************/
int ecnt_netlink_create(struct sockaddr_nl* nl_saddr, int pid) {
	int sock_fd;
	
	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ECNT_EVENT);
	if(sock_fd < 0) {
		printf("pid = %d:nlksock create err:%s!\n", pid,strerror(errno));
		return -1;
	}
	
	nl_saddr->nl_family = AF_NETLINK;
	nl_saddr->nl_pid = pid;   /* pid */
	nl_saddr->nl_groups = 1;  /* mcast groups */

	if(bind(sock_fd, (struct sockaddr*)nl_saddr, sizeof(*nl_saddr)) < 0){
		printf("pid = %d:nlksockt bind err:%s!\n", pid,strerror(errno));
		close(sock_fd);
		return -1;
	}
	
	return sock_fd;
}

static void * event_recv_thread(void *arg)
{
	pthreadArgType *pthreadArg = NULL;
	int ret = 0;
	int	(*event_hook)(struct ecnt_data *in_data);
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
	struct sockaddr_nl nl_daddr;
	int sock_fd;
	
	if (arg == NULL)
		 goto exit;
	
	pthreadArg = (pthreadArgType*)arg;
	sock_fd = pthreadArg->sock_fd;
	event_hook = pthreadArg->hook;
	free(pthreadArg);
	
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
    if(!nlh){
        printf("malloc nlmsghdr error!\n");
        close(sock_fd);
        goto exit;
    }
    memset(nlh, 0, NLMSG_SPACE(MAX_MSGSIZE));

    /*iov structure */
    iov.iov_base = (void *)nlh;
    iov.iov_len = NLMSG_SPACE(MAX_MSGSIZE);

	/* msg */
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&nl_daddr;
    msg.msg_namelen = sizeof(nl_daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
	while(1){
		memset(nlh, 0, NLMSG_SPACE(MAX_MSGSIZE));
		ret = recvmsg(sock_fd, &msg, 0);
		if(ret < 0){
			if(errno==EINTR)
				continue;
			else
				{
			printf("netlink recv err:%s!\n", strerror(errno));
			break;
		}       
		}       
		event_hook(nlh);
	}
	free(nlh);

exit:	
	pthread_exit(0);
}

int ecnt_event_execute(struct ecnt_data *in_data, 
	struct ecnt_event_handle *event_handle)
{
	int i = 0;
	struct nlmsghdr *nlh;
	struct ecnt_event_data *event_data;
	unsigned short event_type;
    
	nlh = (struct nlmsghdr *)in_data;
	
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));

	while(event_handle[i].handle){
		event_type = (((event_handle[i].maintype)<<8) | (event_handle[i].subtype));
		if(nlh->nlmsg_type == event_type){
			event_handle[i].handle(event_data);
		}
		i++;
	}
	
	return 1;
}


int ecnt_event_send(int main_type , int sub_type, 
	struct ecnt_event_data *in_data, int len)
{
	struct nlmsghdr *nlh = NULL;
	int sock_fd = -1, ret = -1;
	struct sockaddr_nl nl_saddr,nl_daddr;
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));
	sock_fd = ecnt_netlink_create(&nl_saddr, (int)pthread_self() | getpid());
	if (sock_fd < 0) {		
		return -1;
	}

	memset(&nl_daddr, 0, sizeof(nl_daddr));
	nl_daddr.nl_family = AF_NETLINK;
	//nl_daddr.nl_pid = pthread_self() | getpid();
	nl_daddr.nl_groups = 1;

	nlh = malloc(NLMSG_SPACE(len));
	if (NULL == nlh) {
		close(sock_fd);
		return -1;
	}

	nlh->nlmsg_len = NLMSG_SPACE(len);
	nlh->nlmsg_pid = getpid();  /* self pid */
	nlh->nlmsg_flags = 0;
	nlh->nlmsg_type = (main_type <<8) | sub_type;
	
	memcpy((char*)(NLMSG_DATA(nlh)), (char *)in_data, len);
	
	ret = sendto(sock_fd, (char *)nlh, NLMSG_SPACE(len), 0, (struct sockaddr*)&nl_daddr, sizeof(nl_daddr));  
	close(sock_fd);
	free(nlh);
	return ret;
}


struct ecnt_event_source* ecnt_event_register(char *name,
	int (*hook)(struct ecnt_data *in_data))
{
	pthreadArgType *threadArg;
	pthread_t thread;
	int sock_fd;
	struct sockaddr_nl nl_saddr;
	struct ecnt_event_source *source;
	
	memset(&nl_saddr, 0, sizeof(nl_saddr));
	
	sock_fd = ecnt_netlink_create(&nl_saddr, getpid());
	if(sock_fd < 0) {
		return NULL;
	}

	source = (struct ecnt_event_source *)malloc(sizeof(struct ecnt_event_source));
	if(source == NULL){
		printf("malloc source fail!\n");
		goto exit2;
	}
		
	threadArg = (struct pthreadArgType *)malloc(sizeof(struct __pthreadArgType));
	if(threadArg == NULL){
		printf("malloc threadArg fail!\n");
		goto exit1;
	}
	
	threadArg->sock_fd = sock_fd;
	threadArg->hook = hook;
	if(pthread_create(&thread, NULL, event_recv_thread, (void*)threadArg) == 0)
	{
		printf("PID=%d event receive pthread_create OK, keep going......\n", getpid());
		source->sock_fd = sock_fd;
		return source;
	}
	printf("pthread_create fail!\n");
	free(threadArg);

exit1:
	free(source);
exit2:
	close(sock_fd);
	return NULL;
}

int ecnt_event_unregister(struct ecnt_event_source* source)
{
	if(!source)
		return 0;
	close(source->sock_fd);
	free(source);
	return 0;
}

