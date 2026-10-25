#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include "libapi_lib_meshmgr.h"
#include <libapi_lib_utility.h>

#define MAX_RETRY_SEND 5

void mesh_printf(char *fmt,...)
{
	FILE *proc_file = NULL;
	char msg[512];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, 512, fmt, args);	

	proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (NULL == proc_file) 
	{
		printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
		return;
	}

	fprintf(proc_file, "%s", msg);
	fclose(proc_file);
	va_end(args);

	return ;
}

static int build_evt_notify(int type, char* evt_buf)
{
	struct evt_ct *event = NULL;
	int send_len  = 0;
	event = (struct evt_ct *)evt_buf;
	event->type = type;
	
	send_len = sizeof(struct evt_ct);

	return send_len;
}

int meshmgr_lib_notify_cfg(int type)
{
	int ret = API_LIB_SUCCESS;
	int sock = -1;
	char *evt_buf = NULL;
	struct sockaddr_un s_addr;
	int len = 0;
	int bytes_sent = 0;
	int n = 0;
	
	if((MAX_EVT < type) || (0 > type))
	{
		return API_LIB_FAIL;
	}
	
    sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		ret = API_LIB_FAIL;
		goto exit;
    }
	
	evt_buf = (char*)malloc(MAX_EVT_BUFFER_LEN * sizeof(char));
	if (!evt_buf) 
	{
		ret = API_LIB_FAIL;
		goto exit;
	}
	
	memset(&s_addr, 0, sizeof(s_addr));
	snprintf(s_addr.sun_path, sizeof(s_addr.sun_path), "%s", TMP_MESH_CTRL_DIR);
 	s_addr.sun_family = AF_UNIX;
	switch (type)
	{
		case NOTIFY_WAPP_READY:
		case NOTIFY_P1905_READY:
		case NOTIFY_MAPD_READY:
		case NOTIFY_WPS_START:
		case NOTIFY_WPS_STOP:
		case NOTIFY_TOPOLOGY_PROCESS:
		case NOTIFY_APCLI_DISASSOCIATED:
		case NOTIFY_APCLI_ASSOCIATED:
		case NOTIFY_BH_READY:
		case NOTIFY_NETWORK_COMPLETE:
		case NOTIFY_BH_DISCONNET:
		case NOTIFY_WAPP_DEV_READY:
		{
			len = build_evt_notify(type, evt_buf);
			break;
		}
		default:
		{
			goto exit;
		}
	}
	
retry_send:
	bytes_sent = sendto(sock, evt_buf, len, 0, (struct sockaddr *)&s_addr, sizeof(s_addr));
	if(0 > bytes_sent)
	{
		mesh_printf("func = %s, Error sending packet: %s\n", __func__, strerror(errno));
		sleep(1);
		if(MAX_RETRY_SEND >= n)
		{
			n++;
			goto retry_send;
		}
		else
		{
			mesh_printf("func = %s, send msg faile: type = %d, %s\n", __func__, type, strerror(errno));
		}
	}
	
exit:
	if(evt_buf)
	{
		free(evt_buf);
	}
	
	if(0 <= sock)
	{
		close(sock);
	}

	return ret;
}

