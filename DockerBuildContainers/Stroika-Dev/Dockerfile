ARG BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests

FROM $BASED_ON_IMAGE

# Get latest packages system, so can do installs
RUN apt-get update

# Cuz often connect to it via ssh
RUN apt-get install -y openssh-server

# handy dev tools to debug, etc...
RUN apt-get install -y clang-format curl vim apt-file nodejs iproute2 lldb

EXPOSE 22
## @todo have not figured out how to get sshd to start automatically
## so for now, start docker container and manually run sudo service ssh restart and then you can connect
#CMD /usr/sbin/sshd && bash
CMD bash

