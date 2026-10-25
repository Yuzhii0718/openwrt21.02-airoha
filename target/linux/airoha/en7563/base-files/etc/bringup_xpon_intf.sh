#!/bin/sh

DEBUG_FILE="/tmp/bringup_xpon_intf.log"
rm -f $DEBUG_FILE

for i in  0 1 2 3 4 5 6 7
do
	vlan_id=`uci show network | grep "gpon_vlan\[$i\]\." | grep vlan_id | cut -d '=' -f2 | sed -e 's|["'\'']||g'`
	payload=`uci show network | grep "gpon_vlan\[$i\]\." | grep payload | cut -d '=' -f2 | sed -e 's|["'\'']||g'`
	if [ ! -z $vlan_id ]; then
		echo "ifconfig pon up" >> $DEBUG_FILE
		ifconfig pon up

		echo "vconfig add pon $vlan_id" >> $DEBUG_FILE
		vconfig add pon $vlan_id

		echo "ifconfig pon.$vlan_id up" >> $DEBUG_FILE
		ifconfig pon.$vlan_id up 
		if [ $payload = "bridged" ];then
        		brctl addif br-lan pon.$vlan_id
		fi 
	fi
done
vlan_id=`uci get network.gpon_vlan.vlan_id`
payload=`uci get network.gpon_vlan.payload`

if [ ! -z $vlan_id ]; then
	echo "ifconfig pon up" >> $DEBUG_FILE
        ifconfig pon up

        echo "vconfig add pon $vlan_id" >> $DEBUG_FILE
        vconfig add pon $vlan_id

        echo "ifconfig pon.$vlan_id up" >> $DEBUG_FILE
        ifconfig pon.$vlan_id up
if [ $payload = "bridged" ];then
	brctl addif br-lan pon.$vlan_id
fi	
fi
