#!/bin/bash

#
# Call from $StroikaRoot
#
#	Examples:
#		ScriptsLib/RunPerformanceRegressionTests.sh
#		CONFIGURATION=Release ScriptsLib/RunPerformanceRegressionTests.sh
#		CONFIGURATION=Release CONFIG_LABEL_IN_OUTPUT=FRED ScriptsLib/RunPerformanceRegressionTests.sh
#		MULTIPLIER=1 ScriptsLib/RunPerformanceRegressionTests.sh
#
VER=`ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`
if   [ "$(expr substr $(uname -s) 1 5)" == "Linux" ] ; then
    # Do something under GNU/Linux platform
    EXE=${EXE-"Builds/gcc-6.1.0-release-c++17/Test47"}
    CONFIG_LABEL_IN_OUTPUT="linux-gcc-6.1.0-x64"
    CMDRUN_PREFIX="nice -10 taskset -c 0 "
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ] ; then
    # Do something under Windows NT platform
    EXE=${EXE-"Builds/Release-U-32/Test47/Test47"}
    CONFIG_LABEL_IN_OUTPUT="x86-vs2k15"
    CMDRUN_PREFIX=""
elif [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ] ; then
    # Do something under Windows NT platform
    EXE=${EXE-"Builds/Release-U-32/Test47/Test47"}
    CONFIG_LABEL_IN_OUTPUT="x86-vs2k15"
    CMDRUN_PREFIX=""
fi

MULTIPLIER=15

echo -n "$CMDRUN_PREFIX" "$EXE --x $MULTIPLIER --show > Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$CONFIG_LABEL_IN_OUTPUT.txt..."
$CMDRUN_PREFIX $EXE --x $MULTIPLIER --show > Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$CONFIG_LABEL_IN_OUTPUT.txt
echo "done"
