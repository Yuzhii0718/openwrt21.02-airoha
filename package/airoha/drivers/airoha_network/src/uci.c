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
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
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
#include <syslog.h>

#include "airoha_network.h"

#define DEFAULT_LOG_LEVEL L_NOTICE

static int log_level = DEFAULT_LOG_LEVEL;
static const int log_class[] = {
	[L_CRIT] = LOG_CRIT,
	[L_WARNING] = LOG_WARNING,
	[L_NOTICE] = LOG_NOTICE,
	[L_INFO] = LOG_INFO,
	[L_DEBUG] = LOG_DEBUG
};

#ifdef DUMMY_MODE
#define use_syslog false
#else
static bool use_syslog = true;
#endif


void
log_message(int priority, const char *format, ...)
{
	va_list vl;

	if (priority > log_level)
		return;

	va_start(vl, format);
	if (use_syslog)
		vsyslog(log_class[priority], format, vl);
	else
		vfprintf(stderr, format, vl);
	va_end(vl);
}



void airoha_shell_execute(char *cmd, uint32_t cmd_len)
{
    log_message(L_NOTICE, "xPON_Log: Command is %s.\r\n", cmd);
    if (system(cmd) < 0) {
        log_message(L_NOTICE, "xPON_Log: Command execution error. \r\n");
	}
    memset(cmd, 0, cmd_len);
}

int airoha_get_section_value(char *section_name, struct uci_section **p_section_obj)
{
    struct uci_section *section_obj = uci_lookup_section(uci_pon_ctx, uci_pon, PON_AUTH_SECTION_NAME);

    if (section_obj == NULL) {
        log_message(L_NOTICE, "airoha_get_section_value[ERROR]: %s section is not found.\r\n", section_name);
        *p_section_obj = NULL;
        return STATUS_NOT_FOUND;
    }
    log_message(L_NOTICE, "airoha_get_section_value: %s section is present.\r\n", section_name);
    *p_section_obj = section_obj;

    return STATUS_OK;
}

int airoha_get_option_value(struct uci_section *section, const char *option_name, const char **p_option_value)
{
    const char *value = uci_lookup_option_string(uci_pon_ctx, section, option_name);
    
    log_message(L_NOTICE, "airoha_get_option_value: read %s from config file as [%s].\r\n", option_name, value);

    if (value == NULL) {
        log_message(L_NOTICE, "airoha_get_option_value: %s value is not present.\r\n", option_name);

        *p_option_value = NULL;
        return STATUS_NOT_FOUND;
    }
    log_message(L_NOTICE,  "airoha_get_option_value - We arrive here[%s].\r\n", option_name, *p_option_value);

    *p_option_value = value;
    log_message(L_NOTICE, "airoha_get_option_value: value of %s is [%s].\r\n", option_name, *p_option_value);

    return STATUS_OK;
}

