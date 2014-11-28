#!/bin/bash
DESKTOP_PATH="/usr/share/applications"
EXTRA_FILES=""
#EXTRA_FILES="synaptic.desktop"
EXTRA_PASSED="${@:2}"

help() {
    echo "This program is to show and hide all of the mx-* apps (accept mx-tools) from the menu"
    echo "Options:"
    echo "-h | --hide    Hide all the applications"
    echo "-s | --show    Show all the applications"
    echo "-H | --help    This help Dialog"
    echo "Examples:"
    echo "$0 -h"
    echo "$0 -h synaptic.desktop"
    echo "$0 -s"
    echo "$0 -s synaptic.desktop"
}

SHOW_HIDE() {
    for DESKTOP_FILE in $(ls -1 $DESKTOP_PATH/{,mx/}mx*.desktop | grep -v mx-tools) $DESKTOP_PATH/$EXTRA_FILES $EXTRA_PASSED
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
                        else [ "$CONDITION" = "show" ];
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