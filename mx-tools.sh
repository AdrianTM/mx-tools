#!/bin/bash
DESKTOP_PATH="/usr/share/applications"

for DESKTOP_FILE in "mx/mx-bootrepair.desktop" "mx/mx-codecs.desktop" "mx/mx-findshares.desktop" "mx/mx-flash.desktop" "mx/mx-packageinstaller.desktop" "mx/mx-switchuser.desktop" "mx/mx-user.desktop" "mx-apt-notifier-menu.desktop" "mx-checkaptgpg.desktop"
do
	OLD_VALUE=$(cat $DESKTOP_PATH/$DESKTOP_FILE | grep "^NoDisplay=" | cut -d "=" -f2)
	case "$OLD_VALUE" in
		true)
			sed -i "s/^NoDisplay=.*/NoDisplay=false/" $DESKTOP_PATH/$DESKTOP_FILE
		;;
		false)
			sed -i -e '$a\' $DESKTOP_PATH/$DESKTOP_FILE
			sed -i "s/^NoDisplay=.*/NoDisplay=true/" $DESKTOP_PATH/$DESKTOP_FILE
		;;
		*)
			echo "NoDisplay=true" >> $DESKTOP_PATH/$DESKTOP_FILE
		;;
	esac
done
