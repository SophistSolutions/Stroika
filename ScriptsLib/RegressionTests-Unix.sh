#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR


PRETTY_II_VERSION=

TEST_OUT_FILE=REGRESSION-TESTS-`cat STROIKA_VERSION | xargs`.OUT
PARALELLMAKEFLAG=-j3

PREFIX_OUT_LABEL=")))-"

STARTAT_INT=$(date +%s)


STARTAT=`date`;
rm -f $TEST_OUT_FILE
echo "Resetting all configurations to standard regression test set (output to $TEST_OUT_FILE) - started at $STARTAT"
echo "$PREFIX_OUT_LABEL" "Resetting all configurations to standard regression test set (output to $TEST_OUT_FILE) - started at $STARTAT" >>$TEST_OUT_FILE 2>&1
make regression-test-configurations >>$TEST_OUT_FILE 2>&1

NUM_CONFIGURATIONS=`sh ScriptsLib/GetConfigurations.sh | wc -w`
NUM_REGTESTS=47
NUM_PASSES_OF_REGTESTS_RUN=$NUM_CONFIGURATIONS

echo "Building configurations ($NUM_CONFIGURATIONS): `sh ScriptsLib/GetConfigurations.sh`"
echo "$PREFIX_OUT_LABEL" "Building configurations ($NUM_CONFIGURATIONS): `sh ScriptsLib/GetConfigurations.sh`" >>$TEST_OUT_FILE 2>&1


make clobber >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
echo -n "Make all..."
echo "$PREFIX_OUT_LABEL" "Make all..."  >>$TEST_OUT_FILE 2>&1
make all $PARALELLMAKEFLAG >>$TEST_OUT_FILE 2>&1
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1

echo -n "Run-Tests ALL..."
echo "$PREFIX_OUT_LABEL" "Run-Tests ALL..." >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
make run-tests >>$TEST_OUT_FILE 2>&1
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1


RASPBERRYPI_REMOTE_MACHINE=raspberrypi
RASPBERRYPI_REMOTE_WITH_LOGIN=lewis@$RASPBERRYPI_REMOTE_MACHINE
echo -n "Run-Tests raspberrypi remote..."
echo "$PREFIX_OUT_LABEL" "Run-Tests raspberrypi remote..." >>$TEST_OUT_FILE 2>&1
ARMTESTMACHINEAVAIL=`(ping raspberrypi -c 4 2>/dev/null 1>/dev/null); echo $?`
if [ $ARMTESTMACHINEAVAIL -eq 0 ]; then
	STAGE_STARTAT_INT=$(date +%s)
	make run-tests CONFIGURATION=raspberrypi-gcc-4.9 REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
else
	echo "...skipped raspberrypi tests cuz machine not available"
	echo "...skipped raspberrypi tests cuz machine not available">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN - 1))
fi




#test with usual set of valgrind suppressions
echo -n "Run-Tests VALGRIND PURIFY/BLOCK_ALLOC..."
echo "$PREFIX_OUT_LABEL" "Run-Tests VALGRIND PURIFY/BLOCK_ALLOC..." >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
VALGRIND_SUPPRESSIONS="Common-Valgrind.supp"  make CONFIGURATION=ReleaseConfig_With_VALGRIND_PURIFY_NO_BLOCK_ALLOC VALGRIND=1 run-tests >>$TEST_OUT_FILE 2>&1 
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1

#more suppressions, but run on debug build...
echo -n "Run-Tests VALGRIND gcc-5.2.0-debug-c++17..."
echo "$PREFIX_OUT_LABEL" "Run-Tests VALGRIND gcc-5.2.0-debug-c++17..." >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
VALGRIND_SUPPRESSIONS="OpenSSL.supp Common-Valgrind.supp BlockAllocation-Valgrind.supp" make CONFIGURATION=gcc-5.2.0-debug-c++17 VALGRIND=1 run-tests >>$TEST_OUT_FILE 2>&1
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1




echo "Regression Tests Summary:"
echo "$PREFIX_OUT_LABEL" "Regression Tests Summary:" >>$TEST_OUT_FILE 2>&1
X1=`cat $TEST_OUT_FILE | grep seconds | grep -i -F [Succeeded] | wc -l`
TOTAL_REGTESTS_EXPECTED_TO_PASS=$(($NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS))
if [ $X1 -lt $TOTAL_REGTESTS_EXPECTED_TO_PASS ]; then
	echo "   ***   $X1 tests succeeded and expected $TOTAL_REGTESTS_EXPECTED_TO_PASS";
fi

XF=`cat $TEST_OUT_FILE | grep -i -F FAILED | wc -l`
XC=`cat $TEST_OUT_FILE | grep -i -F "core dump" | wc -l`

echo "   $X1 items succeeded"
echo "   $X1 items succeeded">>$TEST_OUT_FILE 2>&1
echo "   $XF items failed"
echo "   $XF items failed">>$TEST_OUT_FILE 2>&1
echo "   $XC core dumps"
echo "   $XC core dumps">>$TEST_OUT_FILE 2>&1

TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STARTAT_INT )) / 60))
echo "Finished at `date` ($TOTAL_MINUTES_SPENT minutes)"
echo "$PREFIX_OUT_LABEL" "Finished at `date`  ($TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
