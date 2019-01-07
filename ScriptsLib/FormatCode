#!/bin/bash

##
## USAGE:
##		ScriptsLib/FormatCode.sh DIRECTORY [WILDCARD PATTERNS]+
##
## EXAMPLE USAGE
##		ScriptsLib/FormatCode.sh Library *.h *.inl *.cpp
##		ScriptsLib/FormatCode.sh Library/Sources/Stroika/Foundation/Execution NullMutex.h
##
##	NOTE ALSO - can override default clang settings with
##		Configuration/.SourceCodeFormatOverrides.sh
##


EXPAND="${EXPAND:-/usr/bin/expand}"
hash $EXPAND 2> /dev/null
if [ $? -ne 0 ]; then
   echo "Need missing tab expander: $EXPAND"
   exit 1
fi

FIND=find
if [[ `uname` =~ "CYGWIN" ]] ; then
	FIND=/bin/find
fi

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

BASEDIR=$(get_script_dir)
OPTIONAL_CONFIG_FILE=$BASEDIR/../Configuration/.SourceCodeFormatOverrides.sh

#optionally override config settings
if [ -f $OPTIONAL_CONFIG_FILE ]; then
    source $OPTIONAL_CONFIG_FILE
fi

dirPattern=$1

FORMATTER="${FORMATTER:-clang-format}"
hash $FORMATTER 2> /dev/null
if [ $? -ne 0 ]; then
   echo "Need missing formatter: $FORMATTER"
   exit 1
fi

for filePattern in "${@:2}"
do
    $FIND $dirPattern -name $filePattern -exec sh -c "$EXPAND --tabs=4 {} | $FORMATTER --assume-filename={} > {}.tmp && if cmp -s {} {}.tmp ; then rm -f {}.tmp; else echo Updating {} && mv {}.tmp {} ; fi" \;
done
