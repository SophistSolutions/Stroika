#!/bin/sh
VERSION_IN_FILE=$1
VERSION_OUT_FILE=$2
DEFINE_PROTECTOR=$3
CPPNAMESPACE=$4

function getawk() {
    if hash gawk 2>/dev/null; then
        echo -n "gawk"
    else
        echo -n "awk";
    fi
}

AWK=$(getawk)

#echo "-----------"
#echo VERSION_OUT_FILE=$VERSION_OUT_FILE
#echo DEFINE_PROTECTOR=$DEFINE_PROTECTOR
#echo "-----------"

MAJOR=`cat $VERSION_IN_FILE | gawk -F. '{ print $1 }'`
MINOR=`cat $VERSION_IN_FILE | gawk -F. '{ print $2 }'`
VERSIONSTAGE=`cat $VERSION_IN_FILE | gawk -F. '{ print $3 }'`
VERSIONSUBSTAGE=`cat $VERSION_IN_FILE | gawk -F. '{ print $4 }'`
VERSIONFINAL=`cat $VERSION_IN_FILE | gawk -F. '{ print $5 }'`
if [ "$VERSIONFINAL" == "x" ] ; then VERSIONFINAL="false"; fi
if [ "$VERSIONFINAL" == "" ] ; then VERSIONFINAL="true"; fi
if [ "$VERSIONFINAL" != "false" ] && [ "$VERSIONFINAL" != "true" ] ; then echo "BAD VERSIONFINAL"; fi

rm -f $VERSION_OUT_FILE
touch $VERSION_OUT_FILE
echo "#ifndef $DEFINE_PROTECTOR " >> $VERSION_OUT_FILE
echo "#define $DEFINE_PROTECTOR 1" >> $VERSION_OUT_FILE
echo "#include \"Stroika/Foundation/StroikaPreComp.h\"" >> $VERSION_OUT_FILE
echo "#include \"Stroika/Foundation/Configuration/Version.h\"" >> $VERSION_OUT_FILE
echo "namespace $CPPNAMESPACE {" >> $VERSION_OUT_FILE
echo "constexpr Stroika::Foundation::Configuration::Version  kVersion = Stroika::Foundation::Configuration::Version ($MAJOR, $MINOR, Stroika::Foundation::Configuration::VersionStage::$VERSIONSTAGE, $VERSIONSUBSTAGE, $VERSIONFINAL);" >> $VERSION_OUT_FILE
echo "}" >> $VERSION_OUT_FILE
echo "#endif /*$DEFINE_PROTECTOR*/" >> $VERSION_OUT_FILE
