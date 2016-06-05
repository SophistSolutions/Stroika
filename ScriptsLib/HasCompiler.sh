#!/bin/bash

function hasCompiler
{
	COMPILER_DRIVER="$1"
	echo "#include <stdio.h>" > /tmp/foo.cpp
	RES="1"
	cd /tmp && (($COMPILER_DRIVER -c /tmp/foo.cpp) 1> /dev/null  2> /dev/null) || RES="0"
	rm /tmp/foo.cpp
	echo $RES
}
hasCompiler $1
