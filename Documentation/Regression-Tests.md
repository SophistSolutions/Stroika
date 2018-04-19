***Regression Tests***


   * File Output 
   
     *  PerformanceDump-$TEST_TARGET-$VER.txt 
     *  REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

   * Tests run for each release:
     * $TEST_TARGET=Ubuntu1604_x86_64 

       PerformanceDump-Ubuntu1604_x86_64-$VER.txt REGRESSION-TESTS-Ubuntu1604_x86_64-$VER-OUT.txt


~~~
MACHINE=StroikaTestVM-Ubuntu1604-1 USE_TEST_BASENAME=Ubuntu1604_x86_64 ./ScriptsLib/RunRemoteRegressionTests.sh
~~~
     * $TEST_TARGET=Ubuntu1710_x86_64 
     
~~~
       MACHINE=StroikaTestVM-Ubuntu1710-1 USE_TEST_BASENAME=Ubuntu1710_x86_64 DO_ONLY_DEFAULT_CONFIGURATIONS=0 ./ScriptsLib/RunRemoteRegressionTests.sh
~~~
     
     * $TEST_TARGET=MacOS_XCode9.2 
     
       PerformanceDump-MacOS_XCode9.2-$VER.txt REGRESSION-TESTS-MacOS_XCode9.2-$VER-OUT.txt
     
~~~
       MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode9.2 ./ScriptsLib/RunRemoteRegressionTests.sh
~~~

     * $TEST_TARGET=Windows_VS2k17 
     
       PerformanceDump-Windows_VS2k17-$VER.txt REGRESSION-TESTS-Windows_VS2k17-$VER-OUT.txt
     
~~~{on windows bash shell run)
  USE_TEST_BASENAME=Windows_VS2k17 ./ScriptsLib/RegressionTests.sh
  USE_TEST_BASENAME=Windows_VS2k17 ./ScriptsLib/RunPerformanceRegressionTests.sh
~~~

   * Checkin results:

~~~
    VER=2.0a231 sh -c 'git add Tests/*TestResults/REGRESSION-TESTS-*-$VER-OUT.txt Tests/*TestResults/PerformanceDump-*-$VER.txt'
~~~
