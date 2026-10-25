#!/bin/sh
echo 1 >/tmp/testDhcp
ethif=$(uci get network.lan.ifname)
if [ "x$ethif" = "x" ] ; then
	ethif="eth0"
fi
conf_mode=$(uci get wireless.2g_5g.conf_mode)
linkap=$(uci get network.lan.linkrootap)
recvack=$(uci get network.lan.recvack)

ebtables -D OUTPUT -o $ethif -p ipv4 --ip-protocol 17 --ip-sport 68  --ip-dport 67 -j DROP 2>>/dev/null

ebtables -D OUTPUT -o apcli0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP 2>>/dev/null
ebtables -D OUTPUT -o apclii0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP 2>>/dev/null
ebtables -I OUTPUT -o apcli0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP
ebtables -I OUTPUT -o apclii0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP

/sbin/startdhcp.sh &

sleep 10
rm /tmp/testDhcp 2>>/dev/null
if [ "$conf_mode" != "0" ]; then
	ebtables -I OUTPUT -o $ethif -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP 2>>/dev/null
fi
ebtables -D OUTPUT -o apcli0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP 2>>/dev/null
ebtables -D OUTPUT -o apclii0 -p ipv4 --ip-protocol 17  --ip-sport 68 --ip-dport 67 -j DROP 2>>/dev/null

