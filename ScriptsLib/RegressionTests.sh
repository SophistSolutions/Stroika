#!/bin/bash

set -E
trap '[ "$?" -ne 77 ] || exit 77' ERR


### until we really support makefiles better - instead of using project files to parallelize
if [ `uname -s | cut -b 1-6` == "CYGWIN" ] ; then
	if [ -z ${PARALELLMAKEFLAG} ] ; then : ${PARALELLMAKEFLAG:=-j1}; fi
fi


: ${CONTINUE:=0}
if [ -z ${INCLUDE_VALGRIND_MEMCHECK_TESTS+x} ] ; then hash valgrind 2> /dev/null; if [ $? -eq 0 ]; then  INCLUDE_VALGRIND_MEMCHECK_TESTS=1; else  INCLUDE_VALGRIND_MEMCHECK_TESTS=0; fi; fi
if [ -z ${INCLUDE_VALGRIND_HELGRIND_TESTS+x} ] ; then hash valgrind 2> /dev/null; if [ $? -eq 0 ]; then  INCLUDE_VALGRIND_HELGRIND_TESTS=1; else  INCLUDE_VALGRIND_HELGRIND_TESTS=0; fi; fi
: ${INCLUDE_PERFORMANCE_TESTS:=1}
if [ -z ${CLOBBER_FIRST+x} ] ; then if [ $CONTINUE -eq 1 ]; then  CLOBBER_FIRST=0; else  CLOBBER_FIRST=1; fi; fi
: ${PARALELLMAKEFLAG:=-j10}
: ${DO_ONLY_DEFAULT_CONFIGURATIONS:=0}
: ${USE_TEST_BASENAME:=""}


VER=`ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`


mkdir -p Tests/HistoricalRegressionTestResults


if [ "$(uname -s)" == "Darwin" ] ; then
	if [ "$USE_TEST_BASENAME" == "" ] ; then USE_TEST_BASENAME="MacOS_XCode10"; fi
    echo "USING MacOS($USE_TEST_BASENAME)..."
    DO_ONLY_DEFAULT_CONFIGURATIONS=1
fi
if [ `uname -s | cut -b 1-6` == "CYGWIN" ] ; then
	if [ "$USE_TEST_BASENAME" == "" ] ; then USE_TEST_BASENAME="Windows_VS2k17"; fi
    echo "USING VS2k17 ($USE_TEST_BASENAME)..."
    #@todo maybe augment to pass on make default-configurations call:  DEFAULT_CONFIGURATION_ARGS="--platform VisualStudio.Net-2017"
    DO_ONLY_DEFAULT_CONFIGURATIONS=1
fi


if [ "$USE_TEST_BASENAME" == "" ] ; then USE_TEST_BASENAME="Linux"; fi
TEST_OUT_FILE=Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-$USE_TEST_BASENAME-$VER-OUT.txt



if [ $DO_ONLY_DEFAULT_CONFIGURATIONS -eq 1 ] ; then
	INCLUDE_VALGRIND_MEMCHECK_TESTS=0
	INCLUDE_VALGRIND_HELGRIND_TESTS=0
	INCLUDE_PERFORMANCE_TESTS=0
fi




PREFIX_OUT_LABEL=")))-"

STARTAT_INT=$(date +%s)


STARTAT=`date`;
if [ $CONTINUE -ne 0 ] ; then
	echo "CONTINUING (output to $TEST_OUT_FILE) - started at $STARTAT"
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "CONTINUING (output to $TEST_OUT_FILE) - started at $STARTAT" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
	echo "$PREFIX_OUT_LABEL" "---------------------" >>$TEST_OUT_FILE 2>&1
else
	rm -f $TEST_OUT_FILE
	echo "Resetting all configurations to standard regression test set (output to $TEST_OUT_FILE) - started at $STARTAT"
	echo "$PREFIX_OUT_LABEL" "Resetting all configurations to standard regression test set (output to $TEST_OUT_FILE) - started at $STARTAT" >>$TEST_OUT_FILE 2>&1
	if [ $DO_ONLY_DEFAULT_CONFIGURATIONS -eq 1 ] ; then
		make default-configurations >>$TEST_OUT_FILE 2>&1
	else
		make regression-test-configurations >>$TEST_OUT_FILE 2>&1
	fi
