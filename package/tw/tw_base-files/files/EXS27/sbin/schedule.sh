echo  "$1"

if [ -z "$1" ]
then 
	echo "str fail"
	exit 0
fi

action="-D"

g_week=`date +"%a"`
g_hour=`date +"%H"`
g_min=`date +"%M"`

if [ $g_hour -eq "01" ];then
	g_hour=1
elif [ $g_hour -eq "02" ];then
	g_hour=2
elif [ $g_hour -eq "03" ];then
	g_hour=3
elif [ $g_hour -eq "04" ];then
	g_hour=4
elif [ $g_hour -eq "05" ];then
	g_hour=5
elif [ $g_hour -eq "06" ];then
	g_hour=6
elif [ $g_hour -eq "07" ];then
	g_hour=7
elif [ $g_hour -eq "08" ];then
	g_hour=8
elif [ $g_hour -eq "09" ];then
	g_hour=9
fi

if [ $g_min -eq "01" ];then
	g_min=1
elif [ $g_min -eq "02" ];then
	g_min=2
elif [ $g_min -eq "03" ];then
	g_min=3
elif [ $g_min -eq "04" ];then
	g_min=4
elif [ $g_min -eq "05" ];then
	g_min=5
elif [ $g_min -eq "06" ];then
	g_min=6
elif [ $g_min -eq "07" ];then
	g_min=7
elif [ $g_min -eq "08" ];then
	g_min=8
elif [ $g_min -eq "09" ];then
	g_min=9
fi

internetTimeTransform(){
	min=0
	hour=0
	tmp_min=0

	if test "48" = $1
	then 
		min=59
		hour=23
		echo $min $hour
		return 0
	fi

	$((tmp_min=$1%2))
	if test "0" = $tmp_min
	then
		min=0
	else
		min=30
	fi

	$((hour=$1/2))
	echo $min $hour
	return 0
}

week_current=0
if test $g_week = 'Mon'
then
	week_current=1
elif test $g_week = 'Tue'
then
	week_current=2
elif test $g_week = 'Wed'
then
	week_current=3
elif test $g_week = 'Thu'
then
	week_current=4
elif test $g_week = 'Fri'
then
	week_current=5
elif test $g_week = 'Sat'
then
	week_current=6
elif test $g_week = 'Sun'
then
	week_current=0
fi

enable=`uci get system.schedule.scheduleEnable`
extmode=`uci get wireless.2g_5g.conf_mode`

MANUALCLOCK="/tmp/manual_clock"
FIRSTTIME="/tmp/first_time"

sys_clock=`uci get system.ntp.sys_clock`

if [ '1' == "$sys_clock" ]; then
	if [ ! -f "$MANUALCLOCK" ]; then
		`uci set system.ntp.sys_clock='0'`
		`uci set system.ntp.enabled='1'`
		`uci commit system`
	fi
fi

if [ ! -f "$MANUALCLOCK" ] && [ ! -f "$FIRSTTIME" ]; then
	enable="0"
fi

if [ $action == "-D" ] && [ '1' == $enable ] && [ '2' == $extmode ]; then
	num=0
	while test "schedulelist" = `uci get system.@schedulelist[$num]`
	do
		schedulelist_from=$(internetTimeTransform  `uci get system.@schedulelist[$num].fromTime`)
		schedulelist_to=$(internetTimeTransform `uci get system.@schedulelist[$num].toTime`)
		week_cfg="`uci get system.@schedulelist[$num].week | grep $week_current`"
		week_flag="`uci get system.@schedulelist[$num].week | grep "*"`"
		schedulelist_all=`uci get system.@schedulelist[$num].toTime`

		if [ -n "$week_cfg" ] || [ -n "$week_flag" ]; then
			schedulelist_from=$(internetTimeTransform "`uci get system.@schedulelist[$num].fromTime`")
			schedulelist_to=$(internetTimeTransform "`uci get system.@schedulelist[$num].toTime`")
			start_min=$(echo "$schedulelist_from" | cut -d ' ' -f1)
			start_hour=$(echo "$schedulelist_from" | cut -d ' ' -f2)
			end_min=$(echo "$schedulelist_to" | cut -d ' ' -f1)
			end_hour=$(echo "$schedulelist_to" | cut -d ' ' -f2)

			# echo $g_hour $g_min $start_hour $start_min  $end_hour $end_min

			a=$((start_hour == end_hour))
			b=$((g_hour == start_hour && g_min >= start_min && g_min < end_min))
			c=$((g_hour > start_hour && g_hour < end_hour))
			d=$((g_hour == start_hour && g_min >= start_min))
			e=$((g_hour == end_hour && g_min < end_min))
			f=$((48 == schedulelist_all && 59 == g_min && 23 == g_hour))
			# echo "$a $b $c $d $e $f"
			if [ '1' -eq $f ]; then
				action="-A"
				break
			elif [ '1' -eq $a ] ; then
				if [ '1' -eq $b ]; then
					action="-A"
					break
				fi
			else
				if [ '1' -eq $c ] || [ '1' -eq $d ] || [ '1' -eq $e ]; then
					action="-A"
					break
				fi
			fi
		fi
		let num=$num+1
	done
fi

if [ $action == "-A" ] && [ '1' == $enable ]; then
	ebtables -D FORWARD -p ipv4 --ip-protocol 17 --ip-dport 67 -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -p ipv4 --ip-protocol 17 --ip-dport 68 -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -p ARP -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -j DROP 2>>/dev/null
	ebtables -A FORWARD -p ipv4 --ip-protocol 17 --ip-dport 67 -j ACCEPT
	ebtables -A FORWARD -p ipv4 --ip-protocol 17 --ip-dport 68 -j ACCEPT
	ebtables -A FORWARD -p ARP -j ACCEPT
	ebtables -A FORWARD -j DROP
elif [ $action == "-D" ]; then
	ebtables -D FORWARD -p ipv4 --ip-protocol 17 --ip-dport 67 -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -p ipv4 --ip-protocol 17 --ip-dport 68 -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -p ARP -j ACCEPT 2>>/dev/null
	ebtables -D FORWARD -j DROP 2>>/dev/null
fi

if [ -f "/etc/crontabs/root" ]; then
	cp -a /etc/crontabs/root /etc/config/root
elif [ -f "/etc/config/root" ]; then
	cp -a /etc/config/root  /etc/crontabs/root
fi