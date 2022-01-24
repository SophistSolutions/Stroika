# escape=`

# SEE https://hub.docker.com/_/microsoft-windows-servercore for list of versions
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:2004
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:20H2
#ARG BASE_IMAGE=mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019
###### ltsc2022 not working (yet) on github actions - LGP 2021-12-23
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019
#ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022
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
ARG VS_16_7_7=https://download.visualstudio.microsoft.com/download/pr/a319c7ec-a0bd-4619-b966-4c58a50f7c76/8ba9f0872faf7640709098adbd88f156902529de9032d214844bec363865d99e/vs_BuildTools.exe
ARG VS_16_9_6=https://download.visualstudio.microsoft.com/download/pr/e730a0bd-baf1-4f4c-9341-ca5a9caf0f9f/9a5f58f745e70806220238cb31d9da147462331eaa6ff61af1607052010b20e0/vs_BuildTools.exe
ARG VS_16_11_0=https://download.visualstudio.microsoft.com/download/pr/45dfa82b-c1f8-4c27-a5a0-1fa7a864ae21/75e7f5779a42dddabc647af82a7eae4bf1417484f0cd1ac9e8fd87cbe7450c39/vs_BuildTools.exe
ARG VS_16_11_1=https://download.visualstudio.microsoft.com/download/pr/9efbe138-ff42-4deb-95c9-1d78cdc1f98b/920981c883089c445a6a3a617396d089e7999437c1d70fc4629f557a75ac4fa5/vs_BuildTools.exe
ARG VS_16_11_3=https://download.visualstudio.microsoft.com/download/pr/22c17f05-944c-48dc-9f68-b1663f9df4cb/f3f6868ff82ea90b510c3ef76b8ee3ed2b559795da8dd80f3706fb8a7f7510d6/vs_BuildTools.exe
ARG VS_16_11_5=https://download.visualstudio.microsoft.com/download/pr/5a50b8ac-2c22-47f1-ba60-70d4257a78fa/4e0f5197da02b62b9fa48f05b55f2e206265785a6f0bab7235ef88fbdbe49e5e/vs_BuildTools.exe
ARG VS_16_11_7=https://download.visualstudio.microsoft.com/download/pr/c91ba3a2-4ed9-4ada-ac4a-01f62c9c86a9/5dc5c6649b2d35ab400df8536e8ee509304e48f560c431a264298feead70733c/vs_BuildTools.exe
ARG VS_16_11_8=https://download.visualstudio.microsoft.com/download/pr/b763973d-da6e-4025-834d-d8bc48e7d37f/4c9d3173a35956d1cf87e0fa8a9c79a0195e6e2acfe39f1ab92522d54a3bebb9/vs_BuildTools.exe
ARG VS_16_11_9=https://download.visualstudio.microsoft.com/download/pr/9a26f37e-6001-429b-a5db-c5455b93953c/f1c4f7b32e6da59b0a80c3a800d702211551738bcec68331aee1ab06d859be3d/vs_BuildTools.exe


SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_16_11_9" `
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe `
    --add Microsoft.VisualStudio.Workload.VCTools `
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools `
    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
    --includeRecommended `
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
