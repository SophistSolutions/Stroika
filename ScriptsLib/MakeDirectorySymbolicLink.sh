#!/bin/bash
FROM=$1
TO=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	#Sigh...
	# Try it the quiet way, and if that fails, try with elevation
	cmd /C "mklink /D $FROM $TO" > /dev/null || cmd /C "ScriptsLib\\RunMKLinkViaUAC.bat $FROM $TO" || (echo "mklink failed- Allow UAC or run as administrator"  && exit 1)
else
	ln -s $TO $FROM
fi
