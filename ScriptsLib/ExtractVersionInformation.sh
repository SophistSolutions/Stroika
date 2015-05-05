#!/bin/bash
VERSION_IN_FILE=$1
OUT_FIELD_NAME=$2

#echo "-----------"
#echo VERSION_IN_FILE=$VERSION_IN_FILE
#echo OUT_FIELD_NAME=$OUT_FIELD_NAME
#echo "-----------"

FULLVERSIONSTRING=`sed 's/[ \t]*$//' $VERSION_IN_FILE | xargs echo -n`
MAJOR=`sed 's/\([0-9]*\).*/\1/' $VERSION_IN_FILE`
MINOR=`sed 's/[0-9]*\.\([0-9]*\).*/\1/' $VERSION_IN_FILE`
VERSIONSTAGE=`sed 's/[0-9]*\.[0-9]*\([abdr]\).*/\1/' $VERSION_IN_FILE`
VERSIONSUBSTAGE=`sed 's/[0-9]*\.[0-9]*[abdr]\([0-9]*\).*/\1/' $VERSION_IN_FILE`
VERSIONFINAL=`sed 's/[0-9]*\.[0-9]*[abdr][0-9]*\(.*\)/\1/' $VERSION_IN_FILE`
SHORT_VERSION_STAGE=$VERSIONSTAGE
SHORT_VERSIONFINAL=$VERSIONFINAL
if [ "$VERSIONSTAGE" == "a" ] ; then VERSIONSTAGE="Alpha"; fi
if [ "$VERSIONSTAGE" == "b" ] ; then VERSIONSTAGE="Beta"; fi
if [ "$VERSIONSTAGE" == "d" ] ; then VERSIONSTAGE="Dev"; fi
if [ "$VERSIONSTAGE" == "r" ] ; then VERSIONSTAGE="ReleaseCandidate"; fi

if [ "$VERSIONFINAL" == "x" ] ; then VERSIONFINAL="false"; fi
if [ "$VERSIONFINAL" == "" ] ; then VERSIONFINAL="true"; fi
if [ "$VERSIONFINAL" != "false" ] && [ "$VERSIONFINAL" != "true" ] ; then echo "BAD VERSIONFINAL"; fi

VERSIONFINALBUILD='0'
if [ "$VERSIONFINAL" == "true" ] ; then VERSIONFINALBUILD="1"; fi

if [ "$OUT_FIELD_NAME" == "Major" ]
  then
	echo -n "$MAJOR"
fi
if [ "$OUT_FIELD_NAME" == "Minor" ]
  then
	echo -n "$MINOR"
fi
if [ "$OUT_FIELD_NAME" == "Stage" ]
  then
	echo -n "$VERSIONSTAGE"
fi
if [ "$OUT_FIELD_NAME" == "SubStage" ]
  then
	echo -n "$VERSIONSUBSTAGE"
fi
if [ "$OUT_FIELD_NAME" == "Final" ]
  then
	echo -n "$VERSIONFINAL"
fi
if [ "$OUT_FIELD_NAME" == "Major.Minor" ]
  then
	echo -n "$MAJOR.$MINOR"
fi
if [ "$OUT_FIELD_NAME" == "DecoratedStageInfo" ]
  then
	echo -n "$SHORT_VERSION_STAGE$VERSIONSUBSTAGE$SHORT_VERSIONFINAL"
fi
