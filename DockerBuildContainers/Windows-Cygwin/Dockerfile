# See https://hub.docker.com/_/microsoft-windows
#   for list of available windows server types.
#
#   It appears empirically, that nanoserver doesn't work with the visual studio installer.
#   servercore is the next smallest, and that appears to work fine.
#
# SEE 
#   https://hub.docker.com/_/microsoft-windows-servercore
#   https://hub.docker.com/_/microsoft-dotnet-framework-sdk/
#        for list of versions
#
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:2004
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:20H2
# ARG BASE_IMAGE=mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019  ## Referenced in https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019                          ## ltsc2019 not working on github actions - LGP 2022-02-23

## Seems to work now on github actions using runner=Windows-2022 --LGP 2022-02-25
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022

FROM ${BASE_IMAGE}

# Sometimes we get sporadic failures building, and I THINK this is due to running out of disk space in builder, so
# adding this to the docker engine config may help:
# "storage-opts": [
# "size=300GB"
# ]
# https://unrealcontainers.com/docs/environments/local-windows-10

# install choco (from https://chocolatey.org/install#completely-offline-install)
SHELL ["powershell"]

## 1.1.1.1
# MAYBE WORKAROUND NETWORKING PROBLEM
# RUN \
#     $n=Get-NetAdapter \
#     Set-DnsClientServerAddress -InterfaceAlias $n[0].Name -ServerAddresses("8.8.8.8","8.8.4.4")

RUN Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# The default version of CHOCO CYGWIN fails to install as of 2024-03-08
# Adding specific version to install worked around the problem
# ARG CHOCO_VERSION_ARG="--version 3.5.1"
ARG CHOCO_VERSION_ARG=""
#ARG CHOCO_VERSION_ARG = --version 3.5.1


# install cygwin
SHELL ["powershell"]
RUN choco install -y cygwin cyg-get
# RUN choco install -y cyg-get

# # Add cygwin to the path
# SHELL ["powershell"]
# RUN  \
#     $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin';   \
#     [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);
# RUN  \
#     [Environment]::SetEnvironmentVariable('HOME', 'c:\tools\cygwin\home', [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
# doxygen for generating docs - optional
# gdb for debugging - optional
# xxd for 'resource' strategy (currently just in Tests - as of 2023-12-16)
RUN cyg-get default make git wget patch pkg-config automake dos2unix unzip p7zip doxygen jq xxd

COPY Windows-Cygwin/Getting-Started-With-Stroika.md ./tools/cygwin/home
COPY Windows-Cygwin/.bash_profile ./tools/cygwin/home

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
# CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]
CMD ["bash.exe", "--login"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
