#!/bin/bash
TARGET=$1
ALIASPATH=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	### As of Windows 10 (version??) - this produces an interoperable symbolic link. But alas - the cygwin ln -s still does not -- LGP 2018-04-22
	UPDATED_TARGET=`echo $TARGET | tr '/' '\\' 2>/dev/null`
	(cmd /C "mklink /D  $ALIASPATH $UPDATED_TARGET") > /dev/null
else
	ln -s $TARGET $ALIASPATH 
fi
