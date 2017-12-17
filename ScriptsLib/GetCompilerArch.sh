#!/bin/bash
CC=$1
OUTPUT=`$CC -dumpmachine 2> /dev/null`
if [ $? -eq 0 ] ; then
	echo $OUTPUT | awk '{split($0,a,"-"); print a[1]}'
elif [[ "xxx/x64/cl" =~ .*\/x64\/cl ]] ; then 
	echo x86_64
elif [[ "xxx/x64/cl" =~ .*\/x86\/cl ]] ; then 
	echo x86
else
	echo UNKNOWN
fi
