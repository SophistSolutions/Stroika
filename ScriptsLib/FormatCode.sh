#!/bin/bash

ASTYLE_ARGS=
ASTYLE_ARGS+=' --style=stroustrup'
#ASTYLE_ARGS+=	--style=whitesmith
#ASTYLE_ARGS+=	--style=kr
#ASTYLE_ARGS+=	--style=java

ASTYLE_ARGS+=' --convert-tabs'
ASTYLE_ARGS+=' --indent-namespaces'
ASTYLE_ARGS+=' --indent-cases'
ASTYLE_ARGS+=' --indent-switches'
ASTYLE_ARGS+=' --indent-preprocessor'
ASTYLE_ARGS+=' --pad-oper'
ASTYLE_ARGS+=' --break-closing-brackets'
ASTYLE_ARGS+=' --keep-one-line-blocks'
ASTYLE_ARGS+=' --indent=spaces'
ASTYLE_ARGS+=' --preserve-date'

ASTYLE_ARGS+=' --align-pointer=type'
ASTYLE_ARGS+=' --align-reference=type'
ASTYLE_ARGS+=' --mode=c'
ASTYLE_ARGS+=' --suffix=none'

ASTYLE='astyle'


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
if [ `hash $FORMATTER 2> /dev/null` ]; then
   echo "Need missing formatter"
   exit 1
fi


for filePattern in "${@:2}"
do
    #$FIND $dirPattern -name $filePattern -exec $ASTYLE $ASTYLE_ARGS --formatted {} \;
    #$FIND $dirPattern -name $filePattern -exec clang-format -i {} \;
    #$FIND $dirPattern -name $filePattern -exec sh -c "clang-format {} | $EXPAND > {}.tmp; mv {}.tmp {}" \;
    $FIND $dirPattern -name $filePattern -exec sh -c "(($EXPAND --tabs=4 {} | $FORMATTER --assume-filename={} > {}.tmp) || rm -f {}.tmp && exit 1) && if cmp -s {} {}.tmp ; then rm {}.tmp; else echo Updating {} && mv {}.tmp {} ; fi" \;
done
