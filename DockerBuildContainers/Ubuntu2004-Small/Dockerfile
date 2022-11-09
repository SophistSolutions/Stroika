FROM ubuntu:20.04

# Get latest packages system, so can do installs
RUN apt-get update

# This hack is needed to avoid failure in apt-get install -y pkg-config for Ubuntu 20.04
#https://serverfault.com/questions/949991/how-to-install-tzdata-on-a-ubuntu-docker-image
RUN apt-get -qq install -y tzdata

# cuz you need this alot, like if something else missing; use NOPASSWD in sudoers file so this works, cuz hard
# to get sudo/enter password working under docker; sb relatively safe cuz only for sudo group members from base os
RUN apt-get -qq install -y sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

#locale support not strictly required, but some regression tests depend on it
#and as of Ubuntu 20.04 (prerelease) appears must be included for other apt-get install programs to complete
RUN apt-get -qq install -y locales && locale-gen en_US en_US.UTF-8

# Used to unpack sources for lzma SDK
# A couple ThirdPartyLibs (e.g. sqlite, zlib) deliver library source in this form (.zip) - so get to unpack them
# Used to fetch third party components to build them. Not strictly needed if you don't build any of those...
# pkg-config only needed for building third-party components, like libcurl, etc, but also used in Stroika makefiles to find appropriate
#   components
# cmake required to build Xerces
RUN apt-get -qq install -y p7zip-full; \
    apt-get -qq install -y unzip; \
    apt-get -qq install -y wget; \
    apt-get -qq install -y pkg-config; \
    apt-get -qq install -y cmake

#required to build libcurl
RUN apt-get -qq install -y automake autoconf libtool-bin

#Not required, but if you want to generate docs
RUN apt-get -qq install -y doxygen

#needed for ApplyConfiguration to update vs-code configuration files
RUN apt-get -qq install -y jq

#Not really a Stroika dependency, but its how we download/fetch stroika
RUN apt-get -qq install -y git

#Only needed to use sanitizer builds
RUN apt-get -qq install -y libasan5 libubsan1

# Stroika v3 requires new chrono-support, which appears first available in g++-11
# g++-11 not naturally available in Ubuntu 20.04, so do a little lifting
RUN apt-get -qq install -y software-properties-common && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update

#You need some compiler, - can be clang or g++, but pick g++-11 as a good default
RUN apt-get -qq install -y g++ g++-11

COPY Shared-Files/Getting-Started-With-Stroika.md ./

CMD /bin/bash
