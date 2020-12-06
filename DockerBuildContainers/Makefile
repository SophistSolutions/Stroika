DEFAULT_GOAL := help

STROIKA_VERSION=$(shell dos2unix < ../STROIKA_VERSION | xargs echo -n)
STROIKA_VERSION_Final=$(shell ../ScriptsLib/ExtractVersionInformation ../STROIKA_VERSION FinalBuild)


help:
	@echo "make docker-images to create the docker images on your computer"
	@echo
	@echo "to run the image, use:"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu1804-small   OR"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests   ETC"
	@echo
	@echo "n.b. you may need to use (add to run command)"
	@echo "    --security-opt seccomp=unconfined"
	@echo "to run debugger (ptrace/address randomization disable, or for debug builds that use sanitizers)"
	@echo
	@echo "And first thing when you run docker image, cat /Getting-Started-With-Stroika.md"


build-images:
ifeq ($(shell uname),Linux)
	docker build -f Ubuntu1804-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-small .
	docker build -f Ubuntu1804-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests .
	docker build -f Ubuntu2004-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-small .
	docker build -f Ubuntu2004-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests .
	docker build -f Ubuntu2010-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2010-small .
	docker build -f Ubuntu2010-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests .
	docker build -f Centos-7-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-centos-7-small .
	docker build -f Centos-8-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-centos-8-small .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	# to build this you may need  --storage-opts "size 100GB", but that currently doesn't work with build, so set
	# in docker windows configuration settings (can get from docker gui - settings/Docker Engine)
	# Build with -m 4GB cuz default of 1GB pages alot
	docker build -m 4GB -f Windows-Cygwin-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 .
	#docker build -f Windows-MinGW-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19 .
endif


tag-images:
ifeq ($(shell uname),Linux)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu1804-small sophistsolutionsinc/stroika-buildvm-ubuntu1804-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests  sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2004-small sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2010-small sophistsolutionsinc/stroika-buildvm-ubuntu2010-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-centos-7-small sophistsolutionsinc/stroika-buildvm-centos-7-small:$(STROIKA_VERSION)
	docker tag sophistsolutionsinc/stroika-buildvm-centos-8-small sophistsolutionsinc/stroika-buildvm-centos-8-small:$(STROIKA_VERSION)
	#docker tag sophistsolutionsinc/stroika-dev sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker tag sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:latest sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:$(STROIKA_VERSION)
	#docker tag sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19 $(STROIKA_VERSION)
endif


pull-base-images:
ifeq ($(shell uname),Linux)
	docker pull ubuntu:18.04
	docker pull ubuntu:20.04
	docker pull ubuntu:20.10
	docker pull centos:centos7
	docker pull centos:centos8
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker pull mcr.microsoft.com/windows/servercore:ltsc2019
endif


push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
ifeq ($(shell uname),Linux)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2010-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:latest
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests:latest
	docker push sophistsolutionsinc/stroika-buildvm-centos-7-small:latest
	docker push sophistsolutionsinc/stroika-buildvm-centos-8-small:latest
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev:latest
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:latest
	#docker push sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19:latest
endif
ifeq ($(STROIKA_VERSION_Final),true)
ifeq ($(shell uname),Linux)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2010-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2004-regression-tests:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-centos-7-small:$(STROIKA_VERSION)
	docker push sophistsolutionsinc/stroika-buildvm-centos-8-small:$(STROIKA_VERSION)
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev:$(STROIKA_VERSION)
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19:$(STROIKA_VERSION)
	#docker push sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19:$(STROIKA_VERSION)
endif
endif