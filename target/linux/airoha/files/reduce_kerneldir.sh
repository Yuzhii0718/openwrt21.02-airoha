#!/bin/sh

echo clean kernel dir!

set -v

pwd
rm -rf ../linux-4.4.115/arch/alpha
rm -rf ../linux-4.4.115/arch/arc
rm -rf ../linux-4.4.115/arch/arm
rm -rf ../linux-4.4.115/arch/arm64
rm -rf ../linux-4.4.115/arch/avr32
rm -rf ../linux-4.4.115/arch/blackfin
rm -rf ../linux-4.4.115/arch/c6x
rm -rf ../linux-4.4.115/arch/cris
rm -rf ../linux-4.4.115/arch/frv
rm -rf ../linux-4.4.115/arch/h8300
rm -rf ../linux-4.4.115/arch/hexagon
rm -rf ../linux-4.4.115/arch/ia64
rm -rf ../linux-4.4.115/arch/m32r
rm -rf ../linux-4.4.115/arch/m68k
rm -rf ../linux-4.4.115/arch/metag
rm -rf ../linux-4.4.115/arch/microblaze
rm -rf ../linux-4.4.115/arch/mn10300
rm -rf ../linux-4.4.115/arch/nios2
rm -rf ../linux-4.4.115/arch/openrisc
rm -rf ../linux-4.4.115/arch/parisc
rm -rf ../linux-4.4.115/arch/powerpc

rm -rf ../linux-4.4.115/arch/s390
rm -rf ../linux-4.4.115/arch/score
rm -rf ../linux-4.4.115/arch/sh
rm -rf ../linux-4.4.115/arch/sparc
rm -rf ../linux-4.4.115/arch/tile
#rm -rf ../linux-4.4.115/arch/um
rm -rf ../linux-4.4.115/arch/unicore32
#rm -rf ../linux-4.4.115/arch/x86
rm -rf ../linux-4.4.115/arch/xtensa

