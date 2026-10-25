#!/bin/sh

if [ $# != 2 ]; then
	echo "wan_stop_atm.sh  [vpi] [vci]"
	exit 0
fi

vpi=$1
vci=$2

killall -9 pppd
killall -9 br2684ctl
sleep 1
/userfs/bin/atm_cmd vcc delete $vpi $vci
sleep 1
/userfs/bin/atm_cmd stop
