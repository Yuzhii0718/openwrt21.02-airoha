#!/bin/bash

change_before_feeds() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_after_feeds() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_before_prepare() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_after_prepare() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_config_before_defconfig() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_config_after_defconfig() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_before_autobuild_apply() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

change_after_autobuild_apply() {
	echo "======== virtual.${FUNCNAME[0]} ========"
	echo $*
}

_change_before_feeds() {
	change_before_feeds $*
}

_change_before_prepare() {
	change_before_prepare $*
}

_change_after_prepare() {
	change_after_prepare $*
}

_change_after_feeds() {
	change_after_feeds $*
}

_change_config_before_defconfig() {
	change_config_before_defconfig $*
}

_change_config_after_defconfig() {
	change_config_after_defconfig $*
}

_change_before_autobuild_apply() {
	change_before_autobuild_apply $*
}

_change_after_autobuild_apply() {
	change_after_autobuild_apply $*
}
