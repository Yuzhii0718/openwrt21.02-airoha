#!/bin/bash
self=$(dirname -- $(realpath ${BASH_SOURCE:-$0}))
source ${self}/scripts/autobuild-hooks.sh
source ${self}/lede-build-sanity-orig-d4d04a8a71.sh
export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

if [ -z ${OPENWRT_VER} ]; then
	echo "Use use OpenWrt-21.02 as default version"
	OPENWRT_VER=-21.02
fi

AUTOBUILD_DIR=${self}
OPENWRT_FEED_CONF=${AUTOBUILD_DIR}/feeds.conf.default${OPENWRT_VER}

MTK_FEED_NAME=mtk_openwrt_feed
AIROHA_FEED_NAME=airoha

MTK_FEED_DIR=${BUILD_DIR}/feeds/${MTK_FEED_NAME}
AIROHA_FEED_DIR=${BUILD_DIR}/feeds/${AIROHA_FEED_NAME}

ENV_MAKE_CPUS=`grep -c ^processor /proc/cpuinfo`
if [ ${ENV_MAKE_CPUS} -gt 8 ]; then
	ENV_MAKE_JOBS=8
else
	ENV_MAKE_JOBS=${ENV_MAKE_CPUS}
fi

autobuild_clean_staging () {
	cd ${BUILD_DIR}
	${AUTOBUILD_DIR}/clean-staging.sh
}

install_output_Image() {
	# Unified output directory: images are consumed directly from
	# bin/targets/... and are deliberately NOT copied to ${INSTALL_DIR}.

	# FIT configs (CONFIG_TARGET_AIROHA_EN7563_FIT_IMAGE) only emit *.itb; the
	# tc*.bin/tc*.img names come from the legacy tclinux packaging.
	files=`find bin/targets/$3/*${2}* -name "tc*.bin" -o -name "tc*.img" -o -name "*.itb"`
	file_count=`echo "$files" | grep -c .`

	if [ ${file_count} = 0 ]; then
		if [ ${build_flag} -eq 0 ]; then
			let  build_flag+=1
			echo " Restart to debug-build with "make V=s -j1", starting......"
			build $1 -j1 || [ "$LOCAL" != "1" ]
		else
			echo " **********Failed to build $1, bin missing.**********"
		fi
	else
		echo "Install image OK!!! (kept in bin/targets/$3)"
		echo "Build $1 successfully!"
	fi
}

do_patch(){
	files=`find $1 -name "*.patch" | sort`
	for file in $files
	do
		echo "patch -f -p1 -i ${file}"
		patch -f -p1 -i ${file} > /dev/null || exit 1
	done
}

copy_main_Config() {
	echo "cp -rfa ${AUTOBUILD_DIR}/$1/.config ${BUILD_DIR}/.config"
	cp -rfa ${AUTOBUILD_DIR}/$1/.config ${BUILD_DIR}/.config
}

# Only the packages under package/tw/ (mainly closed-source / prebuilt) keep a
# stale ipk in the shared bin/packages across config switches: their ipk control
# metadata is not regenerated when DEPENDS changes, so opkg can fail to resolve
# dependencies during package/install. We therefore drop the bin ipks of every
# package whose Makefile lives under package/tw/ whenever the config content
# changes since the last build, so the new build regenerates them with current
# metadata (the control Depends comes from the current Makefile). Other packages
# are left untouched, so the incremental build is unaffected.
autobuild_clean_for_config_switch() {
	local cfg_hash stamp pkg mk
	cfg_hash=$(sha256sum "${AUTOBUILD_DIR}/$1/.config" | cut -d' ' -f1)
	stamp="${BUILD_DIR}/.autobuild_last_config.sha256"
	if [ -f "${stamp}" ] && [ "$(cat "${stamp}")" != "${cfg_hash}" ]; then
		echo "config changed; removing stale tw-package ipks so they regenerate with current metadata"
		while IFS= read -r mk; do
			pkg=$(grep -m1 -E '^PKG_NAME[:?]?=' "$mk" | sed -E 's/^PKG_NAME[:?]?=//; s/[[:space:]]//g')
			[ -z "$pkg" ] && continue
			rm -f "${BUILD_DIR}"/bin/packages/*/*/"${pkg}_"*.ipk
		done < <(find "${BUILD_DIR}/package/tw" -name Makefile 2>/dev/null)
	fi
	echo "${cfg_hash}" > "${stamp}"
}

