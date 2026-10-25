#!/bin/sh

SN=`protest --sn -r | head -n 1 | awk -F '!' '{print $2}'`
MODEL=`protest --model -r | head -n 1 | awk -F '!' '{print $2}'`

echo 20 > /proc/sys/net/ipv4/tcp_keepalive_time
echo 3 > /proc/sys/net/ipv4/tcp_keepalive_probes
echo 20 > /proc/sys/net/ipv4/tcp_keepalive_intvl

#/opt/xagent/xagent -w -d --ca_file /opt/xagent/certs/ca-bundle-mega.crt --hardware_id ${SN} --model_id ${MODEL}
