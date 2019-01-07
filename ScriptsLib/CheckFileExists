#!/bin/bash
for file2Check in "$@"
do
	if [ ! -e $file2Check ] ; then
		echo "[FAILED]: $file2Check does not exist";
		exit 1;
	fi
done