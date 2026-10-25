#include "eth_lan.h"
#include <ecnt_hook/ecnt_hook_1588.h>
#include "./tcphy/mtkswitch_api.h"
#include <linux/ethtool.h>
#include "ethtool_support.h"

extern int ecnt_set_ethtool_ops(struct net_device *netdev, struct ethtool_ts_info *info);

int getPortAutonegState(struct ethtool_link_ksettings *cmd, u8 portId)
{
	/*  Return values from macMT7530GetPortAutoNegState
		-1: invalid port
		1: on
		0: off */
	int ret=0;
	
	ret= macMT7530GetPortAutoNegState(portId);
	if (ret ==-1) /* if auto neg state fails, the autoneg will not be reported*/
	{
		return ret;
	}
	cmd->base.autoneg=ret;
	return ret;
}	
	
void getSupportedLinkMode(const struct ethtool_link_ksettings *cmd)
{
	u32 supported=0;
	/* currently these values are fixed to report to ethtool interface */
	supported = (SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full | 
				SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full |
				SUPPORTED_1000baseT_Full | SUPPORTED_Autoneg);
				
	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.supported,supported);
	return;
}

/* Reference values for getPortAdvertiseLinkMode()
	autodetect: argument for macMT7530SetAutoDetection() and macMT7530GetAutoDetection()
			values described at tclinux_phoenix\modules\private\ether\en7512\tcphy\mtkswitch_api_krl :line no 172
			
			    *Speed*    *Duplex*
**        0x00 - Auto;      Auto
**        0x01 - 10 only;   Full duplex only
**        0x02 - 100 only;  Full duplex only
**        0x03 - 1000 only; Full duplex only
**        0x04 - Auto;      Full duplex only
**        0x10 - 10 only;   Auto
**        0x11 - 10 only;   Half duplex only
**        0x12 - 100 only;  Half duplex only
**        0x13 - 1000 only; Half duplex only
**        0x14 - Auto;      Half duplex only
**        0x20 - 1000 only; Auto
**        0x30 - 100 only;  Auto
			
*/
struct ethtool_table1{ 
	u8 autodetect;
	u32 advertising;
};

/*this table is used by getPortAdvertiseLinkMode() and setPortAdvertiseLinkMode()*/
struct ethtool_table1 autodetect_to_advertising[]={
	{0x00, (ADVERTISED_10baseT_Full | ADVERTISED_10baseT_Half | ADVERTISED_100baseT_Full | ADVERTISED_100baseT_Half |
	ADVERTISED_1000baseT_Full)},
	{0x01, (ADVERTISED_10baseT_Full )},
	{0x02, (ADVERTISED_100baseT_Full )},
	{0x03, (ADVERTISED_1000baseT_Full )},
	{0x04,	(ADVERTISED_10baseT_Full | ADVERTISED_100baseT_Full |
	ADVERTISED_1000baseT_Full)},
	{0x10,	(ADVERTISED_10baseT_Full | ADVERTISED_10baseT_Half )},
	{0x11,	(ADVERTISED_10baseT_Half )},
	{0x12,	(ADVERTISED_100baseT_Half )},
	{0x13,	(ADVERTISED_1000baseT_Half )},
	{0x14,	(ADVERTISED_10baseT_Half | ADVERTISED_100baseT_Half |
	ADVERTISED_1000baseT_Half )},
	{0x20,	(ADVERTISED_1000baseT_Full | ADVERTISED_1000baseT_Half )},
	{0x30, (ADVERTISED_100baseT_Full | ADVERTISED_100baseT_Half )}
};
#define ethtool_auto_to_advert_entries (sizeof(autodetect_to_advertising)/sizeof(autodetect_to_advertising[0]))


void getPortAdvertiseLinkMode(struct ethtool_link_ksettings *cmd, u8 portId)
{
	u32 advertised=0;	
	u8 autodetect =0;
	int i;
	
	macMT7530GetAutoDetection(&autodetect, portId);
	/*if (ret==-1)
	{
		// if port is up somehow, and at the boot time, the switch is returning -1, still is advertising all mode
		// check by connecting another board and checking their link partner modes 
		
			return;
	}*/
	
	for (i=0; i<ethtool_auto_to_advert_entries; i++)
	{
		if (autodetect_to_advertising[i].autodetect==autodetect)
		{
			advertised = autodetect_to_advertising[i].advertising;
			break;
		}
	}
	
	if (i==ethtool_auto_to_advert_entries)
	{
		advertised = autodetect_to_advertising[0].advertising;
		/* default to 0x00 all, other wise, after boot, switch return is not from table*/
	}
	
	if (getPortAutonegState(cmd,portId)==1)
	{
		advertised |= ADVERTISED_Autoneg;
	}
	/*
	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising,advertised);
	*/
	return;
}

