# Copyright (C) 2014 OpenWrt.org
#

. /lib/functions.sh

# 'kernel' partition on NAND contains the kernel
CI_KERNPART="${CI_KERNPART:-kernel}"

# 'ubi' partition on NAND contains UBI
CI_UBIPART="${CI_UBIPART:-ubi}"

# 'rootfs' partition on NAND contains the rootfs
CI_ROOTPART="${CI_ROOTPART:-rootfs}"

ubi_mknod() {
	local dir="$1"
	local dev="/dev/$(basename $dir)"

	[ -e "$dev" ] && return 0

	local devid="$(cat $dir/dev)"
	local major="${devid%%:*}"
	local minor="${devid##*:}"
	mknod "$dev" c $major $minor
}

nand_find_volume() {
	local ubidevdir ubivoldir
	ubidevdir="/sys/devices/virtual/ubi/$1"
	[ ! -d "$ubidevdir" ] && return 1
	for ubivoldir in $ubidevdir/${1}_*; do
		[ ! -d "$ubivoldir" ] && continue
		if [ "$( cat $ubivoldir/name )" = "$2" ]; then
			basename $ubivoldir
			ubi_mknod "$ubivoldir"
			return 0
		fi
	done
}

nand_remove_ubiblock() {
	local ubivol="$1"
	local ubiblk="ubiblock${ubivol:3}"
	local dev

	# This kernel is built without devtmpfs, so block devices created after
	# the boot-time mdev scan have no /dev node. Create it from sysfs first,
	# otherwise the ubiblock would be skipped and the volume stays busy.
	if [ ! -e "/dev/$ubiblk" ] && [ -e "/sys/class/block/$ubiblk/dev" ]; then
		dev="$(cat "/sys/class/block/$ubiblk/dev" 2>/dev/null)"
		[ -n "$dev" ] && mknod "/dev/$ubiblk" b "${dev%%:*}" "${dev##*:}" 2>/dev/null
	fi

	if [ -e "/dev/$ubiblk" ]; then
		umount "/dev/$ubiblk" && echo "unmounted /dev/$ubiblk" || :
		if ! ubiblock -r "/dev/$ubivol"; then
			echo "cannot remove $ubiblk"
			return 1
		fi
	fi
	return 0
}

nand_find_ubi() {
	local ubidevdir ubidev mtdnum
	mtdnum="$( find_mtd_index $1 )"
	[ ! "$mtdnum" ] && return 1
	for ubidevdir in /sys/devices/virtual/ubi/ubi*; do
		[ ! -d "$ubidevdir" ] && continue
		cmtdnum="$( cat $ubidevdir/mtd_num )"
		[ ! "$mtdnum" ] && continue
		if [ "$mtdnum" = "$cmtdnum" ]; then
			ubidev=$( basename $ubidevdir )
			ubi_mknod "$ubidevdir"
			echo $ubidev
			return 0
		fi
	done
}

nand_get_magic_long() {
	dd if="$1" skip=$2 bs=4 count=1 2>/dev/null | hexdump -v -n 4 -e '1/1 "%02x"'
}

get_magic_long_tar() {
	( tar xf $1 $2 -O | dd bs=4 count=1 | hexdump -v -n 4 -e '1/1 "%02x"') 2> /dev/null
}

identify_magic() {
	local magic=$1
	case "$magic" in
		"55424923")
			echo "ubi"
			;;
		"31181006")
			echo "ubifs"
			;;
		"68737173")
			echo "squashfs"
			;;
		"d00dfeed")
			echo "fit"
			;;
		"4349"*)
			echo "combined"
			;;
		*)
			echo "unknown $magic"
			;;
	esac
}


identify() {
	identify_magic $(nand_get_magic_long "$1" "${2:-0}")
}

identify_tar() {
	identify_magic $(get_magic_long_tar "$1" "$2")
}

nand_restore_config() {
	sync
	local ubidev=$( nand_find_ubi $CI_UBIPART )
	local ubivol="$( nand_find_volume $ubidev rootfs_data )"
	[ ! "$ubivol" ] &&
		ubivol="$( nand_find_volume $ubidev $CI_ROOTPART )"
	mkdir /tmp/new_root
	if ! mount -t ubifs /dev/$ubivol /tmp/new_root; then
		echo "mounting ubifs $ubivol failed"
		rmdir /tmp/new_root
		return 1
	fi
	mv "$1" "/tmp/new_root/$BACKUP_FILE"
	umount /tmp/new_root
	sync
	rmdir /tmp/new_root
}

