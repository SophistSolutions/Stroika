# escape=`

FROM mcr.microsoft.com/windows/servercore:ltsc2019
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
ARG VS_16_4_4=https://download.visualstudio.microsoft.com/download/pr/8ab6eab3-e151-4f4d-9ca5-07f8434e46bb/df2db8df6dbb2d9c22aa627ce065811926a7396cb7c8e421c1dd865aba2cf231/vs_BuildTools.exe

RUN Invoke-WebRequest "$env:VS_16_4_4" `
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

COPY Windows-VS2k19/Getting-Started-With-Stroika.md ./
COPY Windows-VS2k19/FixupPath.bat ./

CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-vs2k19:latest
###     docker run -it sophistsolutionsinc/stroika-buildvm-windows-vs2k19:latest
