# escape=`

# SEE https://hub.docker.com/_/microsoft-windows-servercore for list of versions
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019
FROM ${BASE_IMAGE}
SHELL ["powershell"]

#
# List of workloads and components to add from
# https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?vs-2019&view=vs-2019
#
#    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools `
#    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
#

#
# VS versions
#       https://docs.microsoft.com/en-us/visualstudio/releases/2019/history
#
ARG VS_Latest=https://aka.ms/vs/16/release/vs_buildtools.exe
ARG VS_16_4_6=https://download.visualstudio.microsoft.com/download/pr/5714a1fb-d8a9-4e24-b6b7-c1a40a001d4c/f68093ddf8d523354d63ac326697473c98439c92c7df09159fe974ea6622b2f5/vs_BuildTools.exe
ARG VS_16_5_5=https://download.visualstudio.microsoft.com/download/pr/68d6b204-9df0-4fcc-abcc-08ee0eff9cb2/17af83ed545d1287df7575786c326009e459708b60a6821d2a4f5606ef8efb9e/vs_BuildTools.exe
ARG VS_16_6_0=https://download.visualstudio.microsoft.com/download/pr/0fed0c12-ccd3-4767-b151-a616aaf99d86/2d180d9c6b8d24f38a88301652581793353d0ffa0d9f04f6dff5ad26d720a97b/vs_BuildTools.exe

RUN Invoke-WebRequest "$env:VS_16_6_0" `
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
RUN & "$env:TEMP\vs_buildtools.exe" `
    --add Microsoft.VisualStudio.Workload.VCTools `
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools `
    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
    --includeRecommended `
    --quiet --wait --norestart --noUpdateInstaller `
    | Out-Default
# || IF "%ERRORLEVEL%"=="3010" EXIT 0

# choco
RUN iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))

RUN choco install -y cyg-get
RUN cyg-get default make unzip git wget patch pkg-config dos2unix p7zip
## Sometimes helpful
## cyg-get gdb
RUN  `
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin';   `
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

COPY Windows-Cygwin-VS2k19/Getting-Started-With-Stroika.md ./
COPY Windows-Cygwin-VS2k19/FixupPath.bat ./

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, and mcuh worse with windows (in part due to available shell commands)
CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests