#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR

TEST_OUT_FILE=REGRESSION-TESTS.OUT
PARALELLMAKEFLAG=-j3



### STUFF TO STEAL FROM TO MAKE SEPARTE REGOUT FILES
function doOneTest
{
	TESTNAME=$1
	COMPILER_DRIVER=$2
	COMPILER_DRIVER_WITH_EXTRA_ARGS2TEST=$3
	CONFIG_ARGS=$4
	EXTRA_MAKE_RUNTESTS_ARGS=$5

	####
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
	echo && echo "   done"
}



STARTAT=`date`;
echo "Resetting all configurations to standard regression test set (output to $TEST_OUT_FILE) - started at $STARTAT"
rm -f $TEST_OUT_FILE
make regression-test-configurations >>$TEST_OUT_FILE 2>&1

make clobber >>$TEST_OUT_FILE 2>&1
echo "Make all..."
make all $PARALELLMAKEFLAG >>$TEST_OUT_FILE 2>&1

echo "Run-Tests ALL..."
make run-tests >>$TEST_OUT_FILE 2>&1

echo "Run-Tests raspberrypi remote..."
ARMTESTMACHINEAVAIL=`(ping www.sophists.codm 2>/dev/null 1>/dev/null); echo $?`
if [ $ARMTESTMACHINEAVAIL -eq 0 ]; then
	make run-tests CONFIGURATION=raspberrypi-gcc-4.9 REMOTE=lewis@raspberrypi >>$TEST_OUT_FILE 2>&1
else
	echo "...skipped raspberrypi tests cuz machine not available"
fi


#test with usual set of valgrind suppressions
echo "Run-Tests VALGRIND PURIFY/BLOCK_ALLOC..."
VALGRIND_SUPPRESSIONS="Common-Valgrind.supp"  make CONFIGURATION=ReleaseConfig_With_VALGRIND_PURIFY_NO_BLOCK_ALLOC VALGRIND=1 run-tests >>$TEST_OUT_FILE 2>&1 

#more suppressions, but run on debug build...
VALGRIND_SUPPRESSIONS="OpenSSL.supp Common-Valgrind.supp BlockAllocation-Valgrind.supp" make CONFIGURATION=gcc-5.2.0-debug-c++17 VALGRIND=1 run-tests >>$TEST_OUT_FILE 2>&1


X1=`cat $TEST_OUT_FILE | grep seconds | grep -i -F [Succeeded] | wc -l`
if [ $ARMTESTMACHINEAVAIL -eq 0 ]; then
	if [ $X1 -lt 564 ]; then
		echo "        ***    $X1 tests succeeded and expected 564";
	fi
else
	if [ $X1 -lt 611 ]; then
		echo "        ***    $X1 tests succeeded and expected 611";
	fi
fi
XF=`cat $TEST_OUT_FILE | grep -i -F FAILED | wc -l`
XC=`cat $TEST_OUT_FILE | grep -i -F "core dump" | wc -l`

echo "$X1 items succeeded"
echo "$XF items failed"
echo "$XC core dumps"

echo "Finished at `date`"

