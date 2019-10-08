FROM centos:centos8

COPY Shared-Files/Getting-Started-With-Stroika.md ./

RUN dnf update -y
RUN yum install -y glibc-locale-source.x86_64 && localedef -v -c -i en_US -f UTF-8 en_US.UTF-8 || echo "ignored partial failure"
RUN dnf install -y epel-release		# for p7zip
RUN dnf install -y git make which wget patch automake libtool p7zip unzip diffutils.i686
RUN dnf install -y cmake3 && ln -s /usr/bin/cmake3 /usr/local/bin/cmake
RUN dnf groupinstall -y "Development Tools"
RUN dnf install -y libubsan.x86_64
#RUN yum install -y centos-release-scl
#RUN yum install -y devtoolset-8-gcc* devtoolset-8-libasan-devel devtoolset-8-libubsan-devel 
#RUN yum install -y gcc-c++.x86_64
