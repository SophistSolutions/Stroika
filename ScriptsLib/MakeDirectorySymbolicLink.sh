#!/bin/bash
TARGET=$1
ALIASPATH=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	### As of Windows 10 (version 1803) - this produces an interoperable symbolic link. But alas - the cygwin ln -s still does not -- LGP 2018-04-22
	UPDATED_TARGET=`echo $TARGET | tr '/' '\\' 2>/dev/null`
	#(cmd /C "mklink /D  $ALIASPATH $UPDATED_TARGET") > /dev/null
	#Sigh...
	# Try it the quiet way, and if that fails, try with elevation
	# Not sure how to enable/disable this make more/less noisy. I have one windows 10 laptop that always needs RunMKLinkViaUAC and one that doesn't. Not sure what (relevant) is different
	#
	# NOTE - according to https://blogs.windows.com/buildingapps/2016/12/02/symlinks-windows-10/ this SHOULD worked
	#
	(cmd /C "mklink /D $ALIASPATH $UPDATED_TARGET" > /dev/null  2> /dev/null) || (echo -n "mklink failed- Trying RunMKLinkViaUAC.bat ... " && cmd /C "ScriptsLib\\RunMKLinkViaUAC.bat $ALIASPATH $UPDATED_TARGET" ; if [ $? -eq 0 ] ; then echo "OK -RunMKLinkViaUAC worked"; else echo 'mklink failed- Allow UAC or run as administrator' && exit 1; fi)
else
	ln -s $TARGET $ALIASPATH
fi
