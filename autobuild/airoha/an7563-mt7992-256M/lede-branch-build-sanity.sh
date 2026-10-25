#!/bin/bash
self=$(dirname -- $(realpath ${BASH_SOURCE:-$0}))
source ${self}/../lede-build-sanity.sh

#========= common variable ====================================================
chip_name="en7563"
arch_name="airoha"
branch_name=an7563-mt7992-256M
build_openwrt=1
prepare_openwrt=1
clean_autobuild=0
cfg80211_on=1
kasan=0
twopcie=1
sku_type=2
idx_log=1
simulated_256M=1
args=
ap_driver_version="8.2.2.2"

#========= customized variable ================================================


#========= parse common arguments =============================================
for arg in $*; do
	case "$arg" in
	"--config")
		clean_autobuild=0
		prepare_openwrt=0
		build_openwrt=0
		;;
	"clean")
		clean_autobuild=1
		prepare_openwrt=0
		build_openwrt=0
		;;
	"prepare")
		prepare_openwrt=1
		build_openwrt=0
		;;
	"build")
		prepare_openwrt=0
		build_openwrt=1
		;;
	*)
		args="$args $arg"
		;;
	esac
done

#========= parse customized arguments =========================================


#========= show arguments =====================================================
set -- $args
echo "======== autobuild ========"
echo "build time          : ${BUILD_TIME}"
echo "openwrt ver         : ${OPENWRT_VER}"
echo "openwrt feed conf   : ${OPENWRT_FEED_CONF} (${MTK_FEED_NAME})"
echo "mtk feeds folder    : ${MTK_FEED_DIR}"
echo "airoha feeds folder : ${AIROHA_FEED_DIR}"
echo "autobuild folder    : ${AUTOBUILD_DIR}/${branch_name}"
echo "build folder        : ${BUILD_DIR}"
echo "-------- options ----------"
echo "clean_autobuild=${clean_autobuild}"
echo "prepare_openwrt=${prepare_openwrt}"
echo "build_openwrt=${build_openwrt}"
echo "==========================="
sleep 2

#========= handle release & releease build ====================================
if [ -n ${1} ]; then
	if [ "${1}" = "release" ] || [ "${1}" = "release_build" ]; then
		echo "temp: ${temp}"
		echo "branch_name: ${branch_name}"
		rel_conf=${BUILD_DIR}/../tools/release_conf/${branch_name}/release.conf
        if [ "$map" = "1" ]; then
		echo "map: ${map}"
		package_conf=${BUILD_DIR}/../tools/release_conf/${branch_name}/package_map.conf
        else
            package_conf=${BUILD_DIR}/../tools/release_conf/${branch_name}/package.conf
        fi
		echo "rel_conf: ${rel_conf}"
		echo "package_conf: ${package_conf}"

		if [ ! -f ${rel_conf} ] || [ ! -f ${package_conf} ]; then
			echo "no release or pakcage config. release terminated"
		else
			source ${rel_conf}
			source ${package_conf}
			source ${BUILD_DIR}/../tools/release.sh
		fi
		exit 0;
	fi
fi

prepare_wifi_package() {
	# GPL-tree adaptation:
	# The vendor flow replaces the in-tree hostapd/mac80211/libnl-tiny/iw/iwinfo
	# with copies from the sibling "mac80211_package" repo (plus patch cleanup in
	# ../mtk-openwrt-feeds). That repo is not part of this GPL release, and the
	# in-tree package/ copies are already the MTK-adapted versions this tree was
	# built with (hostapd Makefile + patches, mac80211 backports-6.5 matching
	# dl/backports-6.5.tar.xz). So the swap steps are skipped and the in-tree
	# package/ versions are used directly.
	[ "$cfg80211_on" = "1" ] && {
		sed -i 's/$(LN) iwconfig $(1)\/usr\/sbin\/iwpriv/$(LN) mwctl $(1)\/usr\/sbin\/iwpriv/g' ${BUILD_DIR}/package/network/utils/wireless-tools/Makefile
	}
}

