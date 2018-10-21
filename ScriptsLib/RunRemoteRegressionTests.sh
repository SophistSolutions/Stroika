#
# Run a build and set of regression tests on the given remote machine, and pull back the built logs.
#
### @todo - make MACHINE a required parameter - no good default!!!
: ${MACHINE:=lewis-Mac}
: ${LOGIN:=lewis}
: ${SSH_TARGET:=$LOGIN@$MACHINE}
: ${BUILD_DIR:=Sandbox/Stroika-Remote-Build-Dir}
: ${BRANCH:=V2.1-Dev}
: ${BUILD_CONFIGURATIONS_MAKEFILE_TARGET:=default-configurations}

echo ssh $SSH_TARGET rm -rf $BUILD_DIR
ssh $SSH_TARGET rm -rf $BUILD_DIR

echo ssh $SSH_TARGET "mkdir -p $BUILD_DIR"
ssh $SSH_TARGET "mkdir -p $BUILD_DIR"

echo ssh $SSH_TARGET "git clone git@github.com:SophistSolutions/Stroika.git $BUILD_DIR --branch $BRANCH"
ssh $SSH_TARGET "git clone git@github.com:SophistSolutions/Stroika.git $BUILD_DIR --branch $BRANCH"

VER=`ssh $SSH_TARGET cd $BUILD_DIR && ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`
if [ "$USE_TEST_BASENAME" == "" ] ; then 
	echo "ERROR: Require setting USE_TEST_BASENAME as argument to RunRemoteRegressionTest."
	exit 1;
fi
#see RegressionTests.sh for this name, and why we require $USE_TEST_BASENAME set
TEST_OUT_FILE=Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-$USE_TEST_BASENAME-$VER-OUT.txt

echo "Invoking remote RegressionTests.sh"
CMD2Exec=''
CMD2Exec+='export PATH=$PATH:/usr/local/bin/;'
CMD2Exec+="export BUILD_CONFIGURATIONS_MAKEFILE_TARGET=$BUILD_CONFIGURATIONS_MAKEFILE_TARGET;"
CMD2Exec+="export USE_TEST_BASENAME=$USE_TEST_BASENAME;"
CMD2Exec+="cd $BUILD_DIR && ScriptsLib/RegressionTests.sh"
echo ssh $SSH_TARGET $CMD2Exec
ssh $SSH_TARGET "$CMD2Exec"

CMD2Exec=''
CMD2Exec+="export USE_TEST_BASENAME=$USE_TEST_BASENAME;"
#if [ $DO_ONLY_DEFAULT_CONFIGURATIONS -eq 1 ] ; then
#	CMD2Exec+="export EXE=Builds/Release/Test50;"
#fi
CMD2Exec+="cd $BUILD_DIR && ScriptsLib/RunPerformanceRegressionTests.sh"
echo ssh $SSH_TARGET $CMD2Exec
ssh $SSH_TARGET "$CMD2Exec"



echo "Fetching results:"
echo scp $SSH_TARGET:$BUILD_DIR/$TEST_OUT_FILE Tests/HistoricalRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/$TEST_OUT_FILE Tests/HistoricalRegressionTestResults/

echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$USE_TEST_BASENAME-$VER.txt Tests/HistoricalPerformanceRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$USE_TEST_BASENAME-$VER.txt Tests/HistoricalPerformanceRegressionTestResults/
