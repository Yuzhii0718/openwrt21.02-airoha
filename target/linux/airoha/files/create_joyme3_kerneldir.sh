#!/bin/sh

echo create joyme3 kernel dir!
if [ "$1" == "" ] ;then
	echo Please inout kernel dir!
	exit 0
fi

KERNEL_DIR=$1
KERNEL_JOYME3_DIR=../linux-3.18.21-joyme3

set -v

pwd

rm -rf $KERNEL_JOYME3_DIR

mkdir -p $KERNEL_JOYME3_DIR

cp -rf ../$KERNEL_DIR/arch $KERNEL_JOYME3_DIR
cp -rf ../$KERNEL_DIR/include $KERNEL_JOYME3_DIR
cp -rf ../$KERNEL_DIR/scripts $KERNEL_JOYME3_DIR
cp  ../$KERNEL_DIR/Makefile $KERNEL_JOYME3_DIR
cp  ../$KERNEL_DIR/Module.symvers $KERNEL_JOYME3_DIR

find $KERNEL_JOYME3_DIR/arch -name "*.c" | xargs rm -f
