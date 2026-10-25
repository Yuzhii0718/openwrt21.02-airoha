#!/bin/sh

if [ $# != 4 ]; then
	echo "wan_start_ptm.sh [ipaddr] [netmask] [gateway] [interface]"
	exit 0
fi
PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    . $PROFILE_CFG
fi


ipaddr=$1
netmask=$2
gateway=$3
interface=$4

/userfs/bin/ptm_cmd start $interface
if [ "$TCSUPPORT_CPU_EN7516" != "" ] ;then
/userfs/bin/qdmamgr_wan set rxratelimit config Enable packet
/userfs/bin/qdmamgr_wan set rxratelimit value 0 6000
/userfs/bin/qdmamgr_wan set rxratelimit value 1 1000000
fi
/sbin/ifconfig "$interface"8 mtu 1500
/sbin/ifconfig "$interface"8 up
/sbin/ifconfig "$interface"8 $ipaddr netmask $netmask
route add default gw $gateway dev "$interface"8
iptables -t nat -A POSTROUTING -o "$interface"8 -j MASQUERADE
echo 1 > /proc/sys/net/ipv4/ip_forward
