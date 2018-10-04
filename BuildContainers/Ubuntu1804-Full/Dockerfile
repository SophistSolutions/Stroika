FROM ubuntu:18.04
RUN apt-get update && apt-get install -y \
	g++ \
	g++-7 \
	g++-8 \
	clang++-6 \
	libasan4 \
	libasan5 \
	libc++-dev \
	libubsan0 \
	libubsan1 \
	p7zip-full \
	git \
	pkg-config \
	automake \
	wget \
	unzip \
	cmake \
	libtool-bin

RUN mkdir Sandbox && cd Sandbox && git clone https://github.com/SophistSolutions/Stroika.git Stroika-Dev && cd Stroika-Dev && git checkout V2.1-Release
