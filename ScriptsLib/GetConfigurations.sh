#!/bin/bash
(>&2 echo "***GetConfigurations.sh is DEPRECATED since v2.1d15 - use GetConfigurations")
MY_PATH="`dirname \"$0\"`"
MY_PATH=`realpath $MY_PATH`
SORT=/usr/bin/sort
if [ ! -e $SORT ]; then
	SORT=sort
fi
#NB: need full path top find cuz of winDoze
if [ -e "$MY_PATH/../ConfigurationFiles/" ] ; then
	#/usr/bin/find "$MY_PATH/../ConfigurationFiles/" -mindepth 1 -maxdepth 1 -type f -print0 | xargs -0 -I {} basename "{}" .xml | tr '\n' ' '
	/usr/bin/find "$MY_PATH/../ConfigurationFiles/" -type f -exec basename \{} .xml \; | $SORT | tr '\n' ' '
fi
