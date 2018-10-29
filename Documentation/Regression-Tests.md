***Regression Tests***


   * File Output 
   
     *  PerformanceDump-$TEST_TARGET-$VER.txt 
     *  REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

   * Tests run for each release:
     
~~~
MACHINE=StroikaTestVM-Ubuntu1804-1 USE_TEST_BASENAME=Ubuntu1804_x86_64 DO_ONLY_DEFAULT_CONFIGURATIONS=0 \
    ./ScriptsLib/RunRemoteRegressionTests.sh
~~~
     
     * $TEST_TARGET=MacOS_XCode10
     
       PerformanceDump-MacOS_XCode10-$VER.txt REGRESSION-TESTS-MacOS_XCode10-$VER-OUT.txt
     
~~~
MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode10 \
    ./ScriptsLib/RunRemoteRegressionTests.sh
~~~

     * $TEST_TARGET=Windows_VS2k17 
     
       PerformanceDump-Windows_VS2k17-$VER.txt REGRESSION-TESTS-Windows_VS2k17-$VER-OUT.txt
     
{on windows bash shell run)
~~~
USE_TEST_BASENAME=Windows_VS2k17 \
    ./ScriptsLib/RegressionTests.sh && ./ScriptsLib/RunPerformanceRegressionTests.sh

~~~



   * Draft REMOTE IN DOCKER RUN

~~~
  RUN_IN_DOCKER=1 \
    USE_TEST_BASENAME=Ubuntu1804 \
    BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
    PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
    MACHINE=Hercules \
    ./ScriptsLib/RunRemoteRegressionTests.sh
~~~


~~~
	RUN_IN_DOCKER=1 \
	 USE_TEST_BASENAME=Ubuntu1804-Cross-Compile2RaspberryPi \
	 BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
	 PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
	 MACHINE=Hercules \
	 ./ScriptsLib/RunRemoteRegressionTests.sh

~~~



   * Checkin results:

~~~
VER=2.1d5 sh -c \
    'git add Tests/*TestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/*TestResults/PerformanceDump-*-$VER.txt'
~~~
