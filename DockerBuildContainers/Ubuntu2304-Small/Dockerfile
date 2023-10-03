FROM ubuntu:23.04

# Get latest packages system, so can do installs
RUN apt-get update

# cuz you need this alot, like if something else missing; use NOPASSWD in sudoers file so this works, cuz hard
# to get sudo/enter password working under docker; sb relatively safe cuz only for sudo group members from base os
RUN apt-get -qq install -y sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

#
# NO IDEA why needed, but as of 2022-11-29 this extra apt-get updated needed before installing 
#       build-essential
# --LGP 2022-11-29

RUN apt-get update && apt-get -qq install -y build-essential

#
# p7zip-full:                   Used to unpack sources for lzma SDK
# unzip:                        A couple ThirdPartyLibs (e.g. sqlite, zlib) deliver library source in this form (.zip) - so get to unpack them
# wget:                         Used to fetch third party components to build them. Not strictly needed if you don't build any of those...
# pkg-config:                   only needed for building third-party components, like libcurl, etc, but also used in Stroika makefiles to find
#                               appropriate components
#cmake:                         required to build Xerces
#automake autoconf libtool-bin: required to build libcurl
#doxygen:                       Not required, but if you want to generate docs
#jq:                            needed for ApplyConfiguration to update vs-code configuration files
#git:                           Not really a Stroika dependency, but its how we download/fetch stroika
#
RUN apt-get -qq install -y p7zip-full; \
    apt-get -qq install -y unzip; \
    apt-get -qq install -y wget; \
    apt-get -qq install -y pkg-config; \
    apt-get -qq install -y cmake; \
    apt-get -qq install -y automake autoconf libtool-bin; \
    apt-get -qq install -y doxygen; \
    apt-get -qq install -y jq; \
    apt-get -qq install -y git

#You need some compiler, - can be clang or g++, but pick g++ as a good default
RUN apt-get -qq install -y g++

#Only needed to use sanitizer builds
RUN apt-get -qq install -y libasan5 libasan6 libubsan1

COPY Shared-Files/Getting-Started-With-Stroika.md ./

CMD /bin/bash
