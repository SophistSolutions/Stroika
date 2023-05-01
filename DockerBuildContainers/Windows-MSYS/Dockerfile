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

# install choco
SHELL ["powershell"]
ENV CHOCO_URL=https://chocolatey.org/install.ps1
RUN Set-ExecutionPolicy Bypass -Scope Process -Force; \
 [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]'Tls,Tls11,Tls12'; \
 iex ((New-Object System.Net.WebClient).DownloadString("$env:CHOCO_URL"));


## install MSYS2
#### https://stroika.atlassian.net/browse/STK-766
#### NOTE https://github.com/microsoft/hcsshim/issues/696 - is the source of the del /f /s /q "C:\$Recycle.Bin
#### Magic - that PREVENTS the MSYS install hang!!!
ARG MSYS_LatestRelease=https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe
ARG MSYS_20220118=https://github.com/msys2/msys2-installer/releases/download/2022-01-18/msys2-base-x86_64-20220118.sfx.exe
ARG MSYS_20220904=https://github.com/msys2/msys2-installer/releases/download/2022-09-04/msys2-base-x86_64-20220904.sfx.exe
ARG MSYS_20230127=https://github.com/msys2/msys2-installer/releases/download/2023-01-27/msys2-base-x86_64-20230127.sfx.exe

SHELL ["cmd", "/S", "/C"]
RUN choco install -y msys2 && del /f /s /q "C:\$Recycle.Bin"

## ALTERNATIVE MSYS INSTALL, USED TO USE - MAYBE OK as well with DELETE RECYLYE BIG TRICK
# SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]
# RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; \
#   Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_20230127" -OutFile msys2.exe; \
#   .\msys2.exe -y -oC:\; \
#   Remove-Item msys2.exe ; && del /f /s /q "C:\$Recycle.Bin" \


# Add msys64 to the path (and mingw64/bin since that is where jq gets installed)
SHELL ["powershell"]
RUN  \
    $env:PATH =  $env:PATH + ';c:\tools\msys64\usr\bin;c:\tools\msys64\mingw64\bin';   \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
SHELL ["cmd", "/S", "/C"]
RUN \
    pacman --noconfirm -Syuu && \
    pacman --noconfirm -Syuu && \
    pacman --noconfirm -S base-devel git unzip p7zip doxygen mingw-w64-x86_64-jq && \
    pacman --noconfirm -Syuu

COPY Windows-MSYS/Getting-Started-With-Stroika.md c:/tools/msys64/
COPY Windows-MSYS/.bash_profile c:/tools/msys64/

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
# CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]
CMD ["bash.exe", "--login"]

## @todo - NOT SURE A GOOD IDEA - EXPLAIN WHY WE CARE - think had todo with runnning -exec and coming up in a reasonable spot? And maybe
## home dir??? soemthign quirly with bashrc default scripts on startup - dont remember
# MSYS defaults to ignorning the inherited path - see https://www.msys2.org/wiki/MSYS2-introduction/
ENV MSYS2_PATH_TYPE=inherit

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
