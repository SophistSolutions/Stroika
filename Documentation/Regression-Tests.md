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

- \$TEST_TARGET=MacOS_XCode15_m1

  (takes about 1 HR)

  ```bash
  MACHINE=lewis-Mac2 USE_TEST_BASENAME=MacOS_XCode15_m1 PARALELLMAKEFLAG=-j5 \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Windows_VS2k22

  (on windows bash shell run; takes about 4 HRs)
  (note this done on my laptop, not windows-dev-vm)

  ```bash
  USE_TEST_BASENAME=Windows_`./ScriptsLib/DetectedHostOS`_VS2k22 PLATFORM=VisualStudio.Net-2022 \
      ./ScriptsLib/RegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2310_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2310_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2310-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2404_x86_64

  (remote execute on machine hercules using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2404_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204-Cross-Compile2RaspberryPi

  (remote execute on machine hercules (and then that will test on raspberrypi) - using docker and copy back results; takes about 4 HRs).

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204-Cross-Compile2RaspberryPi \
      RASPBERRYPI_REMOTE_MACHINE=192.168.244.32 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      MACHINE=Hercules \
      ./ScriptsLib/RunRemoteRegressionTests
  ```


- Docker windows tests


Must be done on Windows machine (currently doesnt work on - even windows - vm)

  ```bash
  for var in  "Cygwin-VS2k22" "MSYS-VS2k22" ; do LCV=`echo "${var}" | tr '[:upper:]' '[:lower:]'` CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-${LCV} USE_TEST_BASENAME=Windows_${var}-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests ; done
  ```

  OR alternatively

  ```sh
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22 USE_TEST_BASENAME=Windows_Cygwin_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
   CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22 USE_TEST_BASENAME=Windows_MSYS_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
  ```

- WSL (tested on Ubuntu 22.04) test
  (inside WSL2 bash prompt)
  (may work on WSL1, but very slow, and not worth it - just test WSL2 from now on)

  ```bash
  ScriptsLib/RunLocalWSLRegressionTests
  ```
