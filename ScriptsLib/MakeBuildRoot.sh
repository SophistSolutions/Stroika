#!/bin/bash
ROOT=$1

MAKE_INDENT_LEVEL="${MAKE_INDENT_LEVEL:-0}"
PREFIX=`ScriptsLib/PrintLevelLeader.sh $MAKE_INDENT_LEVEL`

if [ ! -e $ROOT ] ; then
	echo "$PREFIX$ROOT doesn't exist, so you cannot link to it as a build root"
	exit 1;
fi

echo "$PREFIX""Linking Stroika output and configuration directories from `pwd` to $ROOT"

rm -rf Builds ConfigurationFiles IntermediateFiles

TARGET_INTERMEDIATEFILES=$ROOT/IntermediateFiles
TARGET_BUILDS=$ROOT/Builds
TARGET_CONFIGURATIONFILES=$ROOT/ConfigurationFiles

mkdir -p $TARGET_INTERMEDIATEFILES
ln -s $ROOT/IntermediateFiles IntermediateFiles

mkdir -p $TARGET_BUILDS
ln -s $ROOT/Builds Builds

mkdir -p $TARGET_CONFIGURATIONFILES
ln -s $ROOT/ConfigurationFiles ConfigurationFiles