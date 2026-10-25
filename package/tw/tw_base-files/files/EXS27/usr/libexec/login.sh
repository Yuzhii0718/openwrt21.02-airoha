#!/bin/sh

if [ ! -e /tmp/bootup ]; then
    . /lib/functions/led_control.sh
    platform_led_on green power
    if [ "$(uci get system.@system[0].led_enable)" = "0" ]; then
        /usr/libexec/energy_led.sh &
    fi
    touch /tmp/bootup
fi

[ "$(uci get system.@system[0].ttylogin)" == 1 ] || exec /bin/ash --login

exec /bin/login
