FROM sophistsolutionsinc/stroika-buildvm-ubuntu1810-small

# Get latest packages system, so can do installs
RUN apt-get update

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7  g++-8

#install the latest (C++17 compat) clang compilers (and libs)
#nb: do clang6/7 on separate lines due to python conflict
RUN apt-get install -y clang++-6
RUN apt-get install -y clang++-7 libc++-7-dev libc++abi-dev libc++-dev

# for testing configuration 'only-zlib-system-third-party-component'
RUN  apt-get install zlib1g-dev

#Only needed to run with valgrind regtests
RUN apt-get install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get update && apt-get install -y g++-7-arm-linux-gnueabihf g++-8-arm-linux-gnueabihf iputils-ping
