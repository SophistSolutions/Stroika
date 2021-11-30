FROM sophistsolutionsinc/stroika-buildvm-ubuntu2110-small

# Get latest packages system, so can do installs
RUN apt-get update

# Needed to install lsb-release since Ubuntu 21.04
ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-8 g++-9 g++-10 g++-11

# For some reason not installed on ubuntu 21.04 otherwise
RUN apt-get install -y libz.a

#install the latest (C++17 compat) clang compilers (and libs)
# Note - start at clang++10 because 9 has some issue with std::filesystem, not worth investigating, since covered on earlier ubuntu containers
RUN apt-get install -y clang++-10
RUN apt-get install -y clang++-11
RUN apt-get install -y clang++-12

# note libunwind-13-dev dependend on by clang++13, it appears
RUN apt-get install -y clang++-13 libunwind-13-dev
# For reasons which elude me, installing libc++-N removes all the other
# libc++ versions! There can be only one, so just test the most recent
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
RUN apt-get install -y libc++-13-dev libc++abi-13-dev

# for testing configuration 'only-zlib-system-third-party-component'
# retest - seems to fail installing (--LGP 2021-04-23)
#RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf g++-11-arm-linux-gnueabihf iputils-ping
