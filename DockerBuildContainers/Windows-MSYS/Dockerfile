# Usually specify this from commandline
ARG BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22

FROM ${BASE_IMAGE}

#
# NOTE - can install via choco or via MSYS installer directly.
# Default to MSYS installer directly cuz
#   - smaller that way (no choco)
#   - more flexible picking versions of msys to install
# but not significant difference
#
#   Just comment out <CHOCO_BASED_INSTALLER> or <MYS_INSTALLER> sections according to taste.
#   WISH DOCKERFILES HAD AN IF!!! OMG...
#


#### https://stroika.atlassian.net/browse/STK-766
#### NOTE https://github.com/microsoft/hcsshim/issues/696 - is the source of the del /f /s /q "C:\$Recycle.Bin
#### Magic - that PREVENTS the MSYS install hang!!!

### INSTALL VIA CHOCO

# ### <CHOCO_BASED_INSTALLER>
# ARG MSYS_DIR=c:/tools/msys64/
# ARG MSYS_ASDOS_DIR=c:\tools\msys64
# # install choco itself
# SHELL ["powershell"]
# ENV CHOCO_URL=https://chocolatey.org/install.ps1
# RUN Set-ExecutionPolicy Bypass -Scope Process -Force; \
#  iex ((New-Object System.Net.WebClient).DownloadString("$env:CHOCO_URL"));
# # install via choco
# SHELL ["cmd", "/S", "/C"]
# RUN choco install -y msys2 && del /f /s /q "C:\$Recycle.Bin"
# ### </CHOCO_BASED_INSTALLER>


### OR INSTALL VIA MSYS INSTALLER
### See list at https://github.com/msys2/msys2-installer/releases
#### <MSYS_INSTALLER>
ARG MSYS_DIR=c:/msys64/
ARG MSYS_ASDOS_DIR=c:\msys64
ARG MSYS_LatestRelease=https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe
# ARG MSYS_20220904=https://github.com/msys2/msys2-installer/releases/download/2022-09-04/msys2-base-x86_64-20220904.sfx.exe
# ARG MSYS_20230127=https://github.com/msys2/msys2-installer/releases/download/2023-01-27/msys2-base-x86_64-20230127.sfx.exe
# ARG MSYS_20240507=https://github.com/msys2/msys2-installer/releases/download/2024-05-07/msys2-base-x86_64-20240507.sfx.exe
# ARG MSYS_20241208=https://github.com/msys2/msys2-installer/releases/download/2024-12-08/msys2-base-x86_64-20241208.sfx.exe
ARG MSYS_20250622=https://github.com/msys2/msys2-installer/releases/download/2025-06-22/msys2-base-x86_64-20250622.sfx.exe

SHELL ["powershell"]
RUN Set-ExecutionPolicy Bypass -Scope Process -Force; \
  $ErrorActionPreference = 'Stop'; \
  Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_20250622" -OutFile msys2.exe; \
  .\msys2.exe -y -oC:\; \
  Remove-Item msys2.exe ;\
  function msys() { C:\msys64\usr\bin\bash.exe @('-lc') + @Args; } \
  msys ' '; \
  msys 'pacman --noconfirm -Syuu'; \
  msys 'pacman --noconfirm -Syuu'; \
  cmd /c del /f /s /q 'C:\$Recycle.Bin'
#### </MSYS_INSTALLER>


## Complete MSYS install, once the basic (core components of) MSYS are installed one way or another

#
# Add MSYS bin directory to the path
#
# Setup path to include msys tools (and mingw dir for jq)
# ENV PATH="${PATH};${MSYS_ASDOS_DIR}\usr\bin;${MSYS_ASDOS_DIR}\mingw64\bin"
#
# NOTE - instead of doing this here, you could do it in the .bash_profile for just the mingw directory and
# use full path in the CMD line below.
#
RUN  \
    $env:PATH =  $env:PATH + ';C:\msys64\usr\bin;C:\msys64\mingw64\bin'; \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

# Need to know exact path to copy .bash_profile to home directory
RUN  \
    [Environment]::SetEnvironmentVariable('HOME', 'c:\msys64\home', [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
# xxd for 'resource' strategy (currently just in Tests - as of 2023-12-16) - weirdly in vim package
RUN \
    function msys() { C:\msys64\usr\bin\bash.exe @('-lc') + @Args; } \
    msys 'pacman --noconfirm -S base-devel git unzip p7zip doxygen pkg-config mingw-w64-x86_64-jq vim mingw-w64-x86_64-python'; \
    msys 'pacman --noconfirm -Syuu'

COPY Windows-MSYS/Getting-Started-With-Stroika.md $MSYS_DIR/home/
COPY Windows-MSYS/.bash_profile $MSYS_DIR/home/

ARG INSTALL_QUASAR=0
# SHELL ["cmd", "/S", "/C"]
# RUN IF %INSTALL_QUASAR%==1 ( pacman --noconfirm -S mingw-w64-x86_64-nodejs && npm i -g npm @quasar/cli )
SHELL ["powershell"]
# It appears this installed nodejs 23.5 - which appears incompatible with quasar-vite 2.0 (recent)  - so
# try installing nodejs 20 - from nodesource
ARG MSYS_NODEJSURI='https://nodejs.org/dist/v20.9.0/node-v20.9.0-x64.msi'
RUN IF ($env:INSTALL_QUASAR -eq 1) { \
        Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_NODEJSURI" -OutFile node.msi; \
        Start-Process msiexec.exe -Wait -ArgumentList '/i "node.msi" /L*V ".\node.msi.log" /quiet /norestart'; \
        $env:PATH = $env:PATH + ';C:\Program Files\nodejs'; \
        [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine); \
        Remove-Item node.msi ;\
        Remove-Item node.msi.log ;\
        npm install -g npm@10.9;\
        npm i -g npm @quasar/cli ;\
    }

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["bash.exe", "--login"]

# NOT sure if this is a good idea. Docs on MSYS2_PATH_TYPE in https://www.msys2.org/wiki/MSYS2-introduction/
# suggest its NOT a good idea. But without it, I get:
#       Missing component jq: try pacman -S mingw-w64-x86_64-jq; and maybe add export PATH="$PATH:/mingw64/bin" to ~/.bashrc
#       make[1]: *** [Makefile:363: check-prerequisite-tools-common] Error 1
# when I try to build Stroika inside a container (following the instructions in Getting-Started-With-Stroika.md)
ENV MSYS2_PATH_TYPE=inherit


### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