prepare() {
	echo "Preparing....${ENV_MAKE_JOBS}"
	cd ${BUILD_DIR}

	#@change_before_feeds
	_change_before_feeds $*

	# handle feeds
	echo "cp ${OPENWRT_FEED_CONF} ${BUILD_DIR}/feeds.conf.default"
	cp ${OPENWRT_FEED_CONF} ${BUILD_DIR}/feeds.conf.default

	# update feed
	#echo "${BUILD_DIR}/scripts/feeds update -a"
	#${BUILD_DIR}/scripts/feeds update -a
	# feeds/routing and feeds/packages are not tracked in git: fetch them at the
	# commit pinned in feeds.conf.default (only if they are missing) and apply the
	# product delta from feeds-patches/. Idempotent. Must run before "update -i",
	# otherwise a fresh checkout has no feed directory to index.
	${BUILD_DIR}/scripts/feeds-vendor-patch.sh

	# local additions under feeds/ (e.g. luci-theme-argon) must be re-indexed,
	# otherwise "feeds install -a" does not see them. -i only rebuilds the
	# index, it never re-clones the feed.
	${BUILD_DIR}/scripts/feeds update -i

	# install feed
	${BUILD_DIR}/scripts/feeds install -a
	${BUILD_DIR}/scripts/feeds install -a luci

	#@change_after_feeds
	_change_after_feeds $*

	#@change_before_prepare
	_change_before_prepare $*

	# The vendor's prepare_sdk.sh used to copy feeds/airoha/{openwrt_files,
	# package,tools} and feeds/mtk_openwrt_feed/prepare_sdk.sh into the tree.
	# The airoha feed now lives in package/ and target/, and neither of those
	# sources exists any more, so the hook chain above is the only thing that
	# still lands configuration.

	#@change_after_prepare
	_change_after_prepare $*
}

prepare_mtwifi() {
	#remove officail OpenWRT wifi script
	#wifi-profile pkg will install wifi_jedi instead
	# sed -i '/^[\t ]*\[[ ]*![ ]*-f[ ]*\/etc\/config\/wireless[ ]*\]/a \\t\t\/sbin\/wifi detect' ${BUILD_DIR}/package/base-files/files/etc/init.d/boot
	#rm -rf ${BUILD_DIR}/package/base-files/files/sbin/wifi

	add_proprietary_kernel_files

	#do mtk_wifi openwrt patch
	do_patch ${BUILD_DIR}/autobuild/openwrt_patches${OPENWRT_VER}/mtk_wifi || exit 1
}

prepare_final() {
	# customized autobuild
	_change_before_autobuild_apply $*
	# copy and use specify SoC autobuild instead of original
	echo "cp -fpR ${AUTOBUILD_DIR}/$1/target/ ${BUILD_DIR}"
	cp -fpR ${AUTOBUILD_DIR}/$1/target/ ${BUILD_DIR}
	echo "cp -fpR ${AUTOBUILD_DIR}/$1/package/ ${BUILD_DIR}"
	cp -fpR ${AUTOBUILD_DIR}/$1/package/ ${BUILD_DIR}
	echo "cp -fpR ${AUTOBUILD_DIR}/$1/tools/ ${BUILD_DIR}"
	cp -fpR ${AUTOBUILD_DIR}/$1/tools/ ${BUILD_DIR}
	echo "cp -fpR ${AUTOBUILD_DIR}/$1/feeds/ ${BUILD_DIR}"
	cp -fpR ${AUTOBUILD_DIR}/$1/feeds/ ${BUILD_DIR}
	_change_after_autobuild_apply $*

	cd ${BUILD_DIR}
	[ -f ${AUTOBUILD_DIR}/$1/.config ] || {
		echo "unable to locate ${AUTOBUILD_DIR}/$1/.config !"
		return
	}

	rm -rf ./tmp
	# copy main test config(.config)
	copy_main_Config $1

	# Drop stale shared bin/ output left by a different config so this build
	# regenerates consistent ipks (see autobuild_clean_for_config_switch).
	autobuild_clean_for_config_switch $1

	_change_config_before_defconfig $*
	echo "make defconfig"
	make defconfig
	# "make defconfig" runs 'conf --defconfig', which drops every symbol that has
	# no Kconfig definition. In this GPL tree that is the entire MTK_* set
	# (WiFi7 / MAP / HWIFI ...). Re-apply the enabled ones from the board .config
	# so the build actually sees them (the build's conf keeps unknown symbols).
	if [ -f "${AUTOBUILD_DIR}/$1/.config" ]; then
		grep -E '^CONFIG_[A-Za-z0-9_]+=' "${AUTOBUILD_DIR}/$1/.config" | while IFS= read -r _cl; do
			_ck="${_cl%%=*}"
			grep -q "^${_ck}=" .config || printf '%s\n' "$_cl" >> .config
		done
	fi
	_change_config_after_defconfig $*
}

