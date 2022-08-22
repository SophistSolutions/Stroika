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

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_3_1" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --includeRecommended \
    --add Microsoft.VisualStudio.Workload.VCTools \
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
    --remove Microsoft.VisualStudio.Component.Windows81SDK \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0


# install choco
SHELL ["powershell"]
ENV CHOCO_URL=https://chocolatey.org/install.ps1
RUN Set-ExecutionPolicy Bypass -Scope Process -Force; \
 [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]'Tls,Tls11,Tls12'; \
 iex ((New-Object System.Net.WebClient).DownloadString("$env:CHOCO_URL"));

# install cygwin
SHELL ["cmd", "/S", "/C"]
RUN choco install -y cygwin
RUN choco install -y cyg-get
# doxygen for generating docs - optional
# gdb for debugging - optional
RUN cyg-get default make git wget patch pkg-config automake dos2unix unzip p7zip doxygen jq

# Add cygwin to the path
SHELL ["powershell"]
RUN  \
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin';   \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

COPY Windows-Cygwin-VS2k22/Getting-Started-With-Stroika.md ./
COPY Windows-Cygwin-VS2k22/FixupPath.bat ./

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
