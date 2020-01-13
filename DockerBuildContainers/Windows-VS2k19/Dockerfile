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

# msbuild
#RUN & 'C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe' /version

# choco
RUN iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))

# 7zip
RUN choco install -y 7zip.install

#   --params "/InstallDir:C:\CYGWIN /NoStartMenu"
SHELL ["cmd"]
RUN choco install -y cyg-get
RUN cyg-get default make unzip git
RUN set PATH=%PATH%;c:\tools\cygwin\bin\



CMD ["cmd"]



#### NOTE - to run vs tools, you may need to run VsDevCmd.bat first
#### But we may store in c:\BuildTools (if I specifed --intallpath there)
#### or c:\program files (x86)....


# Start developer command prompt with any other commands specified.
#ENTRYPOINT C:\BuildTools\Common7\Tools\VsDevCmd.bat &&
# "\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\Tools\VsDevCmd.bat"

# Default to PowerShell if no other command specified.
#CMD ["powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it testwinbld2:latest
