while :
do
s=`uci get system.ntp.period`
/etc/init.d/sysntpd restart
START_SCHDULE_FLAG="/tmp/start_schedule_flag"
if [ ! -f "$START_SCHDULE_FLAG" ]; then
	/sbin/schedule.sh internet_event_end
fi
sleep "$s"
done
