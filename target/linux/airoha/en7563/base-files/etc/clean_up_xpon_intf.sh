#!/bin/sh

DEBUG_FILE="/tmp/cleanup_xpon.log"
rm -f $DEBUG_FILE

for i in  0 1 2 3 4 5 6 7
do
	vlan_id=`cat /tmp/network_uci | grep "gpon_vlan\[$i\]\." | grep vlan_id | cut -d '=' -f2 | sed -e 's|["'\'']||g'`
	if [ ! -z $vlan_id ]; then
		echo "ifconfig pon.$vlan_id down" >> $DEBUG_FILE
		ifconfig pon.$vlan_id down
	fi
done
