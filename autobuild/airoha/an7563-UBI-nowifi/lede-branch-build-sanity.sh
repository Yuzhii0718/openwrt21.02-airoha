#!/bin/bash
self=$(dirname -- $(realpath ${BASH_SOURCE:-$0}))
source ${self}/../lede-build-sanity.sh

#========= common variable ====================================================
chip_name="en7563"
arch_name="airoha"
temp=${0%/*}
branch_name=${temp##*/}
build_openwrt=1
prepare_openwrt=1
clean_autobuild=0
cfg80211_on=0
kasan=0
twopcie=0
sku_type=0
idx_log=1
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
		# Only compile: skip the (expensive) prepare stage. The build tree is
		# expected to have been prepared by an earlier "prepare" run.
		prepare_openwrt=0
		build_openwrt=1
		;;
	"noprepare")
		# Skip prepare but keep the default build action; can be combined with
		# other options, e.g. "./lede-branch-build-sanity.sh noprepare gsw".
		prepare_openwrt=0
		;;
	"map")
		map=1
		;;
	"gsw")
		gsw=1
		;;
	"andlink")
		andlink=1
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
		#if [ "$map" = "1" ]; then
		echo "map: ${map}"
		package_conf=${BUILD_DIR}/../tools/release_conf/${branch_name}/package_map.conf
		#else
		#    package_conf=${BUILD_DIR}/../tools/release_conf/${branch_name}/package.conf
		#fi
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

	#[ "$idx_log" = "1" ] && {
		#echo "Default FW idx log"
		#sed -i 's/CONFIG_MTK_WIFI7_FW_LOG_TYPE="string_log"/CONFIG_MTK_WIFI7_FW_LOG_TYPE="idx_log"/g' ${BUILD_DIR}/.config
	#}

	[ "$map" = "1" ] && {
		echo "map enable"
		sed -i 's/# CONFIG_PACKAGE_1905daemon is not set/CONFIG_PACKAGE_1905daemon=y/g' ${BUILD_DIR}/.config
		sed -i 's/# CONFIG_PACKAGE_mapd is not set/CONFIG_PACKAGE_mapd=y/g' ${BUILD_DIR}/.config
		sed -i 's/# CONFIG_PACKAGE_kmod-mapfilter is not set/CONFIG_PACKAGE_kmod-mapfilter=y/g' ${BUILD_DIR}/.config
		sed -i 's/# CONFIG_PACKAGE_libmapd is not set/CONFIG_PACKAGE_libmapd=y/g' ${BUILD_DIR}/.config
		sed -i 's/# CONFIG_PACKAGE_ip-bridge is not set/CONFIG_PACKAGE_ip-bridge=y/g' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MAP_R2_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MAP_R3_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MAP_R4_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MAP_R5_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_PACKAGE_mapd=y/a CONFIG_MAP_R5_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_PACKAGE_mapd=y/a CONFIG_MAP_R4_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_PACKAGE_mapd=y/a CONFIG_MAP_R3_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_PACKAGE_mapd=y/a CONFIG_MAP_R2_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R2_VER_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R3_VER_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R4_VER_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R5_VER_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R2_6E_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_R3_6E_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_HOSTAPD_SUPPORT/d' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_HOSTAPD_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R3_6E_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R2_6E_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R5_VER_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R4_VER_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R3_VER_SUPPORT=y' ${BUILD_DIR}/.config
		sed -i '/CONFIG_MTK_WIFI7_MAP_SUPPORT=y/a CONFIG_MTK_WIFI7_MAP_R2_VER_SUPPORT=y' ${BUILD_DIR}/.config
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

	[ "$gsw" = "1" ] && {
		echo "build gsw"
		sed -i '/TCSUPPORT_SEPERATED_ETH_ITF=y/d' ${AUTOBUILD_DIR}/${branch_name}/target/linux/airoha/en7563/UNION_EN7563_MULTI_CHIP_KERNEL_5_4_demo.mak
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
	rm ${BUILD_DIR}/autobuild/openwrt_patches${OPENWRT_VER}/mtk_wifi/*miniupnpd*
	prepare_mtwifi ${branch_name}

	#do prepare_final stuff
	prepare_final ${branch_name} ${AIROHA_FEED_DIR} ${MTK_FEED_DIR} ${OPENWRT_VER}
}

[ "$andlink" = "1" ] && {
	echo "build andlink"
	cd ${BUILD_DIR}/package/mtk/applications && git clone "https://gerrit.mediatek.inc/neptune/wlan_daemon/rmt_mgmtd" && 
	cd ${BUILD_DIR}/package/mtk/applications/rmt_mgmtd && git checkout -b dev origin/dev &&	cd -
	rm -rf ${BUILD_DIR}/package/feeds/packages/miniupnpd/patches
	sed -i 's/# CONFIG_PACKAGE_nodogsplash is not set/CONFIG_PACKAGE_nodogsplash=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_libmicrohttpd-no-ssl is not set/CONFIG_PACKAGE_libmicrohttpd-no-ssl=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_libmapd is not set/CONFIG_PACKAGE_libmapd=y/g' ${BUILD_DIR}/.config
	sed -i '/CONFIG_PACKAGE_regs=y/a CONFIG_PACKAGE_rmt_mgmtd=y' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_kmod-ebtables is not set/CONFIG_PACKAGE_kmod-ebtables=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_kmod-ebtables-ipv4 is not set/CONFIG_PACKAGE_kmod-ebtables-ipv4=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_kmod-ebtables-ipv6 is not set/CONFIG_PACKAGE_kmod-ebtables-ipv6=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_ebtables is not set/CONFIG_PACKAGE_ebtables=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_miniupnpd is not set/CONFIG_PACKAGE_miniupnpd=y/g' ${BUILD_DIR}/.config
	sed -i 's/# CONFIG_PACKAGE_libcap-ng is not set/CONFIG_PACKAGE_libcap-ng=y/g' ${BUILD_DIR}/.config
}

# build
[ "$build_openwrt" = "1" ] && {
	# create dummy tcboot.bin to prevent build fail
	build ${branch_name} -pb || [ "$LOCAL" != "1" ]
}
#==============================================================================
