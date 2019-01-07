#!/bin/bash
VERSION_IN_FILE=$1
VERSION_OUT_FILE=$2
OUT_FILE_TYPE=$3

THIS_SCRIPT_DIR="`dirname \"$0\"`"
THIS_SCRIPT_DIR=`realpath $THIS_SCRIPT_DIR`
THIS_SCRIPT_DIR="$THIS_SCRIPT_DIR/"

#echo "-----------"
#echo VERSION_IN_FILE=$VERSION_IN_FILE
#echo VERSION_OUT_FILE=$VERSION_OUT_FILE
#echo OUT_FILE_TYPE=$OUT_FILE_TYPE
#echo "-----------"

FULLVERSIONSTRING=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE FullVersionString`
MAJOR=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE Major`
MINOR=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE Minor`
VERSIONSTAGE=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE Stage`
VERSIONSUBSTAGE=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE SubStage`
VERSIONSUBSTAGEINT=$VERSIONSUBSTAGE
if [ "$VERSIONSUBSTAGEINT" == "" ] ; then VERSIONSUBSTAGEINT="0"; fi
VERSIONFINAL=`$THIS_SCRIPT_DIR/ExtractVersionInformation $VERSION_IN_FILE Final`

if [ "$VERSIONSTAGE" == "a" ] ; then VERSIONSTAGE="Alpha"; fi
if [ "$VERSIONSTAGE" == "b" ] ; then VERSIONSTAGE="Beta"; fi
if [ "$VERSIONSTAGE" == "d" ] ; then VERSIONSTAGE="Dev"; fi
if [ "$VERSIONSTAGE" == "r" ] ; then VERSIONSTAGE="ReleaseCandidate"; fi

if [ "$VERSIONFINAL" == "x" ] ; then VERSIONFINAL="false"; fi
if [ "$VERSIONFINAL" == "" ] ; then VERSIONFINAL="true"; fi
if [ "$VERSIONFINAL" != "false" ] && [ "$VERSIONFINAL" != "true" ] ; then echo "BAD VERSIONFINAL"; fi

VERSIONFINALBUILD='0'
if [ "$VERSIONFINAL" == "true" ] ; then VERSIONFINALBUILD="1"; fi

rm -f $VERSION_OUT_FILE
touch $VERSION_OUT_FILE

if [ "$OUT_FILE_TYPE" == "AppCPlusPlusVersion" ]
  then
	DEFINE_PROTECTOR="__"`basename $VERSION_OUT_FILE | tr '-' '_' | tr '.' '_'`"__"
	CPPNAMESPACE=`basename $VERSION_OUT_FILE .h | tr '-' '_' | tr '.' '_'`
	echo "#ifndef $DEFINE_PROTECTOR " >> $VERSION_OUT_FILE
	echo "#define $DEFINE_PROTECTOR 1" >> $VERSION_OUT_FILE
	echo "#include \"Stroika/Foundation/StroikaPreComp.h\"" >> $VERSION_OUT_FILE
	echo "#include \"Stroika/Foundation/Configuration/Version.h\"" >> $VERSION_OUT_FILE
	echo "namespace $CPPNAMESPACE {" >> $VERSION_OUT_FILE
	echo "constexpr Stroika::Foundation::Configuration::Version  kVersion = Stroika::Foundation::Configuration::Version ($MAJOR, $MINOR, Stroika::Foundation::Configuration::VersionStage::$VERSIONSTAGE, $VERSIONSUBSTAGEINT, $VERSIONFINAL);" >> $VERSION_OUT_FILE
	echo "}" >> $VERSION_OUT_FILE
	echo "#endif /*$DEFINE_PROTECTOR*/" >> $VERSION_OUT_FILE
fi

if [ "$OUT_FILE_TYPE" == "StroikaLibVersion" ]
  then
	echo "#define kStroika_Version_Major                  $MAJOR" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_Minor                  $MINOR" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_MajorMinor             0x$MAJOR$MINOR" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_Stage                  kStroika_Version_Stage_$VERSIONSTAGE" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_SubStage               $VERSIONSUBSTAGEINT" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_FinalBuild             $VERSIONFINALBUILD" >> $VERSION_OUT_FILE
		
	echo "#define kStroika_Version_MajorMinorVersionString        \"$MAJOR.$MINOR\"" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_ShortVersionString             \"$FULLVERSIONSTRING\"" >> $VERSION_OUT_FILE
	echo "#define kStroika_Version_FullVersion                    Stroika_Make_FULL_VERSION (kStroika_Version_Major, kStroika_Version_Minor, kStroika_Version_Stage, kStroika_Version_SubStage, kStroika_Version_FinalBuild)" >> $VERSION_OUT_FILE
fi
