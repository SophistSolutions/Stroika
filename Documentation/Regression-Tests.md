# **_Regression Tests_**

## Background

This is the test script I use to test each release. It assumes appropriately setup machines on
my local network.

To simply run the regression tests on your system, use (on that sysmtem)

```bash
./ScriptsLib/RegressionTests
```

## File Output (for each \$TEST_TARGET)

- PerformanceDump-$TEST_TARGET-$VER.txt
- REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

## Tests run for each release (currently used \$TEST_TARGET values)

These can all be run at the same time (each in a separate bash shell window). Most of them
do work remotely on other machines (MACHINE=ARG), and then copy back results to this machine where I can
checkin from one spot.

- \$TEST_TARGET=MacOS_XCode13

  (takes about 2 HRs)

  ```bash
  MACHINE=lewis-Mac USE_TEST_BASENAME=MacOS_XCode13 PARALELLMAKEFLAG=-j5 \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=MacOS_XCode13_m1

  (takes about 1 HR)

  ```bash
  MACHINE=lewis-Mac2 USE_TEST_BASENAME=MacOS_XCode13_m1 PARALELLMAKEFLAG=-j5 \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Windows_VS2k17

  (on windows bash shell run; takes about 4 HRs)

  ```bash
  USE_TEST_BASENAME=Windows_`./ScriptsLib/DetectedHostOS`_VS2k17 PLATFORM=VisualStudio.Net-2017 \
      ./ScriptsLib/RegressionTests
  ```

- \$TEST_TARGET=Windows_VS2k19

  (on windows bash shell run; takes about 4 HRs)

  ```bash
  USE_TEST_BASENAME=Windows_`./ScriptsLib/DetectedHostOS`_VS2k19 PLATFORM=VisualStudio.Net-2019 \
      ./ScriptsLib/RegressionTests
  ```

- \$TEST_TARGET=Windows_VS2k22

  (on windows bash shell run; takes about 4 HRs)
  (note this done on my laptop, not windows-dev-vm)

  ```bash
  USE_TEST_BASENAME=Windows_`./ScriptsLib/DetectedHostOS`_VS2k22 PLATFORM=VisualStudio.Net-2022 \
      ./ScriptsLib/RegressionTests
  ```

- \$TEST_TARGET=Ubuntu1804_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu1804_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests \
      PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-1804-x64 \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2004_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2004_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests \
      PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-2004-x64 \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2110_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2110_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2110-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      PRIVATE_COMPILER_BUILDS_DIR=/private-compiler-builds-Ununtu-2204-x64 \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu1804-Cross-Compile2RaspberryPi

  (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu1804-Cross-Compile2RaspberryPi \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2004-Cross-Compile2RaspberryPi

  (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs) - note - many of these tests fail as of 2019-04-22, because its hard to get the latest version of libc on a raspberrypi. Ignore those failures.

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2004-Cross-Compile2RaspberryPi \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204-Cross-Compile2RaspberryPi

  (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 3 HRs) - note - many of these tests fail as of 2019-04-22, because its hard to get the latest version of libc on a raspberrypi. Ignore those failures.

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204-Cross-Compile2RaspberryPi \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```


- Docker windows tests

  ```bash
  for var in "Cygwin-VS2k19" "Cygwin-VS2k22" "MSYS-VS2k19" "MSYS-VS2k22" ; do LCV=`echo "${var}" | tr '[:upper:]' '[:lower:]'` CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-${LCV} USE_TEST_BASENAME=Windows_${var}-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests ; done
  ```


- Must be done on Windows machine (currently doesnt work on - even windows - vm)

  ```sh
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 USE_TEST_BASENAME=Windows_Cygwin_VS2k19-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22 USE_TEST_BASENAME=Windows_Cygwin_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19 USE_TEST_BASENAME=Windows_MSYS_VS2k19-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22 USE_TEST_BASENAME=Windows_MSYS_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
  ```

- WSL (tested on Ubuntu 20.04) test
  (inside WSL2 bash prompt)
  (may work on WSL1, but very slow, and not worth it - just test WSL2 from now on)

  ```bash
  ScriptsLib/RunLocalWSLRegressionTests
  ```
