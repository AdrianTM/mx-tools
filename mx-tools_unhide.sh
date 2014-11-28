#!/bin/bash
DESKTOP_PATH="/usr/share/applications"

for DESKTOP_FILE in $(ls -1 $DESKTOP_PATH/{,mx/}mx*.desktop | cut -f5- -d/ | grep -v mx-tools)
do
	OLD_VALUE=$(cat $DESKTOP_PATH/$DESKTOP_FILE | grep "^NoDisplay=" | cut -d "=" -f2)
	case "$OLD_VALUE" in
		true)
			sed -i "s/^NoDisplay=.*/NoDisplay=false/" $DESKTOP_PATH/$DESKTOP_FILE
		;;
		false)
		;;
		*)
			sed -i -e '$a\' $DESKTOP_PATH/$DESKTOP_FILE
			echo "NoDisplay=false" >> $DESKTOP_PATH/$DESKTOP_FILE
		;;
	esac
done
