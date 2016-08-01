#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR


: ${INCLUDE_HELGRIND_TESTS:=true}
: ${CLOBBER_FIRST:=true}
: ${PARALELLMAKEFLAG:=-j2}


VER=`ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`


mkdir -p Tests/HistoricalRegressionTestResults
TEST_OUT_FILE=Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-$VER-OUT.txt


if [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ] ; then
    #todo - rewrite - but for now  this works
    #LGP 2016-07-31
    TEST_OUT_FILE=Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Windows-$VER-OUT.txt
    echo - "make all run-tests REDIR TO:  $TEST_OUT_FILE ..."
    make clobber all run-tests 2>&1 > $TEST_OUT_FILE
    echo done
    exit 0;
fi




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

echo "Building configurations ($NUM_CONFIGURATIONS):"
echo "$PREFIX_OUT_LABEL" "Building configurations ($NUM_CONFIGURATIONS):" >>$TEST_OUT_FILE 2>&1
for i in `ScriptsLib/GetConfigurations.sh`; do
	echo "   $i";
	echo "   $i"  >>$TEST_OUT_FILE 2>&1;
done

if [ "$CLOBBER_FIRST" = true ] ; then
  	echo "Make clobber"
  	echo "Make clobber" >>$TEST_OUT_FILE 2>&1
  	make clobber >>$TEST_OUT_FILE 2>&1
else
	echo "Skipping Clobber"
	echo "Skipping Clobber" >>$TEST_OUT_FILE 2>&1
fi
STAGE_STARTAT_INT=$(date +%s)
echo -n "Make all ($PARALELLMAKEFLAG)..."
echo "$PREFIX_OUT_LABEL" "Make all ($PARALELLMAKEFLAG)..."  >>$TEST_OUT_FILE 2>&1
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
	make run-tests CONFIGURATION=raspberrypi-gcc-5 REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN >>$TEST_OUT_FILE 2>&1
	make run-tests CONFIGURATION=raspberrypi_valgrind_gcc-5_NoBlockAlloc REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
else
	echo "...skipped raspberrypi tests cuz machine not available"
	echo "...skipped raspberrypi tests cuz machine not available">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN - 2))
fi



############## VALGRIND TESTS ################
#MEMCHECK: release, no block allocation
echo -n "Run-Tests VALGRIND memcheck VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc..."
echo "$PREFIX_OUT_LABEL" "Run-Tests VALGRIND memcheck VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc..." >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp"  make CONFIGURATION=VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc VALGRIND=memcheck run-tests >>$TEST_OUT_FILE 2>&1 
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))

#MEMCHECK: debug
echo -n "Run-Tests VALGRIND memcheck VALGRIND_LatestGCC_Dbg_SSLPurify..."
echo "$PREFIX_OUT_LABEL" "Run-Tests VALGRIND memcheck VALGRIND_LatestGCC_Dbg_SSLPurify..." >>$TEST_OUT_FILE 2>&1
STAGE_STARTAT_INT=$(date +%s)
VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp" make CONFIGURATION=VALGRIND_LatestGCC_Dbg_SSLPurify VALGRIND=memcheck run-tests >>$TEST_OUT_FILE 2>&1
STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))

#HELGRIND
if [ "$INCLUDE_HELGRIND_TESTS" = true ] ; then
	echo -n "Run-Tests VALGRIND=helgrind VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc ..."
	echo "$PREFIX_OUT_LABEL" "Run-Tests VALGRIND=helgrind VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc ..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	VALGRIND_SUPPRESSIONS="Valgrind-Helgrind-Common.supp" make CONFIGURATION=VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc VALGRIND=helgrind run-tests >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))
else
	echo "Skipping helgrind test because INCLUDE_HELGRIND_TESTS=$INCLUDE_HELGRIND_TESTS"
	echo "$PREFIX_OUT_LABEL" "Skipping helgrind test because INCLUDE_HELGRIND_TESTS=$INCLUDE_HELGRIND_TESTS" >>$TEST_OUT_FILE 2>&1
fi



#HELGRIND ON SYSPERFORM (experimental - must find better way)
if [ "$INCLUDE_HELGRIND_TESTS" = true ] ; then
	echo -n "valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc/Samples_SystemPerformanceClient ..."
	echo "$PREFIX_OUT_LABEL" "valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc/Samples_SystemPerformanceClient..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc/Samples_SystemPerformanceClient 2>&1 > /dev/null
	cat valgrind-log.tmp >> $TEST_OUT_FILE
	rm -f valgrind-log.tmp 
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
else
	echo "Skipping helgrind Samples_SystemPerformanceClient because INCLUDE_HELGRIND_TESTS=$INCLUDE_HELGRIND_TESTS"
	echo "$PREFIX_OUT_LABEL" "Skipping helgrind Samples_SystemPerformanceClient because INCLUDE_HELGRIND_TESTS=$INCLUDE_HELGRIND_TESTS" >>$TEST_OUT_FILE 2>&1
fi


echo "Regression Tests Summary:"
echo "$PREFIX_OUT_LABEL" "Regression Tests Summary:" >>$TEST_OUT_FILE 2>&1

TOTAL_REGTESTS_EXPECTED_TO_PASS=$(($NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS))

X1=`cat $TEST_OUT_FILE | grep seconds | grep -F "[Succeeded]" | grep -F " seconds)" | wc -l`
XF=`cat $TEST_OUT_FILE | grep -i FAILED | wc -l`
XC=`cat $TEST_OUT_FILE | grep -i "core dump" | wc -l`
VOL=`grep == $TEST_OUT_FILE | wc -l`
XW=`cat $TEST_OUT_FILE | grep -i "warning:" | wc -l`

if [ $X1 -lt $TOTAL_REGTESTS_EXPECTED_TO_PASS ]; then
	echo "   ***   WARNING: $X1 tests succeeded and expected $TOTAL_REGTESTS_EXPECTED_TO_PASS";
fi

#VERY ROUGH GUESTIMATE - I think excludes 2 clang tests, and a few more?
TOTAL_WARNINGS_EXPECTED=$(($NUM_PASSES_OF_REGTESTS_RUN - 5))

echo "   $X1 items succeeded (expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)"
echo "   $X1 items succeeded (expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)">>$TEST_OUT_FILE 2>&1
echo "   $XF items failed (expected 0)"
echo "   $XF items failed (expected 0)">>$TEST_OUT_FILE 2>&1
echo "   $XW items warned (expected $TOTAL_WARNINGS_EXPECTED)"
echo "   $XW items warned (expected $TOTAL_WARNINGS_EXPECTED)">>$TEST_OUT_FILE 2>&1
echo "   $XC core dumps (expected 0)"
echo "   $XC core dumps (expected 0)">>$TEST_OUT_FILE 2>&1
echo "   $VOL valgrind output lines (apx $(($VOL / 27)) errors (expected 0)"
echo "   $VOL valgrind output lines (apx $(($VOL / 27)) errors (expected 0)" >>$TEST_OUT_FILE 2>&1

TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STARTAT_INT )) / 60))
echo "Finished at `date` ($TOTAL_MINUTES_SPENT minutes)"
echo "$PREFIX_OUT_LABEL" "Finished at `date`  ($TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
