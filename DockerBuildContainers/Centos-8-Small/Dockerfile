FROM centos:centos8

COPY Shared-Files/Getting-Started-With-Stroika.md ./

RUN dnf update -y
RUN yum install -y glibc-locale-source.x86_64 && localedef -v -c -i en_US -f UTF-8 en_US.UTF-8 || echo "ignored partial failure"
RUN dnf install -y epel-release		# needed to be able to install p7zip in next step
RUN dnf groupinstall -y "Development Tools"


RUN dnf install -y git which wget  p7zip
#RUN dnf install -y git make which wget patch automake libtool p7zip unzip
RUN dnf install -y cmake3 && ln -s /usr/bin/cmake3 /usr/local/bin/cmake
RUN dnf install -y libubsan.x86_64
