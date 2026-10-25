#!/bin/sh
set -eu
# NOTE: For this script to work properly, the feature name should be the same as the field name in featureSwitch DAL table
help() {

    cat <<EOF
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
-=- Unified Feature Switch Utility Tool -=-
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

Usage:
 feature_switch.sh <Supported action> <Supported feature>

Eg. feature_switch.sh enable dile
    feature_switch.sh show dile
    feature_switch.sh show

Supported Features:
  dal_cob               -- Cloud On-Boarding
  dile                  -- DIL Enhancement
  rp                    -- Router Protection
  armorra               -- Armor Router Analytics
  gamebooster           -- GameBooster (Netduma)
  rsu_updates           -- Package updates from RSU cloud service (ONLY available on SEAL based routers)

Supported actions:
  enable                -- enable a feature
  disable               -- disable a feature
  show                  -- display states of feature(s)

EOF
}

list_contains() {
    list="$1"
    item="$2"
    for list_item in $list; do
        if [ "$item" = "$list_item" ]; then
            return 0
        fi
    done
    return 1
}

validate_feature() {
    feature="$2"
    available_features="dal_cob dile rp armorra rsu_updates gamebooster"
    if [ "$feature" != "" ] && ! list_contains "$available_features" "$feature"; then
        logger -s -t feature_switch.sh "Error: Invalid feature: '$2'."
        help
        exit 1
    fi
}

validate_action() {
    action="$1"
    available_actions="enable disable show"
    if [ "$action" != "" ] && ! list_contains "$available_actions" "$action"; then
        logger -s -t feature_switch.sh "Error: Invalid action: '$1'."
        help
        exit 1
    fi
}

if [ $# -lt 1 ]; then
    logger -s -t feature_switch.sh "ERROR: No. of arguments are less than 2"
    help
    exit 1
fi

validate_action "$@"
if [ "$1" = "show" ]; then
    if [ -z "${2:-}" ]; then
        d2 featureSwitch[0]
        # Check if rsu_updates supported or not
        if [ -f "/usr/bin/ntgr_functions.sh" ]; then
            rsu_updates=$(ntgr_functions.sh ini_read /etc/netgear.conf seal rsu_updates)
            echo "rsu_updates:      $rsu_updates"
        fi
    else
        validate_feature "$@"
        if [ "$2" != "rsu_updates" ]; then
            d2 -s featureSwitch[0]."$2"
        elif [ -f "/usr/bin/ntgr_functions.sh" ]; then
            rsu_updates=$(ntgr_functions.sh ini_read /etc/netgear.conf seal rsu_updates)
            echo "$rsu_updates"
        fi
    fi
    exit 0
else
    if [ -z "${2:-}" ]; then
        logger -s -t feature_switch.sh "ERROR: No. of arguments are less than 3 for enable/disable action"
        help
        exit 1
    elif [ "$2" = "rsu_updates" ]; then
    logger -s -t feature_switch.sh "ERROR: Currently enable/disable action not supported for rsu_updates feature"
    exit 1
    fi
fi
validate_feature "$@"

if [ -f "$(which /usr/bin/"$2")" ]; then
    echo "Feature process $2 is being $1d"
    if [ "$1" = "disable" ]; then
        monit stop "$2"
    fi
    monit_gen.lua "$1" "$2"
else
    echo "Integrated feature $2 is being $1d"
fi
d2 -c featureSwitch[0]."$2" "$1"
