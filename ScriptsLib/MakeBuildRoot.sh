#!/bin/sh
ROOT=$1
if [ ! -e $ROOT ] ; then
	echo "$ROOT doesn't exist, so you cannot link to it as a build root"
	exit 1;
fi
ROOT=`realpath $ROOT`

echo "Linking Stroika output and configuration directories from `pwd` to $ROOT"

rm -rf Builds ConfigurationFiles IntermediateFiles

TARGET_INTERMEDIATEFILES=$ROOT/IntermediateFiles
TARGET_BUILDS=$ROOT/Builds
TARGET_CONFIGURATIONFILES=$ROOT/ConfigurationFiles
if [[ `uname` =~ "CYGWIN" ]] ; then
	TARGET_INTERMEDIATEFILES=`cygpath -w $TARGET_INTERMEDIATEFILES`
	TARGET_BUILDS=`cygpath -w $TARGET_BUILDS`
	TARGET_CONFIGURATIONFILES=`cygpath -w $TARGET_CONFIGURATIONFILES`
fi

mkdir -p $TARGET_INTERMEDIATEFILES
sh ScriptsLib/MakeDirectorySymbolicLink.sh IntermediateFiles $TARGET_INTERMEDIATEFILES

mkdir -p $TARGET_BUILDS
sh ScriptsLib/MakeDirectorySymbolicLink.sh Builds $TARGET_BUILDS

mkdir -p $TARGET_CONFIGURATIONFILES
sh ScriptsLib/MakeDirectorySymbolicLink.sh ConfigurationFiles $TARGET_CONFIGURATIONFILES

