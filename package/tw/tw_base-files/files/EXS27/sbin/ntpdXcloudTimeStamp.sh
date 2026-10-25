#!/bin/sh
rm -rf "/tmp/xCloud_syncTime"

hardwareId=$(protest --sn -r | cut -d '!' -f2)
model=$(uci get system.@system[0].model)
hiddenselect=$(uci get system.ntp.hidden_select)
#timezone=$(uci get system.@system[0].zone)
#timezonearrayname=("-12" "-11" "-10" "-9" "-8" "-7" "-7" "-6" "-6" "-5" "-5" "-5" "-4" "-3" "-3" "-2" "-1" "0" "0" "+1" "+1" "+1" "+1" "+2" "+2" "+2" "+3" "+4" "+4" "+5" "+5" "+6" "+7" "+8" "+8" "+9" "+10" "+10" "+10" "+11" "+11" "+12" "+12" "+13")

if [ $hiddenselect == 0 ]
then
timezonearrayname='-12'
elif [ $hiddenselect == 1 ]
then
timezonearrayname='-11'
elif [ $hiddenselect == 2 ]
then
timezonearrayname='-10'
elif [ $hiddenselect == 3 ]
then
timezonearrayname='-9'
elif [ $hiddenselect == 4 ]
then
timezonearrayname='-8'
elif [ $hiddenselect == 5 ]
then
timezonearrayname='-7'
elif [ $hiddenselect == 6 ]
then
timezonearrayname='-7'
elif [ $hiddenselect == 7 ]
then
timezonearrayname='-6'
elif [ $hiddenselect == 8 ]
then
timezonearrayname='-6'
elif [ $hiddenselect == 9 ]
then
timezonearrayname='-5'
elif [ $hiddenselect == 10 ]
then
timezonearrayname='-5'
elif [ $hiddenselect == 11 ]
then
timezonearrayname='-5'
elif [ $hiddenselect == 12 ]
then
timezonearrayname='-4'
elif [ $hiddenselect == 13 ]
then
timezonearrayname='-3'
elif [ $hiddenselect == 14 ]
then
timezonearrayname='-3'
elif [ $hiddenselect == 15 ]
then
timezonearrayname='-2'
elif [ $hiddenselect == 16 ]
then
timezonearrayname='-1'
elif [ $hiddenselect == 17 ]
then
timezonearrayname='0'
elif [ $hiddenselect == 18 ]
then
timezonearrayname='0'
elif [ $hiddenselect == 19 ]
then
timezonearrayname='1'
elif [ $hiddenselect == 20 ]
then
timezonearrayname='1'
elif [ $hiddenselect == 21 ]
then
timezonearrayname='1'
elif [ $hiddenselect == 22 ]
then
timezonearrayname='1'
elif [ $hiddenselect == 23 ]
then
timezonearrayname='2'
elif [ $hiddenselect == 24 ]
then
timezonearrayname='2'
elif [ $hiddenselect == 25 ]
then
timezonearrayname='2'
elif [ $hiddenselect == 26 ]
then
timezonearrayname='3'
elif [ $hiddenselect == 27 ]
then
timezonearrayname='4'
elif [ $hiddenselect == 28 ]
then
timezonearrayname='4'
elif [ $hiddenselect == 29 ]
then
timezonearrayname='5'
elif [ $hiddenselect == 30 ]
then
timezonearrayname='5'
elif [ $hiddenselect == 31 ]
then
timezonearrayname='6'
elif [ $hiddenselect == 32 ]
then
timezonearrayname='7'
elif [ $hiddenselect == 33 ]
then
timezonearrayname='8'
elif [ $hiddenselect == 34 ]
then
timezonearrayname='8'
elif [ $hiddenselect == 35 ]
then
timezonearrayname='9'
elif [ $hiddenselect == 36 ]
then
timezonearrayname='10'
elif [ $hiddenselect == 37 ]
then
timezonearrayname='10'
elif [ $hiddenselect == 38 ]
then
timezonearrayname='10'
elif [ $hiddenselect == 39 ]
then
timezonearrayname='11'
elif [ $hiddenselect == 40 ]
then
timezonearrayname='11'
elif [ $hiddenselect == 41 ]
then
timezonearrayname='12'
elif [ $hiddenselect == 42 ]
then
timezonearrayname='12'
elif [ $hiddenselect == 43 ]
then
timezonearrayname='13'
fi

if [ $hiddenselect == 13 ]
then
zoneOffsetvalue=19800
elif [ $hiddenselect == 30 ]
then
zoneOffsetvalue=-12600
else
zoneOffsetvalue=`expr $timezonearrayname \* 3600`
fi
echo $timezonearrayname $hiddenselect $zoneOffsetvalue $hardwareId $model
#curl -X POST -k -S -v https://devicelocation.dev.ngxcld.com/device-location/syncTime -H "content-type:application/json" -H "Authorization::" -d "{\"hardwareId\": \"$hardwareId\", \"model\": \"$model\"}" >>/tmp/xCloud_syncTime
curl -X POST -k -S -v https://devicelocation.ngxcld.com/device-location/syncTime -H "content-type:application/json" -H "Authorization::" -d "{\"hardwareId\": \"$hardwareId\", \"model\": \"$model\"}" >> /tmp/xCloud_syncTime
sleep 15
CheckPath="/tmp/xCloud_syncTime"
firstTime="/tmp/first_time"
timesynchronizedfromxcloud="/tmp/Time_synchronized_From_Xcloud"
while true; do
	if [ -f "$CheckPath" ]; then #-f for file
		echo "$CheckPath detected"
		tmptimestamp=''
		tmpzoneOffset=''
		Setstandardtime=''
		tmptimestamp=$(cat $CheckPath | awk 'match($0,"timestamp")' | awk -F '[:]' '{print $NF}' | awk '{gsub(",", "");print}')
		tmpzoneOffset=$(cat $CheckPath | awk 'match($0,"zoneOffset")' | awk -F '[:]' '{print $NF}')

		echo $tmptimestamp $tmpzoneOffset
		if [ ! -n "$tmptimestamp" ]; then
			{
				rm -rf $CheckPath
			}
		else
			{
				Setstandardtime=$(($tmptimestamp-$tmpzoneOffset+$zoneOffsetvalue+15));
				echo $Setstandardtime
				date -s "@$Setstandardtime"
				rm -rf $CheckPath
				echo > "$firstTime"
				echo > "$timesynchronizedfromxcloud"
				/sbin/schedule.sh internet_event_end
			}
		fi
		break
	else
		echo "$CheckPath does not exist, sleeping 10 seconds"
		sleep 10
	fi
done
