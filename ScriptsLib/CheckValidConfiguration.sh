#!/bin/bash
MY_PATH="`dirname \"$0\"`"
MY_PATH=`realpath $MY_PATH`
CONFIGURATION=$1
if [ ! -e "$MY_PATH/../ConfigurationFiles/$CONFIGURATION.xml" ] ; then
	echo -n "\"$CONFIGURATION\" is not a valid configuration; valid configurations include: [ " && $MY_PATH/GetConfigurations && echo "]";
	exit 1;
fi
