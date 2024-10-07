FROM sophistsolutionsinc/stroika-buildvm-ubuntu2204-small

# Get latest packages system, so can do installs
RUN apt-get update

#locale support not strictly required, but some regression tests depend on it
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#install the latest (C++20 compat) g++ compilers
RUN apt-get -qq install -y g++-11 g++-12

RUN apt-get install -y software-properties-common; \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test; \
    apt-get update; \
    apt-get -qq install -y g++-13


# For reasons which elude me, installing libc++-N removes all the other
# https://askubuntu.com/questions/1227092/how-can-i-install-multiple-versions-of-llvm-libc-on-the-same-computer-at-the-s
#
#install the latest (C++20 compat) clang compilers (and libs)
#and only install libc++-14-dev since others don't appear to work on this release of ubuntu (at least not instlaling mulitple versions)
RUN apt-get update  # unclear why update needed but container build fails without
RUN apt-get -qq install -y clang++-15 && apt-get install -y libc++-15-dev libc++abi-15-dev

# # Used for Samples/HTMLUI (not needed - autodetected)
#
# Because Ubuntu2004 node version not working with vite (requires 14.x or later, but that has version of npm that didn't seem to work with builds)
# Instructions from https://github.com/nodesource/distributions#readme
RUN \
    apt-get install -y ca-certificates curl gnupg; \
    mkdir -p /etc/apt/keyrings; \
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg

ARG NODE_MAJOR=20
RUN echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_${NODE_MAJOR}.x nodistro main" | sudo tee /etc/apt/sources.list.d/nodesource.list
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y nodejs npm ; npm i -g npm @quasar/cli

#Only needed to run with valgrind regtests
RUN apt-get -qq install -y valgrind

# Cross-compile tests for ARM (iputils-ping is how we detect if machine exists to run regtests on)
RUN apt-get -qq install -y g++-11-arm-linux-gnueabihf g++-12-arm-linux-gnueabihf iputils-ping

## GCC 13 or later is required to support std::format, and no obvious way to get this on Ubuntu 22.04 using apt-get etc
## (EVEN WITH ppa:ubuntu-toolchain-r/ppa) --LGP 2024-03-07
## Still not ACTUALLY NEEDED, HOWEVER, since Stroika will pull in fmtlib instead if needed (for older gcc) --LGP 2024-07-08 
# ADD ./Ubuntu2204-RegressionTests/BuildGCC .
# RUN CLEANUP=1 GCC_VERSION=13.3.0 ./BuildGCC; \
#     (cd /usr/local/bin ; ln -s g++ g++-13; ln -s gcc gcc-13; ln -s `which gcc-ranlib-12` gcc-ranlib-13; ln -s `which gcc-ar-12` gcc-ar-13; ); \
#     rm BuildGCC