# $(1): rootfs image length (empty to keep/reuse the existing rootfs volume)
# $(2): rootfs type ("ubifs" or "squashfs")
# $(3): 1 to (re)create the kernel volume
# $(4): 1 to create ubootenv volumes on a freshly formatted UBI
# $(5): kernel image length (defaults to the caller's $kernel_length)
nand_upgrade_prepare_ubi() {
	local rootfs_length="$1"
	local rootfs_type="$2"
	local has_kernel="${3:-0}"
	local has_env="${4:-0}"
	local kernel_length="${5:-$kernel_length}"

	local mtdnum="$( find_mtd_index "$CI_UBIPART" )"
	if [ ! "$mtdnum" ]; then
		echo "cannot find ubi mtd partition $CI_UBIPART"
		return 1
	fi

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	if [ ! "$ubidev" ]; then
		ubiattach -m "$mtdnum"
		sync
		ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	fi

	if [ ! "$ubidev" ]; then
		ubiformat /dev/mtd$mtdnum -y
		ubiattach -m "$mtdnum"
		sync
		ubidev="$( nand_find_ubi "$CI_UBIPART" )"
		[ "$has_env" -gt 0 ] && {
			ubimkvol /dev/$ubidev -n 0 -N ubootenv -s 1MiB
			ubimkvol /dev/$ubidev -n 1 -N ubootenv2 -s 1MiB
		}
	fi

	local kern_ubivol="$( nand_find_volume $ubidev $CI_KERNPART )"
	local root_ubivol="$( nand_find_volume $ubidev $CI_ROOTPART )"
	local data_ubivol="$( nand_find_volume $ubidev rootfs_data )"

	# The FIT flow releases /dev/fit0 (fitblk) but that only drops the fitblk
	# device - the ubiblock underneath stays attached and keeps the "fit"
	# UBI volume busy, so "ubirmvol -N fit" would fail with EBUSY and the
	# following "ubimkvol -N fit" would abort the upgrade. Remove the
	# ubiblock of every volume we are about to delete/recreate first.
	[ "$root_ubivol" = "$kern_ubivol" ] && root_ubivol=

	[ "$kern_ubivol" ] && { nand_remove_ubiblock "$kern_ubivol" || return 1; }
	[ "$root_ubivol" ] && { nand_remove_ubiblock "$root_ubivol" || return 1; }
	[ "$data_ubivol" ] && { nand_remove_ubiblock "$data_ubivol" || return 1; }

	# kill volumes
	[ "$kern_ubivol" ] && ubirmvol /dev/$ubidev -N $CI_KERNPART || true
	[ "$root_ubivol" ] && ubirmvol /dev/$ubidev -N $CI_ROOTPART || true
	[ "$data_ubivol" ] && ubirmvol /dev/$ubidev -N rootfs_data || true

	# update kernel
	if [ "$has_kernel" = "1" ]; then
		if ! ubimkvol /dev/$ubidev -N $CI_KERNPART -s $kernel_length; then
			echo "cannot create kernel volume"
			return 1;
		fi
	fi

	# update rootfs
	if [ -n "$rootfs_length" ]; then
		local root_size_param
		if [ "$rootfs_type" = "ubifs" ]; then
			root_size_param="-m"
		else
			root_size_param="-s $rootfs_length"
		fi
		if ! ubimkvol /dev/$ubidev -N $CI_ROOTPART $root_size_param; then
			echo "cannot create rootfs volume"
			return 1;
		fi
	fi

	# create rootfs_data for non-ubifs rootfs. The FIT flow passes an empty
	# rootfs_length, but nand_restore_config still needs a fresh volume to
	# store the config backup, otherwise the restore fails with EBUSY on the
	# old (still mounted) volume.
	if [ "$rootfs_type" != "ubifs" ]; then
		if ! ubimkvol /dev/$ubidev -N rootfs_data -m; then
			echo "cannot initialize rootfs_data volume"
			# For the FIT flow the firmware itself is still written below;
			# aborting here would leave the device on the old image just
			# because the config restore cannot be prepared.
			[ -n "$rootfs_length" ] && return 1
			echo "continuing without rootfs_data, config backup will not be restored"
		fi
	fi
	sync
	return 0
}

# Write the FIT image (kernel + rootfs in one uImage.FIT) to the UBI volume
# it is booted from. The volume name comes from CI_KERNPART which is normally
# exported by fit.sh from the device-tree 'chosen/rootdisk' hint.
nand_upgrade_fit() {
	local fit_file="$1"
	local cmd="${2:-cat}"

	local fit_length=$( ($cmd < "$fit_file" | wc -c) 2> /dev/null)

	nand_upgrade_prepare_ubi "" "" "1" "1" "$fit_length" || return 1

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	local fit_ubivol="$(nand_find_volume $ubidev "$CI_KERNPART")"
	$cmd < "$fit_file" | ubiupdatevol /dev/$fit_ubivol -s $fit_length -

	nand_do_upgrade_success
}

