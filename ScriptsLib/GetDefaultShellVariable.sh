#!/bin/bash
#
#	Put in one place default logic to generate default values for various system/shell variables
#	(initial inspiration/point is workaround crazy echo in AIX)
#

SHELLVAR=$1

if [ "$SHELLVAR" == "ECHO" ] ; then
	echo -n "echo"
fi
