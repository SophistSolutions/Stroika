#!/bin/bash
FROM=$1
TO=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	#Sigh...
	# Try it the quiet way, and if that fails, try with elevation
	# Not sure how to enable/disable this make more/less noisy. I have one windows 10 laptop that always needs RunMKLinkViaUAC and one that doesn't. Not sure what (relevant) is different
	#
	# NOTE - according to https://blogs.windows.com/buildingapps/2016/12/02/symlinks-windows-10/ this SHOULD worked
	#
	(cmd /C "mklink /D $FROM $TO" > /dev/null  2> /dev/null) || (echo -n "mklink failed- Trying RunMKLinkViaUAC.bat ... " && cmd /C "ScriptsLib\\RunMKLinkViaUAC.bat $FROM $TO" ; if [ $? -eq 0 ] ; then echo "OK -RunMKLinkViaUAC worked"; else echo 'mklink failed- Allow UAC or run as administrator' && exit 1; fi)
else
	ln -s $TO $FROM
fi
