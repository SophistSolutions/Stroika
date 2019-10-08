FROM sophistsolutionsinc/stroika-buildvm-ubuntu1910-regression-tests

# Get latest packages system, so can do installs
RUN apt-get update

RUN apt-get install -y clang-format curl vim apt-file iproute2
RUN apt-file update
