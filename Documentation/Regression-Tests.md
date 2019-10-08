# ***Regression Tests***

## Background

This is the test script I use to test each release. It assumes appropriately setup machines on
my local network.

To simply run the regression tests on your system, use (on that sysmtem)

~~~bash
    ./ScriptsLib/RegressionTests
~~~

## File Output (for each $TEST_TARGET)

* PerformanceDump-$TEST_TARGET-$VER.txt
* REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

## Tests run for each release (currently used $TEST_TARGET values)

These can all be run at the same time (each in a separate bash shell window). Most of them
do work remotely on other machines (MACHINE=ARG), and then copy back results to this machine where I can
checkin from one spot.

* $TEST_TARGET=MacOS_XCode10

    (takes about 1 HR)

    ~~~bash
    MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode10 \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Windows_VS2k17

    (on windows bash shell run; takes about 8 HRs)

    ~~~bash

    USE_TEST_BASENAME=Windows_VS2k17 PLATFORM=VisualStudio.Net-2017 \
        ./ScriptsLib/RegressionTests
    ~~~

* $TEST_TARGET=Windows_VS2k19

    (on windows bash shell run; takes about 8 HRs)

    ~~~bash
    USE_TEST_BASENAME=Windows_VS2k19 PLATFORM=VisualStudio.Net-2019 \
        ./ScriptsLib/RegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1804_x86_64

    (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1804_x86_64 \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests \
        PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1804-Cross-Compile2RaspberryPi

    (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1804-Cross-Compile2RaspberryPi \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1810_x86_64

    (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1810_x86_64 \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=default-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1810-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1810-Cross-Compile2RaspberryPi

    (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs)

    ~~~bash
    sleep 14000 && RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1810-Cross-Compile2RaspberryPi \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1810-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1904_x86_64

    (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1904_x86_64 \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=default-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1904-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1904-Cross-Compile2RaspberryPi

    (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs)
      - note - many of these tests fail as of 2019-04-22, because its hard to get the latest version of libc on a raspberrypi. Ignore those failures.

    ~~~bash
    sleep 28000 && RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1904-Cross-Compile2RaspberryPi \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1904-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Ubuntu1910_x86_64

    (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Ubuntu1910_x86_64 \
        BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1910-regression-tests \
        MACHINE=Hercules \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

* $TEST_TARGET=Centos7_x86_64

    (remote execute on machine hercules using docker and copy back results; takes about 30 minutes)

    ~~~bash
    RUN_IN_DOCKER=1 \
        USE_TEST_BASENAME=Centos7_x86_64 \
        CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-centos-7-small \
        MACHINE=Hercules \
        CMD2RUN="scl enable devtoolset-8 ScriptsLib/RegressionTests" \
        ./ScriptsLib/RunRemoteRegressionTests
    ~~~

## Checkin results

~~~bash
VER=2.1d14 sh -c \
    'git add Tests/*TestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/*TestResults/PerformanceDump-*-$VER.txt'
~~~
