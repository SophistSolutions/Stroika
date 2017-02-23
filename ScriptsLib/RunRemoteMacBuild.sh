MACHINE=lewis-Mac
LOGIN=lewis
SSH_TARGET=$LOGIN@$MACHINE
BUILD_DIR=Sandbox/Stroika-Remote-Build-Dir

echo ssh $SSH_TARGET rm -rf $BUILD_DIR
ssh $SSH_TARGET rm -rf $BUILD_DIR

echo ssh $SSH_TARGET "mkdir -p $BUILD_DIR"
ssh $SSH_TARGET "mkdir -p $BUILD_DIR"

echo ssh $SSH_TARGET "git clone git@github.com:SophistSolutions/Stroika.git $BUILD_DIR --branch V2-Dev"
ssh $SSH_TARGET "git clone git@github.com:SophistSolutions/Stroika.git $BUILD_DIR --branch V2-Dev"

CMD2Exec='export PATH=$PATH:/usr/local/bin/&&'
CMD2Exec=$CMD2Exec"cd $BUILD_DIR && ScriptsLib/RegressionTests.sh"
echo ssh $SSH_TARGET $CMD2Exec
ssh $SSH_TARGET "$CMD2Exec"

echo ssh $SSH_TARGET "cd $BUILD_DIR && ScriptsLib/RunPerformanceRegressionTests.sh"
ssh $SSH_TARGET "cd $BUILD_DIR && ScriptsLib/RunPerformanceRegressionTests.sh"


VER=`ssh $SSH_TARGET cd $BUILD_DIR && ScriptsLib/ExtractVersionInformation.sh STROIKA_VERSION FullVersionString`
echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-MacOS-XCode8-$VER-OUT.txt Tests/HistoricalRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-MacOS-XCode8-$VER-OUT.txt Tests/HistoricalRegressionTestResults/

echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-MacOS-x86-XCode8.txt Tests/HistoricalPerformanceRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-MacOS-x86-XCode8.txt Tests/HistoricalPerformanceRegressionTestResults/
