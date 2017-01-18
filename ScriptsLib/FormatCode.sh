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

BASEDIR=$(dirname "$0")
OPTIONAL_CONFIG_FILE=$BASEDIR/../Configuration/.SourceCodeFormatOverrides.sh

#optionally override config settings
if [ -f $OPTIONAL_CONFIG_FILE ]; then
    source $OPTIONAL_CONFIG_FILE
fi

dirPattern=$1

for filePattern in "${@:2}"
do
    /bin/find $dirPattern -name $filePattern -exec $ASTYLE $ASTYLE_ARGS --formatted {} \;
done