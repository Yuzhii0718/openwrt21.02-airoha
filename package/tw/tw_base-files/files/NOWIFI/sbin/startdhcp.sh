#!/bin/sh

deviceName=$(uci -q get system.@system[0].hostname || echo OpenWrt)
dhcppid=$(cat /var/run/udhcpc-br-lan.pid)
if [ $dhcppid -gt 0 ]; then
	sleep 3
	killall -SIGUSR2 udhcpc
	sleep 1
	killall -SIGUSR1 udhcpc
else
	#udhcpc -p /var/run/udhcpc-br-lan.pid -s /lib/netifd/dhcp.script -f -r -t 0 -i br-lan -H $deviceName &
	echo "!!! error .udhcpc is missing" > /dev/console
	/etc/init.d/network restart &
fi
