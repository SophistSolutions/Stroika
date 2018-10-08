FROM sophistsolutionsinc/stroika-buildvm-ubuntu1804-small

RUN apt-get update && apt-get install -y \
	g++-7 \
	g++-8 \
	clang++-6 \
	libc++-dev 

#Only needed to run with valgrind
RUN apt-get install -y valgrind

#needed to run older clang/g++7?
#RUN apt-get install -y libasan4 libubsan0

# libubsan0 libubsan1