int airoha_gpon_active(int type, const char *username, const char *password, const char *sn_auth_type)
{
    char cmd[CMD_BUF_MAX_LEN] = {0};
    
	log_message(L_NOTICE, "airoha_gpon_active: SN: auth_username is [%s].\r\n", username);
	
	log_message(L_NOTICE,"airoha_gpon_active: SN: sn_auth_type is [%s], password is [%s].\r\n", sn_auth_type,password);

    //snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set linkMode %d", LINK_MODE_GPON);
    //airoha_shell_execute(cmd, sizeof(cmd));

    if (type == TYPE_SN) {
        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set sn %s", username);
        airoha_shell_execute(cmd, sizeof(cmd));
		
		if (strcmp(sn_auth_type, "hex") == 0) {
			snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set passwdHex %s", password);
        	airoha_shell_execute(cmd, sizeof(cmd));
		}
		else if (strcmp(sn_auth_type, "ascii") == 0) {
			snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set passwdAscii %s", password);
			airoha_shell_execute(cmd, sizeof(cmd));
		}
    } else if (type == TYPE_LOID) {
		log_message(L_NOTICE, "xPON_Log - GPON_Log: /userfs/bin/omcicfgCmd set sn %s.\r\n", sn_auth_type);
        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set sn %s", sn_auth_type);
        airoha_shell_execute(cmd, sizeof(cmd));

        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set loid %s", username);
        airoha_shell_execute(cmd, sizeof(cmd));

        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omcicfgCmd set loidPasswd %s", password);
        airoha_shell_execute(cmd, sizeof(cmd));
    } else {
        log_message(L_NOTICE, "airoha_gpon_active - GPON_Log: unknown authentication type [%d].\r\n", type);
        return STATUS_FAIL;
    }

	if (access("/tmp/load_process", 0) != 0) {
		snprintf(cmd, CMD_BUF_MAX_LEN - 1, "killall omci");
		airoha_shell_execute(cmd, sizeof(cmd));
		
		snprintf(cmd, CMD_BUF_MAX_LEN - 1, "killall ponmgr_cfg");
		airoha_shell_execute(cmd, sizeof(cmd));
		
		if ((access("/proc/tc3162/nopon", F_OK) != 0)&&(access("/proc/tc3162/wan_2_5", F_OK) != 0))	{	
			snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/ponmgr_cfg &");
			airoha_shell_execute(cmd, sizeof(cmd));
		
			snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omci &");
			airoha_shell_execute(cmd, sizeof(cmd));

			snprintf(cmd, CMD_BUF_MAX_LEN - 1, "touch /tmp/load_process");
			airoha_shell_execute(cmd, sizeof(cmd));
		}
	} else {
		snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/omci set reconfig");
		airoha_shell_execute(cmd, sizeof(cmd));	
	}

    return STATUS_OK;

}

int airoha_epon_active(int type, const char *username, const char *password)
{
    char cmd[CMD_BUF_MAX_LEN] = {0};
    
    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/oamcfgCmd set mode %d", LINK_MODE_EPON);
    airoha_shell_execute(cmd, sizeof(cmd));

    if (type == TYPE_LOID) {
        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/oamcfgCmd set loid0 %s", username);
        airoha_shell_execute(cmd, sizeof(cmd));

        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/oamcfgCmd set loidPasswd0 %s", password);
        airoha_shell_execute(cmd, sizeof(cmd));
    } else {
        log_message(L_NOTICE, "airoha_epon_active - EPON_Log: unknown authentication type [%d].\r\n", type);
        return STATUS_FAIL;
    }

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "killall epon_oam");
    airoha_shell_execute(cmd, sizeof(cmd));

	snprintf(cmd, CMD_BUF_MAX_LEN - 1, "rm -f /tmp/epon_oam.pid");
	airoha_shell_execute(cmd, sizeof(cmd));
	sleep(1);

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "killall ponmgr_cfg");
    airoha_shell_execute(cmd, sizeof(cmd));
        
    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/ponmgr_cfg &");
    airoha_shell_execute(cmd, sizeof(cmd));
    
    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "/userfs/bin/epon_oam &");
    airoha_shell_execute(cmd, sizeof(cmd));

    return STATUS_OK;
}

int airoha_vlan_active(const char *mode, const char *vlan_id, const char *payload)
{
    char cmd[CMD_BUF_MAX_LEN] = {0};

    log_message(L_NOTICE,"airoha_vlan_active - LAN: %s vlan id is %s\n", mode, vlan_id);

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "brctl delif br-lan %s.%s", mode, vlan_id);
    airoha_shell_execute(cmd, sizeof(cmd));

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "ifconfig %s up", mode);
    airoha_shell_execute(cmd, sizeof(cmd));

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "vconfig add %s %s", mode, vlan_id);
    airoha_shell_execute(cmd, sizeof(cmd));

    snprintf(cmd, CMD_BUF_MAX_LEN - 1, "ifconfig %s.%s up", mode, vlan_id);
    airoha_shell_execute(cmd, sizeof(cmd));

    if ((payload != NULL) && (strncmp(payload, "bridged", strlen("bridged")) == 0)) {
        snprintf(cmd, CMD_BUF_MAX_LEN - 1, "brctl addif br-lan %s.%s", mode, vlan_id);
        airoha_shell_execute(cmd, sizeof(cmd));
		snprintf(cmd, CMD_BUF_MAX_LEN - 1, "echo 2 > /sys/class/net/br-lan/brif/%s.%s/multicast_router", mode, vlan_id);
        airoha_shell_execute(cmd, sizeof(cmd));
		snprintf(cmd, CMD_BUF_MAX_LEN - 1, "brctl quickleave br-lan on");
        airoha_shell_execute(cmd, sizeof(cmd));
    }

    return STATUS_OK;
}

