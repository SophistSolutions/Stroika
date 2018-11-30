#!/bin/bash

get_script_dir () {
     #https://www.ostricher.com/2014/10/the-right-way-to-get-the-directory-of-a-bash-script/
     SOURCE="${BASH_SOURCE[0]}"
     # While $SOURCE is a symlink, resolve it
     while [ -h "$SOURCE" ]; do
          DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
          SOURCE="$( readlink "$SOURCE" )"
          # If $SOURCE was a relative symlink (so no "/" as prefix, need to resolve it relative to the symlink base directory
          [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
     done
     DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
     echo "$DIR"
}
THIS_SCRIPT_DIR=$(get_script_dir)

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
		*)     if [ "$PRINT_STUFF_EXTRAS" = "" ] ; then PRINT_STUFF_EXTRAS="$PRINT_STUFF_EXTRAS$arg" ; else PRINT_STUFF_EXTRAS="$PRINT_STUFF_EXTRAS $arg"; fi
				;;
		esac
	fi
done
ECHO=${ECHO:-echo}

STR=`$THIS_SCRIPT_DIR/PrintLevelLeader.sh $LEVEL`
$ECHO $NFLAG "$STR$PRINT_STUFF_EXTRAS"
