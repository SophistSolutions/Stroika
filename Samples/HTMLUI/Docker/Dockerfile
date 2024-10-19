
# Build this container with 
#   docker build -t htmlui-test --target deploy ../../../ -f Dockerfile
# Run produced container with
#   docker run --rm -it -p 8080:80 --name htmlui test


# Define Builder and do actual compiling
FROM sophistsolutionsinc/stroika-buildvm-ubuntu2404-small as builder
ARG CONFIGURATION=Release
ARG JFLAG=-j8

# Build someplace we can copy-out from, and copy in to this container all the contents of the current directory
# (see .dockerignore file in that directory for stuff omitted - mostly for speed reasons; note we make distclean in case stuff accidentally left in)
WORKDIR /tmp/build
COPY . .

RUN apt-get install -y nodejs; apt-get install -y  npm; npm i -g npm @quasar/cli

RUN make CONFIGURATION= distclean && \
    make CONFIGURATION=${CONFIGURATION} libraries ${JFLAG} && \
    make CONFIGURATION=${CONFIGURATION} --directory=Samples/HTMLUI/ --no-print-directory all ${JFLAG}


### Actual running container
FROM ubuntu:24.04 as deploy

ARG CONFIGURATION=Release

WORKDIR /opt/Stroika-Sample-HTMLUI

# For configuration/log data etc...
RUN mkdir /var/opt/Stroika-Sample-HTMLUI

# CONFIGURATION
#   @todo - allow config info to be sepecified some other way - at docker startup time? Or better document how todo (probably container BUILD_ARG)
RUN /bin/echo -e "{\n\
    \"ShowAsExternalURL\" : null,\n\
    \"Logging\" : {\"ToStdOut\" : true, \"ToSysLog\" : false},\n\
    \"WebServerPort\" : null\n\
}\n" > /var/opt/Stroika-Sample-HTMLUI/Server.json

COPY --from=builder /tmp/build/Builds/${CONFIGURATION}/Samples-HTMLUI/Backend/HTMLUI .
COPY --from=builder /tmp/build/Builds/${CONFIGURATION}/Samples-HTMLUI/html/ html

EXPOSE 80

ENTRYPOINT [ "/opt/Stroika-Sample-HTMLUI/HTMLUI", "--run-directly" ]
