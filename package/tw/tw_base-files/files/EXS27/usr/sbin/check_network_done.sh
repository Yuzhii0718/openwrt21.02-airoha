#!/bin/sh

while [ true ]
do
	sleep 2
	ap2gName="ra0"
	ap5gName="rai0"
	guest2gName="ra1"
	guest5gName="rai1"
	apcli2gName="apcli0"
	apcli5gName="apclii0"

	ap2gRadio="$(uci -q get wireless.ra0.disabled)"
	ap5gRadio="$(uci -q get wireless.rai0.disabled)"
	ap2gGuest="$(uci -q get wireless.ra1.disabled)"
	ap5gGuest="$(uci -q get wireless.rai1.disabled)"

	conf_mode="$(uci get wireless.2g_5g.conf_mode)"
	sRootApExist_2g="$(uci -q get wireless.apcli0.disabled)"
	isRootApExist_5g="$(uci -q get wireless.apclii0.disabled)"
	
	# map_mode="$(uci get wireless.2g_5g.map_mode)"
	# apclimlo="$(uci get wireless.stamld2.disabled)"

	# echo " --->>> ap2gRadio=$ap2gRadio ap5gRadio=$ap5gRadio ap2gGuest=$ap2gGuest ap5gGuest=$ap5gGuest conf_mode=$conf_mode sRootApExist_2g=$sRootApExist_2g isRootApExist_5g=$isRootApExist_5g"

	if [ $ap2gRadio = 0 ] ; then
			wifi2gintf="$(brctl show | grep $ap2gName)"
			if [ "x$wifi2gintf" == "x" ] ; then
					brctl addif br-lan $ap2gName
			fi
	
			if [ $ap2gGuest = 0 ] ; then
				wifi2gintf2="$(brctl show | grep $guest2gName)"
				if [ "x$wifi2gintf2" == "x" ] ; then
						brctl addif br-lan $guest2gName
				fi
			fi
	fi
	if [ $ap5gRadio = 0 ] ; then
			wifi5gintf="$(brctl show | grep $ap5gName)"
			if [ "x$wifi5gintf" == "x" ] ; then
					brctl addif br-lan $ap5gName
			fi

			if [ $ap5gGuest = 0 ] ; then
				wifi5gintf2="$(brctl show | grep $guest5gName)"
				if [ "x$wifi5gintf2" == "x" ] ; then
						brctl addif br-lan $guest5gName
				fi
			fi
	fi
	
	# if [ "$map_mode" = 2 ] && [ "$apclimlo" = 0 ]; then
	# 	apcli2gintf="$(brctl show | grep $apcli2gName)"
		
	# 	if [ "x$apcli2gintf" != "x" ] ; then
	# 			brctl delif br-lan $apcli2gName
	# 	fi
	# fi

	# if [ $conf_mode = 2 ] ; then
	# 	if [ $sRootApExist_2g = 0 ] ; then
	# 		apcli2gintf="$(brctl show | grep $apcli2gName)"
	# 		if [ "x$apcli2gintf" == "x" ] ; then
	# 				brctl addif br-lan $apcli2gName
	# 		fi
	# 	fi
	# 	if [ $isRootApExist_5g = 0 ] ; then
	# 		apcli5gintf="$(brctl show | grep $apcli5gName)"
	# 		if [ "x$apcli5gintf" == "x" ] ; then
	# 				brctl addif br-lan $apcli5gName
	# 		fi
	# 	fi
	# fi

    sleep 2
done