/*
	This table is to be used by getSpeedDuplex()
	values reffered from tclinux_phoenix\modules\private\ether\en7512\tcphy\mtkswitch_api_krl 
						: macMT7530GetPortLinkState()
	
	*speedmode*			*speed* 		*duplex*		
	1				1000M			FULL
	6				1000M			HALF		// in this mode speed and duplex can't be read
	2				100M			FULL
	3				100M			HALF
	4				10M				FULL
	5				10M				HALF
*/
struct ethtool_table2{
	int speedmode;
	u32 speed;
	u32 duplex;
};

struct ethtool_table2 speed_duplex[]={
	{1,	(SPEED_1000),	(DUPLEX_FULL)},
	{2,	(SPEED_100),	(DUPLEX_FULL)},
	{3,	(SPEED_100),	(DUPLEX_HALF)},
	{4,	(SPEED_10),		(DUPLEX_FULL)},
	{5,	(SPEED_10),		(DUPLEX_HALF)}
	/*{6,	(SPEED_1000),	(DUPLEX_HALF)}*/ 
};
#define ethtool_speed_duplex_entries (sizeof(speed_duplex)/sizeof(speed_duplex[0]))

u8 getSpeedDuplex(struct ethtool_link_ksettings *cmd, u8 portId)
{
	u8 linkstate=0, speedmode=0;
	int ret=0;
	int i;
	
	ret= macMT7530GetPortLinkState(portId, &linkstate, &speedmode);
	if(ret ==-1)
	{
		return 0;
	}
	
	for (i=0; i<ethtool_speed_duplex_entries; i++)
	{
		if (speed_duplex[i].speedmode==speedmode)
		{
			cmd->base.speed= speed_duplex[i].speed;
			cmd->base.duplex= speed_duplex[i].duplex;
			break;
		}
	}
	
	if (i==ethtool_speed_duplex_entries)
	{
		cmd->base.speed=SPEED_UNKNOWN;
		cmd->base.duplex= DUPLEX_UNKNOWN;
		return 0;
	}
	return speedmode;
}


void getPortAddress(struct ethtool_link_ksettings *cmd, u8 portId)
{
	int ret= 0;
	u32 extsw_flag = 0;
/*	
	#define EXT_SWITCH_FLAG 0x80  //defined in \modules\private\ether\en7512\tcphy\mtkswitch_api_krl
	#define SWITCH_PORT_MASK 0x7f //defined in \modules\private\ether\en7512\tcphy\mtkswitch_api_krl
*/
	if(portId & 0x80)
		extsw_flag = 1;
	portId = portId & 0x7f;
	
	ret= macMT7530GetPortPhyAddr(portId,extsw_flag);
	if(ret==-1)
	{
		return; /* phy address would not be reported */
	}
	cmd->base.phy_address=ret;
}


static u32 getPortLinkUpDown(struct net_device *ndev)
{
/***************************1 Linkstate get ******************************/	
	u8 portId =getSwitchLANIndex(ndev)+1;
	return macMT7530GetPortUpDownState(portId);
}

static int airoha_get_link_ksettings(struct net_device *ndev,
				  struct ethtool_link_ksettings *cmd)
{
	u8 portId =0;
	u8 speedmode=0;
	u32 advertised=0;

	portId= (u8)getSwitchLANIndex(ndev)+1; /* get port id(x) from eth0.x */

/***************************2 Linkmode get ******************************/	
	/*get autonegotiation*/
	getPortAutonegState(cmd,portId);
	
	/* send supported link mode to ethtool interface*/
	getSupportedLinkMode(cmd); 

	/* get port address from the switch */
	getPortAddress(cmd,portId);

	/* get speed and duplex mode */
	speedmode= getSpeedDuplex(cmd,portId);
	

	if (macMT7530GetPortUpDownState(portId))
	{
		if (speedmode == 1 || macMT7530GetPortAutoNegState(portId))/* Auto Mode*/
		{
			advertised = (ADVERTISED_10baseT_Full | ADVERTISED_10baseT_Half | ADVERTISED_100baseT_Full | ADVERTISED_100baseT_Half |
		ADVERTISED_1000baseT_Full | ADVERTISED_Autoneg);
			cmd->base.autoneg=1;
		}
		else if (speedmode ==2)
		{
			advertised = (ADVERTISED_100baseT_Full);
		}
		else if (speedmode == 3)
		{
			advertised = (ADVERTISED_100baseT_Half);
		}
		else if (speedmode == 4)
		{
			advertised = (ADVERTISED_10baseT_Full);
		}
		else if (speedmode == 5)
		{
			advertised = (ADVERTISED_10baseT_Half);
		}
		else 
		{
			return 0;
		}
		ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising,advertised);
		/* advertise link mode */
		/*getPortAdvertiseLinkMode(cmd,portId);	*/
	}

	return 0;
}
void setPortAutonegState(const struct ethtool_link_ksettings *cmd, u8 portId)
{
	int ret=0;
	u8 enable=(u8)cmd->base.autoneg;
	ret=macMT7530PortAutoNegActive(portId, enable);
}

