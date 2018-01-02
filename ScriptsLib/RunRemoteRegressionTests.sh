#
# Run a build and set of regression tests on the given remote machine, and pull back the built logs.
#
: ${MACHINE:=lewis-Mac}
: ${LOGIN:=lewis}
: ${SSH_TARGET:=$LOGIN@$MACHINE}
: ${BUILD_DIR:=Sandbox/Stroika-Remote-Build-Dir}
: ${BRANCH:=V2-Dev}

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

echo "Invokging remote regression tests"
CMD2Exec='export PATH=$PATH:/usr/local/bin/&&'
CMD2Exec=$CMD2Exec"cd $BUILD_DIR && ScriptsLib/RegressionTests.sh"
echo ssh $SSH_TARGET $CMD2Exec
ssh $SSH_TARGET "$CMD2Exec"

echo ssh $SSH_TARGET "cd $BUILD_DIR && ScriptsLib/RunPerformanceRegressionTests.sh"
ssh $SSH_TARGET "cd $BUILD_DIR && ScriptsLib/RunPerformanceRegressionTests.sh"



echo "Fetching results:"
echo scp $SSH_TARGET:$BUILD_DIR/$TEST_OUT_FILE Tests/HistoricalRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/$TEST_OUT_FILE Tests/HistoricalRegressionTestResults/

echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$USE_TEST_BASENAME.txt Tests/HistoricalPerformanceRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-$USE_TEST_BASENAME.txt Tests/HistoricalPerformanceRegressionTestResults/
