#!/bin/sh
#
# hongchen add 20240618

IFNAME=$1
CMD=$2

echo `date "+%Y-%m-%d %H:%M:%S"`"--->>>zzzz $IFNAME $CMD" > /dev/console

WPS_TMP_UCI_DIR=/tmp/uci
mkdir -p $WPS_TMP_UCI_DIR
touch $WPS_TMP_UCI_DIR/wps
uci -c $WPS_TMP_UCI_DIR set wps.connection=connection
#uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-DISABLED"

case "$CMD" in
	WPS-NEW-AP-SETTINGS)
		echo " --->>>WPS-NEW-AP-SETTINGS not used in NTGR!" > /dev/console
		;;
	WPS-CANCEL|WPS-TIMEOUT)
		wpsstatus=`uci -q -c $WPS_TMP_UCI_DIR get wps.connection.status_$IFNAME`
		if ["$wpsstatus" == "WPS-TIMEOUT" ]; then
			echo "already canceled, no need more." > /dev/console
		else
			uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-TIMEOUT"
			kill "$(cat "/var/run/hostapd_cli-$IFNAME.pid")"
			env -i ACTION="wps-timeout" INTERFACE=$IFNAME /sbin/hotplug-call iface
			/lib/functions/wpsled.sh
			echo "--->>>led_ctl_wps_led_cancel." > /dev/console
		fi
		;;
	WPS-SUCCESS)
		uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-SUCCESS"
		
        #if [ -r /var/run/wifi-wps-enhc-extn.pid ]; then
		#	echo $IFNAME > /var/run/wifi-wps-enhc-extn.done
		#	kill -SIGUSR1 "$(cat "/var/run/wifi-wps-enhc-extn.pid")"
		#fi

		kill "$(cat "/var/run/hostapd_cli-$IFNAME.pid")"
		env -i ACTION="wps-success" INTERFACE=$IFNAME /sbin/hotplug-call iface

		/lib/functions/wpsled.sh
		echo "--->>>led_ctl_wps_led_success." > /dev/console
		;;
	WPS-OVERLAP-DETECTED)
		wpsstatus=`uci -q -c $WPS_TMP_UCI_DIR get wps.connection.status_$IFNAME`
		if [ "$wpsstatus" == "WPS-PBC-ACTIVE" ]; then
			uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-OVERLAP-DETECTED"
			/lib/functions/wpsled.sh fast_blink
			echo "--->>>led_ctl_wps_led_overlap." > /dev/console
		fi
		;;
	WPS-PBC-ACTIVE)
		uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-PBC-ACTIVE"
		/lib/functions/wpsled.sh slow_blink
		echo "--->>>led_ctl_wps_led_in_progress." > /dev/console
		;;
	DISCONNECTED)
		kill "$(cat "/var/run/hostapd_cli-$IFNAME.pid")"
		;;
esac
