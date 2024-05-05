FROM sophistsolutionsinc/stroika-buildvm-ubuntu2404-small

# Get latest packages system, so can do installs
RUN apt-get update

#locale support not strictly required, but some regression tests depend on it
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat) g++ compilers
RUN apt-get -qq install -y g++-12 g++-13 g++-14

# For reasons which elude me, installing libc++-N removes all the other
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
#
#install the latest (C++20 compat) clang compilers (and libs)
#and only install libc++-14-dev since others don't appear to work on this release of ubuntu (at least not instlaling mulitple versions)
RUN apt-get update  # unclear why update needed but container build fails without
RUN apt-get -qq install -y clang++-15
RUN apt-get -qq install -y clang++-16
RUN apt-get -qq install -y clang++-17
RUN apt-get -qq install -y clang++-18 && apt-get install -y libc++-18-dev libc++abi-18-dev

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get -qq install -y g++-13-arm-linux-gnueabihf iputils-ping
