FROM centos:centos8

COPY Shared-Files/Getting-Started-With-Stroika.md ./

RUN dnf update -y

# For locale support tests
RUN dnf install langpacks-en glibc-all-langpacks -y

# As of 2020-04-28, this completes, but with lots of errors that indicate its pretty buggy
RUN dnf groupinstall -y "Development Tools"

RUN dnf install -y which wget
RUN dnf install -y cmake3 && ln -s /usr/bin/cmake3 /usr/local/bin/cmake
RUN dnf install -y libubsan.x86_64

# epel-release needed to be able to install p7zip
RUN dnf install -y epel-release && dnf install -y p7zip