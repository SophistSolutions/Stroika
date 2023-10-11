# See https://hub.docker.com/_/microsoft-windows
#   for list of available windows server types.
#
#   It appears empirically, that nanoserver doesn't work with the visual studio installer.
#   servercore is the next smallest, and that appears to work fine.
#
# SEE 
#   https://hub.docker.com/_/microsoft-windows-servercore
#   https://hub.docker.com/_/microsoft-dotnet-framework-sdk/
#        for list of versions
#
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019                          ## ltsc2019 not working on github actions - LGP 2022-02-23

## Seems to work now on github actions using runner=Windows-2022 --LGP 2022-02-25
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022

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
#    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools \
#    --add Microsoft.VisualStudio.Component.UniversalBuildTools \
#    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
#

#
# VS versions
#       https://docs.microsoft.com/en-us/visualstudio/releases/2022/release-history
#
ARG VS_Latest=https://aka.ms/vs/17/release/vs_buildtools.exe
ARG VS_17_0=https://download.visualstudio.microsoft.com/download/pr/8cea3871-c742-43fb-bf8b-8da0699ab4af/faa4a70c6431c2bc5e915c6ad07425c17fb1a96cd106405677aa08b6d949ba63/vs_BuildTools.exe
ARG VS_17_1_6=https://download.visualstudio.microsoft.com/download/pr/949751db-6687-4a88-a0cf-047f10908a29/3d9b988f8850d1af4fae60807d8695249731fc19488eed013d1dd4a21c7309d5/vs_BuildTools.exe
ARG VS_17_2_6=https://download.visualstudio.microsoft.com/download/pr/91cf5cbb-c34a-4766-bff6-aea28265d815/645d56f3dc5b12783a0c9a19dc90a2cfc63191d837e6c988fba91e6db3525bf3/vs_BuildTools.exe
ARG VS_17_3_6=https://download.visualstudio.microsoft.com/download/pr/5c9aef4f-a79b-4b72-b379-14273860b285/bd2dd3a59d2553382f89712d19e4d5c3d930d9a41c9426cf8194dd5a3a75875f/vs_BuildTools.exe
ARG VS_17_4_2=https://download.visualstudio.microsoft.com/download/pr/f3f8db49-2cd0-43df-9ced-12dcb6b3954b/8f91b73ed92af58f25d4fe12e7963619af9e8b2997b9a83ec520fe3f52a40099/vs_BuildTools.exe
ARG VS_17_5_5=https://download.visualstudio.microsoft.com/download/pr/7ac7457f-ba6e-4a12-ba2c-1adebad20070/619bb425ddaecbbb256036b1b2d8b5756c64cb1b1dffe5251facf9dabdd88258/vs_BuildTools.exe
ARG VS_17_6_4=https://download.visualstudio.microsoft.com/download/pr/33081bfc-10f1-42d4-8f5a-df6709b8b105/45260991ae48300c333ff98ce5d066064cb2ae03433b126cc5fdce1926ff8807/vs_BuildTools.exe
ARG VS_17_7_0=https://download.visualstudio.microsoft.com/download/pr/82cc7543-5e64-4e74-b990-5ebdc333739f/102a97bd21844fea39df874d028dc9281eeb138f060d14dcc4f0a0feafd69f8b/vs_BuildTools.exe
ARG VS_17_7_1=https://download.visualstudio.microsoft.com/download/pr/cda788db-d262-4261-ac9e-064293a93399/db1fbd87816ca23aa0c0e487b78129794b61aec1e12b30c2fc277b3192aa8913/vs_BuildTools.exe
ARG VS_17_7_3=https://download.visualstudio.microsoft.com/download/pr/1d66edfe-3c83-476b-bf05-e8901c62ba7f/bac71effb5a23d7cd1a81e5f628a0c8dcb7e8a07e0aa7077c853ed84a862dceb/vs_BuildTools.exe
ARG VS_17_7_4=https://download.visualstudio.microsoft.com/download/pr/e514a25b-a89d-4051-a63c-05ccd9be41e9/6ca2980598be2c888c3bb3b288c688a04b3fc1f1acb62d7c3cdaada47b36c3ea/vs_BuildTools.exe
ARG VS_17_7_5=https://download.visualstudio.microsoft.com/download/pr/8bee1b10-0767-416b-a3c0-d658a719947b/58afd70e8fbc37b1a3f48d2950c90b1a9f4a926bb52adef3749046cd70ab14dc/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_7_5" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

## ALTERNATE COMPONENT LINES TO CONSIDER
#    --add Microsoft.VisualStudio.Component.Windows10SDK.19041 \

# del stuff to minimize space left in the filesystem layer
SHELL ["cmd", "/S", "/C"]
RUN (%TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --add Microsoft.VisualStudio.Workload.MSBuildTools  \
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools \
    --add Microsoft.VisualStudio.Workload.VCTools  \
    --add Microsoft.VisualStudio.Component.VC.ATL \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --add Microsoft.VisualStudio.Component.Windows10SDK.20348 \
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 \ 
    --add Microsoft.VisualStudio.Component.VC.CMake.Project \ 
    --add Microsoft.VisualStudio.Component.VC.ASAN \
    --add Microsoft.VisualStudio.Component.VC.143 \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0) \
    && (del /f /s /q "%TEMP%\*.*")

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygin-vs2k22
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
