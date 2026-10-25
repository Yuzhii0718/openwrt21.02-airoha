#!/bin/sh

. /lib/functions/leds.sh
board="exs27"

platform_led_on() {
    if [ -e /tmp/.led_protest ]; then
        exit 0
    fi
    color=$1
    led=$2
    led_off ${board}:red:${led}
    led_off ${board}:green:${led}
    led_off ${board}:amber:${led}
    led_on ${board}:${color}:${led}
    case ${color} in
    amber)
    led_on ${board}:red:${led}
    led_on ${board}:green:${led}
    ;;
    esac
}

platform_led_off() {
    if [ -e /tmp/.led_protest ]; then
        exit 0
    fi
    color=$1
    led=$2
    led_off ${board}:red:${led}
    led_off ${board}:green:${led}
    led_off ${board}:amber:${led}
}

platform_led_set_timer() {
    if [ -e /tmp/.led_protest ]; then
        exit 0
    fi
    color=$1
    led=$2
    delay_on=$3
    delay_off=$4
    led_off ${board}:red:${led}
    led_off ${board}:green:${led}
    led_off ${board}:amber:${led}
    led_timer ${board}:${color}:${led} $delay_on $delay_off
}


platform_protest_on() {
    touch /tmp/.led_protest
    platform_set_led_power_state all 0
}


platform_protest_off() {
    rm /tmp/.led_protest
    platform_set_led_power_state all 2
}


platform_set_led_power_state() {
    led=$1
    case $led in
    all)
    state=$2
    led_set_attr ${board}:green:power power_ctl $state
    led_set_attr ${board}:green:wps power_ctl $state
    led_set_attr ${board}:green:clink power_ctl $state
    led_set_attr ${board}:green:rlink power_ctl $state
    #led_set_attr ${board}:green:speed power_ctl $state
    led_set_attr ${board}:red:power power_ctl $state
    led_set_attr ${board}:red:clink power_ctl $state
    led_set_attr ${board}:red:rlink power_ctl $state
    #led_set_attr ${board}:yellow:speed power_ctl $state
    if [ ${state} == "1" ]; then
        tce emiiw 15 0x1f 24 c1c1
        tce emiiw 15 0x1f 26 c042
    elif [ ${state} == "0" ]; then
        tce emiiw 15 0x1f 24 41c1
        tce emiiw 15 0x1f 26 4042
    elif [ ${state} == "2" ]; then
        tce emiiw 15 0x1f 24 c181
        tce emiiw 15 0x1f 26 c002
    fi
    ;;
    ntgr_all)
    state=$2
    led_set_attr ${board}:green:wps power_ctl $state
    led_set_attr ${board}:green:clink power_ctl $state
    led_set_attr ${board}:green:rlink power_ctl $state
    #led_set_attr ${board}:green:speed power_ctl $state
    led_set_attr ${board}:red:power power_ctl $state
    led_set_attr ${board}:red:clink power_ctl $state
    led_set_attr ${board}:red:rlink power_ctl $state
    #led_set_attr ${board}:yellow:speed power_ctl $state
    if [ ${state} == "1" ]; then
        tce emiiw 15 0x1f 24 c1c1
        tce emiiw 15 0x1f 26 c042
    elif [ ${state} == "0" ]; then
        tce emiiw 15 0x1f 24 41c1
        tce emiiw 15 0x1f 26 4042
    elif [ ${state} == "2" ]; then
        tce emiiw 15 0x1f 24 c181
        tce emiiw 15 0x1f 26 c002
    fi
    ;;
    speed)
    color=$2
    state=$3
    if [ ${color} == "green" ]; then
        if [ ${state} == "1" ]; then
            tce emiiw 15 0x1f 24 c1c1
        elif [ ${state} == "0" ]; then
            tce emiiw 15 0x1f 24 41c1
        elif [ ${state} == "2" ]; then
            tce emiiw 15 0x1f 24 c181
        fi
    else
        if [ ${state} == "1" ]; then
            tce emiiw 15 0x1f 26 c042
        elif [ ${state} == "0" ]; then
            tce emiiw 15 0x1f 26 4042
        elif [ ${state} == "2" ]; then
            tce emiiw 15 0x1f 26 c002
        fi
    fi
    ;;
    power|wps|clink|rlink)
    color=$2
    state=$3
    led_set_attr ${board}:${color}:${led} power_ctl $state
    ;;
    esac
}
