# Could use about any version of ubuntu, but 1804 or later comes with C++17 friendly compilers
FROM ubuntu:19.10


# Get latest packages system, so can do installs
RUN apt-get update

# cuz you need this alot, like if something else missing; use NOPASSWD in sudoers file so this works, cuz hard
# to get sudo/enter password working under docker; sb relatively safe cuz only for sudo group members from base os
RUN apt-get install -y sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Used to unpack sources for lzma SDK
RUN apt-get install -y p7zip-full

# A couple ThirdPartyLibs (e.g. sqlite, zlib) deliver library source in this form (.zip) - so get to unpack them
RUN apt-get install -y unzip

# Used to fetch third party components to build them. Not strictly needed if you don't build any of those...
RUN apt-get install -y wget

#pkg-config only needed for building third-party components, like libcurl, etc, but also used in Stroika makefiles to find appropriate
# components
RUN apt-get install -y pkg-config

#You need some compiler, - can be clang or g++, but pick g++ as a good default
RUN apt-get install -y g++

#required to build Xerces
RUN apt-get install -y cmake

#required to build libcurl
RUN apt-get install -y automake autoconf libtool-bin

#Not really a Stroika dependency, but its how we download/fetch stroika
RUN apt-get install -y git

#Only needed to use sanitizer builds
RUN apt-get install -y libasan5 libubsan1

#locale support not strictly required, but some regression tests depend on it
RUN apt-get install -y locales && locale-gen en_US en_US.UTF-8

COPY Shared-Files/Getting-Started-With-Stroika.md ./

CMD /bin/bash