int airoha_config_is_modified(const char *mode, const char *type, const char *name, const char *pswd, const char *auth_type)
{
    if (pswd == NULL) {
        if (prev_pswd != NULL && strlen(prev_pswd) != 0) {
			log_message(L_CRIT, "airoha_config_is_modified - Passwd Changed.\r\n");
            return TRUE;
        }
    }
    if (mode == NULL || type == NULL || name == NULL) {
        log_message(L_CRIT, "airoha_config_is_modified - Invalid check parameter [%s] [%s] [%s] [%s].\r\n", mode, type, 
                                   name, pswd);
        return FALSE;
    }
    log_message(L_CRIT, "airoha_config_is_modified - Previous Parameters [%s] [%s] [%s] [%s] [%s].\r\n", prev_mode, prev_type, 
                                   prev_name, prev_pswd, prev_sn_auth);
    log_message(L_CRIT, "airoha_config_is_modified - Current Parameters [%s] [%s] [%s] [%s] [%s].\r\n", mode, type, 
                                   name, pswd, auth_type);
    if ((strcmp(mode, prev_mode) == 0) && (strcmp(type, prev_type) == 0) && 
        (strcmp(name, prev_name) == 0) && (strcmp(auth_type, prev_sn_auth)) == 0) {
        if (pswd != NULL) {
    		if (prev_pswd == NULL || strlen(prev_pswd) == 0) {
                log_message(L_CRIT, "airoha_config_is_modified - Passwd Changed.\r\n");
    			return TRUE;
    		} else if (strcmp(pswd, prev_pswd) == 0) {
    		    log_message(L_CRIT, "airoha_config_is_modified - Parameters NOT Changed.\r\n");
    			return FALSE;
    		} else {
    		    log_message(L_CRIT, "airoha_config_is_modified - Passwd Changed.\r\n");
    			return TRUE;
    		}
        } else {
            return FALSE;
        }
    }
    log_message(L_CRIT, "xPON_Log - Parameter Changed.\r\n");
    return TRUE;
}

void airoha_config_backup(const char *mode, const char *type, const char *name, const char *pswd, const char *auth_type)
{
    log_message(L_NOTICE, "airoha_config_backup - do backup [%s] [%s] [%s] [%s].\r\n", mode, type, 
                                     name, pswd);
    if (mode == NULL || type == NULL || name == NULL) {
        log_message(L_CRIT, "airoha_config_backup - Invalid backup parameter.\r\n");
        return ;
    }
    strncpy(prev_mode, mode, MSG_BUF_MAX_LEN - 1);
    strncpy(prev_type, type, MSG_BUF_MAX_LEN - 1);
    strncpy(prev_name, name, MSG_BUF_MAX_LEN - 1);
	if (auth_type == NULL) {
        strncpy(prev_sn_auth, "ascii", MSG_BUF_MAX_LEN - 1);
    } else {
        strncpy(prev_sn_auth, auth_type, MSG_BUF_MAX_LEN - 1);
    }
    if (pswd == NULL) {
        memset(prev_pswd, 0, MSG_BUF_MAX_LEN);
    } else {
        strncpy(prev_pswd, pswd, MSG_BUF_MAX_LEN - 1);
    }
    
    log_message(L_NOTICE, "airoha_config_backup - Prev config saved [%s] [%s] [%s] [%s].\r\n", prev_mode, prev_type, 
                                 prev_name, prev_pswd);
    
}

