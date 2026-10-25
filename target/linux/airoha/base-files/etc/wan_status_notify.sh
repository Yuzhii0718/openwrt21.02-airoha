#!/bin/sh
# Four input params
inPutIfName=$1
inPutWanIP_v4=$2
inPutWanIP_v6=$3
inPutWanStatus=$4

#init set needNotifyFlg as 1,means default need notify voip.
needNotifyFlg=1

curWanIP="$(uci get voip.VoIPSysParam_Common.SC_SYS_CFG_WAN_IP)"
VoIPBindIfName="$(uci get voip.VoIPAdvanced_Common.VoIPBindWanIf)"
IPProtocal="$(uci get voip.VoIPAdvanced_Common.IPProtocal)"
lineNum="$(uci get voip.VoIPBasic_Common.VoIPLineNumber)"

if [ "$IPProtocal" = "IPV6" ] ;then
	inPutWanIP=$inPutWanIP_v6
	initWanIP="::"
else
	inPutWanIP=$inPutWanIP_v4
	initWanIP="0.0.0.0"
fi

# we set node only when the interface bind to voip.
if [ "$VoIPBindIfName" = "$inPutIfName" ] ;then
	if [ "$inPutWanStatus" = "up" ] ;then
#   		if input status is "up" and input wanip is 0.0.0.0,it is not illigal, don't need to notify voip.
		if [ "$inPutWanIP" = "initWanIP" ] ;then
			needNotifyFlg=0
		fi
#   		if current ip=init ip,it means current status is down,so we set wanStatus as down_up
		if [ "$curWanIP" = "$initWanIP" ] ;then
			wanStatus="down_up"
		else
#       	if input status is "up" and wanip no change ,it don't need to notify voip.
			if [ "$inPutWanIP" = "$curWanIP" ] ;then
				needNotifyFlg=0
			else
				wanStatus="up"			
			fi
		fi
	else
		if [ "$curWanIP" != "$initWanIP" ] ;then
			wanStatus="up_down"
		else
			needNotifyFlg=0
		fi
	fi
#	if need notify voip,then commit related node and reload sipclient
	if [ $needNotifyFlg -eq 1 ] ;then
#   		set wan status change flag node
		maxIndex=$(($lineNum - 1))
		for i in $(seq 0 $maxIndex)
		do
		WanStatusChangeNode="voip.InfoVoIP_Entry$i.WAN_STATUS_CHANGE_FLAG"
		uci set $WanStatusChangeNode=$wanStatus
		done
		uci set voip.VoIPSysParam_Common.SC_SYS_CFG_WAN_IFNAME=$inPutIfName
		uci set voip.VoIPSysParam_Common.SC_SYS_CFG_WAN_IP=$inPutWanIP
		
		uci commit voip
		echo reload sipclient update wan status:$wanStatus > /proc/tc3162/dbg_msg
		/userfs/bin/sipclient -r -1
	fi
fi





