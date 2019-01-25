DEFAULT_GOAL := help


help:
	@echo "make docker-images to create the docker images on your computer"
	@echo
	@echo "to run the image, use:"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu1804-small   OR"
	@echo "    docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests   ETC"
	@echo
	@echo "n.b. use --security-opt seccomp=unconfined to run debugger (ptrace/address randomization disable)"
	@echo "and first thing when you run docker image, cat ReadMe.md"


docker-images:
	docker build -f Ubuntu1804-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-small .
	docker build -f Ubuntu1804-RegressionTests/Dockerfile -t sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests .
	docker build -f Centos-7-Small/Dockerfile -t sophistsolutionsinc/stroika-buildvm-centos-7-small .
	docker build -f Stroika-Dev/Dockerfile -t sophistsolutionsinc/stroika-dev .


push-images:
	#if you get errors doing this (permission denied) make sure you ran docker login (as sophists - else no permission to push these)
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1810-small
	docker push sophistsolutionsinc/stroika-buildvm-ubuntu1804-regression-tests
	docker push sophistsolutionsinc/sophistsolutionsinc/stroika-buildvm-centos-7-small
	#no need to push this yet/now
	#docker push sophistsolutionsinc/stroika-dev
