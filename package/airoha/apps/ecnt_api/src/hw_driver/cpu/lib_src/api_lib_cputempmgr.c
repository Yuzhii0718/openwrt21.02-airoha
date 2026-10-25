#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "libapi_lib_cputempmgr.h"

ECNT_CPUTEMPMGR_RET cputempmgr_lib_get(cpu_temp_msg_t *msg)
{
	int ret = -1;
	cpu_temp_msg_t msg_recv;
	int sockfd = 0, len = 0;
	struct sockaddr_un remote;

	if (msg == NULL)
		return ECNT_CPUTEMPMGR_FAIL;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return ECNT_CPUTEMPMGR_FAIL;
	}
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, CPU_TEMP_SOCK);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		close(sockfd);
		return ECNT_CPUTEMPMGR_FAIL;
	}
	if (send(sockfd, msg, sizeof(cpu_temp_msg_t), 0) == -1){
		perror("send");
		close(sockfd);
		return ECNT_CPUTEMPMGR_FAIL;
	}
	memset(&msg_recv, 0, sizeof(cpu_temp_msg_t));

	if ((recv(sockfd, &msg_recv, sizeof(cpu_temp_msg_t), 0)) <= 0 ) {
		/*Failure to receive information from cfg manage*/
		perror("recv");
		close(sockfd);
		return ECNT_CPUTEMPMGR_FAIL;
	}
	close(sockfd);

	msg->cpu_temp = msg_recv.cpu_temp;
	return ECNT_CPUTEMPMGR_SUCCESS;
}


