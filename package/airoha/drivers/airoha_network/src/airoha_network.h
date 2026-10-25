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



#define CMD_BUF_MAX_LEN 256
#define MAX_NUM_VLANID 8
#define MSG_BUF_MAX_LEN 64
#define STATUS_OK 0
#define STATUS_FAIL -1
#define STATUS_NOT_FOUND -2
#define DSL_ENABLED 1
#define DSL_DISABLED 0
#define ADSL 1
#define VDSL 2

#define airoha_FUNCTION

#define VENDOR_ID_MAX_LEN 5
#define VSSN_MAX_LEN 10
#define MAX_NUM_INTF 16
#define AUTH_TYPE_DEFAULT "sn"
#define AUTH_NAME_DEFAULT "econet"
#define AUTH_PASSWORD_DEFAULT "econet"
#define AUTH_PWD_TYPE_DEFAULT "ascii"

#define PON_AUTH_SECTION_NAME "xpon_auth"
#define TYPE_GPON_AUTH "auth_type_g"
#define TYPE_EPON_AUTH "auth_type_e"

#define XPON_STATE_FLAG "/tmp/.xponconfig"
#define WAN_MODE_FLAG "/proc/tc3162/wan_2_5"
#define LINK_MODE_GPON 1
#define LINK_MODE_EPON 2

#define TYPE_SN 1
#define TYPE_LOID 2

#define VLAN_SECTION_NAME "lan"
#define WAN_VLAN_SECTION_NAME "wan_vlan"
#define INTF_STATE_FLAG "/tmp/.intf_refresh"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

enum {
	L_CRIT,
	L_WARNING,
	L_NOTICE,
	L_INFO,
	L_DEBUG
};
#define STATUS_OK 0
#define STATUS_FAIL -1
#define STATUS_NOT_FOUND -2

static char prev_mode[MSG_BUF_MAX_LEN];
static char prev_type[MSG_BUF_MAX_LEN];
static char prev_name[MSG_BUF_MAX_LEN];
static char prev_pswd[MSG_BUF_MAX_LEN];
static char prev_sn_auth[MSG_BUF_MAX_LEN];
static char prev_ipv6_pd[MAX_NUM_INTF][MSG_BUF_MAX_LEN] = {0};

void airoha_shell_execute(char *cmd, uint32_t cmd_len);
int airoha_get_section_value(char *section_name, struct uci_section **p_section_obj);
int airoha_get_option_value(struct uci_section *section, const char *option_name, const char **p_option_value);
int airoha_gpon_active(int type, const char *username, const char *password, const char *sn_auth_type);
int airoha_epon_active(int type, const char *username, const char *password);
int airoha_vlan_active(const char *mode, const char *vlan_id, const char *payload);
int airoha_aewan_vlan_active(const char *vlan_id, const char *payload);
int airoha_config_is_modified(const char *mode, const char *type, const char *name, const char *pswd, const char *auth_type);
void airoha_config_backup(const char *mode, const char *type, const char *name, const char *pswd, const char *auth_type);
int config_init_xpon(void);
int config_init_aewan(void);
static void bridge_if_stale_ipv6_remove(void);
int config_init_brlan(void);

void log_message(int priority, const char *format, ...);


extern struct uci_context *uci_pon_ctx;
extern struct uci_package *uci_pon;


