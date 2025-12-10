FROM sophistsolutionsinc/stroika-buildvm-ubuntu2404-small

# Get latest packages system, so can do installs
RUN apt-get update

#locale support not strictly required, but some regression tests depend on it
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat) g++ compilers
RUN apt-get -qq install -y g++-12 g++-13 g++-14; \
    apt-get -qq clean -y;

#Just so we test this in installers (some samples build rpm installers)
RUN apt-get -qq install -y rpm; \
    apt-get -qq clean -y;

# For reasons which elude me, installing libc++-N removes all the other
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
#
#install the latest (C++20 compat) clang compilers (and libs)
#and only install libc++-14-dev since others don't appear to work on this release of ubuntu (at least not instlaling mulitple versions)
RUN apt-get update  # unclear why update needed but container build fails without
RUN apt-get -qq install -y clang++-15; \
    apt-get -qq clean -y;
RUN apt-get -qq install -y clang++-16; \
    apt-get -qq clean -y;
RUN apt-get -qq install -y clang++-17; \
    apt-get -qq clean -y;
RUN apt-get -qq install -y clang++-18 && apt-get install -y libc++-18-dev libc++abi-18-dev; \
    apt-get -qq clean -y;

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind; \
    apt-get -qq clean -y;

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get -qq install -y g++-13-arm-linux-gnueabihf g++-14-arm-linux-gnueabihf iputils-ping; \
    apt-get -qq clean -y;

## Because whatever comes with 24.04 stopped working with quasar 2024-12-18
ARG NODE_MAJOR=20
RUN apt-get install -y curl; \
    apt-get -qq clean -y;
RUN curl -fsSL https://deb.nodesource.com/setup_${NODE_MAJOR}.x | sudo -E bash -

# Used for Samples/HTMLUI (not needed - autodetected - but good to test)
RUN apt-get update; apt-get install -y nodejs ;apt-get install -y  npm ; npm i -g npm @quasar/cli; \
    apt-get -qq clean -y;
