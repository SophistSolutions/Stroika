# escape=`

# SEE https://hub.docker.com/_/microsoft-windows-servercore for list of versions
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:2004
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:20H2
#ARG BASE_IMAGE=mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019
###### ltsc2022 not working (yet) on github actions - LGP 2021-12-23
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022
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
#    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools `
#    --add Microsoft.VisualStudio.Component.UniversalBuildTools `
#    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
#

# # From hints on https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 `
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 `
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 `
#    --remove Microsoft.VisualStudio.Component.Windows81SDK `
#    --quiet --wait --norestart --noUpdateInstaller --nocache modify `

#
# VS versions
#       https://docs.microsoft.com/en-us/visualstudio/releases/2022/release-history
#
ARG VS_Latest=https://aka.ms/vs/17/release/vs_buildtools.exe
ARG VS_17_0=https://download.visualstudio.microsoft.com/download/pr/8cea3871-c742-43fb-bf8b-8da0699ab4af/faa4a70c6431c2bc5e915c6ad07425c17fb1a96cd106405677aa08b6d949ba63/vs_BuildTools.exe
ARG VS_17_0_1=https://download.visualstudio.microsoft.com/download/pr/8cea3871-c742-43fb-bf8b-8da0699ab4af/faa4a70c6431c2bc5e915c6ad07425c17fb1a96cd106405677aa08b6d949ba63/vs_BuildTools.exe
ARG VS_17_0_2=https://download.visualstudio.microsoft.com/download/pr/a53da67f-8d8a-448c-b211-d234d17e6398/810b12ab293714c34c29654ef5089f11bdc49d180236b0f1927f599cdb01d5f3/vs_BuildTools.exe
ARG VS_17_0_3=https://download.visualstudio.microsoft.com/download/pr/ab119488-4f37-455b-a5de-86064bd15c4e/aafa8b3da26e3103984b2ef6a5debb5cb86b407ec9a40eb94281660edb14ba5e/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_0_3" `
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe `
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" `
    --includeRecommended `
    --add Microsoft.VisualStudio.Workload.VCTools `
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools `
    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 `
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 `
    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 `
    --remove Microsoft.VisualStudio.Component.Windows81SDK `
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

#RUN choco install -y cygwin
#RUN powershell -Command "Get-ChildItem -Path C:\tools -Recurse -Attributes ReparsePoint | Remove-Item"
#RUN choco install -y cyg-get
#RUN cyg-get default make unzip git wget patch pkg-config dos2unix p7zip

## Sometimes helpful
## cyg-get gdb
SHELL ["powershell"]
RUN  `
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin';   `
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
