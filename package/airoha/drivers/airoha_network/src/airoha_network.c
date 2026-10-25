/*
 * mtd - simple memory technology device manipulation tool
 *
 * Copyright (c) 2006, 2007 Thorsten Glaser <tg@freewrt.org>
 * Copyright (C) 2005 Waldemar Brodkorb <wbx@freewrt.org>,
 *	                  Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * The code is based on the linux-mtd examples.
 */


#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <string.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <trx.h>
#include <mtd/mtd-user.h>
#include <uci.h>

#include "airoha_network.h"

#define DEFAULT_CONFIG_PATH	"/etc/config"

struct uci_context *uci_pon_ctx;
struct uci_package *uci_pon;
const char *config_path = DEFAULT_CONFIG_PATH;

static struct uci_package *
config_init_package(const char *config)
{
	struct uci_context *ctx = uci_pon_ctx;
	struct uci_package *p = NULL;

	if (!ctx) {
		ctx = uci_alloc_context();
		uci_pon_ctx = ctx;

		ctx->flags &= ~UCI_FLAG_STRICT;
		if (config_path)
			uci_set_confdir(ctx, config_path);

#ifdef DUMMY_MODE
		uci_set_savedir(ctx, "./tmp");
#endif
	} else {
		p = uci_lookup_package(ctx, config);
		if (p)
			uci_unload(ctx, p);
	}

	if (uci_load(ctx, config, &p))
		return NULL;
	return p;
}

/*****************************************************************************
 * Function : static void bridge_if_stale_ipv6_remove(void)
 * Purpose : To remove the stale ipv6 pd in the bridge / br-lan interface
 * Input args : Nil
 * Output args : Nil
 * Return value : Nil
 *****************************************************************************/
static void bridge_if_stale_ipv6_remove(void)
{
    const char *type = NULL, *ip6prefix = NULL;
    int wan_index = 0, refresh_br_lan = 0;
    int fd = -1;
    struct uci_section *vlan;
    char *p[MAX_NUM_INTF] = { "wan",
                              "wan1",
                              "wan2",
                              "wan3",
                              "wan4",
                              "wan5",
                              "wan6",
                              "wan7",
                              "wan8",
                              "wan9",
                              "wan10",
                              "wan11",
                              "wan12",
                              "wan13",
                              "wan14",
                              "wan15"
                            };
    if (airoha_get_section_value(VLAN_SECTION_NAME, &vlan) != STATUS_OK) {
        return ;
    }

    if (airoha_get_option_value(vlan, "type", &type) != STATUS_OK) {
        log_message(L_NOTICE, "xPON_Log - LAN: Failed to get vlan type.\r\n");
        return ;
    } 
    if ((type != NULL) && (strncmp(type, "bridge", 6) == 0))
    {
        for ( wan_index = 0; wan_index <= MAX_NUM_INTF-1 && p[wan_index] ; wan_index++)
        {
            struct uci_section *wan = NULL;
            if (airoha_get_section_value(p[wan_index], &wan) != STATUS_OK) {
                    continue;
            }

            if (airoha_get_option_value(wan, "ip6prefix", &ip6prefix) != STATUS_OK) {
                log_message(L_NOTICE, "xPON_Log - LAN: ip6prefix is not present, set to default NULL.\r\n");
                ip6prefix = "";
            }
            log_message(L_NOTICE, "xPON_Log - LAN: ip6prefix is %s.\r\n", ip6prefix);

            if (strcmp(prev_ipv6_pd[wan_index], ip6prefix) != 0)
            {
                log_message(L_NOTICE, "xPON_Log - LAN: ip6prefix has changed, set the flag.\r\n");
                refresh_br_lan = 1;
            }
        }
    }
    log_message(L_NOTICE,"xPON_Log - LAN: Refresh br_lan is %d.\r\n", refresh_br_lan);

    if (refresh_br_lan) {
         log_message(L_NOTICE, "xPON_Log - LAN: bring down the bridge interface (br-lan).\r\n");
         if ((fd = open(INTF_STATE_FLAG, O_RDONLY | O_CREAT, 0644)) < 0) {
	        log_message(L_NOTICE,"xPON_Log - LAN: error when create file [%s].\r\n", INTF_STATE_FLAG);
         } else {
	        close(fd);
         }
         log_message(L_NOTICE, "xPON_Log - LAN: invoke intf_refresh.sh.\r\n");
         if (system("/bin/sh /etc/intf_refresh.sh &") < 0) {
			 log_message(L_NOTICE, "xPON_Log - LAN: failed to run /etc/intf_refresh.sh.\r\n");
		 }
    }
}

int config_init_brlan(void)
{
    char cmd[CMD_BUF_MAX_LEN] = {0};
    
	snprintf(cmd, CMD_BUF_MAX_LEN - 1, "brctl quickleave br-lan on");
    airoha_shell_execute(cmd, sizeof(cmd));

    return STATUS_OK;
}

int
main(int argc, char **argv)
{
	int ret = 0;
	char *err;
	  //const char *pon_mode;
    const char *auth_type_name, *auth_type, *auth_password, *auth_username;
	const char *vlan_id, *payload;
	const char *sn_auth_type;
    struct uci_section *pon_auth;
	struct uci_element *element;
	struct stat file_buf;
	//fprintf(stderr, "xPON_Log: pon_mode=%s\r\n", pon_mode);

	
	uci_pon = config_init_package("pon");
	if (!uci_pon) {
		uci_get_errorstr(uci_pon_ctx, &err, NULL);
		if(err != NULL)
			free(err);
		return -1;
	}

	/* The file /tmp/.xponconfig will be set only if wan configs
	   are changed in the GUI and only if so, need to do the
	   wan configurations in the backend */
	fprintf(stderr, "xPON_Log:=========================\r\n");
	if(access(WAN_MODE_FLAG, F_OK) != 0) {
		// PON MODE
		
		fprintf(stderr, "xPON_Log: PON Mode.\r\n");
		if (TRUE) {
			fprintf(stderr, "xPON_Log: xPON configs are changed, clean up old xpon config.\r\n");
			if (system("/bin/sh /etc/clean_up_xpon_intf.sh") < 0) {
			
				fprintf(stderr, "xPON_Log: clean_up_xpon error.\r\n");
			}
			config_init_xpon();
		}
	} else {
		// WAN MODE
		log_message(L_NOTICE,"xPON_Log: AE WAN Mode.\r\n");
		config_init_aewan();
	}
	fprintf(stderr, "xPON_Log:=======================================\r\n");



	if (stat(INTF_STATE_FLAG, &file_buf) != 0) {
		bridge_if_stale_ipv6_remove();
	} else {
		log_message(L_NOTICE, "xPON_Log - LAN: seems a cli network restart, remove the flag file %s.\r\n", INTF_STATE_FLAG);
		if (remove(INTF_STATE_FLAG) != 0) {
			log_message(L_CRIT,"xPON_Log: error when remove file [%s].\r\n", XPON_STATE_FLAG);
		}
	}

	config_init_brlan();

	return 0;
}
