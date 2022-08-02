DEFAULT_GOAL := help

STROIKA_VERSION=$(shell dos2unix < ../STROIKA_VERSION | xargs echo -n)
STROIKA_VERSION_Final=$(shell ../ScriptsLib/ExtractVersionInformation ../STROIKA_VERSION FinalBuild)

DETECTED_HOST_OS=$(shell ../ScriptsLib/DetectedHostOS)

help:
	@echo "make docker-images to create the docker images on your computer"
	@echo
	@echo "to run the image, use:"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2004-small   OR"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests   ETC"
	@echo
	@echo "n.b. you may need to use (add to run command)"
	@echo "    --security-opt seccomp=unconfined"
	@echo "to run debugger (ptrace/address randomization disable, or for debug builds that use sanitizers)"
	@echo
	@echo "And first thing when you run docker image, cat /Getting-Started-With-Stroika.md"


build-images:
ifeq (${DETECTED_HOST_OS},Linux)
	docker build -f Ubuntu2004-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-small .
	docker build -f Ubuntu2204-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-small .
	docker build -f Ubuntu1804-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests .
	docker build -f Ubuntu2004-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests .
	docker build -f Ubuntu2204-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2004 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2204 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	# to build this you may need  --storage-opts "size 100GB", but that currently doesn't work with build, so set
	# in docker windows configuration settings (can get from docker gui - settings/Docker Engine)
	# Build with -m 4GB cuz default of 1GB pages alot
	# Due to https://stroika.atlassian.net/browse/STK-742 - add   --network "Default Switch" 
	#docker build -m 4GB --network "Default Switch" -f Windows-Cygwin-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 .
	# as of 2021-09-20 - on new laptop  fails to build with -m, but works without (crazy buggy docker)
	docker build --network "Default Switch" -f Windows-Cygwin-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 .
	docker build --network "Default Switch" -f Windows-Cygwin-VS2k22/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22 .
	echo "BEWARE DOCKER BUG: https://stroika.atlassian.net/browse/STK-766 - see that for instructions on workaorund for hang building MSYS containers"
	docker build -f Windows-MSYS-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19 .
	docker build -f Windows-MSYS-VS2k22/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22 .
endif


stroika-dev-containers:
ifeq (${DETECTED_HOST_OS},Linux)
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10022:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-1804 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-1804 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10122:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2004 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2004 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10222:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2204 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2204 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10422:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
endif


tag-images:
ifeq (${DETECTED_HOST_OS},Linux)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2004-small sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2204-small sophistsolutionsinc/stroika-buildvm-ubuntu2204-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests:$(STROIKA_VERSION)
	#docker tag sophistsolutionsinc/stroika-dev sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	docker tag sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22 sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19 sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22 sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22:$(STROIKA_VERSION)
endif


pull-base-images:
ifeq (${DETECTED_HOST_OS},Linux)
	docker pull ubuntu:20.04
	docker pull ubuntu:22.04
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	docker pull mcr.microsoft.com/windows/servercore:ltsc2022
endif


push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
ifeq (${DETECTED_HOST_OS},Linux)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2204-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests:latest
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev:latest
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:latest
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22:latest
	docker push sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19:latest
	docker push sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22:latest
endif
ifeq ($(STROIKA_VERSION_Final),true)
ifeq (${DETECTED_HOST_OS},Linux)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2204-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests:$(STROIKA_VERSION)
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22:$(STROIKA_VERSION)
endif
endif