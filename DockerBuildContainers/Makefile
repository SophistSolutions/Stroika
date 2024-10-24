DEFAULT_GOAL := help

STROIKA_VERSION=$(shell dos2unix < ../STROIKA_VERSION | xargs echo -n)
STROIKA_VERSION_Final=$(shell ../ScriptsLib/ExtractVersionInformation ../STROIKA_VERSION FinalBuild)

DETECTED_HOST_OS=$(shell ../ScriptsLib/DetectedHostOS)

BUILD_DEV_IMAGES?=1

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



# Was originally mostly for https://stroika.atlassian.net/browse/STK-742, but better workaround in ReadMe.md file.
EXTRA_DOCKER_BUILD_ARGS=
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
#
# to build this you may need  --storage-opts "size 100GB", but that currently doesn't work with build, so set
# in docker windows configuration settings (can get from docker gui - settings/Docker Engine)
# Build with -m 4GB cuz default of 1GB pages alot
#EXTRA_DOCKER_BUILD_ARGS= --storage-opts "size 100GB" 
#
endif


## NOTE: Since these can be slow/big, the IMAGES arg can be given on the commandline to override this
ifeq (${DETECTED_HOST_OS},Linux)
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2204-small
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2310-small
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2404-small
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2310-regression-tests
IMAGES+= sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests
ifeq (${BUILD_DEV_IMAGES},1)
IMAGES+= sophistsolutionsinc/stroika-dev
IMAGES+= sophistsolutionsinc/stroika-dev-2204
IMAGES+= sophistsolutionsinc/stroika-dev-2310
IMAGES+= sophistsolutionsinc/stroika-dev-2404
endif
endif

ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
IMAGES+= sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
IMAGES+= sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
endif

build-images:
	${MAKE} --no-print-directory ${IMAGES}


## Detailed recipies for each image type
sophistsolutionsinc/stroika-buildvm-ubuntu2204-small:
	docker build -f Ubuntu2204-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-small .

sophistsolutionsinc/stroika-buildvm-ubuntu2310-small:
	docker build -f Ubuntu2310-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2310-small .

sophistsolutionsinc/stroika-buildvm-ubuntu2404-small:
	docker build -f Ubuntu2404-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2404-small .

sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests:
	@# Sadly docker build doesn't work with symbolic links
	@cp ../ScriptsLib/BuildGCC Ubuntu2204-RegressionTests/BuildGCC
	docker build -f Ubuntu2204-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .
	@rm -f Ubuntu2204-RegressionTests/BuildGCC

sophistsolutionsinc/stroika-buildvm-ubuntu2310-regression-tests:
	docker build -f Ubuntu2310-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2310-regression-tests .

sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests:
	docker build -f Ubuntu2404-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests .



sophistsolutionsinc/stroika-buildvm-windows-vs2k22:
	docker build ${EXTRA_DOCKER_BUILD_ARGS} -f Windows-VS2k22/Dockerfile --build-arg BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022 -t $@ .

sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22:		sophistsolutionsinc/stroika-buildvm-windows-vs2k22
	docker build ${EXTRA_DOCKER_BUILD_ARGS} -f Windows-Cygwin/Dockerfile --build-arg BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22 -t $@ .

sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22:		sophistsolutionsinc/stroika-buildvm-windows-vs2k22
	docker build ${EXTRA_DOCKER_BUILD_ARGS} -f Windows-MSYS/Dockerfile --build-arg BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22 -t $@ .


sophistsolutionsinc/stroika-dev:
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .

sophistsolutionsinc/stroika-dev-2204:
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2204 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests .

sophistsolutionsinc/stroika-dev-2310:
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2310 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2310-regression-tests .

sophistsolutionsinc/stroika-dev-2404:
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev-2404 --build-arg BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests .


stroika-dev-containers:
ifeq (${DETECTED_HOST_OS},Linux)
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10022:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2204 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2204 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10422:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2310 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2310 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10622:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
	cd .. && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2404 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2404 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10522:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
endif


tag-images:
	for image in ${IMAGES}; do \
		docker tag $${image} $${image}:v3; \
	done
ifeq ($(STROIKA_VERSION_Final),true)
	for image in ${IMAGES}; do  \
		docker tag $${image} $${image}:${STROIKA_VERSION}; \
	done
endif


push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
	for image in ${IMAGES}; do \
		docker push $${image}:latest; \
		docker push $${image}:v3; \
	done
ifeq ($(STROIKA_VERSION_Final),true)
	for image in ${IMAGES}; do  \
		docker push $${image}:${STROIKA_VERSION}; \
	done
endif
