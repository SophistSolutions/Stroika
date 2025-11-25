# Usually specify this from commandline
ARG BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22

FROM ${BASE_IMAGE}

# Sometimes we get sporadic failures building, and I THINK this is due to running out of disk space in builder, so
# adding this to the docker engine config may help:
# "storage-opts": [
# "size=300GB"
# ]
# https://unrealcontainers.com/docs/environments/local-windows-10

# install choco (from https://chocolatey.org/install#completely-offline-install)
SHELL ["powershell"]


RUN \
    Set-ExecutionPolicy Bypass -Scope Process -Force; \
    $ErrorActionPreference = 'Stop'; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))



# The default version of CHOCO CYGWIN fails to install as of 2024-03-08
# Adding specific version to install worked around the problem (no longer issue as of 2024-05-23, but keep around note for a bit)
# ARG CHOCO_VERSION_ARG="--version 3.5.1"
ARG CHOCO_VERSION_ARG=""



# ENV \
#     CYGWIN=winsymlinks:native


### revert these hacks = once debugged
# install cygwin
RUN echo "GOT THIS FAR1"

# Add this before the choco install step
RUN powershell -Command "Start-Process powershell -Verb runAs -ArgumentList 'icacls \"C:\ProgramData\chocolatey\" /grant \\\"Everyone:(OI)(CI)F\\\" /T' -Wait"
RUN echo "GOT THIS FAR1.1"

RUN powershell try { choco install -y --confirm cygwin } catch { type C:\ProgramData\chocolatey\logs\chocolatey.log }

RUN echo "GOT THIS FAR1.5"

RUN \
    try { \
        choco install -y --confirm --execution-timeout=2700 cygwin \
     }\
     catch {\
         type C:\ProgramData\chocolatey\logs\chocolatey.log\
     }
RUN echo "GOT THIS FAR2"
RUN choco install -y cyg-get
RUN echo "GOT THIS FAR3"


# Add cygwin to the path
RUN  \
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin'; \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

# Set HOME so we know where to copy bash_profile (seems to default to "" before we've actually run cygwin)
RUN  \
    [Environment]::SetEnvironmentVariable('HOME', 'c:\tools\cygwin\home', [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
# doxygen for generating docs - optional
# gdb for debugging - optional
# xxd for 'resource' strategy (currently just in Tests - as of 2023-12-16)
RUN cyg-get default make git wget patch pkg-config automake dos2unix unzip p7zip doxygen jq xxd python3

COPY Windows-Cygwin/Getting-Started-With-Stroika.md ./tools/cygwin/home
COPY Windows-Cygwin/.bash_profile ./tools/cygwin/home

ARG INSTALL_QUASAR=0
SHELL ["cmd", "/S", "/C"]
RUN IF %INSTALL_QUASAR%==1 ( choco install -y --force nodejs-lts )
RUN IF %INSTALL_QUASAR%==1 ( npm i -g npm @quasar/cli )

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["bash.exe", "--login"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
