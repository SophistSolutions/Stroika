#!/bin/sh
FROM=$1
TO=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	cmd /C "mklink /D $FROM $TO"
else
	ln -s $TO $FROM
fi
