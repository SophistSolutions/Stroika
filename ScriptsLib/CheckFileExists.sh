#!/bin/bash
file2Check=$1
if [ ! -e $file2Check ] ; then
	echo "[FAILED]: $file2Check does not exist";
	exit 1;
fi
