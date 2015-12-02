#!/bin/bash
#
#	Put in one place default logic to generate default values for various system/shell variables
#	(initial inspiration/point is workaround crazy echo in AIX)
#

SHELLVAR=$1

if [ "$SHELLVAR" == "ECHO" ] ; then
	#quick hack impl - to workaround horrible default on AIX
	if [[ `uname` =~ "AIX" ]] ; then
		/opt/freeware/bin/echo -n /opt/freeware/bin/echo
	else
		echo -n "echo"
	fi
fi
