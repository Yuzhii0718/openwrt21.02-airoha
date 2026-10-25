#!/bin/sh

echo cp kernel dir!!

KERNEL_DIR=$1

echo "$KERNEL_DIR"

#add p for preserve the specified attributes(mode,ownership,timestamps)
#add L for always follow symbolic links(arm64)
cp -rfpL ./* $KERNEL_DIR/

if [ "$TCSUPPORT_5_4_KERNEL" != "" ];then
	echo cp pcie controller code for linux-5.4.55
	if [ "$TCSUPPORT_CPU_EN7581" != "" ];then
		cp -rfp ./drivers/pci/controller/* $KERNEL_DIR/drivers/pci/controller/
	elif [ "$TCSUPPORT_CPU_AN7552" != "" ];then
		cp -rfp ./drivers/pci/host_7552/* $KERNEL_DIR/drivers/pci/controller/
	else
		cp -rfp ./drivers/pci/host/* $KERNEL_DIR/drivers/pci/controller/
	fi
fi
