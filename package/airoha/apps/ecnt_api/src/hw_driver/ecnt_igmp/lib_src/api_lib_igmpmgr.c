/*********************************************************************************
 * decleration and function prototype for igmp module
 *
 * Copyright (C) 2015 Econet Technologies, Corp.
 * All Rights Reserved.
 *
 *********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "libapi_lib_igmpmgr.h"

#define IGMP_DEV_NAME	"/dev/ecnt_igmp"

int igmp_fd = -1;

int ecnt_init_igmp(void)
{
	if (igmp_fd < 0)
	{
		igmp_fd = open(IGMP_DEV_NAME, O_RDONLY);    
		if (igmp_fd < 0)
		{    	
			printf("Open %s device failed\n",IGMP_DEV_NAME);    	
			return -1;
		}
	}
	
	return 0;
}

static inline int do_igmp_ioctl(unsigned int command,void *arg)
{
	int err = -1;
	
	if (igmp_fd < 0)
	{
		if (ecnt_init_igmp() < 0)
		{
			return -1;
		}
	}

	err = ioctl(igmp_fd,command,arg);
	if (err < 0)
	{
		printf("do_igmp_ioctl err, command = 0x%x.\n", command);
	}

	return err;
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_add
**      descriptions:
**      	add a entry into multicast forward table
**      input parameters:
**      	entry_data: entry init information
**      output parameters:
**      	N/A
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_add(ECNT_MC_PORT_INFO *entry_data)
{
	return do_igmp_ioctl(IGMP_ADD_ENTRY, entry_data);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_del
**      descriptions:
**      	delete a entry from multicast forward table
**      input parameters:
**      	entry_data: entry init information
**      output parameters:
**      	N/A
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_del(ECNT_MC_PORT_INFO *entry_data)
{
	return do_igmp_ioctl(IGMP_DEL_ENTRY, entry_data);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_update
**      descriptions:
**      	update the entry in multicast forward table
**      input parameters:
**      	entry_data: entry init information
**      output parameters:
**      	N/A
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_update(ECNT_MC_PORT_INFO *entry_data)
{
	return do_igmp_ioctl(IGMP_UPDATE_ENTRY, entry_data);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_clear_by_port
**      descriptions:
**      	clear the total entry in multicast forward table by port
**      parameters:
**      	port_id : port id
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_clear_by_port(unsigned char port_id)
{
	unsigned char arg = port_id;
	
	return do_igmp_ioctl(IGMP_CLEAR_ENTRY_BY_PORT, &arg);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_clear_all
**      descriptions:
**      	clear the total entry in multicast forward table
**      parameters:
**      	void
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_clear_all(void)
{
	int arg = 0;
	
	return do_igmp_ioctl(IGMP_CLEAR_ENTRY_ALL, &arg);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_entry_get
**      descriptions:
**      	get the entrys belongs to this port_id from multicast forward table
**      input parameters:
**      	port_id:	port id
**          entry[]:	entry array
**          array_size: entry array max size
**      output parameters:
**      	array_size: real entry amount
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_entry_get(unsigned char port_id, ECNT_MC_PORT_INFO entry[], int *array_size)
{
	ECNT_MC_PORT_INFO_GET arg;

	memset(&arg, 0, sizeof(ECNT_MC_PORT_INFO_GET));

	arg.port_id = port_id;
	arg.entry_num = *array_size;
	arg.pEntry = entry;
	
	if ((do_igmp_ioctl(IGMP_GET_ENTRY, &arg)) < 0)
	{
		printf("ecnt_igmp_entry_get failed\n");
		return -1;
	}

	/* fill in the real number */
	*array_size = arg.entry_num;
	
	return 0;
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_mode_set
**      descriptions:
**      	set current multicast data forward mode
**      input parameters:
**      	mode: forward mode
**      output parameters:
**      	N/A
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_mode_set(ECNT_MC_FWD_MODE mode)
{
	int arg = mode;

	if(arg > ECNT_FWD_MODE_MAC_AND_IP_AND_VID){
		printf("ecnt_igmp filter mode should be 0~%d\n", ECNT_FWD_MODE_MAC_AND_IP_AND_VID);
		return -1;
	}
	
	return do_igmp_ioctl(IGMP_SET_FILTER_MODE, &arg);
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_mode_get
**      descriptions:
**      	get current multicast data forward mode
**      input parameters:
**      	N/A
**      output parameters:
**      	mode: forward mode
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_mode_get(ECNT_MC_FWD_MODE *mode)
{
	int arg = 0;
	
	if (do_igmp_ioctl(IGMP_GET_FILTER_MODE, &arg) < 0)
	{
		printf("ecnt_igmp_mode_get failed\n");
		return -1;
	}

	*mode = (ECNT_MC_FWD_MODE)arg;
	
	return 0;
}


/*___________________________________________________________________________
**      function name: ecnt_igmp_snooping_on_off_set
**      descriptions:
**      	set the igmp snooping on or off
**      input parameters:
**      	enable:	[0: disable  1: enable]
**      output parameters:
**      	N/A
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_snooping_on_off_set(unsigned char enable)
{
	unsigned char snooping_on_off;
	
	if (enable)
	{
		snooping_on_off = ECNT_IGMP_SNOOPING_ON;
	}
	else
	{
		snooping_on_off = ECNT_IGMP_SNOOPING_OFF;
	}

	if ((do_igmp_ioctl(IGMP_SET_IGMP_SNOOPING_ON_OFF, &snooping_on_off)) < 0)
	{
		printf("ecnt_igmp_snooping_on_off_set failed\n");
		return -1;
	}
	
	return 0;
}

/*___________________________________________________________________________
**      function name: ecnt_igmp_snooping_on_off_get
**      descriptions:
**      	get the igmp snooping state
**      input parameters:
**      	enable: 
**      output parameters:
**      	enable: [0: disable  1: enable]
**
**      return:
**      	success:	0
**		failure:	-1
**___________________________________________________________________________
*/
int ecnt_igmp_snooping_on_off_get(unsigned char *enable)
{

	if ((do_igmp_ioctl(IGMP_GET_IGMP_SNOOPING_ON_OFF, enable)) < 0)
	{
		printf("ecnt_igmp_snooping_on_off_get failed\n");
		return -1;
	}
	
	return 0;
}


