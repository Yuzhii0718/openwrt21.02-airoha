#
# Copyright (C) 2010 OpenWrt.org
#

RAMFS_COPY_BIN='fitblk'

PART_NAME=firmware
get_current_partition(){
	MTD=`cat /proc/mtd`
	RESULT=$(echo $MTD | grep ["slave"])	
	if [ "$RESULT" = "" ]; then
		PART_NAME=firmware
	else
		/sbin/ecnt_sys get bootflag /tmp/boot
		BOOT=`cat /tmp/boot`
		STR="1"
		RESULT=$(echo $BOOT | grep "${STR}")	
		if [ "$RESULT" != "" ]; then
			PART_NAME=tclinux
			echo 0 > /tmp/boot
		else
			PART_NAME=tclinux_slave
			echo 1 > /tmp/boot
		fi
#		/sbin/ecnt_sys set bootflag /tmp/boot
	fi
}

get_magic_long_offset() {
	(get_image $1 | dd bs=1 count=4 skip="$2" | hexdump -v -n 4 -e '1/1 "%02x"') 2>/dev/null
}

platform_check_image() {
	[ "$#" -gt 1 ] && return 1

	echo "$1"

	# uImage.FIT (kernel + rootfs in one image) - validated by fit.sh
	if [ "$(get_magic_long "$1")" = "d00dfeed" ]; then
		fit_check_image "$1"
		return $?
	fi

	if [ -f "/proc/tc3162/secure_boot_check_key" ]; then
		FIP_HEADER=`/userfs/bin/blapi_cmd system check_secure_tclinux 0 $1 | awk '{print $9}'`
		if [ $FIP_HEADER == "-1" ]; then
			echo "\r\nSecure header check fail. \r\n"
			echo "$1"
			echo "\r\n"
			return 1
		fi
		GET_MAGIC=$(get_magic_long_offset "$1" $FIP_HEADER)
	else
		GET_MAGIC=$(get_magic_long "$1")
	fi
	
	case "$GET_MAGIC" in
		010064aa | 32524448 | 48445232)
			# Use first full magic number to check
			return 0
			;;
		*)
			echo "\r\nInvalid image type. \r\n"
			echo "$1"
			echo "\r\n"
			echo "$GET_MAGIC"
			echo "\r\n"
			return 1
			;;
	esac
}

platform_do_upgrade() {
	# uImage.FIT: written as a whole into the UBI volume referenced by the
	# device-tree 'chosen/rootdisk' hint (normally named "fit").
	if [ "$(get_magic_long "$1")" = "d00dfeed" ]; then
		fit_do_upgrade "$1"
		return
	fi

	default_do_upgrade "$1"
}
