FROM sophistsolutionsinc/stroika-buildvm-ubuntu2004-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7 g++-8 g++-9 g++-10

#install the latest (C++17 compat) clang compilers (and libs)
# must include clang++-VERSION on different lines due to issue with some python dependency
# UNCLEAR why some interdependency - cannot install different versions of libc++-10-dev at the same time??
# See https://stackoverflow.com/questions/61165575/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
RUN apt-get install -y clang++-7
RUN apt-get install -y clang++-8
RUN apt-get install -y clang++-9
RUN apt-get install -y clang++-10
RUN apt-get install -y libc++-10-dev libc++abi-10-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install -y zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf iputils-ping
