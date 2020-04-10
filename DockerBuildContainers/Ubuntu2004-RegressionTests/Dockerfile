FROM sophistsolutionsinc/stroika-buildvm-ubuntu2004-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7 g++-8 g++-9 g++-10

#install the latest (C++17 compat) clang compilers (and libs)
RUN apt-get install -y clang++-7 libc++abi-7-dev libc++-7-dev
RUN apt-get install -y clang++-8 libc++-8-dev libc++abi-8-dev
RUN apt-get install -y clang++-9 libc++-9-dev libc++abi-9-dev
RUN apt-get install -y clang++-10 libc++-10-dev libc++abi-10-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf iputils-ping
