FROM sophistsolutionsinc/stroika-buildvm-ubuntu1810-small

# Get latest packages system, so can do installs
RUN apt-get update

# Just to echo information in regression test output
RUN apt-get install -y lsb-release

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7  g++-8

#install the latest (C++17 compat) clang compilers (and libs)
#nb: do clang6/7 on separate lines due to python conflict
### INSTALLING THEM THIS WAY CAUSES part of clang6 to get uninstalled so building clang++-6-debug-libc++ fails.
### NOT IMPORTANT - SO JUST LEAVE OUT CLANG6 for now from this build -- LGP 2018-11-16
#RUN apt-get install -y clang++-6 libc++-dev libc++abi-dev
RUN apt-get install -y clang
RUN apt-get install -y clang++-7 libc++-7-dev libc++abi-7-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-7-arm-linux-gnueabihf g++-8-arm-linux-gnueabihf iputils-ping
