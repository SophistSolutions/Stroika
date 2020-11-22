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
ARG VS_16_8_1=https://download.visualstudio.microsoft.com/download/pr/2f4a234d-6e7c-4049-8248-6d9ac0d05c96/11f7d9f212a5195ed9680cda0baddced4eb99d06762c769163498344fa239d5b/vs_BuildTools.exe
ARG VS_16_8_2=https://download.visualstudio.microsoft.com/download/pr/9d2147aa-7b01-4336-b665-8fe07735e5ee/61cc248a8f240911db3e7ae5f0d0cd7358211d9a9424202e7a69786bb6d34357/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_16_8_2" `
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
