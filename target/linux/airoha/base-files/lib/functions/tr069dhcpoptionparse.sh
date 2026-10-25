
####################################################################################################
#   API  to parse TR069 related DHCP options  #
####################################################################################################

UCI_SET="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set"
UCI_GET="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} get"
UCI_COMMIT="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit"

####################################################################################################

dhcp_option_parse() {
	local prov_code acsurl acsuname acspwd minwaitint intmult crurl cruname crpwd perinfenable perinfinterval
	local acsunamev6 acspwdv6 crurlv6 crunamev6 crpwdv6 perinfenablev6 perinfintervalv6
	local addr addr1
	if [ -f /tmp/client_vendor_options ];then
		acsurl=`grep 'acsurl' /tmp/client_vendor_options | cut -d '=' -f 2`
		prov_code=`grep 'provcode' /tmp/client_vendor_options | cut -d '=' -f 2`
		acsuname=`grep -w 'acsuname' /tmp/client_vendor_options | cut -d '=' -f 2`
		acspwd=`grep -w 'acspwd' /tmp/client_vendor_options | cut -d '=' -f 2`
		minwaitint=`grep 'minwaitint' /tmp/client_vendor_options | cut -d '=' -f 2`
		intmult=`grep 'intmult' /tmp/client_vendor_options | cut -d '=' -f 2`
		crurl=`grep -w 'crurl' /tmp/client_vendor_options | cut -d '=' -f 2`
		cruname=`grep -w 'cruname' /tmp/client_vendor_options | cut -d '=' -f 2`
		crpwd=`grep -w 'crpwd' /tmp/client_vendor_options | cut -d '=' -f 2`
		perinfenable=`grep -w 'perinfenable' /tmp/client_vendor_options | cut -d '=' -f 2`
		perinfinterval=`grep -w 'perinfinterval' /tmp/client_vendor_options | cut -d '=' -f 2`
		acsunamev6=`grep -w 'acsunamev6' /tmp/client_vendor_options | cut -d '=' -f 2`
		acspwdv6=`grep -w 'acspwdv6' /tmp/client_vendor_options | cut -d '=' -f 2`
		crurlv6=`grep -w 'crurlv6' /tmp/client_vendor_options | cut -d '=' -f 2`
		crunamev6=`grep -w 'crurlv6' /tmp/client_vendor_options | cut -d '=' -f 2`
		crpwdv6=`grep -w 'crpwdv6' /tmp/client_vendor_options | cut -d '=' -f 2`
		perinfenablev6=`grep -w 'perinfenablev6' /tmp/client_vendor_options | cut -d '=' -f 2`
		perinfintervalv6=`grep -w 'perinfintervalv6' /tmp/client_vendor_options | cut -d '=' -f 2`


		if [ ! -z $prov_code ];then
                	$UCI_SET easycwmp.@local[0].provisioning_code=$prov_code
                fi

                if [ ! -z $acsurl ];then
                        $UCI_SET easycwmp.@acs[0].url=$acsurl
                        addr="$acsurl"
                        addr1=`echo "$addr" | sed -e 's/\\//\\\\\//g'`
                        sed -i -e "s/\(default_management_server_acs_hostname=\).*/\1\"$addr1\"/" /usr/share/easycwmp/defaults
                fi

		if [ ! -z $acsunamev6 ];then
                        $UCI_SET easycwmp.@acs[0].username=$acsunamev6
                elif [ ! -z $acsuname ];then
                        $UCI_SET easycwmp.@acs[0].username=$acsuname
                fi

		if [ ! -z $acspwdv6 ];then
                        $UCI_SET easycwmp.@acs[0].password=$acspwdv6
                elif [ ! -z $acspwd ];then
                        $UCI_SET easycwmp.@acs[0].password=$acspwd
                fi

                if [ ! -z $minwaitint ];then
                        $UCI_SET easycwmp.@acs[0].cwmpretryinterval=$minwaitint
                fi

                if [ ! -z $intmult ];then
                        $UCI_SET easycwmp.@acs[0].cwmpretryintervalmultiplier=$intmult
                fi

                if [ ! -z $crurlv6 ];then
                        $UCI_SET easycwmp.@local[0].ip=$crurlv6
                elif [ ! -z $crurl ];then
                        $UCI_SET easycwmp.@local[0].ip=$crurl
		fi

 		if [ ! -z $crunamev6 ];then
                        $UCI_SET easycwmp.@local[0].username=$crunamev6
		elif [ ! -z $cruname ];then
                        $UCI_SET easycwmp.@local[0].username=$cruname
                fi

                if [ ! -z $crpwdv6 ];then
                        $UCI_SET easycwmp.@local[0].password=$crpwdv6
                elif [ ! -z $crpwd ];then
                        $UCI_SET easycwmp.@local[0].password=$crpwd
		fi

                if [ ! -z $perinfenablev6 ];then
                        $UCI_SET easycwmp.@acs[0].periodic_enable=$perinfenablev6
		elif [ ! -z $perinfenable ];then
                        $UCI_SET easycwmp.@acs[0].periodic_enable=$perinfenable
                fi

                if [ ! -z $perinfintervalv6 ];then
                        $UCI_SET easycwmp.@acs[0].periodic_interval=$perinfintervalv6
		elif [ ! -z $perinfinterval ];then
                        $UCI_SET easycwmp.@acs[0].periodic_interval=$perinfinterval
                fi
			$UCI_COMMIT
        fi
}


