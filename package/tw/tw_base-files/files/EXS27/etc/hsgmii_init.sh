#!/bin/sh

DEBUG_FILE="/tmp/hsgmii_init.log"
rm -f $DEBUG_FILE

USB_serdes=SerDes-USB1
WIFI1_serdes=SerDes-WiFi1 
WIFI2_serdes=SerDes-WiFi2 
ETH_serdes=SerDes-Ethernet 
wan_proc=/proc/tc3162/wan_2_5
lan_proc=/proc/tc3162/lan_2_5


find_serdes()
{
	itf="$1"
	num="$2"
	network="$3"
	lan_str="$4"

	result=$(echo $lan_str | grep "${itf}")
	if [ "$result" != "" ];then
					itf=eth0.$num
	        result=$(echo $network | grep "$itf")
					if [ "$result" = "" ];then
						echo "$itf up" >> $DEBUG_FILE
						network="$network $itf"
					fi
	fi

}

main()
{
	network=`uci get network.@device[0].ports`

	#ae_wan interface
	if [ -f $wan_proc ];then
		wan_str=`cat $wan_proc`
		result=$(echo $wan_str | grep "SerDes-PON")
		echo "$result"
		if [ "$result" != "" ];then
#			result=$(echo $network | grep "ae_wan")
#			if [ "$result" = "" ];then
				echo "ae_wan up" >> $DEBUG_FILE
				ifconfig eth1 up
#				network="$network ae_wan"
#			fi
		fi
	fi

	#xfi interface
	if [ -f $lan_proc ];then
		hsgmii_str=`cat /proc/tc3162/board_args_hsgmii`
		if [ "$hsgmii_str" = "" ];then
			echo "no boardargs" >> $DEBUG_FILE
			lan_str=`cat $lan_proc`

		  find_serdes $USB_serdes 5 "$network" "$lan_str"
		  find_serdes $WIFI1_serdes 6 "$network" "$lan_str"
		  find_serdes $WIFI2_serdes 7 "$network" "$lan_str"
			find_serdes $ETH_serdes 8 "$network" "$lan_str"
		else
			echo "exsit boardargs" >> $DEBUG_FILE
			str=eth${hsgmii_str#*eth}
			for itf in $str
			do
				result=$(echo $network | grep "${itf}")
				if [ "$result" = "" ];then
				        echo "$itf up" >> $DEBUG_FILE
				        network="$network $itf"
				fi
			done
		fi
	echo "now up: $network" >> $DEBUG_FILE
	uci set network.@device[0].ports="$network"
	uci commit
	fi

}

main;
