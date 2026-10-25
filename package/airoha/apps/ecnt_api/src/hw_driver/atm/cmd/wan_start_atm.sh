#!/bin/sh


if [ $# != 12 ]; then
	echo "wan_start_atm.sh  [vpi] [vci] [ipoe:0|ipoa:1|pppoa:2] [vc_mux:0|llc:1] [ubr:0|cbr:1|vbr:2|nrt-vbr:3] [pcr] [scr] [mbs] [ipaddr] [netmask] [gateway] [interface]"
	exit 0
fi
PROFILE_CFG=/userfs/profile.cfg
if [ -f $PROFILE_CFG ] ; then
    . $PROFILE_CFG
fi


vpi=$1
vci=$2
mode=$3
encap=$4
qos=$5
pcr=$6
scr=$7
mbs=$8
ipaddr=$9
netmask=$10
gateway=$11
interface=$12

/userfs/bin/atm_cmd start
if [ "$TCSUPPORT_CPU_EN7516" != "" ] ;then
/userfs/bin/qdmamgr_wan set rxratelimit config Enable packet
/userfs/bin/qdmamgr_wan set rxratelimit value 0 6000
/userfs/bin/qdmamgr_wan set rxratelimit value 1 1000000
fi
/userfs/bin/atm_cmd vcc create $vpi $vci $mode $encap $qos $pcr $scr $mbs

MAC_CONFFILE=/etc/mac.conf
ifconfig eth0 | sed -n '1p' | sed 's/^.*HWaddr /WAN_MAC=/g' >> $MAC_CONFFILE
chmod 777 $MAC_CONFFILE
. $MAC_CONFFILE

if [ "$mode" = "2" ]; then
	if [ "$encap" = "0" ]; then
		ppp_encap="vc-encaps"
	elif [ "$encap" = "1" ]; then
		ppp_encap="llc-encaps"
	fi

	pppd unit 0 user 1 password 1 nodetach holdoff 4 maxfail 0 usepeerdns lcp-echo-interval 1 lcp-echo-failure 60 plugin libpppoatm.so $ppp_encap $vpi.$vci defaultroute $ipaddr:$gateway netmask $netmask persist mtu 1492 mru 1492 &
	/sbin/ifconfig ppp0 0.0.0.0
	iptables -t nat -A POSTROUTING -o ppp0 -j MASQUERADE
else
	if [ "$mode" = "0" ]; then
		if [ "$encap" = "0" ]; then
			br2684ctl -b -c 0 -e 1 -n $interface -a 0.$vpi.$vci
		elif [ "$encap" = "1" ]; then
			br2684ctl -b -c 0 -e 0 -n $interface -a 0.$vpi.$vci
		fi
		/sbin/ifconfig "$interface"0 hw ether $WAN_MAC
	elif [ "$mode" = "1" ]; then
		if [ "$encap" = "0" ]; then
			br2684ctl -b -c 0 -e 3 -n $interface -a 0.$vpi.$vci
		elif [ "$encap" = "1" ]; then
			br2684ctl -b -c 0 -e 2 -n $interface -a 0.$vpi.$vci
		fi
	fi
	/sbin/ifconfig "$interface"0 mtu 1500
	/sbin/ifconfig "$interface"0 up
	/sbin/ifconfig "$interface"0 $ipaddr netmask $netmask
	route add default gw $gateway dev "$interface"0
	iptables -t nat -A POSTROUTING -o "$interface"0 -j MASQUERADE
fi

echo 1 > /proc/sys/net/ipv4/ip_forward

