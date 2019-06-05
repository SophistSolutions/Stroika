FROM centos:centos7

#RUN yum install -y libmpc-devel mpfr-devel gmp-devel make
COPY Shared-Files/Getting-Started-With-Stroika.md ./
RUN (head -3 Getting-Started-With-Stroika.md > tmp.md) && \
    (echo "scl enable devtoolset-8 bash" >> tmp.md) && \
    (tail -$(expr $(wc -l Getting-Started-With-Stroika.md | awk '{print $1;}') - 2) Getting-Started-With-Stroika.md >> tmp.md) && \
    mv tmp.md Getting-Started-With-Stroika.md

RUN yum install -y epel-release
RUN yum install -y git make which wget patch automake libtool p7zip unzip
RUN yum install -y cmake3 && ln -s /usr/bin/cmake3 /usr/local/bin/cmake
RUN yum install -y centos-release-scl
RUN yum install -y devtoolset-8-gcc*
