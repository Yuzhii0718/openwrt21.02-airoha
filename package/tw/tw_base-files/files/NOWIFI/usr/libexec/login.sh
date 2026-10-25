#!/bin/sh
# nowifi variant: standard OpenWrt console login (root, no password)

[ "$(uci -q get system.@system[0].ttylogin)" = 0 ] && exec /bin/ash --login

exec /bin/login
