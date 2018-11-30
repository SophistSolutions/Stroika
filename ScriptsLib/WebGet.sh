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
MY_PATH=$(get_script_dir)

OUTPUTFILE=$1
SRCURL1=$2
SRCURL2=$3
SRCURL3=$4
SRCURL4=$5
SRCURL5=$6
SRCURL6=$7

if [ "$SRCURL2" == "" ] ; then
	SRCURL2=$SRCURL1
fi
if [ "$SRCURL3" == "" ] ; then
	SRCURL3=$SRCURL2
fi
if [ "$SRCURL4" == "" ] ; then
	SRCURL4=$SRCURL1
fi
if [ "$SRCURL5" == "" ] ; then
	SRCURL5=$SRCURL3
fi
if [ "$SRCURL6" == "" ] ; then
	SRCURL6=$SRCURL4
fi

: ${MAKE_INDENT_LEVEL:=0}

RETRIES=2

WGET_CMD_BASE="wget --quiet --no-check-certificate --tries=$RETRIES -O $OUTPUTFILE"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL "$WGET_CMD_BASE $SRCURL1 ... "
$WGET_CMD_BASE $SRCURL1
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $((${MAKE_INDENT_LEVEL}+1)) failed
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "WARNING: mirror failed, so retrying:"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "$WGET_CMD_BASE $SRCURL2 ... "
$WGET_CMD_BASE $SRCURL2
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $((${MAKE_INDENT_LEVEL}+1)) failed
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "WARNING: mirror failed, so retrying:"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "$WGET_CMD_BASE $SRCURL3 ... "
$WGET_CMD_BASE -O $OUTPUTFILE $SRCURL3
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $((${MAKE_INDENT_LEVEL}+1)) failed
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "WARNING: mirror failed, so retrying:"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "$WGET_CMD_BASE $SRCURL4 ... "
$WGET_CMD_BASE $SRCURL4
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $((${MAKE_INDENT_LEVEL}+1)) failed
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "WARNING: mirror failed, so retrying:"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "$WGET_CMD_BASE $SRCURL5 ... "
$WGET_CMD_BASE $SRCURL5
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $((${MAKE_INDENT_LEVEL}+1)) failed
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "WARNING: mirror failed, so retrying:"
$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  "$WGET_CMD_BASE $SRCURL6 ... "
$WGET_CMD_BASE $SRCURL6
if [ $? -eq 0 ] ; then
    	exit 0;
fi

$MY_PATH/PrintProgressLine.sh $MAKE_INDENT_LEVEL  Failed downloading $OUTPUTFILE
rm -f $OUTPUTFILE
exit 1
