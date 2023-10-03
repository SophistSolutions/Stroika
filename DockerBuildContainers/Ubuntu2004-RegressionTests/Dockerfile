FROM sophistsolutionsinc/stroika-buildvm-ubuntu2004-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat g++11 or later)
RUN apt-get -qq install -y g++-11

# for testing configuration 'only-zlib-system-third-party-component'
RUN apt-get -qq install -y zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
# (g++-11-arm-linux-gnueabihf doesnt appear to exist in Ubuntu 20.04, at least not with repositories I've tried so far --LGP 2022-11-08)
RUN apt-get -qq install -y iputils-ping