nand_do_upgrade_success() {
	local conf_tar="/tmp/sysupgrade.tgz"

	sync
	[ -f "$conf_tar" ] && nand_restore_config "$conf_tar"
	echo "sysupgrade successful"
	umount -a
	reboot -f
}

# Flash the UBI image to MTD partition
nand_upgrade_ubinized() {
	local ubi_file="$1"
	local mtdnum="$(find_mtd_index "$CI_UBIPART")"

	[ ! "$mtdnum" ] && {
		CI_UBIPART="rootfs"
		mtdnum="$(find_mtd_index "$CI_UBIPART")"
	}

	if [ ! "$mtdnum" ]; then
		echo "cannot find mtd device $CI_UBIPART"
		umount -a
		reboot -f
	fi

	local mtddev="/dev/mtd${mtdnum}"
	ubidetach -p "${mtddev}" || true
	sync
	ubiformat "${mtddev}" -y -f "${ubi_file}"
	ubiattach -p "${mtddev}"
	nand_do_upgrade_success
}

# Write the UBIFS image to UBI volume
nand_upgrade_ubifs() {
	local rootfs_length=$( (cat $1 | wc -c) 2> /dev/null)

	nand_upgrade_prepare_ubi "$rootfs_length" "ubifs" "0" "0"

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	local root_ubivol="$(nand_find_volume $ubidev $CI_ROOTPART)"
	ubiupdatevol /dev/$root_ubivol -s $rootfs_length $1

	nand_do_upgrade_success
}

nand_upgrade_tar() {
	local tar_file="$1"
	local kernel_mtd="$(find_mtd_index $CI_KERNPART)"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	local kernel_length=$( (tar xf $tar_file ${board_dir}/kernel -O | wc -c) 2> /dev/null)
	local rootfs_length=$( (tar xf $tar_file ${board_dir}/root -O | wc -c) 2> /dev/null)

	local rootfs_type="$(identify_tar "$tar_file" ${board_dir}/root)"

	local has_kernel=1
	local has_env=0

	[ "$kernel_length" != 0 -a -n "$kernel_mtd" ] && {
		tar xf $tar_file ${board_dir}/kernel -O | mtd write - $CI_KERNPART
	}
	[ "$kernel_length" = 0 -o ! -z "$kernel_mtd" ] && has_kernel=0

	nand_upgrade_prepare_ubi "$rootfs_length" "$rootfs_type" "$has_kernel" "$has_env"

	local ubidev="$( nand_find_ubi "$CI_UBIPART" )"
	[ "$has_kernel" = "1" ] && {
		local kern_ubivol="$(nand_find_volume $ubidev $CI_KERNPART)"
		tar xf $tar_file ${board_dir}/kernel -O | \
			ubiupdatevol /dev/$kern_ubivol -s $kernel_length -
	}

	local root_ubivol="$(nand_find_volume $ubidev $CI_ROOTPART)"
	tar xf $tar_file ${board_dir}/root -O | \
		ubiupdatevol /dev/$root_ubivol -s $rootfs_length -

	nand_do_upgrade_success
}

# Recognize type of passed file and start the upgrade process
nand_do_upgrade() {
	local file_type=$(identify $1)

	[ ! "$(find_mtd_index "$CI_UBIPART")" ] && CI_UBIPART="rootfs"

	case "$file_type" in
		"ubi")		nand_upgrade_ubinized $1;;
		"ubifs")	nand_upgrade_ubifs $1;;
		"fit")		nand_upgrade_fit $1;;
		*)		nand_upgrade_tar $1;;
	esac
}

# Check if passed file is a valid one for NAND sysupgrade. Currently it accepts
# 4 types of files:
# 1) UBI - should contain an ubinized image, header is checked for the proper
#    MAGIC
# 2) UBIFS - should contain UBIFS partition that will replace "rootfs" volume,
#    header is checked for the proper MAGIC
# 3) FIT - should contain a uImage.FIT holding kernel and rootfs, written to
#    the UBI volume the device boots from
# 4) TAR - archive has to include "sysupgrade-BOARD" directory with a non-empty
#    "CONTROL" file (at this point its content isn't verified)
#
# You usually want to call this function in platform_check_image.
#
# $(1): board name, used in case of passing TAR file
# $(2): file to be checked
nand_do_platform_check() {
	local board_name="$1"
	local tar_file="$2"
	local control_length=$( (tar xf $tar_file sysupgrade-$board_name/CONTROL -O | wc -c) 2> /dev/null)
	local file_type="$(identify $2)"

	[ "$control_length" = 0 -a "$file_type" != "ubi" -a "$file_type" != "ubifs" \
		-a "$file_type" != "fit" ] && {
		echo "Invalid sysupgrade file."
		return 1
	}

	return 0
}