fi

NUM_CONFIGURATIONS=`ScriptsLib/GetConfigurations | wc -w`
NUM_REGTESTS=`wc -l Tests/Tests-Description.txt | awk '{print $1;}'`
NUM_PASSES_OF_REGTESTS_RUN=$NUM_CONFIGURATIONS

echo "Building configurations ($NUM_CONFIGURATIONS):"
echo "$PREFIX_OUT_LABEL" "Building configurations ($NUM_CONFIGURATIONS):" >>$TEST_OUT_FILE 2>&1
for i in `ScriptsLib/GetConfigurations`; do
	echo "   $i";
	echo "   $i"  >>$TEST_OUT_FILE 2>&1;
done

if [ $CLOBBER_FIRST -ne 0 ] ; then
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


### @todo - cleanup so have better logical test
# cuz arm tests done explicitly below (maybe should automatically substract 'cross-compiled tests')
if [ $DO_ONLY_DEFAULT_CONFIGURATIONS -eq 0 ] ; then
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN - 6))
	RASPBERRYPI_REMOTE_MACHINE=raspberrypi
	RASPBERRYPI_REMOTE_WITH_LOGIN=lewis@$RASPBERRYPI_REMOTE_MACHINE
	echo -n "Run-Tests raspberrypi remote..."
	echo "$PREFIX_OUT_LABEL" "Run-Tests raspberrypi remote..." >>$TEST_OUT_FILE 2>&1
	ARMTESTMACHINEAVAIL=`(ping raspberrypi -c 4 2>/dev/null 1>/dev/null); echo $?`
	if [ $ARMTESTMACHINEAVAIL -eq 0 ]; then
		STAGE_STARTAT_INT=$(date +%s)
		#RASPBERRYPICONFIGS=`./ScriptsLib/GetConfigurations | grep -oh "raspberrypi-.*\w"`
		RASPBERRYPICONFIGS=`make list-configurations TAGS="raspberrypi"`
		#RASPBERRYPIVALGRINDCONFIGS=`./ScriptsLib/GetConfigurations | grep -oh "raspberrypi-valgrind.*\w"`
		RASPBERRYPIVALGRINDCONFIGS=`make list-configurations TAGS="raspberrypi valgrind"`
		for i in $RASPBERRYPICONFIGS; do 
			echo "$PREFIX_OUT_LABEL" "make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN" >>$TEST_OUT_FILE 2>&1
			make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN >>$TEST_OUT_FILE 2>&1
			NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))
		done; 
		for i in $RASPBERRYPIVALGRINDCONFIGS; do 
			echo "$PREFIX_OUT_LABEL" "make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN VALGRIND=memcheck" >>$TEST_OUT_FILE 2>&1
			make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN VALGRIND=memcheck >>$TEST_OUT_FILE 2>&1
			echo "$PREFIX_OUT_LABEL" "make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN VALGRIND=helgrind" >>$TEST_OUT_FILE 2>&1
			make run-tests CONFIGURATION=$i REMOTE=$RASPBERRYPI_REMOTE_WITH_LOGIN VALGRIND=helgrind >>$TEST_OUT_FILE 2>&1
			NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 2))
		done; 
		STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
		echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
		echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	else
		echo "...skipped raspberrypi tests cuz machine not available"
		echo "...skipped raspberrypi tests cuz machine not available">>$TEST_OUT_FILE 2>&1
	fi
fi



############## VALGRIND TESTS ################
#MEMCHECK: release, no block allocation
if [ "$INCLUDE_VALGRIND_MEMCHECK_TESTS" -ne 0 ] ; then
	echo -n "make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc  VALGRIND=memcheck run-tests ..."
	echo "$PREFIX_OUT_LABEL" "make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc VALGRIND=memcheck run-tests ..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc VALGRIND=memcheck run-tests >>$TEST_OUT_FILE 2>&1 
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))

	#MEMCHECK: debug (with block alloc)
	echo -n 'VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp" make CONFIGURATION=g++-valgrind-debug-SSLPurify VALGRIND=memcheck run-tests ...'
	echo "$PREFIX_OUT_LABEL" 'VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp" make CONFIGURATION=g++-valgrind-debug-SSLPurify VALGRIND=memcheck run-tests ...' >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp" make CONFIGURATION=g++-valgrind-debug-SSLPurify VALGRIND=memcheck run-tests >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))

	#MEMCHECK: debug (without block alloc)
	echo -n "make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=memcheck run-tests ..."
	echo "$PREFIX_OUT_LABEL" "make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=memcheck run-tests ..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=memcheck run-tests >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))
