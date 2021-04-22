
###TMPHACK
### --allow-unauthenticated --allow-insecure-repositories 


FROM sophistsolutionsinc/stroika-buildvm-ubuntu2104-small

# Get latest packages system, so can do installs
RUN apt-get update --allow-insecure-repositories 

# Just to echo information in regression test output
RUN apt-get install -y lsb-release --allow-unauthenticated

#install the latest (C++17 compat) g++ compilers
RUN apt-get install -y g++-7 g++-8 g++-9 g++-10 g++-11 --allow-unauthenticated

#install the latest (C++17 compat) clang compilers (and libs)
### NOTE - for some reason, installing libc++-7-dev AND libc++-8-dev appears to work poorly - only one can be installed at
### a time - or so it appears --LGP 2019-04-19
RUN apt-get install -y clang++-7 --allow-unauthenticated
RUN apt-get install -y clang++-8 --allow-unauthenticated
RUN apt-get install -y clang++-9 --allow-unauthenticated
RUN apt-get install -y clang++-10 --allow-unauthenticated
RUN apt-get install -y clang++-11 --allow-unauthenticated
RUN apt-get install -y clang++-12 --allow-unauthenticated
# For reasons which elude me, installing libc++-N removes all the other
# libc++ versions! There can be only one, so just test the most recent
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
RUN apt-get install -y libc++-12-dev libc++abi-12-dev --allow-unauthenticated

# for testing configuration 'only-zlib-system-third-party-component'
### @todo retest if this still needed
#RUN  apt-get install zlib1g-dev --allow-unauthenticated

#Only needed to run with valgrind regtests
#RUN apt-get install -y valgrind --allow-unauthenticated

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get install -y g++-8-arm-linux-gnueabihf g++-9-arm-linux-gnueabihf g++-10-arm-linux-gnueabihf iputils-ping --allow-unauthenticated
