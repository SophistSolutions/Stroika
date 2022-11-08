FROM sophistsolutionsinc/stroika-buildvm-ubuntu2004-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat) g++ compilers
RUN apt-get -qq install -y g++-11

#install the latest (C++20 compat) clang compilers (and libs)
# must include clang++-VERSION on different lines due to issue with some python dependency
# UNCLEAR why some interdependency - cannot install different versions of libc++-10-dev at the same time??
# See https://stackoverflow.com/questions/61165575/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
RUN apt-get -qq install -y clang++-10
RUN apt-get -qq install -y libc++-10-dev libc++abi-10-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN apt-get -qq install -y zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
# (note 9/10 for use in building Stroika v2.1, not v3)
# (g++-11-arm-linux-gnueabihf doesnt appear to exist in Ubuntu 20.04, at least not with repositories I've tried so far --LGP 2022-11-08)
RUN apt-get -qq install -y g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf iputils-ping
