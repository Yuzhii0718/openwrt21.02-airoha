#!/bin/bash 

if [ $# -ne 2 ]; then
	echo "arguments error"
	exit
fi

method=$1
mac=$2
echo "method=$method  base mac=$mac"
declare -a new_2g_e2p_arry
declare -a new_5g_e2p_arry

mac=`printf "%012x" "0x$mac"`
mac=${mac:0:12}
mac_sn_num=`printf "%d" "0x${mac:6:6}"`

mac_vendor="${mac:0:2}:${mac:2:2}:${mac:4:2}"
#local lan_mac="${mac:0:2}:${mac:2:2}:${mac:4:2}:${mac:6:2}:${mac:8:2}:${mac:10:2}"

mac_sn_num=$(($mac_sn_num+1))
mac_sn=`printf "%06x" $mac_sn_num`
wifi_2g_mac="${mac:0:2}:${mac:2:2}:${mac:4:2}:${mac_sn:0:2}:${mac_sn:2:2}:${mac_sn:4:2}"
wifi_2g_mac=$(echo $wifi_2g_mac | awk '{print toupper($0)}')

new_2g_e2p_arry[0]="${mac:2:2}${mac:0:2}"
new_2g_e2p_arry[1]="${mac_sn:0:2}${mac:4:2}"
new_2g_e2p_arry[2]="${mac_sn:4:2}${mac_sn:2:2}"

echo "new_2g_e2p_arry:${new_2g_e2p_arry[0]} ${new_2g_e2p_arry[1]} ${new_2g_e2p_arry[2]}"
mac_sn_num=$(($mac_sn_num+1))
mac_sn=`printf "%06x" $mac_sn_num`
wifi_5g_mac="${mac:0:2}:${mac:2:2}:${mac:4:2}:${mac_sn:0:2}:${mac_sn:2:2}:${mac_sn:4:2}"
wifi_5g_mac=$(echo $wifi_5g_mac | awk '{print toupper($0)}')

new_5g_e2p_arry[0]="${mac:2:2}${mac:0:2}"
new_5g_e2p_arry[1]="${mac_sn:0:2}${mac:4:2}"
new_5g_e2p_arry[2]="${mac_sn:4:2}${mac_sn:2:2}"
echo "new_5g_e2p_arry:${new_5g_e2p_arry[0]} ${new_5g_e2p_arry[1]} ${new_5g_e2p_arry[2]}"


if [ $method == "factory_reset" ];then
	echo "factory_reset set wifi mac"
	mwctl phy phy0 e2p 4=${new_2g_e2p_arry[0]} 
	mwctl phy phy0 e2p 6=${new_2g_e2p_arry[1]} 
	mwctl phy phy0 e2p 8=${new_2g_e2p_arry[2]} 
	ated -i ra0 -c "sync eeprom all"

	mwctl phy phy0 e2p a=${new_5g_e2p_arry[0]} 
	mwctl phy phy0 e2p c=${new_5g_e2p_arry[1]} 
	mwctl phy phy0 e2p e=${new_5g_e2p_arry[2]} 
	ated -i rai0 -c "sync eeprom all"
elif [ $method == "wifi_mac_init" ];then
	declare -a curr_2g_e2p_arry
	declare -a curr_2g_mac_arry
	declare -a curr_5g_e2p_arry
	declare -a curr_5g_mac_arry

#2.4g
	curr_2g_e2p_arry[0]=$(mwctl phy phy0 e2p 4 | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_2g_mac_arry[1]=${curr_2g_e2p_arry[0]:0:2}
	curr_2g_mac_arry[0]=${curr_2g_e2p_arry[0]:2}
	
	curr_2g_e2p_arry[1]=$(mwctl phy phy0 e2p 6 | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_2g_mac_arry[3]=${curr_2g_e2p_arry[1]:0:2}
	curr_2g_mac_arry[2]=${curr_2g_e2p_arry[1]:2}
	
	curr_2g_e2p_arry[2]=$(mwctl phy phy0 e2p 8 | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_2g_mac_arry[5]=${curr_2g_e2p_arry[2]:0:2}
	curr_2g_mac_arry[4]=${curr_2g_e2p_arry[2]:2}
	
	e2p2g_mac="${curr_2g_mac_arry[0]}:${curr_2g_mac_arry[1]}:${curr_2g_mac_arry[2]}:${curr_2g_mac_arry[3]}:${curr_2g_mac_arry[4]}:${curr_2g_mac_arry[5]}"
	e2p2g_mac=$(echo $e2p2g_mac | awk '{print toupper($0)}')
	echo "curr_2g_mac_arry:$e2p2g_mac,wifi_2g_mac:$wifi_2g_mac"
	
	if [ "$wifi_2g_mac" != "$e2p2g_mac" ]; then
		echo "Set wifi 2g mac"
		mwctl phy phy0 e2p 4=${new_2g_e2p_arry[0]} 
		mwctl phy phy0 e2p 6=${new_2g_e2p_arry[1]} 
		mwctl phy phy0 e2p 8=${new_2g_e2p_arry[2]} 
		ated -i ra0 -c "sync eeprom all"
	fi
#5g
	curr_5g_e2p_arry[0]=$(mwctl phy phy0 e2p a | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_5g_mac_arry[1]=${curr_5g_e2p_arry[0]:0:2}
	curr_5g_mac_arry[0]=${curr_5g_e2p_arry[0]:2}
	
	curr_5g_e2p_arry[1]=$(mwctl phy phy0 e2p c | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_5g_mac_arry[3]=${curr_5g_e2p_arry[1]:0:2}
	curr_5g_mac_arry[2]=${curr_5g_e2p_arry[1]:2}
	
	curr_5g_e2p_arry[2]=$(mwctl phy phy0 e2p e | awk -F "0x" '{print $3}' | sed 's/[[:space:]]*$//')
	curr_5g_mac_arry[5]=${curr_5g_e2p_arry[2]:0:2}
	curr_5g_mac_arry[4]=${curr_5g_e2p_arry[2]:2}
	
	e2p5g_mac="${curr_5g_mac_arry[0]}:${curr_5g_mac_arry[1]}:${curr_5g_mac_arry[2]}:${curr_5g_mac_arry[3]}:${curr_5g_mac_arry[4]}:${curr_5g_mac_arry[5]}"
	e2p5g_mac=$(echo $e2p5g_mac | awk '{print toupper($0)}')
	echo "curr_5g_mac_arry:$e2p5g_mac wifi_5g_mac:$wifi_5g_mac"
	if [ "$wifi_5g_mac" != "$e2p5g_mac" ]; then
		echo "Set wifi 5g mac"
		mwctl phy phy0 e2p a=${new_5g_e2p_arry[0]} 
		mwctl phy phy0 e2p c=${new_5g_e2p_arry[1]} 
		mwctl phy phy0 e2p e=${new_5g_e2p_arry[2]} 
		ated -i rai0 -c "sync eeprom all"
	fi
fi
