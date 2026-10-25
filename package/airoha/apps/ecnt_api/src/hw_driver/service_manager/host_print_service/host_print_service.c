#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdarg.h>
#include "../pt/lib_pt.h"

#define BUF_SIZ		1400


int main (int argc, char **argv)
{
	int ret = 0;
	int idx = 0;
	uint8_t *r_buf;

	if (argc <2)
	{
		SISM_DBG_MSG(LOG_ERR, "Usage: hps [interface]");
		return 0;
	}

	SISM_DBG_MSG(LOG_INFO, "init WAN !");
	pt_fd_open(ETH_TYPE_WAN,5);

	r_buf = req_pt_pkt_buf(BUF_SIZ);
	if (r_buf == NULL)
	{
		SISM_DBG_MSG(LOG_ERR,"%alloc failed!");
		return;
	}

	while(1)
	{
		ret = pt_pkt_recv(ETH_TYPE_WAN, r_buf);

		if(ret == 0)
		{
#if DEBUG
			SISM_DBG_MSG(LOG_DEBUG, "\tData:");
			for (idx=0; idx<BUF_SIZ; idx+=8)
			{
				SISM_DBG_MSG(LOG_DEBUG, "\t%02x:02x:02x:02x:02x:02x:02x:02x",
					r_buf[idx], r_buf[idx+1], r_buf[idx+2], r_buf[idx+3], r_buf[idx+4], r_buf[idx+5], r_buf[idx+6], r_buf[idx+7]);
			}
#endif
			for (idx=0; idx<BUF_SIZ; idx++)
			{
				SISM_DBG_MSG(LOG_PRINTF, "%c", r_buf[idx]);
			}
			memset(r_buf, 0, BUF_SIZ);
		}
		else
			SISM_DBG_MSG(LOG_ERR, "unhandle packet!!");

	}

	return 0;
}