else
	echo "Skipping valgrind MemCheck test because INCLUDE_VALGRIND_MEMCHECK_TESTS=$INCLUDE_VALGRIND_MEMCHECK_TESTS"
	echo "$PREFIX_OUT_LABEL" "Skipping valgrind memcheck test because INCLUDE_VALGRIND_MEMCHECK_TESTS=$INCLUDE_VALGRIND_MEMCHECK_TESTS" >>$TEST_OUT_FILE 2>&1
fi



#HELGRIND
if [ "$INCLUDE_VALGRIND_HELGRIND_TESTS" -ne 0 ] ; then
	#HELGRIND: release, no block allocation
	echo -n "make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc  VALGRIND=helgrind run-tests ..."
	echo "$PREFIX_OUT_LABEL" "make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc VALGRIND=helgrind run-tests ..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	make CONFIGURATION=g++-valgrind-release-SSLPurify-NoBlockAlloc  VALGRIND=helgrind run-tests >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))

	#HELGRIND: debug (without block alloc)
	echo -n "make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=helgrind run-tests ..."
	echo "$PREFIX_OUT_LABEL" "make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=helgrind run-tests ..." >>$TEST_OUT_FILE 2>&1
	STAGE_STARTAT_INT=$(date +%s)
	make CONFIGURATION=g++-valgrind-debug-SSLPurify-NoBlockAlloc VALGRIND=helgrind run-tests >>$TEST_OUT_FILE 2>&1
	STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
	echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	NUM_PASSES_OF_REGTESTS_RUN=$(($NUM_PASSES_OF_REGTESTS_RUN + 1))
else
	echo "Skipping helgrind test because INCLUDE_VALGRIND_HELGRIND_TESTS=$INCLUDE_VALGRIND_HELGRIND_TESTS"
	echo "$PREFIX_OUT_LABEL" "Skipping helgrind test because INCLUDE_VALGRIND_HELGRIND_TESTS=$INCLUDE_VALGRIND_HELGRIND_TESTS" >>$TEST_OUT_FILE 2>&1
fi

SAMPLE_APPS_2_VALGRIND=()
SAMPLE_APPS_2_VALGRIND+=("Samples-ArchiveUtility/ArchiveUtility --list ThirdPartyComponents/Origs-Cache/sqlite-amalgamation-*.zip")
SAMPLE_APPS_2_VALGRIND+=("Samples-ArchiveUtility/ArchiveUtility --list ThirdPartyComponents/Origs-Cache/lzma1604.7z")
SAMPLE_APPS_2_VALGRIND+=("Samples-Containers/Containers")
SAMPLE_APPS_2_VALGRIND+=("Samples-Serialization/Serialization")
SAMPLE_APPS_2_VALGRIND+=("Samples-SystemPerformanceClient/SystemPerformanceClient")
SAMPLE_APPS_2_VALGRIND+=("Samples-Traceroute/Traceroute www.sophists.com")
SAMPLE_APPS_2_VALGRIND+=("Samples-WebServer/WebServer --quit-after 10")

#HELGRIND ON SYSPERFORM (experimental - must find better way)
if [ "$INCLUDE_VALGRIND_HELGRIND_TESTS" -ne 0 ] ; then

	for app in "${SAMPLE_APPS_2_VALGRIND[@]}" ; do 
		echo -n "valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/$app ..."
		echo -n "$PREFIX_OUT_LABEL" "valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/$app..." >>$TEST_OUT_FILE 2>&1
		STAGE_STARTAT_INT=$(date +%s)
		valgrind -q --tool=helgrind --suppressions=Tests/Valgrind-Helgrind-Common.supp --log-file=valgrind-log.tmp Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/$app 2>&1 > /dev/null
		cat valgrind-log.tmp >> $TEST_OUT_FILE
		rm -f valgrind-log.tmp 
		STAGE_TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STAGE_STARTAT_INT )) / 60))
		echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)"
		echo "done (in $STAGE_TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
	done; 

