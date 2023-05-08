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


ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
# Due to https://stroika.atlassian.net/browse/STK-742 - add   --network "Default Switch" 
# to build this you may need  --storage-opts "size 100GB", but that currently doesn't work with build, so set
# in docker windows configuration settings (can get from docker gui - settings/Docker Engine)
# Build with -m 4GB cuz default of 1GB pages alot
EXTRA_DOCKER_BUILD_ARGS_=
endif
build-images:
ifeq (${DETECTED_HOST_OS},Linux)
	docker build -f Ubuntu2004-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-small .
	docker build -f Ubuntu2204-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-small .
	docker build -f Ubuntu2004-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests .
	docker build -f Ubuntu2204-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2004 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2204 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	${MAKE} --silent sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
	${MAKE} --silent sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
endif

sophistsolutionsinc/stroika-buildvm-windows-vs2k22:
	docker build ${EXTRA_DOCKER_BUILD_ARGS_} -f Windows-VS2k22/Dockerfile --build-arg BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022 -t $@ .

sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22:		sophistsolutionsinc/stroika-buildvm-windows-vs2k22
	docker build ${EXTRA_DOCKER_BUILD_ARGS_} -f Windows-Cygwin/Dockerfile --build-arg BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22 -t $@ .

sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22:		sophistsolutionsinc/stroika-buildvm-windows-vs2k22
	docker build ${EXTRA_DOCKER_BUILD_ARGS_} -f Windows-MSYS/Dockerfile --build-arg BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22 -t $@ .


stroika-dev-containers:
ifeq (${DETECTED_HOST_OS},Linux)
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10022:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2004 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2004 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10222:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2204 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2204 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10422:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
endif


export DOCKER_TAG_LINE_=\
	echo docker tag sophistsolutionsinc/$${DO_TO_IMAGE_} sophistsolutionsinc/$${DO_TO_IMAGE_}:v3; \
	docker tag sophistsolutionsinc/$${DO_TO_IMAGE_} sophistsolutionsinc/$${DO_TO_IMAGE_}:v3;

ifeq ($(STROIKA_VERSION_Final),true)
DOCKER_TAG_LINE_+=
	echo docker tag sophistsolutionsinc/$${DO_TO_IMAGE_} sophistsolutionsinc/$${DO_TO_IMAGE_}:$(STROIKA_VERSION); \
	docker tag sophistsolutionsinc/$${DO_TO_IMAGE_} sophistsolutionsinc/$${DO_TO_IMAGE_}:$(STROIKA_VERSION);
endif

tag-images:
ifeq (${DETECTED_HOST_OS},Linux)
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2004-small; ${DOCKER_TAG_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2004-regression-tests; ${DOCKER_TAG_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2204-small; ${DOCKER_TAG_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2204-regression-tests; ${DOCKER_TAG_LINE_}
	#docker tag sophistsolutionsinc/stroika-dev sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@export DO_TO_IMAGE_=stroika-buildvm-windows-cygwin-vs2k22; ${DOCKER_TAG_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-windows-msys-vs2k22; ${DOCKER_TAG_LINE_}
endif


pull-base-images:
ifeq (${DETECTED_HOST_OS},Linux)
	docker pull ubuntu:20.04
	docker pull ubuntu:22.04
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	docker pull mcr.microsoft.com/windows/servercore:ltsc2022
endif


export DOCKER_PUSH_LINE_=\
	echo docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:latest; \
	docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:latest; \
	echo docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:v3; \
	docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:v3;

ifeq ($(STROIKA_VERSION_Final),true)
DOCKER_PUSH_LINE_+=\
	echo docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:$(STROIKA_VERSION); \
	docker push sophistsolutionsinc/$${DO_TO_IMAGE_}:$(STROIKA_VERSION);
endif

push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
ifeq (${DETECTED_HOST_OS},Linux)
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2004-small; ${DOCKER_PUSH_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2004-regression-tests; ${DOCKER_PUSH_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2204-small; ${DOCKER_PUSH_LINE_}
	@export DO_TO_IMAGE_=stroika-buildvm-ubuntu2204-regression-tests; ${DOCKER_PUSH_LINE_}
	# #docker tag sophistsolutionsinc/stroika-dev sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@export DO_TO_IMAGE_=stroika-buildvm-windows-cygwin-vs2k22; ${DOCKER_PUSH_LINE_}
	#@export DO_TO_IMAGE_=stroika-buildvm-windows-msys-vs2k22; ${DOCKER_PUSH_LINE_}
endif
