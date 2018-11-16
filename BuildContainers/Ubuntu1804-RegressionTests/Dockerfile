FROM sophistsolutionsinc/stroika-buildvm-ubuntu1804-small

# Get latest packages system, so can do installs
RUN apt-get update


# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y  g++-7  g++-8

#install the latest (C++17 compat) clang compilers (and libs)
RUN apt-get install -y  clang++-6  libc++abi-dev libc++-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get update && apt-get install -y g++-7-arm-linux-gnueabihf g++-8-arm-linux-gnueabihf iputils-ping

#needed to run older clang/g++7?
#RUN apt-get install -y libasan4 libubsan0

# libubsan0 libubsan1
