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
ARG VS_17_0_4=https://download.visualstudio.microsoft.com/download/pr/99fe5fea-e07c-4e6e-87ef-32a88c6ec393/c4ea44aa066ea725a2fe1e0796857530cd2ae8954cf0d546590db20f8e5d771f/vs_BuildTools.exe
ARG VS_17_0_5=https://download.visualstudio.microsoft.com/download/pr/ce8663b0-08ed-410a-9f5d-4f9469d1b2cb/0279f6067c003f8b81621502fa95a1df377b1fdd7769a8e80ff63df26756d4e2/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_0_5" `
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



# Install MSYS
SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]

ARG MSYS_LatestRelease=https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe

RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
  Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_LatestRelease" -OutFile msys2.exe; `
  .\msys2.exe -y -oC:\; `
  Remove-Item msys2.exe ; `
  function msys() { C:\msys64\usr\bin\bash.exe @('-lc') + @Args; } `
  msys ' '; `
  msys 'pacman --noconfirm -Syuu'; `
  msys 'pacman --noconfirm -Syuu'; `
  msys 'pacman --noconfirm -S base-devel git unzip p7zip'; `
  msys 'pacman --noconfirm -Scc'; 


# Set PATH variable (to include MSYS)
SHELL ["powershell"]
RUN  `
    $env:PATH =  $env:PATH + ';c:\msys64\usr\bin';   `
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);


COPY Windows-MSYS-VS2k19/Getting-Started-With-Stroika.md c:/msys64
COPY Windows-MSYS-VS2k19/.bash_profile c:/msys64/


# # Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# # and much worse with windows (in part due to available shell commands)
# CMD ["cmd", "/c", "call FixupPath.bat && echo see Getting-Started-With-Stroika.md && cmd"]
# CMD ["cmd", "/c", "echo see Getting-Started-With-Stroika.md && cmd"]
CMD ["bash.exe", "-l"]

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
