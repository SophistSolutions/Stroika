FROM sophistsolutionsinc/stroika-buildvm-ubuntu2204-small

# Get latest packages system, so can do installs
RUN apt-get update

#locale support not strictly required, but some regression tests depend on it
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat) g++ compilers
RUN apt-get -qq install -y g++-11 g++-12

# For reasons which elude me, installing libc++-N removes all the other
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
#
#install the latest (C++20 compat) clang compilers (and libs)
#and only install libc++-14-dev since others don't appear to work on this release of ubuntu (at least not instlaling mulitple versions)
RUN apt-get update  # unclear why update needed but container build fails without
RUN apt-get -qq install -y clang++-14
RUN apt-get -qq install -y clang++-15 && apt-get install -y libc++-15-dev libc++abi-15-dev

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get -qq install -y g++-11-arm-linux-gnueabihf g++-12-arm-linux-gnueabihf iputils-ping

## GCC 13 or later is required to support std::format, and no obvious way to get this on Ubuntu 22.04 using apt-get etc
## (EVEN WITH ppa:ubuntu-toolchain-r/ppa) --LGP 2024-03-07
ADD ./Ubuntu2204-RegressionTests/BuildGCC .
RUN CLEANUP=1 GCC_VERSION=13.2.0 ./BuildGCC; \
    (cd /usr/local/bin ; ln -s g++ g++-13; ln -s gcc gcc-13; ln -s `which gcc-ranlib-12` gcc-ranlib-13; ln -s `which gcc-ar-12` gcc-ar-13; ); \
    rm BuildGCC