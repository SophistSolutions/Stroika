FROM sophistsolutionsinc/stroika-buildvm-ubuntu2204-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-9 g++-10 g++-11 g++-12

# For reasons which elude me, installing libc++-N removes all the other
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
#
#install the latest (C++17 compat) clang compilers (and libs)
#and only install libc++-14-dev since others don't appear to work on this release of ubuntu (at least not instlaling mulitple versions)
RUN apt-get install -y clang++-10
RUN apt-get install -y clang++-11
RUN apt-get install -y clang++-12
RUN apt-get install -y clang++-13
RUN apt-get install -y clang++-14 && apt-get install -y libc++-14-dev libc++abi-14-dev

# For some reason not installed on ubuntu 21.04, nor 22.04 otherwise
RUN apt-get install -y libz.a

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf g++-11-arm-linux-gnueabihf g++-12-arm-linux-gnueabihf iputils-ping
