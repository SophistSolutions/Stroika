# escape=`

FROM mcr.microsoft.com/windows/servercore:ltsc2019
SHELL ["powershell"]

RUN Invoke-WebRequest "https://aka.ms/vs/16/release/vs_buildtools.exe" `
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
RUN & "$env:TEMP\vs_buildtools.exe" `
    --add Microsoft.VisualStudio.Workload.NetWeb `
    --quiet --wait --norestart --noUpdateInstaller | Out-Default

# msbuild
#RUN & 'C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe' /version

# choco
RUN iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))

# 7zip
RUN choco install -y 7zip.install

CMD ["powershell"]