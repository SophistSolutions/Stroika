#!/bin/bash
LEVEL=$1
COUNTER=0
while [  $COUNTER -lt $LEVEL ]; do
	echo -n "   "
	let COUNTER=COUNTER+1 
done
