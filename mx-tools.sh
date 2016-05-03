#!/bin/bash
#Base .desktop file location
#This is where you can set the base file path.
DESKTOP_PATH="/usr/share/applications"

#Where you can set any extra files (full path) that are not of the name scheme mx-
#Example:
#EXTRA_FILES="$DESKTOP_PATH/synaptic.desktop $DESKTOP_PATH/xfce4-terminal"
EXTRA_FILES="$DESKTOP_PATH/antix/live-usb.desktop $DESKTOP_PATH/antix/snapshot-mx.desktop"

#Allowance for passed items on the command line, do not edit
EXTRA_PASSED="${@:2}"

help() {
    echo "This program is to show and hide all of the mx-* apps (accept mx-tools) from the menu"
    echo "Options:"
    echo "-h | --hide    Hide all the applications"
    echo "-s | --show    Show all the applications"
    echo "-H | --help    This help Dialog"
    echo "Examples:"
    echo "$0 -h"
    echo "$0 -h $DESKTOP_PATH/synaptic.desktop"
    echo "$0 -s"
    echo "$0 -s $DESKTOP_PATH/synaptic.desktop"
}

SHOW_HIDE() {
    for DESKTOP_FILE in $(ls -1 $DESKTOP_PATH/{,mx/}mx*.desktop | grep -v mx-tools | grep -v mx-welcome | grep -v mx-apt-notifier-menu | grep -v mx-test-repo-installer | grep -v mx-usb-unmounter) $EXTRA_FILES $EXTRA_PASSED
    do
        if [ -f $DESKTOP_FILE ]; then
            OLD_VALUE=$(cat $DESKTOP_FILE | grep "^NoDisplay=" | cut -d "=" -f2)
            case "$OLD_VALUE" in
                true)
                    if [ "$CONDITION" != "hide" ]; then
                        sed -i "s/^NoDisplay=.*/NoDisplay=false/" $DESKTOP_FILE
                    fi
                ;;
                false)
                    if [ "$CONDITION" != "show" ]; then
                        sed -i "s/^NoDisplay=.*/NoDisplay=true/" $DESKTOP_FILE
                    fi
                ;;
                *)
                    sed -i -e '$a\' $DESKTOP_FILE
                    if [ "$CONDITION" = "hide" ]; then
                        echo "NoDisplay=true" >> $DESKTOP_FILE
                    else
                        echo "NoDisplay=false" >> $DESKTOP_FILE
                    fi
                ;;
            esac
        fi
    done
}

case $1 in
    -h|--hide)
        CONDITION="hide"
        SHOW_HIDE
    ;;
    -s|--show)
        CONDITION="show"
        SHOW_HIDE
    ;;
    -H|--help)
        help;
   ;;
   *)
        help;
   ;;
esac
