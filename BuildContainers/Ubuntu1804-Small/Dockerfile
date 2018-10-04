FROM ubuntu:18.04

# install basic compilers and tools needed to build and run basic tests ('make all run-tests')
RUN apt-get update && apt-get install -y \
	g++ \
	libasan5 \
	libubsan1 \
	p7zip-full \
	git \
	pkg-config \
	automake \
	wget \
	unzip \
	cmake \
	libtool-bin

#locale support not strictly required, but some regression tests depend on it
RUN apt-get install -y locales && locale-gen en_US en_US.UTF-8

# Check out a copy of the sources, so you are ready to start building
RUN mkdir Sandbox && cd Sandbox && git clone https://github.com/SophistSolutions/Stroika.git Stroika-Dev && cd Stroika-Dev && git checkout V2.1-Release

CMD /bin/bash