int config_init_xpon(void)
{
    const char *pon_mode;
    const char *auth_type_name, *auth_type, *auth_password, *auth_username;
	const char *vlan_id, *payload;
	const char *sn_auth_type;
    struct uci_section *pon_auth;
	struct uci_element *element;
    log_message(L_NOTICE, "config_init_xpon: enter.\r\n");
	fprintf(stderr, "config_init_xpon: enter\r\n");

    if (airoha_get_section_value(PON_AUTH_SECTION_NAME, &pon_auth) != STATUS_OK) {
        return STATUS_FAIL;
    }
	
    log_message(L_NOTICE, "config_init_xpon: 1111111 .\r\n", pon_mode);
    if (airoha_get_option_value(pon_auth, "pon_mode", &pon_mode) != STATUS_OK) {
        log_message(L_NOTICE, "config_init_xpon: Failed to get pon mode.\r\n");
        return STATUS_FAIL;
    } 
	
    log_message(L_NOTICE, "config_init_xpon: 222222 .\r\n", pon_mode);
    if (strcmp(pon_mode, "GPON") == 0) {
        auth_type_name = TYPE_GPON_AUTH;
    } else if (strcmp(pon_mode, "EPON") == 0) {
        auth_type_name = TYPE_EPON_AUTH;
    } else {
        log_message(L_NOTICE,"config_init_xpon: unknown pon mode [%s].\r\n", pon_mode);
        return STATUS_FAIL;
    }
    log_message(L_NOTICE, "config_init_xpon: pon mode is [%s].\r\n", pon_mode);

    if (airoha_get_option_value(pon_auth, auth_type_name, &auth_type) != STATUS_OK) {
        log_message(L_NOTICE, "config_init_xpon: xpon_auth_type value is not present, set to default value SN.\r\n");
        auth_type = AUTH_TYPE_DEFAULT;
    }

    if (strncmp(auth_type, "sn", 2) == 0) {
        log_message(L_NOTICE, "config_init_xpon: SN authentication.\r\n");
        
        if (airoha_get_option_value(pon_auth, "sn", &auth_username) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: auth_username is not present, set to default value ECONET.\r\n");
            auth_username = AUTH_NAME_DEFAULT;
        }
		
        if (airoha_get_option_value(pon_auth, "xpon_sn_auth_type", &sn_auth_type) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: auth_password is not present, set to default value ECONET.\r\n");
            sn_auth_type = AUTH_PWD_TYPE_DEFAULT;
        }
		if (strcmp(sn_auth_type, "hex") == 0) {
			if (airoha_get_option_value(pon_auth, "sn_hex_password", &auth_password) != STATUS_OK) {
           	 	log_message(L_NOTICE, "config_init_xpon: auth_password is not present, set to default value ECONET.\r\n");
            	auth_password = AUTH_PASSWORD_DEFAULT;
        	}
		}else if (strcmp(sn_auth_type, "ascii") == 0) {
			if (airoha_get_option_value(pon_auth, "sn_ascii_password", &auth_password) != STATUS_OK) {
           	 	log_message(L_NOTICE, "config_init_xpon: auth_password is not present, set to default value ECONET.\r\n");
            	auth_password = AUTH_PASSWORD_DEFAULT;
        	}
		}
		else	
			auth_password = AUTH_PASSWORD_DEFAULT;
        log_message(L_NOTICE, "config_init_xpon: SN: auth_username is [%s].\r\n", auth_username);
		
        log_message(L_NOTICE,"config_init_xpon: SN: sn_auth_type is [%s], password is [%s].\r\n", sn_auth_type,auth_password);
        if (strcmp(pon_mode, "GPON") == 0) {
            if (airoha_config_is_modified(pon_mode, auth_type, auth_username, auth_password, sn_auth_type) == TRUE) {
				
				log_message(L_NOTICE, "config_init_xpon: SN: ssssss  is [%s].\r\n", auth_username);
                airoha_gpon_active(TYPE_SN, auth_username, auth_password, sn_auth_type);
                airoha_config_backup(pon_mode, auth_type, auth_username, auth_password,sn_auth_type);
            } else {
                log_message(L_NOTICE, "config_init_xpon: config not modified.\r\n");
            }
        } else {
            log_message(L_NOTICE, "config_init_xpon: only GPON supports SN authentication.\r\n");
            return STATUS_FAIL;
        }
        
    } else if (strncmp(auth_type, "LOID", 4) == 0) {
        log_message(L_NOTICE, "config_init_xpon: LOID authentication.\r\n");
        
        if (airoha_get_option_value(pon_auth, "loid", &auth_username) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: auth_username is not present, set to default value ECONET.\r\n");
            auth_username = AUTH_NAME_DEFAULT;
            
        }
		if (airoha_get_option_value(pon_auth, "def_sn", &sn_auth_type) != STATUS_OK) {
            log_message(L_NOTICE, "xPON_Log: get default pro sn.\r\n");
            sn_auth_type = AUTH_PWD_TYPE_DEFAULT;
        }
	/*	if (airoha_get_option_value(pon_auth, "xpon_sn_auth_type", &sn_auth_type) != STATUS_OK) {
            log_message(L_NOTICE, "xPON_Log: auth_password is not present, set to default value ECONET.\r\n");
            sn_auth_type = AUTH_PWD_TYPE_DEFAULT;
        }*/
        if (airoha_get_option_value(pon_auth, "loid_password", &auth_password) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: auth_password is not present, set to default value ECONET.\r\n");
            auth_password = AUTH_PASSWORD_DEFAULT;
        }
        log_message(L_NOTICE,"config_init_xpon: LOID: username is [%s], password is [%s].\r\n", auth_username, auth_password);

        if(strcmp(pon_mode, "GPON") == 0) {
            if (airoha_config_is_modified(pon_mode, auth_type, auth_username, auth_password, sn_auth_type) == TRUE) {
                airoha_gpon_active(TYPE_LOID, auth_username, auth_password, sn_auth_type);
                airoha_config_backup(pon_mode, auth_type, auth_username, auth_password, sn_auth_type);
            } else {
                log_message(L_NOTICE, "config_init_xpon: config not modified.\r\n");
            }
        }
        else {
            if (airoha_config_is_modified(pon_mode, auth_type, auth_username, auth_password, sn_auth_type) == TRUE) {
                airoha_epon_active(TYPE_LOID, auth_username, auth_password);
                airoha_config_backup(pon_mode, auth_type, auth_username, auth_password, NULL);
            } else {
                log_message(L_NOTICE, "config_init_xpon: config not modified.\r\n");
            }
        }
    }
    /* Check whether GPON is up or not */
    uci_foreach_element(&uci_pon->sections, element) {
        struct uci_section *section = uci_to_section(element);
        if (strncmp(section->type, WAN_VLAN_SECTION_NAME,strlen(WAN_VLAN_SECTION_NAME)) != 0) {
            continue;
        }
        if (airoha_get_option_value(section, "vlan_id", &vlan_id) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: vlan_id is not present, set to default value 10.\r\n");
            vlan_id = "10";
        }
        if (airoha_get_option_value(section, "payload", &payload) != STATUS_OK) {
            log_message(L_NOTICE, "config_init_xpon: payload is not present, set to default value routed.\r\n");
            payload = "routed";
        }

        airoha_vlan_active("pon", vlan_id, payload);
    }
	return STATUS_OK;
}

int config_init_aewan(void)
{
	const char *vlan_id, *payload;
	struct uci_element *element;

	uci_foreach_element(&uci_pon->sections, element) {
		struct uci_section *section = uci_to_section(element);
		if (strncmp(section->type, WAN_VLAN_SECTION_NAME,strlen(WAN_VLAN_SECTION_NAME)) != 0) {
			continue;
		}
		if (airoha_get_option_value(section, "vlan_id", &vlan_id) != STATUS_OK) {
			log_message(L_NOTICE, "config_init_aewan: vlan_id is not present, set to default value 10.\r\n");
			vlan_id = "10";
		}
		if (airoha_get_option_value(section, "payload", &payload) != STATUS_OK) {
			log_message(L_NOTICE, "config_init_aewan: payload is not present, set to default value routed.\r\n");
			payload = "routed";
		}

		airoha_vlan_active("ae_wan", vlan_id, payload);
	}
	return STATUS_OK;
}

