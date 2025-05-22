# **_Regression Tests_**

## Background

This is the test script I use to test each release. It assumes appropriately setup machines on
my local network.

To simply run the regression tests on your system, use (on that sysmtem)

```bash
./ScriptsLib/RegressionTests
```

## Pre-requisites

### MongoDB

If you wish to test against mongodb, a mongo instance can be setup as:

~~~bash
docker run --name mongodb -d -p 27017:27017 -e MONGO_INITDB_ROOT_USERNAME=admin -e MONGO_INITDB_ROOT_PASSWORD=pass mongo:latest
~~~

Then pass this ENV variable to the regression tests

~~~bash
MONGO_CONNECTION_STRING=mongodb://admin:pass@localhost:27017
~~~

or 

~~~bash
MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017
~~~
or 

~~~bash
MONGO_CONNECTION_STRING=mongodb://admin:pass@192.168.244.234:27017
~~~

For my home regression tests, I run that on 'medusa', and use MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa.local:27017


## File Output (for each \$TEST_TARGET)

- PerformanceDump-$TEST_TARGET-$VER.txt
- REGRESSION-TESTS-$TEST_TARGET-$VER-OUT.txt

## Tests run for each release (currently used \$TEST_TARGET values)

These can all be run at the same time (each in a separate bash shell window). Most of them
do work remotely on other machines (MACHINE=ARG), and then copy back results to this machine where I can
checkin from one spot.

- \$TEST_TARGET=MacOS_XCode16_m1

  (takes about 1 HR)

  ```bash
  MACHINE=lewis-Mac2 USE_TEST_BASENAME=MacOS_XCode16_m1 PARALELLMAKEFLAG=-j5 \
      MONGO_CONNECTION_STRING=mongodb://admin:pass@192.168.244.234:27017 \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Windows_VS2k22

  (on windows bash shell run; takes about 4 HRs)
  (note this done on my laptop, not windows-dev-vm)

  ```bash
  USE_TEST_BASENAME=Windows_`./ScriptsLib/DetectedHostOS`_VS2k22 PLATFORM=VisualStudio.Net-2022 \
    MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 \
      ./ScriptsLib/RegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204_x86_64

  (remote execute on machine medusa using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      MACHINE=medusa \
	    EXTRA_DOCKER_ARGS=" --add-host mongodb:`./ScriptsLib/ResolveIP medusa` " \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2404_x86_64

  (remote execute on machine medusa using docker and copy back results; takes about 10 HRs)

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2404_x86_64 \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
      MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests \
      MACHINE=medusa \
	    EXTRA_DOCKER_ARGS=" --add-host mongodb:`./ScriptsLib/ResolveIP medusa` " \
      ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2504_x86_64

  (remote execute on machine medusa using docker and copy back results; takes about 6 HRs)

  ```bash
RUN_IN_DOCKER=1 \
    USE_TEST_BASENAME=Ubuntu2504_x86_64 \
    BUILD_CONFIGURATIONS_MAKEFILE_TARGET=basic-unix-test-configurations \
    MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 \
    CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2504-regression-tests \
    MACHINE=medusa \
	  EXTRA_DOCKER_ARGS=" --add-host mongodb:`./ScriptsLib/ResolveIP medusa` " \
    ./ScriptsLib/RunRemoteRegressionTests
  ```

- \$TEST_TARGET=Ubuntu2204-Cross-Compile2RaspberryPi

  (remote execute on machine medusa (and then that will test on raspberrypi) - using docker and copy back results; takes about 4 HRs).

  ```bash
  RUN_IN_DOCKER=1 \
      USE_TEST_BASENAME=Ubuntu2204-Cross-Compile2RaspberryPi \
      RASPBERRYPI_REMOTE_MACHINE=raspberrypi \
      BUILD_CONFIGURATIONS_MAKEFILE_TARGET=raspberrypi-cross-compile-test-configurations \
      MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 \
      CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests \
      MACHINE=medusa \
	    XTRA_DOCKER_ARGS=" --add-host mongodb:`./ScriptsLib/ResolveIP medusa`  --add-host raspberrypi:`./ScriptsLib/ResolveIP raspberrypi.local` " \
      ./ScriptsLib/RunRemoteRegressionTests
  ```


- Docker windows tests


Must be done on Windows machine (currently doesnt work on - even windows - vm)

  ```bash
  for var in  "Cygwin-VS2k22" "MSYS-VS2k22" ; do LCV=`echo "${var}" | tr '[:upper:]' '[:lower:]'` MONGO_CONNECTION_STRING=mongodb://admin:pass@medusa:27017 CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-${LCV} USE_TEST_BASENAME=Windows_${var}-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests ; done
  ```

  OR alternatively

  ```sh
  MONGO_CONNECTION_STRING=mongodb://admin:pass@192.168.244.234:27017 \
    CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22 USE_TEST_BASENAME=Windows_Cygwin_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
  MONGO_CONNECTION_STRING=mongodb://admin:pass@192.168.244.234:27017 \
    CONTAINER_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22 USE_TEST_BASENAME=Windows_MSYS_VS2k22-In-Docker ./ScriptsLib/RunLocalWindowsDockerRegressionTests
  ```

- WSL (tested on Ubuntu 22.04) test
  (inside WSL2 bash prompt)
  (may work on WSL1, but very slow, and not worth it - just test WSL2 from now on)

  ```bash
  MONGO_CONNECTION_STRING=mongodb://admin:pass@192.168.244.234:27017 ScriptsLib/RunLocalWSLRegressionTests
  ```
