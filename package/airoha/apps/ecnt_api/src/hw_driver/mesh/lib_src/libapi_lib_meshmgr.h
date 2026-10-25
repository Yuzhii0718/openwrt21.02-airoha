#ifndef _LIBAPI_LIB_MESH_H_
#define _LIBAPI_LIB_MESH_H_
/**
* \file  libapi_lib_meshmgr.h  
* \brief This file is meshmgr api header file of business logic APIs that will be exported for others to use.
* \author Yifan.Chen
* \date     2020-9-28
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif /* GNU_PACKED */

#define TMP_MESH_CTRL_DIR "/tmp/mesh_msg"
#define MAX_EVT_BUFFER_LEN        256

enum evt_type
{
	NOTIFY_WAPP_READY,
	NOTIFY_P1905_READY,
	NOTIFY_MAPD_READY,
	NOTIFY_WPS_START,
	NOTIFY_WPS_STOP,
	NOTIFY_TOPOLOGY_PROCESS,
	NOTIFY_APCLI_DISASSOCIATED,
	NOTIFY_APCLI_ASSOCIATED,
	NOTIFY_BH_READY,
	NOTIFY_NETWORK_COMPLETE,	
	NOTIFY_BH_DISCONNET,
	NOTIFY_WAPP_DEV_READY,
	MAX_EVT,
};

/*********evt content*********/
struct GNU_PACKED evt_ct 
{
	unsigned short type;
	unsigned short length;
	unsigned char buffer[0];
};

/**
 * \brief This function send msg to up process.
 *
 * This function send msg to up process. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  send msg to up process.
 * \par Example
 * \snippet meshmgr.c Write send msg to up process.
 */
int meshmgr_lib_notify_cfg(int type);

#endif
