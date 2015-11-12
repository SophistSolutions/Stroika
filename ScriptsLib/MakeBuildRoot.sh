#!/bin/bash
ROOT=$1
if [ ! -e $ROOT ] ; then
	echo "$ROOT doesn't exist, so you cannot link to it as a build root"
	exit 1;
fi
ROOT=`realpath $ROOT`

echo "Linking Stroika output and configuration directories from `pwd` to $ROOT"
if [[ `uname` =~ "CYGWIN" ]] ; then
	echo "***NOTE - this may need to invoke UAC to create symbolic links***"
fi

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
ScriptsLib/MakeDirectorySymbolicLink.sh IntermediateFiles $TARGET_INTERMEDIATEFILES

mkdir -p $TARGET_BUILDS
ScriptsLib/MakeDirectorySymbolicLink.sh Builds $TARGET_BUILDS

mkdir -p $TARGET_CONFIGURATIONFILES
ScriptsLib/MakeDirectorySymbolicLink.sh ConfigurationFiles $TARGET_CONFIGURATIONFILES
