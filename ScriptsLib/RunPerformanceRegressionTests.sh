#!/bin/bash

#
# Call from $StroikaRoot
#
#	Examples:
#		ScriptsLib/RunPerformanceRegressionTests.sh
#		EXE=Builds/my-gcc-6.1.0-release-c++17/Test49 ScriptsLib/RunPerformanceRegressionTests.sh
#		EXE=Builds/Release/Test49 CONFIG_LABEL_IN_OUTPUT=FRED ScriptsLib/RunPerformanceRegressionTests.sh
#		MULTIPLIER=1 ScriptsLib/RunPerformanceRegressionTests.sh
#
VER=`ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`
MULTIPLIER=${MULTIPLIER-15}
if [ "$(uname -s)" == "Darwin" ] ; then
    EXE=${EXE-"Builds/Release/Test50"}
    CONFIG_LABEL_IN_OUTPUT="MacOS-x86-XCode9"
    CMDRUN_PREFIX="nice -10 "
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ] ; then
    # Do something under GNU/Linux platform
    EXE=${EXE-"Builds/my-gcc-7.2.0-release-c++17/Test50"}
    CONFIG_LABEL_IN_OUTPUT="linux-gcc-7.2.0-x64"
    CMDRUN_PREFIX="nice -10 taskset -c 0 "
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] ; then
    # Do something under Windows NT platform
    EXE=${EXE-"Builds/Release-U-32/Test50/Test50"}
    CONFIG_LABEL_IN_OUTPUT="Windows-x86-vs2k17"
    CMDRUN_PREFIX=""
elif [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ] ; then
    # Do something under Windows NT platform
    EXE=${EXE-"Builds/Release-U-32/Test50/Test50"}
    CONFIG_LABEL_IN_OUTPUT="Windows-x86-vs2k17"
    CMDRUN_PREFIX=""
else
    echo "WARNING - unrecognized OS"
fi


echo -n "$CMDRUN_PREFIX" "$EXE --x $MULTIPLIER --show > Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$CONFIG_LABEL_IN_OUTPUT.txt ... "
$CMDRUN_PREFIX $EXE --x $MULTIPLIER --show > Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$CONFIG_LABEL_IN_OUTPUT.txt
echo "done"
