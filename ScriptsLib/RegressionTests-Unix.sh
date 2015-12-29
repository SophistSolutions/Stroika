#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR

TEST_OUT_DIR=RegressionTests-Output
PARALELLMAKEFLAG=-j4


rm -rf $TEST_OUT_DIR



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
echo "Resetting all configurations to standard regression test set (output to REGRESSION-TESTS.OUT) - started at $STARTAT"
rm -f REGRESSION-TESTS.OUT
make regression-test-configurations 2>&1 >> REGRESSION-TESTS.OUT

make clobber 2>&1 >> REGRESSION-TESTS.OUT
echo "Make all..."
make all $PARALELLMAKEFLAG 2>&1 >> REGRESSION-TESTS.OUT

echo "Run-Tests ALL..."
make run-tests 2>&1 >> REGRESSION-TESTS.OUT

echo "Run-Tests raspberrypi remote..."
make run-tests CONFIGURATION=raspberrypi-gcc-4.9 REMOTE=lewis@raspberrypi 2>&1 >> REGRESSION-TESTS.OUT

#test with usual set of valgrind suppressions
echo "Run-Tests VALGRIND PURIFY/BLOCK_ALLOC..."
VALGRIND_SUPPRESSIONS="Common-Valgrind.supp"  make CONFIGURATION=DefaultConfig_With_VALGRIND_PURIFY_NO_BLOCK_ALLOC VALGRIND=1 run-tests 2>&1 >> REGRESSION-TESTS.OUT

#slow, and largely useless test...
#VALGRIND_SUPPRESSIONS="OpenSSL.supp Common-Valgrind.supp BlockAllocation-Valgrind.supp" make CONFIGURATION=DEFAULT_CONFIG VALGRIND=1 run-tests
echo "Finished at `date`"
