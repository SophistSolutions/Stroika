#!/bin/bash
FROM=$1
TO=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	#Sigh...
	cmd /C "ScriptsLib\\RunMKLinkViaUAC.bat $FROM $TO" || (echo "mklink failed- Allow UAC or run as administrator"  && exit 1)
else
	ln -s $TO $FROM
fi
