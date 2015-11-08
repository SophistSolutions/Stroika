#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR

TEST_OUT_DIR=RegressionTests-Output
PARALELLMAKEFLAG=-j4


rm -rf $TEST_OUT_DIR


function hasCompiler
{
	COMPILER_DRIVER=$1
	echo "#include <stdio.h>" > /tmp/foo.cpp
	($COMPILER_DRIVER -c /tmp/foo.cpp n 2>&1 > /dev/null)  || return 0;
	return 1
}

function doOneTest
{
	TESTNAME=$1
	COMPILER_DRIVER=$2
	CONFIG_ARGS=$3
	EXTRA_MAKE_RUNTESTS_ARGS=$4

	mkdir -p $TEST_OUT_DIR
	OUT_FILE_NAME="$TEST_OUT_DIR/$TESTNAME"

	echo -n "Running Test $TESTNAME (see $OUT_FILE_NAME) cfg=($CONFIG_ARGS)..."
	rm -f $OUT_FILE_NAME

	if [ "$COMPILER_DRIVER" != "" ]; then
		if [ $(hasCompiler($COMPILER_DRIVER); echo $?) -eq 1 ]; then
			CONFIG_ARGS=$CONFIG_ARGS" --compiler-driver $COMPILER_DRIVER"
		else
			echo "Skipping compiler-driver $COMPILER_DRIVER cuz not installed"
			return 0
		fi
		#($COMPILER_DRIVER --version 2>&1 > /dev/null)  || (echo "Skipping compiler-driver $COMPILER_DRIVER cuz not installed" ; return 0;)
		#CONFIG_ARGS=$CONFIG_ARGS" --compiler-driver $COMPILER_DRIVER"
	fi

	((./configure DefaultConfiguration $CONFIG_ARGS 2>&1) >> $OUT_FILE_NAME ) || (echo "fail" && exit 77;)

	echo -n "."
	make clobber 2>&1 >> $OUT_FILE_NAME
	echo -n "."
	(make all $PARALELLMAKEFLAG >> $OUT_FILE_NAME 2>&1) || (echo 'make all failed' ; exit 1;)
	echo -n "."
	(make run-tests $EXTRA_MAKE_RUNTESTS_ARGS >> $OUT_FILE_NAME 2>&1)  || (echo 'make run-tests failed' ; exit 77;)
	X1=`cat $OUT_FILE_NAME | grep seconds | grep -F [Succeeded] | wc -l`
	XF=`cat $OUT_FILE_NAME | grep -F FAILED | wc -l`
	if [ $XF -gt 0 ]; then\
		echo "$XF tests failed -quitting";\
	fi
	if [ $X1 -lt 46 ]; then\
		echo "$X1 tests succeeded and expected 46";\
	fi
	if [ $XF -gt 0 ]; then\
		exit 77;\
	fi
	if [ $X1 -lt 46 ]; then\
		exit 77;\
	fi
	echo "done"
}



doOneTest "DEFAULT_CONFIG" "" "" ""
doOneTest "gcc-5.2.0-release" "/home/lewis/gcc-5.2.0/bin/x86_64-unknown-linux-gnu-gcc" "--assertions disable --trace2file enable --cpp-optimize-flag -O3" ""
doOneTest "gcc-5.2.0-debug-c++17" "/home/lewis/gcc-5.2.0/bin/x86_64-unknown-linux-gnu-gcc" "--assertions enable --trace2file enable --cppstd-version-flag --std=c++1z" ""
doOneTest "gcc49-release" "g++-4.9" "--assertions disable --trace2file enable --cpp-optimize-flag -O3" ""
doOneTest "gcc49-debug-no-TPP" "g++-4.9" "--assertions enable --trace2file enable --LibCurl no --OpenSSL no --Xerces no" ""
doOneTest "gcc-4.8.4-debug" "/home/lewis/gcc-4.8.4/bin/x86_64-unknown-linux-gnu-gcc" "--assertions enable --trace2file enable" ""
doOneTest "gcc48-release" "g++-4.8" "--assertions disable --trace2file disable --cpp-optimize-flag -O3" ""
doOneTest "clang++-3.5-debug" "clang++-3.5" "--assertions enable --trace2file enable" "" 
doOneTest "clang++-3.6-debug" "clang++-3.6" "--assertions enable --trace2file enable --cppstd-version-flag --std=c++1y" ""
#TESTING if -L needed
#doOneTest "gcc-release-32" "" "--trace2file enable --assertions enable --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args  '-m32 -L/usr/lib32/' --static-link-gccruntime disable" ""
doOneTest "gcc-release-32" "" "--trace2file enable --assertions enable --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args  -m32 --static-link-gccruntime disable" ""

#disable blockalloc, and valgrind, so we test with minimal valgrind suppressions
doOneTest "DEFAULT_CONFIG_WITH_VALGRIND_PURIFY_NO_BLOCK_ALLOC" "" "--openssl use --openssl-extraargs purify --block-allocation disable" "VALGRIND=1"

#test with usual set of valgrind suppressions
VALGRIND_SUPPRESSIONS="Common-Valgrind.supp BlockAllocation-Valgrind.supp"  doOneTest "DEFAULT_CONFIG_WITH_VALGRIND_PURIFY_WITH_BLOCK_ALLOC" "" "--openssl use --openssl-extraargs purify" "VALGRIND=1"

#slow, and largely useless test...
#VALGRIND_SUPPRESSIONS="OpenSSL.supp Common-Valgrind.supp BlockAllocation-Valgrind.supp" doOneTest "DEFAULT_CONFIG_WITH_VALGRIND" "" "" "VALGRIND=1"
