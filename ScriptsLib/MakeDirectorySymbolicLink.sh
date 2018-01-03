#!/bin/bash
FROM=$1
TO=$2
if [[ `uname` =~ "CYGWIN" ]] ; then
	#Sigh...
	# Try it the quiet way, and if that fails, try with elevation
	# Not sure how to enable/disable this make more/less noisy. I have one windows 10 laptop that always needs RunMKLinkViaUAC and one that doesnt. Not sure what (relevant) is different
	(cmd /C "mklink /D $FROM $TO" > /dev/null  2> /dev/null) || (echo "mklink failed- Trying RunMKLinkViaUAC.bat" && cmd /C "ScriptsLib\\RunMKLinkViaUAC.bat $FROM $TO" || (echo "mklink failed- Allow UAC or run as administrator"  && exit 1))
else
	ln -s $TO $FROM
fi