change_config_after_defconfig() {
	[ "$twopcie" = "0" ] && {
		echo "1pcie"
		sed -i 's/CONFIG_MTK_HWIFI_MT799A=y/CONFIG_MTK_HWIFI_MT799A=n/g' ${BUILD_DIR}/.config
		sed -i 's/CONFIG_PACKAGE_kmod-mt799a=y/# CONFIG_PACKAGE_kmod-mt799a is not set/g' ${BUILD_DIR}/.config
	}

	[ "$twopcie" = "1" ] && {
		echo "2pcie"
		sed -i 's/CONFIG_MTK_HWIFI_MT7992_OPTION_TYPE=0/CONFIG_MTK_HWIFI_MT7992_OPTION_TYPE=1/g' ${BUILD_DIR}/.config
		sed -i 's/CONFIG_MTK_HWIFI_MT7992_INTR_OPTION_SET=0/CONFIG_MTK_HWIFI_MT7992_INTR_OPTION_SET=4/g' ${BUILD_DIR}/.config
	}

	[ "$sku_type" = "1" ] && {
		echo "SKU 6500"
		sed -i 's/CONFIG_MTK_WIFI7_SKU_TYPE="BE7200"/CONFIG_MTK_WIFI7_SKU_TYPE="BE6500"/g' ${BUILD_DIR}/.config
		sed -i 's/mt7992.1.dat/mt7992.6500.1.dat/g' ${BUILD_DIR}/.config
	}

	[ "$sku_type" = "2" ] && {
		echo "SKU 5040"
		sed -i 's/CONFIG_MTK_WIFI7_SKU_TYPE="BE7200"/CONFIG_MTK_WIFI7_SKU_TYPE="BE5040"/g' ${BUILD_DIR}/.config
		sed -i 's/mt7992.1.dat/mt7992.5040.1.dat/g' ${BUILD_DIR}/.config
	}

	[ "$idx_log" = "1" ] && {
		echo "Default FW idx log"
		sed -i 's/CONFIG_MTK_WIFI7_FW_LOG_TYPE="string_log"/CONFIG_MTK_WIFI7_FW_LOG_TYPE="idx_log"/g' ${BUILD_DIR}/.config
	}

}

#========= customized functions ===============================================
change_before_feeds() {
	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
	echo $*

	echo "do_patch ${AUTOBUILD_DIR}/${branch_name}/openwrt_patches${OPENWRT_VER}/"
	do_patch ${AUTOBUILD_DIR}/${branch_name}/openwrt_patches${OPENWRT_VER}/
	rm -rf ${BUILD_DIR}/package/mtk/drivers/hw_nat
}

#change_after_feeds() {
#	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
#	echo $*
#}

change_before_prepare() {
	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
	echo $*

    # Only rewrite the version of the vendor out-of-tree ko_module tree when it
    # is actually there. This release builds the driver from dl/ and has no
    # ko_module/, and the 8.2.2.2 above belongs to those prebuilt modules.
    [ -n "${ap_driver_version}" ] && [ -f "${BUILD_DIR}/../ko_module/wlan_driver/logan/wifi_driver/include/os/rt_linux.h" ] && {
        sed -i "s/#define AP_DRIVER_VERSION.*/#define AP_DRIVER_VERSION \"$ap_driver_version\"/g" ${BUILD_DIR}/../ko_module/wlan_driver/logan/wifi_driver/include/os/rt_linux.h
    }
}

change_after_prepare() {
	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
	echo $*

	# remove airoha default wifi config
	rm -rf ${BUILD_DIR}/target/linux/airoha/en7563/base-files/etc/config/wireless
	rm -rf ${BUILD_DIR}/target/linux/airoha/base-files/etc/config/wireless

	sed -i '/^[ \t]*ifconfig/d' ${BUILD_DIR}/target/linux/airoha/en7563/base-files/etc/init.d/initconfig

	[ "$simulated_256M" = "1" ] && {
		dts_dir=target/linux/airoha/files/arch/arm/boot/dts/
		cp ${dts_dir}/en7552_simulated_256M.dtsi ${BUILD_DIR}/${dts_dir}/en7552.dtsi
	}
}

#change_before_autobuild_apply() {
#	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
#	echo $*
#}

#change_after_autobuild_apply() {
#	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
#	echo $*
#}

#change_config_before_defconfig() {
#	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
#	echo $*
#}

#change_config_after_defconfig() {
#	echo "======== ${branch_name}.${FUNCNAME[0]} ========"
#	echo $*
#}

#========= build openwrt ======================================================
# import from an7563-mt7992-be5000
import_autobuild=${AUTOBUILD_DIR}/an7563-mt7992-be5000
echo "Import autobuild from ${import_autobuild}"
echo "cp -rn ${import_autobuild}/* ${AUTOBUILD_DIR}/${branch_name}/"
cp -rn ${import_autobuild}/* ${AUTOBUILD_DIR}/${branch_name}/

# clean
[ "$clean_autobuild" = "1" ] && {
	autobuild_clean_staging
}

# build log (config name, date, duration; see build_log_start in lede-build-sanity.sh)
build_log_start "${branch_name}"

# prepare
[ "$prepare_openwrt" = "1" ] && {
	#do prepare stuff
	prepare ${branch_name} ${AIROHA_FEED_DIR} ${MTK_FEED_DIR} ${OPENWRT_VER}
	prepare_wifi_package ${branch_name}
	prepare_mtwifi ${branch_name}

	#do prepare_final stuff
	prepare_final ${branch_name} ${AIROHA_FEED_DIR} ${MTK_FEED_DIR} ${OPENWRT_VER}
}

# build
[ "$build_openwrt" = "1" ] && {
	# create dummy tcboot.bin to prevent build fail
	build ${branch_name} -pb || [ "$LOCAL" != "1" ]
}
#==============================================================================
