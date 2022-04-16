ARG BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2204-regression-tests

FROM $BASED_ON_IMAGE

# Get latest packages system, so can do installs
RUN apt-get update

RUN apt-get install -y openssh-server

RUN apt-get install -y clang-format curl vim apt-file iproute2 npm lldb

EXPOSE 22
## @todo have not figured out how to get sshd to start automatically
## so for now, start docker container and manually run sudo service ssh restart and then you can connect
#CMD /usr/sbin/sshd && bash
CMD bash

