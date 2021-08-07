# escape=`

# SEE https://hub.docker.com/_/microsoft-windows-servercore for list of versions
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:2004
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:20H2
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019
FROM ${BASE_IMAGE}


#
# List of workloads and components to add from
# https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?vs-2019&view=vs-2019
#
#    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools `
#    --add Microsoft.VisualStudio.Component.UniversalBuildTools `
#    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
#

#
# VS versions
#       https://docs.microsoft.com/en-us/visualstudio/releases/2019/history
#
ARG VS_Latest=https://aka.ms/vs/16/release/vs_buildtools.exe
ARG VS_16_5_5=https://download.visualstudio.microsoft.com/download/pr/68d6b204-9df0-4fcc-abcc-08ee0eff9cb2/17af83ed545d1287df7575786c326009e459708b60a6821d2a4f5606ef8efb9e/vs_BuildTools.exe
ARG VS_16_6_5=https://download.visualstudio.microsoft.com/download/pr/067fd8d0-753e-4161-8780-dfa3e577839e/4776935864d08e66183acd5b3647c9616da989c60afbfe100d4afc459f7e5785/vs_BuildTools.exe
ARG VS_16_7_7=https://download.visualstudio.microsoft.com/download/pr/a319c7ec-a0bd-4619-b966-4c58a50f7c76/8ba9f0872faf7640709098adbd88f156902529de9032d214844bec363865d99e/vs_BuildTools.exe
ARG VS_16_8_6=https://download.visualstudio.microsoft.com/download/pr/8aaeb3c2-46bb-4444-9ca6-0361b60b2d16/1f4552565c70ee4355fcb4bf561b11dc23ae0a20cb6b4bcd0546d6819e545eae/vs_BuildTools.exe
ARG VS_16_9_6=https://download.visualstudio.microsoft.com/download/pr/e730a0bd-baf1-4f4c-9341-ca5a9caf0f9f/9a5f58f745e70806220238cb31d9da147462331eaa6ff61af1607052010b20e0/vs_BuildTools.exe
ARG VS_16_10_0=https://download.visualstudio.microsoft.com/download/pr/cb1d5164-e767-4886-8955-2df3a7c816a8/b9ff67da6d68d6a653a612fd401283cc213b4ec4bae349dd3d9199659a7d9354/vs_BuildTools.exe
ARG VS_16_10_2=https://download.visualstudio.microsoft.com/download/pr/2d4f424c-910d-4198-80de-aa829c85ae6a/4bead7a72a5cb6bf93b8e59d297d84e47da9ae2f9f44033c1ae1ef377078c5aa/vs_BuildTools.exe
ARG VS_16_10_3=https://download.visualstudio.microsoft.com/download/pr/9dc321fd-8a9b-47ef-98a9-af0515e08d6f/722879b06e5570c1695878a24e7b46ca4f3ec53fe016826c02a452f8a5e7df0b/vs_BuildTools.exe
ARG VS_16_10_4=https://download.visualstudio.microsoft.com/download/pr/acfc792d-506b-4868-9924-aeedc61ae654/72ae7ec0c234bbe0e655dc4776110c23178c8fbb7bbcf9b5b96a683b95e8d755/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_16_10_4" `
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe `
    --add Microsoft.VisualStudio.Workload.VCTools `
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools `
    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
    --includeRecommended `
    --quiet --wait --norestart --noUpdateInstaller `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0


# choco
SHELL ["powershell"]
RUN iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))

### WORKAROUND DOCKER/CYGWIN issue/regression
###     https://github.com/moby/moby/issues/41058#issuecomment-653865175

### @todo as of 2021-06-01 there appear several perhaps better workaroudns, so try them
#### EG TRY:
###         RUN choco install -y cygwin & powershell -Command "Get-ChildItem -Path C:\tools -Recurse -Attributes ReparsePoint | Remove-Item"
###
###     use cygwinsetup instead of choco intall / cyg-get
SHELL ["cmd", "/S", "/C"]
RUN md c:\tools\cygwin && cd c:\tools\cygwin && curl -Sso cygwinsetup.exe https://cygwin.com/setup-x86_64.exe && start /wait cygwinsetup -q --root C:\tools\cygwin -P default,make,unzip,git,wget,patch,pkg-config,dos2unix,p7zip -X --site http://ctm.crouchingtigerhiddenfruitbat.org/pub/cygwin/circa/64bit/2020/05/31/142136/

#RUN choco install -y cyg-get
#RUN cyg-get default make unzip git wget patch pkg-config dos2unix p7zip

## Sometimes helpful
## cyg-get gdb
SHELL ["powershell"]
RUN  `
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin';   `
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

COPY Windows-Cygwin-VS2k19/Getting-Started-With-Stroika.md ./
COPY Windows-Cygwin-VS2k19/FixupPath.bat ./

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
