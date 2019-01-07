MACHINE=${MACHINE:-LewisLinuxDevVM7}
LOGIN=lewis
SSH_TARGET=$LOGIN@$MACHINE
#BUILD_DIR=Sandbox/Stroika-Reg-Tests-Dev
BUILD_DIR=Sandbox/Stroika-RegressionTests

VER=`ssh $SSH_TARGET cd $BUILD_DIR && ScriptsLib/ExtractVersionInformation STROIKA_VERSION FullVersionString`
echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-$VER-OUT.txt Tests/HistoricalRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalRegressionTestResults/REGRESSION-TESTS-Linux-$VER-OUT.txt Tests/HistoricalRegressionTestResults/

echo scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-linux-gcc-7.2.0-x64.txt Tests/HistoricalPerformanceRegressionTestResults/
scp $SSH_TARGET:$BUILD_DIR/Tests/HistoricalPerformanceRegressionTestResults/PerformanceDump-$VER-linux-gcc-7.2.0-x64.txt Tests/HistoricalPerformanceRegressionTestResults/ || true
