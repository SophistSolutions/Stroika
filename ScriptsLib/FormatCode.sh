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


EXPAND=/usr/bin/expand
if [[ `uname` =~ "CYGWIN" ]] ; then
	EXPAND=/bin/expand
fi

FIND=find
if [[ `uname` =~ "CYGWIN" ]] ; then
	FIND=/bin/find
fi

BASEDIR=$(dirname "$0")
OPTIONAL_CONFIG_FILE=$BASEDIR/../Configuration/.SourceCodeFormatOverrides.sh

#optionally override config settings
if [ -f $OPTIONAL_CONFIG_FILE ]; then
    source $OPTIONAL_CONFIG_FILE
fi

dirPattern=$1

FORMATTER=clang-format
hash $FORMATTER 2> /dev/null
if [ $? -ne 0 ]; then
   echo "Need missing formatter: $FORMATTER"
   exit 1
fi


for filePattern in "${@:2}"
do
    $FIND $dirPattern -name $filePattern -exec sh -c "$EXPAND --tabs=4 {} | $FORMATTER --assume-filename={} > {}.tmp && if cmp -s {} {}.tmp ; then rm {}.tmp; else echo Updating {} && mv {}.tmp {} ; fi" \;
done
