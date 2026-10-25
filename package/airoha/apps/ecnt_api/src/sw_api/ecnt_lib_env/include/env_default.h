/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <env_callback.h>

#ifdef DEFAULT_ENV_INSTANCE_EMBEDDED
env_t environment __PPCENV__ = {
	ENV_CRC,	/* CRC Sum */
#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
	1,		/* Flags: valid */
#endif
	{
#elif defined(DEFAULT_ENV_INSTANCE_STATIC)
static char default_environment[] = {
#else
const uchar default_environment[] = {
#endif
#ifdef	CONFIG_ENV_CALLBACK_LIST_DEFAULT
	ENV_CALLBACK_VAR "=" CONFIG_ENV_CALLBACK_LIST_DEFAULT "\0"
#endif
#ifdef	CONFIG_ENV_FLAGS_LIST_DEFAULT
	ENV_FLAGS_VAR "=" CONFIG_ENV_FLAGS_LIST_DEFAULT "\0"
#endif
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	__stringify(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	__stringify(CONFIG_BAUDRATE)	"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	__stringify(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	__stringify(CONFIG_ETHADDR)	"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	__stringify(CONFIG_ETH1ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	__stringify(CONFIG_ETH2ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	__stringify(CONFIG_ETH3ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	__stringify(CONFIG_ETH4ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	__stringify(CONFIG_ETH5ADDR)	"\0"
#endif
#ifdef	CONFIG_ETHPRIME
	"ethprime="	CONFIG_ETHPRIME			"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	__stringify(CONFIG_IPADDR)	"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	__stringify(CONFIG_SERVERIP)	"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD		"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	CONFIG_ROOTPATH			"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	__stringify(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	__stringify(CONFIG_NETMASK)	"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	__stringify(CONFIG_HOSTNAME)	"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	CONFIG_BOOTFILE			"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	__stringify(CONFIG_LOADADDR)	"\0"
#endif
#ifdef	CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	__stringify(CONFIG_PCI_BOOTDELAY)"\0"
#endif
#ifdef	CONFIG_ENV_VARS_UBOOT_CONFIG
	"arch="		CONFIG_SYS_ARCH			"\0"
	"cpu="		CONFIG_SYS_CPU			"\0"
	"board="	CONFIG_SYS_BOARD		"\0"
	"board_name="	CONFIG_SYS_BOARD		"\0"
#ifdef CONFIG_SYS_VENDOR
	"vendor="	CONFIG_SYS_VENDOR		"\0"
#endif
#ifdef CONFIG_SYS_SOC
	"soc="		CONFIG_SYS_SOC			"\0"
#endif
#endif
#ifdef CONFIG_ENV_ROOT
	"root=" CONFIG_ENV_ROOT						"\0"
#endif
#ifdef CONFIG_ENV_CONSOLE
	"console=" CONFIG_ENV_CONSOLE				"\0"
#endif
#ifdef CONFIG_ENV_SDRAM_CONF
	"sdram_conf="	CONFIG_ENV_SDRAM_CONF		"\0"
#endif
#ifdef CONFIG_ENV_VENDOR_NAME
	"vendor_name="	CONFIG_ENV_VENDOR_NAME		"\0"
#endif
#ifdef CONFIG_ENV_PRODUCT_NAME
	"product_name=" CONFIG_ENV_PRODUCT_NAME		"\0"
#endif
#ifdef CONFIG_ENV_SNMP_SYSOBJID
	"snmp_sysobjid=" CONFIG_ENV_SNMP_SYSOBJID	"\0"
#endif
#ifdef CONFIG_ENV_COUNTRY_CODE
	"country_code=" CONFIG_ENV_COUNTRY_CODE		"\0"
#endif
#ifdef CONFIG_ENV_ETHER_GPIO
	"ether_gpio=" CONFIG_ENV_ETHER_GPIO			"\0"
#endif
#ifdef CONFIG_ENV_POWER_GPIO
	"power_gpio=" CONFIG_ENV_POWER_GPIO			"\0"
#endif
#ifdef CONFIG_ENV_USERNAME
	"username=" CONFIG_ENV_USERNAME				"\0"
#endif
#ifdef CONFIG_ENV_PASSWORD
	"password=" CONFIG_ENV_PASSWORD				"\0"
#endif
#ifdef CONFIG_ENV_DSL_GPIO
	"dsl_gpio=" CONFIG_ENV_DSL_GPIO				"\0"
#endif
#ifdef CONFIG_ENV_INTERNET_GPIO
	"internet_gpio=" CONFIG_ENV_INTERNET_GPIO			"\0"
#endif
#ifdef CONFIG_ENV_MULTI_UPGRADE_GPIO
	"multi_upgrade_gpio=" CONFIG_ENV_MULTI_UPGRADE_GPIO	"\0"
#endif
#ifdef CONFIG_ENV_ONU_TYPE
	"onu_type=" CONFIG_ENV_ONU_TYPE						"\0"
#endif
#ifdef CONFIG_ENV_QDMA_INIT
	"qdma_init=" CONFIG_ENV_QDMA_INIT					"\0"
#endif
#ifdef CONFIG_ENV_ARHT_LOW_POWER
       "arht_low_power=" CONFIG_ENV_ARHT_LOW_POWER	"\0"
#endif

#ifdef	CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS
#endif
	"\0"
#ifdef DEFAULT_ENV_INSTANCE_EMBEDDED
	}
#endif
};
