#!/bin/sh

#from up to down
if [ $1 == "0" ] ;then
#xpon from up to down,restart all wan interface
killall -s SIGTERM udhcpc
killall -s SIGTERM odhcp6c
killall -s SIGTERM pppd
else
#xpon from down to up
/etc/init.d/easycwmpd restart
fi
