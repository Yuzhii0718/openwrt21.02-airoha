#!/bin/sh
#
# hongchen add 20240619

IFNAME=$1
CMD=$2

wps_supplicant_update_uci_log='/tmp/log/wps_supplicant_update_uci_log.txt'

wps_debug_print()
{
	datetime=`date +"%Y-%m-%d %H:%M:%S"`
	#echo "# --->>>zzzz: date[$datetime] $IFNAME $1 $2" > /dev/console
	echo "# --->>>zzzz: date[$datetime] $IFNAME $1 $2" >> $wps_supplicant_update_uci_log
}

wps_debug_print $IFNAME $CMD

WPS_TMP_UCI_DIR=/tmp/uci
mkdir -p $WPS_TMP_UCI_DIR
touch $WPS_TMP_UCI_DIR/wps
uci -c $WPS_TMP_UCI_DIR set wps.connection=connection

convert_special_char() {
	local input=$1
	echo "$input" | sed 's/\\/\\\\\\\\/g' | sed 's/\"/\\\\\\\"/g' | sed 's/\$/\\\\\$/g' | sed 's/\`/\\\\\`/g'
}

# Obtain the named configuration value in the supplicant config file,
# stripping off any double quotes.
#
# Return the value in the config_val global variable.
get_config_val() {
	local conf=$1
	local key=$2

	# This finds the last key in the supplicant config and strips off leading
	# and trailing quotes (if it has them).
	#
	# Note that it uses a leading space to ensure that only an exact match
	# on the key is used.
	config_val=$(awk "BEGIN{FS=\"=\"} /[[:space:]]${key}=/ {print \$0}" $conf |grep "${key}=" |tail -n 1 | cut -f2- -d= | sed 's/^"//' | sed 's/"$//')
}

get_ssid() {
	local conf=$1
	get_config_val $conf 'ssid'
	ssid=$(convert_special_char "$config_val")
}

get_scanresult_wpa_version() {
	local bssid
	wps_debug_print "--->>>zzzz start get_scanresult_wpa_version "

	if [ -f /tmp/wps_success_bssid ]; then
		bssid=$(cat /tmp/wps_success_bssid)
	fi
	
	wps_debug_print "--->>>zzzz bssid=" $bssid

	if [ -z "$bssid" ]; then
		wps_debug_print "--->>>zzzz bssid from /tmp/wps_success_bssid is empty"
		bssid=$(iwconfig $IFNAME | awk '/Access Point/ {print $NF}')

		if [ -z "$bssid" ]; then
			wps_debug_print "--->>>zzzz bssid from iwconfig is empty"
			bssid=$(wpa_cli -i $IFNAME status | grep ^bssid= | cut -d= -f2)
		fi
	fi
	
	local scanresult=$(wpa_cli -i $IFNAME scan_results | grep -i $bssid | awk '{print $4}')

	wps_debug_print "--->>>zzzz scanresult=" $scanresult

    if [[ "$scanresult" =~ "WPA2-PSK" && "$scanresult" =~ "WPA-PSK" ]]; then
        scanresult_wpa_version="WPAWPA2_PSK_MIXED"
    elif [[ "$scanresult" =~ "WPA2-PSK" && "$scanresult" =~ "SAE-H2E" ]]; then
        scanresult_wpa_version="WPA2WPA3_PSK_MIXED"
    elif [[ "$scanresult" =~ "WPA2-PSK\+SAE" ]]; then
        scanresult_wpa_version="WPA2WPA3_PSK_MIXED"
    elif [[ "$scanresult" =~ "WPA2-PSK" ]]; then
        scanresult_wpa_version="WPA2_PSK"
    elif [[ "$scanresult" =~ "WPA-PSK" ]]; then
        scanresult_wpa_version="WPA_PSK"
    else
        scanresult_wpa_version="NONE"
    fi

	rm -f /tmp/wps_success_bssid
}

get_wpa_version_conf() {
	local conf=$1
	local proto_key_str


	get_config_val $conf 'proto'
	proto_key_str="${config_val}"

	get_config_val $conf 'key_mgmt'
	proto_key_str="${proto_key_str} ${config_val}"

	echo "--->>>zzzz proto_key_str=" $proto_key_str >> $wps_supplicant_update_uci_log 

	case "${proto_key_str}" in
		"RSN WPA-PSK")
			wpa_version_conf="WPA2-PSK"
			;;

		"WPA WPA-PSK")
			wpa_version_conf="WPA-PSK"
			;;

		"RSN WPA-PSK SAE")
			wpa_version_conf="WPA2WPA3_PSK_MIXED"
			;;

		# "RSN SAE"|" SAE")
		# 	wpa_version_conf="WPA3-SAE"
		# 	;;

		# Since the proto key does not appear when no encryption
		# is being used, we need to match against all possible
		# combinations since the proto might have been extracted
		# from a previous network section.
		" NONE"|"RSN NONE"|"WPA NONE")
			wpa_version_conf="NONE"
			;;
	esac
}

