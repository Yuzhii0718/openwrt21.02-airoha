#!/bin/sh

DEBUG_FILE="/tmp/default_setting.log"
rm -f $DEBUG_FILE


main()
{
	def_setting=`uci get pon.xpon_auth.def_setting`
	echo "uci def_setting:$def_setting" >> $DEBUG_FILE
	
	if [ "$def_setting" = "1" ];then
		echo 'cmd:/usr/bin/prolinecmd serialnum get | grep "Value="> /tmp/pro_file' >> $DEBUG_FILE
		/usr/bin/prolinecmd serialnum get | grep "Value="> /tmp/pro_file
		serialnum=`cat /tmp/pro_file`
		if [ "$serialnum" != "" ];then
		serialnum=${serialnum#*Value=}
		serialnum=${serialnum%?}
		echo "serialnum:$serialnum" >> $DEBUG_FILE
		if [ "$serialnum" != "" ];then
			uci set pon.xpon_auth.sn="$serialnum"
			uci set pon.xpon_auth.def_sn="$serialnum"
		fi
		fi		
		echo 'cmd:/usr/bin/prolinecmd xponmode get | grep "Value="> /tmp/pro_file' >> $DEBUG_FILE
		/usr/bin/prolinecmd xponmode get | grep "Value="> /tmp/pro_file
		xponmode=`cat /tmp/pro_file`
		if [ "$xponmode" != "" ];then
		xponmode=${xponmode#*Value=}
		xponmode=${xponmode%?}
		echo "xponmode:$xponmode" >> $DEBUG_FILE
		if [ "$xponmode" == "EPON" ];then
			uci set pon.xpon_auth.pon_mode="$xponmode"
			uci set pon.xpon_auth.auth_type_e="LOID"
		fi
		fi	
	fi
	uci set pon.xpon_auth.def_setting=0
}

main;
