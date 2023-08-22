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


#
# List of workloads and components to add from
# https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?vs-2019&view=vs-2019
#
#    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools \
#    --add Microsoft.VisualStudio.Component.UniversalBuildTools \
#    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
#

# # From hints on https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
#    --remove Microsoft.VisualStudio.Component.Windows81SDK \
#    --quiet --wait --norestart --noUpdateInstaller --nocache modify \

#
# VS versions
#       https://docs.microsoft.com/en-us/visualstudio/releases/2022/release-history
#
ARG VS_Latest=https://aka.ms/vs/17/release/vs_buildtools.exe
ARG VS_17_0=https://download.visualstudio.microsoft.com/download/pr/8cea3871-c742-43fb-bf8b-8da0699ab4af/faa4a70c6431c2bc5e915c6ad07425c17fb1a96cd106405677aa08b6d949ba63/vs_BuildTools.exe
ARG VS_17_1_6=https://download.visualstudio.microsoft.com/download/pr/949751db-6687-4a88-a0cf-047f10908a29/3d9b988f8850d1af4fae60807d8695249731fc19488eed013d1dd4a21c7309d5/vs_BuildTools.exe
ARG VS_17_2_0=https://download.visualstudio.microsoft.com/download/pr/dc2793e9-7b80-4f11-9e33-85833e8921a6/e6b0cd7dd16ce03a0cb49e697ca0a5dd23230711235179eb80f6d5c5785d83e4/vs_BuildTools.exe
ARG VS_17_2_4=https://download.visualstudio.microsoft.com/download/pr/2246570b-d03f-487d-8eeb-41e4a9c93199/586ee66cd76232033290f2806141386865e87730e7776116eef35a7316fb0af1/vs_BuildTools.exe
ARG VS_17_2_6=https://download.visualstudio.microsoft.com/download/pr/91cf5cbb-c34a-4766-bff6-aea28265d815/645d56f3dc5b12783a0c9a19dc90a2cfc63191d837e6c988fba91e6db3525bf3/vs_BuildTools.exe
ARG VS_17_3_1=https://download.visualstudio.microsoft.com/download/pr/af9ede5a-5bbf-435e-9cdc-4017b67d3704/185c6e6558660391aa0013dd71f85a80463be8a7e4c8107fcd0c0395fe9d6802/vs_BuildTools.exe
ARG VS_17_3_4=https://download.visualstudio.microsoft.com/download/pr/8106c1cc-df87-4854-8865-3b46bef5867c/771fbda86c3f12a52dc9999e39ad80a7cbbd16b9c0b940671b03d3364fe002d4/vs_BuildTools.exe
ARG VS_17_3_6=https://download.visualstudio.microsoft.com/download/pr/5c9aef4f-a79b-4b72-b379-14273860b285/bd2dd3a59d2553382f89712d19e4d5c3d930d9a41c9426cf8194dd5a3a75875f/vs_BuildTools.exe
ARG VS_17_4_0=https://download.visualstudio.microsoft.com/download/pr/de71f641-13a1-4991-92dc-ba1d44ac1605/714af6dd559604fd9e35463b18167bab1255ec5f171735cc9b2d7c3a37eee00f/vs_BuildTools.exe
ARG VS_17_4_1=https://download.visualstudio.microsoft.com/download/pr/2160190b-bb01-4670-9492-34da461fa0c9/fabda7e422ada90c229262a4447c08933ec5bf66a9f38129cd19490eea2dd180/vs_BuildTools.exe
ARG VS_17_4_2=https://download.visualstudio.microsoft.com/download/pr/f3f8db49-2cd0-43df-9ced-12dcb6b3954b/8f91b73ed92af58f25d4fe12e7963619af9e8b2997b9a83ec520fe3f52a40099/vs_BuildTools.exe
ARG VS_17_5_1=https://download.visualstudio.microsoft.com/download/pr/07db0e25-01f0-4ac0-946d-e03196d2cc8b/c200c70c03378c5d45f16746a1ab6470bb9894f2492ef753aaeff2545c050e96/vs_BuildTools.exe
ARG VS_17_5_2=https://download.visualstudio.microsoft.com/download/pr/3b6ddbf8-a147-4435-a494-52ca53c1322f/5b5fe89fa8b28b8f6623fbce6b8688c469759b24c1dada428c8e5227a83cbb57/vs_BuildTools.exe
ARG VS_17_7_1=https://download.visualstudio.microsoft.com/download/pr/cda788db-d262-4261-ac9e-064293a93399/db1fbd87816ca23aa0c0e487b78129794b61aec1e12b30c2fc277b3192aa8913/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_7_1" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --includeRecommended \
    --add Microsoft.VisualStudio.Workload.VCTools \
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools \
    --add Microsoft.VisualStudio.Component.VC.ATL \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
    --remove Microsoft.VisualStudio.Component.Windows81SDK \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0
# Since Visual Studio.net 17.5.0, adding Microsoft.VisualStudio.Component.VC.ATLMFC above causes install to fail,
# But adding here by itself is fine.
RUN %TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0


### OR INSTALL VIA MSYS INSTALLER
#### <MYS_INSTALLER>
ARG MSYS_DIR=c:/msys64/
ARG MSYS_ASDOS_DIR=c:\msys64
ARG MSYS_LatestRelease=https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe
ARG MSYS_20220118=https://github.com/msys2/msys2-installer/releases/download/2022-01-18/msys2-base-x86_64-20220118.sfx.exe
ARG MSYS_20220904=https://github.com/msys2/msys2-installer/releases/download/2022-09-04/msys2-base-x86_64-20220904.sfx.exe
ARG MSYS_20230127=https://github.com/msys2/msys2-installer/releases/download/2023-01-27/msys2-base-x86_64-20230127.sfx.exe
SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; \
  Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_20230127" -OutFile msys2.exe; \
  .\msys2.exe -y -oC:\; \
  Remove-Item msys2.exe ;\
  function msys() { C:\msys64\usr\bin\bash.exe @('-lc') + @Args; } \
  msys ' '; \
  msys 'pacman --noconfirm -Syuu'; \
  msys 'pacman --noconfirm -Syuu'; \
  cmd /c del /f /s /q 'C:\$Recycle.Bin'
#### </MYS_INSTALLER>


## Complete MSYS install, once the basic (core components of) MSYS are installed one way or another

# Setup path to include msys tools (and mingw dir for jq)
# ENV PATH="${PATH};${MSYS_ASDOS_DIR}\usr\bin;${MSYS_ASDOS_DIR}\mingw64\bin"

# Add cygwin to the path
SHELL ["powershell"]
RUN  \
    $env:PATH =  $env:PATH + ';C:\msys64\usr\bin;C:\msys64\mingw64\bin'; \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
SHELL ["cmd", "/S", "/C"]
RUN \
    pacman --noconfirm -S base-devel git unzip p7zip doxygen mingw-w64-x86_64-jq && \
    pacman --noconfirm -Syuu

COPY Windows-MSYS-VS2k22/Getting-Started-With-Stroika.md $MSYS_DIR
COPY Windows-MSYS-VS2k22/.bash_profile $MSYS_DIR

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
# CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]
CMD ["bash.exe", "--login"]

## @todo - NOT SURE A GOOD IDEA - EXPLAIN WHY WE CARE - think had todo with runnning -exec and coming up in a reasonable spot? And maybe
## home dir??? soemthign quirly with bashrc default scripts on startup - dont remember
# MSYS defaults to ignorning the inherited path - see https://www.msys2.org/wiki/MSYS2-introduction/
ENV MSYS2_PATH_TYPE=inherit

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
