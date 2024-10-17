
# Build this container with 
#   docker build -t test --target deploy ../../../ -f Dockerfile

FROM sophistsolutionsinc/stroika-buildvm-ubuntu2404-small as builder
ARG CONFIGURATION=Release
ARG JFLAG=-j8

WORKDIR /tmp/build
COPY . .
RUN apt-get -qq install -y libz.a

RUN make distclean && \
    make CONFIGURATION=${CONFIGURATION} all ${JFLAG}


    ### Actual running container
FROM ubuntu:24.04 as deploy

WORKDIR /opt/Stroika-Sample-HTMLUI

RUN apt-get update -y && apt-get install -y libbrotli1

COPY --from=builder /tmp/build/Builds/${CONFIGURATION}/Samples-HTMLUI/HTMLUI .
COPY --from=builder /tmp/build/Builds/${CONFIGURATION}/Samples-HTMLUI/html/ html

EXPOSE 80

ENTRYPOINT [ "/opt/Stroika-Sample-HTMLUI/HTMLUI", "--run-directly" ]
