#!/bin/sh

action=$1
WPS_TMP_UCI_DIR=/tmp/uci

ledAction() {
	ap2gRadio=$(uci -q get wireless.ra0.disabled)
	ap5gRadio=$(uci -q get wireless.rai0.disabled)
	ap2gsecurity=$(uci -q get wireless.ra0.encryption)
	ap5gsecurity=$(uci -q get wireless.rai0.encryption)

	if [ "$ap2gRadio" == "1" ] && [ "$ap5gRadio" == "1" ]; then
		. /lib/functions/led_control.sh && platform_led_off green wps
	elif [ "$ap2gRadio" == "0" ] && [ "$ap5gRadio" == "1" ]; then
		if [[ "$ap2gsecurity" == "none" || "$ap2gsecurity" == "OFF" || -z "$ap2gsecurity" ]]; then
			. /lib/functions/led_control.sh && platform_led_off green wps
		else
			. /lib/functions/led_control.sh && platform_led_on green wps
		fi
	elif [ "$ap2gRadio" == "1" ] && [ "$ap5gRadio" == "0" ]; then
		if [[ "$ap5gsecurity" == "none" || "$ap5gsecurity" == "OFF" || -z "$ap5gsecurity" ]]; then
			. /lib/functions/led_control.sh && platform_led_off green wps
		else
			. /lib/functions/led_control.sh && platform_led_on green wps
		fi
	else
		if  [[ "$ap2gsecurity" == "none" || "$ap2gsecurity" == "OFF" || -z "$ap2gsecurity" ]] && \
			[[ "$ap5gsecurity" == "none" || "$ap5gsecurity" == "OFF" || -z "$ap5gsecurity" ]]; then
			. /lib/functions/led_control.sh && platform_led_off green wps
		else
			. /lib/functions/led_control.sh && platform_led_on green wps
		fi
	fi
}

case "$action" in
	on)
		. /lib/functions/led_control.sh && platform_led_on green wps
		;;
	off)
		. /lib/functions/led_control.sh && platform_led_off green wps
		;;
	fast_blink)
		. /lib/functions/led_control.sh && platform_led_set_timer green wps 100 100
		;;
	blink)
		. /lib/functions/led_control.sh && platform_led_set_timer green wps 200 200
		;;
	slow_blink)
		. /lib/functions/led_control.sh && platform_led_set_timer green wps 500 500
		;;
	*)
		ledAction
		;;
esac