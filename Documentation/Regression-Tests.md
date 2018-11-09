# ***Regression Tests***

## Background

This is the test script I use to test each release. It assumes appropriately setup machines on
my local network.

To simply run the regression tests on your system, use (on that sysmtem)

~~~bash
    ./ScriptsLib/RegressionTests.sh
~~~

## File Output (for each $TEST_TARGET)

* PerformanceDump-$TEST_TARGET-$VER.txt
* REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

## Tests run for each release (currently used $TEST_TARGET values)

These can all be run at the same time.

* $TEST_TARGET=MacOS_XCode10

    ~~~bash
    MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode10 \
        ./ScriptsLib/RunRemoteRegressionTests.sh
    ~~~
    (takes about 1 HR)

* $TEST_TARGET=Windows_VS2k17

    (on windows bash shell run)
    ~~~bash
    USE_TEST_BASENAME=Windows_VS2k17 \
        ./ScriptsLib/RegressionTests.sh
        ~~~
    (takes about 8 HRs)

* $TEST_TARGET=Ubuntu1804_x86_64

    (remote execute on machine hercules using docker and copy back results)
    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1804_x86_64 \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
        PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests.sh
    ~~~
    (takes about 10 HRs)

* $TEST_TARGET=Ubuntu1804-Cross-Compile2RaspberryPi

    (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results)
    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1804-Cross-Compile2RaspberryPi \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests.sh
    ~~~
    (takes about 3 HRs)

## Checkin results

~~~bash
VER=2.1d12 sh -c \
    'git add Tests/*TestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/*TestResults/PerformanceDump-*-$VER.txt'
~~~
