FROM sophistsolutionsinc/stroika-buildvm-ubuntu1910-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7 g++-8 g++-9

#install the latest (C++17 compat) clang compilers (and libs)
### NOTE - for some reason, installing libc++-7-dev AND libc++-8-dev appears to work poorly - only one can be installed at
### a time - or so it appears --LGP 2019-04-19
#RUN apt-get install -y clang++-7 libc++-7-dev libc++abi-7-dev
RUN apt-get install -y clang++-7 libc++abi-7-dev
RUN apt-get install -y clang++-8 libc++-8-dev libc++abi-8-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-8-arm-linux-gnueabihf g++-9-arm-linux-gnueabihf iputils-ping