else
	echo "Skipping helgrind SAMPLE_APPS_2_VALGRIND because INCLUDE_VALGRIND_HELGRIND_TESTS=$INCLUDE_VALGRIND_HELGRIND_TESTS"
	echo "$PREFIX_OUT_LABEL" "Skipping helgrind SAMPLE_APPS_2_VALGRIND because INCLUDE_VALGRIND_HELGRIND_TESTS=$INCLUDE_VALGRIND_HELGRIND_TESTS" >>$TEST_OUT_FILE 2>&1
fi


if [ $INCLUDE_PERFORMANCE_TESTS -ne 0 ] ; then
	ScriptsLib/RunPerformanceRegressionTests.sh
fi


echo "Regression Tests Summary:"
echo "$PREFIX_OUT_LABEL" "Regression Tests Summary:" >>$TEST_OUT_FILE 2>&1

TOTAL_REGTESTS_EXPECTED_TO_PASS=$(($NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS))

X1=`cat $TEST_OUT_FILE | grep seconds | grep -F "[Succeeded]" | grep -F " seconds)" | wc -l`
XF=`cat $TEST_OUT_FILE | grep -i FAILED | wc -l`
XE=`cat $TEST_OUT_FILE | grep -E -i "error.*:" | wc -l`
XC=`cat $TEST_OUT_FILE | grep -i "core dump" | wc -l`
VOL=`grep == $TEST_OUT_FILE | wc -l`
XW=`cat $TEST_OUT_FILE | grep -E -i "warning.*:" | wc -l`

if [ $X1 -lt $TOTAL_REGTESTS_EXPECTED_TO_PASS ]; then
	echo "   ***   WARNING: $X1 tests succeeded and expected $TOTAL_REGTESTS_EXPECTED_TO_PASS";
fi

TOTALCONFIGS_=`./ScriptsLib/GetConfigurations | wc -w`
TOTAL_WARNINGS_EXPECTED=0

if [ $X1 -eq $TOTAL_REGTESTS_EXPECTED_TO_PASS ]; then
	echo "   $X1 items succeeded (AS expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)"
	echo "   $X1 items succeeded (AS expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)">>$TEST_OUT_FILE 2>&1
else
	echo "   $X1 items succeeded (expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)"
	echo "   $X1 items succeeded (expected $NUM_PASSES_OF_REGTESTS_RUN * $NUM_REGTESTS)">>$TEST_OUT_FILE 2>&1
fi
echo "   $XF items failed (expected 0)"
echo "   $XF items failed (expected 0)">>$TEST_OUT_FILE 2>&1
echo "   $XE items error (expected 0)"
echo "   $XE items error (expected 0)">>$TEST_OUT_FILE 2>&1
echo "   $XW items warned (expected $TOTAL_WARNINGS_EXPECTED)"
echo "   $XW items warned (expected $TOTAL_WARNINGS_EXPECTED)">>$TEST_OUT_FILE 2>&1
echo "   $XC core dumps (expected 0)"
echo "   $XC core dumps (expected 0)">>$TEST_OUT_FILE 2>&1
if [ "$INCLUDE_VALGRIND_HELGRIND_TESTS" -ne 0 ] || [ "$INCLUDE_VALGRIND_MEMCHECK_TESTS" -ne 0 ] ; then
	echo "   $VOL valgrind output lines (apx $(($VOL / 27)) errors (expected 0)"
	echo "   $VOL valgrind output lines (apx $(($VOL / 27)) errors (expected 0)" >>$TEST_OUT_FILE 2>&1
fi
TOTAL_MINUTES_SPENT=$(($(( $(date +%s) - $STARTAT_INT )) / 60))
echo "Finished at `date` ($TOTAL_MINUTES_SPENT minutes)"
echo "$PREFIX_OUT_LABEL" "Finished at `date`  ($TOTAL_MINUTES_SPENT minutes)">>$TEST_OUT_FILE 2>&1
