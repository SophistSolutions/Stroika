#!/bin/bash
THIS_SCRIPT_DIR="`dirname \"$0\"`"
THIS_SCRIPT_DIR=`realpath $THIS_SCRIPT_DIR`
THIS_SCRIPT_DIR="$THIS_SCRIPT_DIR/"

LEVEL=""
NFLAG=""
PRINT_STUFF_EXTRAS=""
for arg in "$@"
do
	if [ "$LEVEL" = "" ] ; then
		LEVEL=$arg
	else
		case "$arg" in
		-n)    NFLAG=$arg
				;;
		*)     if [ "$PRINT_STUFF_EXTRAS"="" ] ; then PRINT_STUFF_EXTRAS="$PRINT_STUFF_EXTRAS$arg" ; else PRINT_STUFF_EXTRAS="$PRINT_STUFF_EXTRAS $arg"; fi
				;;
		esac
	fi
done
ECHO=${ECHO:-echo}

STR=`$THIS_SCRIPT_DIR/PrintLevelLeader.sh $LEVEL`
$ECHO $NFLAG "$STR$PRINT_STUFF_EXTRAS"
