#!/bin/sh

echo clean kernel dir!

set -v

pwd
rm -rf ../linux-4.4.115/arch/mips/include/generated
rm -rf ../linux-4.4.115/.tmp_versions
rm -rf ../linux-4.4.115/user_headers
rm -rf ../linux-4.4.115/include/config
rm -rf ../linux-4.4.115/include/generated
rm -rf ../linux-4.4.115/usr/include

find ../linux-4.4.115 -name "*.i" | xargs rm -f