build() {
	echo "###############################################################################"
	echo "# $1"
	echo "###############################################################################"
	echo "build $1"

	cd ${BUILD_DIR}

	#echo ln -s ../dl dl
	#ln -s ../dl dl
	echo "make MSDK=1 V=s -j${ENV_MAKE_JOBS} PKG_HASH=skip PKG_MIRROR_HASH=skip"
	make MSDK=1 V=s -j${ENV_MAKE_JOBS} PKG_HASH=skip PKG_MIRROR_HASH=skip || exit 1

	#install output image
	install_output_Image $1 ${chip_name} ${arch_name}
}

#========= build logging ======================================================
# Every run mirrors the whole output (prepare and build alike) into one file,
# build-logs/<config>-<timestamp>.log, with a header that identifies the config,
# date, tree/commit and host, and a footer with the result and the wall-clock
# duration. Call build_log_start once, after the per-config variables and the
# argument parsing are done.
BUILD_LOG_DIR=${BUILD_LOG_DIR:-${BUILD_DIR}/build-logs}
BUILD_LOG_ACTIVE=0

build_log_start () {
	local cfg="$1"
	local stamp commit dirty mode

	[ "${BUILD_LOG_ACTIVE}" = "1" ] && return 0
	BUILD_LOG_ACTIVE=1

	mkdir -p "${BUILD_LOG_DIR}" 2>/dev/null || { BUILD_LOG_ACTIVE=0; return 0; }

	# One invocation already covers every stage it runs, so the stage is only
	# recorded in the header/footer, not in the file name.
	if [ "${clean_autobuild}" = "1" ]; then
		mode="clean"
	elif [ "${prepare_openwrt}" = "1" ] && [ "${build_openwrt}" = "1" ]; then
		mode="prepare-build"
	elif [ "${prepare_openwrt}" = "1" ]; then
		mode="prepare"
	elif [ "${build_openwrt}" = "1" ]; then
		mode="build"
	else
		mode="config"
	fi
	BUILD_LOG_MODE="${mode}"

	stamp=$(date '+%Y%m%d-%H%M%S')
	BUILD_LOG_FILE="${BUILD_LOG_DIR}/${cfg:-unknown}-${stamp}.log"
	BUILD_LOG_START=$(date +%s)

	commit=$(git -C "${BUILD_DIR}" rev-parse --short HEAD 2>/dev/null)
	[ -n "$(git -C "${BUILD_DIR}" status --porcelain 2>/dev/null | head -1)" ] && dirty=" +uncommitted"

	{
		echo "===================== build log ====================="
		echo "config    : ${cfg:-unknown}"
		echo "mode      : ${mode} (prepare=${prepare_openwrt:-0} build=${build_openwrt:-0} clean=${clean_autobuild:-0} kasan=${kasan:-0})"
		echo "date      : $(date '+%Y-%m-%d %H:%M:%S %z')"
		echo "host      : $(uname -srm), $(nproc) cpus, jobs=${ENV_MAKE_JOBS:-?}"
		echo "tree      : ${BUILD_DIR}"
		echo "commit    : ${commit:-unknown}${dirty}"
		echo "args      : ${args:-<none>}"
		echo "script    : $0"
		echo "-----------------------------------------------------"
	} >> "${BUILD_LOG_FILE}"

	exec > >(tee -a "${BUILD_LOG_FILE}") 2>&1
	trap 'build_log_finish $?' EXIT

	echo "build log: ${BUILD_LOG_FILE}"
}

build_log_finish () {
	local rc="$1"
	local dur result

	[ "${BUILD_LOG_ACTIVE}" = "1" ] || return 0
	dur=$(( $(date +%s) - BUILD_LOG_START ))

	if [ "${build_openwrt}" = "1" ]; then
		[ "${rc}" -eq 0 ] && result="SUCCESS" || result="FAILED"
	else
		case "${BUILD_LOG_MODE}" in
		prepare) result="PREPARED (build not requested)" ;;
		clean)   result="CLEANED (no build requested)" ;;
		config)  result="CONFIGURED (no build requested)" ;;
		*)       result="OK (no build requested)" ;;
		esac
	fi

	echo "-----------------------------------------------------"
	if [ "${result}" = "SUCCESS" ]; then
		echo "result    : ${result}"
	else
		echo "result    : ${result} (exit ${rc})"
	fi
	printf 'duration  : %02d:%02d:%02d (%ss)\n' \
		$((dur / 3600)) $(((dur % 3600) / 60)) $((dur % 60)) "${dur}"
	echo "finished  : $(date '+%Y-%m-%d %H:%M:%S %z')"
	echo "log file  : ${BUILD_LOG_FILE}"
	echo "====================================================="
}
