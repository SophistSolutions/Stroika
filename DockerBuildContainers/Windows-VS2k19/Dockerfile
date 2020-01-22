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
RUN Invoke-WebRequest "https://aka.ms/vs/16/release/vs_buildtools.exe" `
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