get_wpa_version() {
	local conf=$1
	get_wpa_version_conf $conf
	#get_scanresult_wpa_version

	# if [ "$scanresult_wpa_version" != "NONE" ] && [ "$scanresult_wpa_version" != "$wpa_version_conf" ]; then
	# 	wpa_version=$scanresult_wpa_version
	# else
		wpa_version=$wpa_version_conf
	# fi

    wps_debug_print "--->>>zzzz scanresult_wpa_version=" $scanresult_wpa_version
    wps_debug_print "--->>>zzzz wpa_version_conf=" $wpa_version_conf
    wps_debug_print "--->>>zzzz wpa_version=" $wpa_version
}

get_psk() {
	local conf=$1

	get_config_val $conf 'psk'
	psk=$(convert_special_char "$config_val")
}

get_sae_password() {
	local conf=$1

	get_config_val $conf 'sae_password'
	psk=$(convert_special_char "$config_val")
}

get_bss_ml_capable() {
	local conf=$1

	get_config_val $conf 'mlo_capable'
	bss_ml_capable=${config_val:-0}
}

psk=
ssid=
wpa_version=
wpa_version_conf=
bss_ml_capable=
scanresult_wpa_version=

case "$CMD" in
	WPS-NEW-AP-SETTINGS)
		wps_debug_print " --->>>WPS-NEW-AP-SETTINGS not used in NTGR!"
		;;
	WPS-CANCEL|WPS-TIMEOUT)
		wpsstatus=`uci -q -c $WPS_TMP_UCI_DIR get wps.connection.status_$IFNAME`
		if ["$wpsstatus" == "WPS-TIMEOUT" ]; then
			wps_debug_print "already canceled, no need more."
		else
			uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-TIMEOUT"
			kill "$(cat "/var/run/wpa_cli-$IFNAME.pid")"

			env -i ACTION="wps-timeout" INTERFACE=$IFNAME /sbin/hotplug-call iface
			ubus send "WPSPBC_MSG_TIMEOUT" "{\"ifname\": \"$IFNAME\"}"

			/lib/functions/wpsled.sh
			wps_debug_print "--->>>led_ctl_wps_led_cancel."
		fi
		;;
	WPS-SUCCESS)
		uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-SUCCESS"
		#get_scanresult_wpa_version

        wpa_cli -i $IFNAME save_config

		sleep 1 # wait for wpa_supplicant to save config
		wps_debug_print "--->>>zzzz start get ssid, wpa_version, psk, bss_ml_capable.---<<<<"

		# Read the configuration from the file to avoid a potential
		# race where the supplicant is not in the associated state when
		# its status command is executed.
		get_ssid /var/run/wpa_supplicant/wpa_supplicant-$IFNAME.conf
		get_wpa_version /var/run/wpa_supplicant/wpa_supplicant-$IFNAME.conf
		get_bss_ml_capable /var/run/wpa_supplicant/wpa_supplicant-$IFNAME.conf
		
		if [ "$wpa_version" == "WPA3-SAE" ]; then
			get_sae_password /var/run/wpa_supplicant/wpa_supplicant-$IFNAME.conf
		else
			get_psk /var/run/wpa_supplicant/wpa_supplicant-$IFNAME.conf
		fi

        wps_debug_print "--->>>zzzz ssid=" $ssid
		wps_debug_print "--->>>zzzz wpa_version=" $wpa_version
		wps_debug_print "--->>>zzzz psk=" $psk
		wps_debug_print "--->>>zzzz bss_ml_capable=" $bss_ml_capable
				
		env -i ACTION="wps-success" INTERFACE=$IFNAME /sbin/hotplug-call iface
		ubus send "WPSPBC_MSG_SUCCESS" "{\"ifname\": \"$IFNAME\", \"ssid\": \"$ssid\", \"wpa_version\": \"$wpa_version\", \"psk\": \"$psk\" , \"bss_ml_capable\": \"$bss_ml_capable\"}"

		kill "$(cat "/var/run/wpa_cli-$IFNAME.pid")"

		/lib/functions/wpsled.sh
		wps_debug_print "--->>>led_ctl_wps_led_success."

		;;
	WPS-OVERLAP-DETECTED)
		wpsstatus=`uci -q -c $WPS_TMP_UCI_DIR get wps.connection.status_$IFNAME`
		if [ "$wpsstatus" == "WPS-PBC-ACTIVE" ]; then
			uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-OVERLAP-DETECTED"
			/lib/functions/wpsled.sh fast_blink
			wps_debug_print "--->>>led_ctl_wps_led_overlap."
		fi
		;;
	WPS-PBC-ACTIVE)
		uci -c $WPS_TMP_UCI_DIR set wps.connection.status_$IFNAME="WPS-PBC-ACTIVE"
		/lib/functions/wpsled.sh slow_blink
		wps_debug_print "--->>>led_ctl_wps_led_in_progress."
		;;
	DISCONNECTED)
		#kill "$(cat "/var/run/wpa_cli-$IFNAME.pid")"
		;;
esac