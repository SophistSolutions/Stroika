FROM sophistsolutionsinc/stroika-buildvm-ubuntu2010-regression-tests

# Get latest packages system, so can do installs
RUN apt-get update

RUN apt-get install -y clang-format curl vim apt-file iproute2 npm
RUN apt-get install -y openssh-server && systemctl enable ssh
