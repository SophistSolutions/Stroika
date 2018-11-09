# ***Regression Tests***

* File Output
  * PerformanceDump-$TEST_TARGET-$VER.txt
  * REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

* Tests run for each release:

  * $TEST_TARGET=MacOS_XCode10
~~~
        MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode10 \
            ./ScriptsLib/RunRemoteRegressionTests.sh
~~~

  * $TEST_TARGET=Windows_VS2k17

     (on windows bash shell run)
~~~
        USE_TEST_BASENAME=Windows_VS2k17 \
            ./ScriptsLib/RegressionTests.sh
~~~

  * $TEST_TARGET=Ubuntu1804

    (remote execute on machine hercules using docker and copy back results)
~~~
        RUN_IN_DOCKER=1 \
            USE_TEST_BASENAME=Ubuntu1804 \
            BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
            PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
            MACHINE=Hercules \
            ./ScriptsLib/RunRemoteRegressionTests.sh
~~~

  * $TEST_TARGET=Ubuntu1804-Cross-Compile2RaspberryPi
    
    (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results)
~~~
        RUN_IN_DOCKER=1 \
            USE_TEST_BASENAME=Ubuntu1804-Cross-Compile2RaspberryPi \
            BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
            MACHINE=Hercules \
            ./ScriptsLib/RunRemoteRegressionTests.sh
~~~



* Checkin results:
~~~
VER=2.1d11 sh -c \
    'git add Tests/*TestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/*TestResults/PerformanceDump-*-$VER.txt'
~~~