int setPortAdvertiseLinkMode(const struct ethtool_link_ksettings *cmd, u8 portId)
{
	u32 advertised=0;
	u8 autodetect_set=0;
	int i=0;
	u8 mode=0;
	/* mask out unsupported modes*/
	getSupportedLinkMode(cmd);
	linkmode_and(cmd->link_modes.advertising, cmd->link_modes.advertising, cmd->link_modes.supported);
	ethtool_convert_link_mode_to_legacy_u32(&advertised, cmd->link_modes.advertising);
/*
	for (i=0; i<ethtool_auto_to_advert_entries; i++)
	{
		if (autodetect_to_advertising[i].advertising==advertised)
		{
			autodetect_set = autodetect_to_advertising[i].autodetect;
			break;
		}
	}
	
	if (i==ethtool_auto_to_advert_entries)
	{
		return -1; 
	}
	printk("\n advertising and autodetect_set(for switch) values decided as : %u and %u \n",advertised, autodetect_set);
	macMT7530SetAutoDetection(autodetect_set,portId);
*/
	if ((advertised & (ADVERTISED_1000baseT_Full | ADVERTISED_1000baseT_Half))!=0  || (advertised & ADVERTISED_100baseT_Full)!=0 || (advertised & ADVERTISED_100baseT_Half) !=0 || (advertised & ADVERTISED_10baseT_Full) !=0 || (advertised & ADVERTISED_10baseT_Half) !=0 )
		mode =1;
	else 
		return -1;
	
	macMT7530SetPortMode(portId, mode);
	return 0;
}


void setSpeedDuplex(const struct ethtool_link_ksettings *cmd, u8 portId)
{
	char *speed = kmalloc(MAX_CHAR*sizeof(char),GFP_KERNEL);
	char *duplex = kmalloc(MAX_CHAR*sizeof(char),GFP_KERNEL);
	
		/* duplex */
		if (cmd->base.duplex == DUPLEX_HALF)
		{
			strcpy(duplex,"Half");
		}
		
		else if(cmd->base.duplex == DUPLEX_FULL)
			strcpy(duplex,"Full"); 
		else
			strcpy(duplex,"Auto"); 	
		
		/* speed */
		
		/*if (cmd->base.speed == SPEED_1000)
		{strcpy(speed,"Auto");}*/
		if (cmd->base.speed == SPEED_100)
		{strcpy(speed,"100");}		
		else if (cmd->base.speed == SPEED_10)
		{strcpy(speed,"10");}
		else
		{strcpy(speed,"Auto");}

	macMT7530SetPortMaxBitRate(portId, speed);
	macMT7530SetPortDuplexMode(portId, duplex);
	printk("\n speed and duplex values decided as : %s and %s \n", speed,duplex);
	
	kfree(speed);
	kfree(duplex);
	return;
}

static int airoha_set_link_ksettings(struct net_device *ndev,
				  const struct ethtool_link_ksettings *cmd)
{
	u8 portId =getSwitchLANIndex(ndev)+1;
	int ret;
	
	if (cmd->base.autoneg != AUTONEG_ENABLE && 
		cmd->base.autoneg != AUTONEG_DISABLE)
		return -EINVAL;
		
	if (cmd->base.autoneg == AUTONEG_DISABLE &&
		((cmd->base.speed != SPEED_100 &&
		cmd->base.speed != SPEED_10) ||
		(cmd->base.duplex !=DUPLEX_FULL &&
		cmd->base.duplex !=DUPLEX_HALF)))
		{
			return -EINVAL;
		}
	
	if (cmd->base.autoneg == AUTONEG_ENABLE){
		/*
		setPortAutonegState(cmd, portId);
		*/
		ret=setPortAdvertiseLinkMode(cmd, portId);
		if (ret==-1)
			return -EINVAL;
		}
	else {
			setPortAutonegState(cmd, portId);
			setSpeedDuplex(cmd, portId);
		}
		return 0;
}
		
int ecnt_set_ethtool_ops_and_1588v2(struct net_device *netdev, bool gsw_support_ptp)
		{
			static struct ethtool_ops ecnt_ethtool_ops = 
			{	
#ifdef TCSUPPORT_KERNEL_API
				.get_link_ksettings = airoha_get_link_ksettings,	
				.set_link_ksettings = airoha_set_link_ksettings,	
				.get_link		= getPortLinkUpDown
			
#endif
			};
		
#ifdef TCSUPPORT_ECNT_1588v2
			//gsw_support_ptp==1 if GSW eth0.1 support ptp
			if(gsw_support_ptp)
			{
				ecnt_ethtool_ops.get_ts_info  = ecnt_set_ethtool_ops;
			}
#endif
			netdev->ethtool_ops = &ecnt_ethtool_ops;
			return 0;
		
		}
		EXPORT_SYMBOL(ecnt_set_ethtool_ops_and_1588v2);
