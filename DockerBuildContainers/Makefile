DEFAULT_GOAL := help


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


docker-images:
ifeq ($(shell uname),Linux)
	docker build -f Ubuntu1804-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-small .
	docker build -f Ubuntu1804-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests .
	docker build -f Ubuntu1810-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1810-small .
	docker build -f Ubuntu1810-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1810-regression-tests .
	docker build -f Ubuntu1904-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1904-small .
	docker build -f Ubuntu1904-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1904-regression-tests .
	docker build -f Ubuntu1910-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1910-small .
	docker build -f Ubuntu1910-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1910-regression-tests .
	docker build -f Centos-7-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-centos-7-small .
	docker build -f Centos-8-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-centos-8-small .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker build -f Windows-Cygwin-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19 -m 2G .
	#docker build -f Windows-MinGW-VS2k19/Dockerfile -t sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19 -m 2G .
endif


docker-pull-base-images:
ifeq ($(shell uname),Linux)
	docker pull ubuntu:18.04
	docker pull ubuntu:18.10
	docker pull ubuntu:19.04
	docker pull ubuntu:19.10
	docker pull centos:centos7
	docker pull centos:centos8
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker pull mcr.microsoft.com/windows/servercore:ltsc2019
endif

push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
ifeq ($(shell uname),Linux)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-small
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1810-small
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1904-small
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1910-small
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1810-regression-tests
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1904-regression-tests
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1910-regression-tests
	docker push sophistsolutionsinc/stroika-buildvm-centos-7-small
	docker push sophistsolutionsinc/stroika-buildvm-centos-8-small
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev
endif
ifeq (Windows,$(findstring Windows,$(OS)))
	docker push sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
	#docker push sophistsolutionsinc/stroika-buildvm-windows-mingw-vs2k19
endif
