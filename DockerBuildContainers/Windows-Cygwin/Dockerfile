ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022
FROM ${BASE_IMAGE}


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

COPY Windows-Cygwin/Getting-Started-With-Stroika.md ./
COPY Windows-Cygwin/FixupPath.bat ./

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
