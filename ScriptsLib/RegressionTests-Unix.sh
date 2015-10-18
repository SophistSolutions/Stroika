#!/bin/bash

TEST_OUT_DIR=RegressionTests-Output
PARALELLMAKEFLAG=-j4


rm -rf $TEST_OUT_DIR


function doOneTest
{
	TESTNAME=$1
	CONFIG_ARGS=$2
	EXTRA_MAKE_RUNTESTS_ARGS=$3

	mkdir -p $TEST_OUT_DIR
	OUT_FILE_NAME="$TEST_OUT_DIR/$TESTNAME"

	echo -n "Running Test $TESTNAME (see $OUT_FILE_NAME) cfg=($CONFIG_ARGS)..."
	rm -f $OUT_FILE_NAME

	(./configure DefaultConfiguration $CONFIG_ARGS >> $OUT_FILE_NAME 2>&1) || {echo 'Configure failed' ; exit 1; }
	echo -n "."
	make clobber 2>&1 >> $OUT_FILE_NAME
	echo -n "."
	(make all $PARALELLMAKEFLAG >> $OUT_FILE_NAME 2>&1) || { echo 'make all failed' ; exit 1; }
	echo -n "."
	(make run-tests $EXTRA_MAKE_RUNTESTS_ARGS >> $OUT_FILE_NAME 2>&1)  || { echo 'make run-tests failed' ; exit 1; }
	X1=`cat $OUT_FILE_NAME | grep seconds | grep -F [Succeeded] | wc -l`
	XF=`cat $OUT_FILE_NAME | grep -F FAILED | wc -l`
	if [ $XF -gt 0 ]; then\
		echo "$XF tests failed -quitting";\
	fi
	if [ $X1 -lt 46 ]; then\
		echo "$X1 tests succeeded and expected 46";\
	fi
	if [ $XF -gt 0 ]; then\
		exit 1;\
	fi
	if [ $X1 -lt 46 ]; then\
		exit 1;\
	fi
	echo "done"
}

doOneTest "DEFAULT_CONFIG" "" ""
doOneTest "gcc-5.2.0-release" "--assertions disable --trace2file enable --compiler-driver '/home/lewis/gcc-5.2.0/bin/x86_64-unknown-linux-gnu-gcc' --cpp-optimize-flag -O3" ""
doOneTest "gcc-5.2.0-debug-c++17" "--assertions enable --trace2file enable --compiler-driver '/home/lewis/gcc-5.2.0/bin/x86_64-unknown-linux-gnu-gcc' --cppstd-version-flag --std=c++1z" ""
doOneTest "gcc49-release" "--assertions disable --trace2file enable --compiler-driver 'g++-4.9' --cpp-optimize-flag -O3" ""
doOneTest "gcc49-debug-no-TPP" "--assertions enable --trace2file enable --compiler-driver 'g++-4.9' --LibCurl no --OpenSSL no --Xerces no" ""
doOneTest "gcc-4.8.4-debug" "--assertions enable --trace2file enable --compiler-driver '/home/lewis/gcc-4.8.4/bin/x86_64-unknown-linux-gnu-gcc'" ""
doOneTest "gcc48-release" "--assertions disable --trace2file disable --compiler-driver 'g++-4.8' --cpp-optimize-flag -O3" ""
doOneTest "clang++-3.4-debug" "--assertions enable --trace2file enable --compiler-driver clang++-3.4" ""
doOneTest "clang++-3.5-debug" "--assertions enable --trace2file enable --compiler-driver clang++-3.5" "" 
doOneTest "clang++-3.6-debug" "--assertions enable --trace2file enable --compiler-driver clang++-3.6 --cppstd-version-flag --std=c++1y" ""
doOneTest "gcc-release-32" "--trace2file enable --assertions enable --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args  '-m32 -L/usr/lib32/' --static-link-gccruntime disable" ""
doOneTest "DEFAULT_CONFIG_WITH_VALGRIND" "" "VALGRIND=1"

