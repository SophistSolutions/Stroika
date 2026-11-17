FROM sophistsolutionsinc/stroika-buildvm-ubuntu2610-small

# The latest non-LTS Ubuntu: here for its newest compilers, and to check the newest Ubuntu still works - the wide compiler
# coverage is on the LTS images. Each RUN joins its steps with &&, so a missing package fails the image build.

# Get latest packages system, so can do installs
RUN apt-get update

#locale support not strictly required, but some regression tests depend on it
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Just to echo information in regression test output
RUN apt-get -qq install -y lsb-release

#the newest g++ compilers
RUN apt-get -qq install -y g++-15 g++-16 && \
    apt-get -qq clean -y

#Just so we test this in installers (some samples build rpm installers)
RUN apt-get -qq install -y rpm && \
    apt-get -qq clean -y

#the newest clang compilers, each with libclang-rt-N-dev (the sanitizer runtimes the debug configurations link) and
#llvm-N (llvm-ar-N and llvm-ranlib-N, a clang configuration's AR and RANLIB) - named, as clang-N only recommends them
#(llvm-N through llvm-N-dev), and apt skips a recommended package it cannot install without a word; libc++ for the
#newest only: installing one libc++-N-dev removes the others
RUN apt-get -qq update && \
    apt-get -qq install -y clang-21 lld-21 libclang-rt-21-dev llvm-21 && \
    apt-get -qq install -y clang-22 lld-22 libclang-rt-22-dev llvm-22 libc++-22-dev libc++abi-22-dev && \
    apt-get -qq clean -y

# Used for Samples/HTMLUI (not needed - autodetected - but good to test)
ARG NODE_MAJOR=26
RUN apt-get -qq install -y curl && \
    apt-get -qq clean -y
RUN curl -fsSL https://deb.nodesource.com/setup_${NODE_MAJOR}.x | sudo -E bash -
RUN apt-get update && apt-get install -y nodejs && npm i -g npm @quasar/cli && \
    apt-get -qq clean -y
