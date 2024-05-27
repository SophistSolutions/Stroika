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
ARG VS_17_7_6=https://download.visualstudio.microsoft.com/download/pr/ebbb3a8f-0b8f-4c9d-ac08-5e244e84b4fe/59b6da403afe6892d4531adb5c58dc52bff5db1e2173477ad7f9cf4b2c490277/vs_BuildTools.exe
ARG VS_17_8_6=https://download.visualstudio.microsoft.com/download/pr/5bebe58c-9308-4a5b-9696-b6f84e90a32e/a2e920fe6fc18aba57bb4b1a70865be01888aa21dd8c7f6a9ec7b4aa25c8cfaa/vs_BuildTools.exe
ARG VS_17_9_7=https://download.visualstudio.microsoft.com/download/pr/7e1f1b8d-73fe-448c-a4e0-0ea481025985/110c25702bb7e40f1c04da43e99afa3d54ee5adf45225908747ac26c1104b60a/vs_BuildTools.exe
ARG VS_17_10_0=https://download.visualstudio.microsoft.com/download/pr/4bc0c2da-4e6d-4a88-9eaa-0748022737fb/a96393451a176f8ec4571814c0e467d3b8bc32e2cafaa42df870e28e278fc344/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_9_7" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

#
# Where to look for list of these available components?
#   o   visual studio installer tool
#       'export' function and GUI to select.
#       C:\Users\lewis\OneDrive\Documents\.vsconfig
#   o   WebPage
#       https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2022
#

# According to https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2022
#   Microsoft.VisualStudio.Workload.VCTools contains many of these below components, but selecting the workload
#   doesn't appear to do anything. So we have to install needed components manually anyhow...

## ALTERNATE COMPONENT LINES TO CONSIDER
#    --add Microsoft.VisualStudio.Component.Windows10SDK.19041 \
#    --add Microsoft.VisualStudio.Component.VC.143 \

# ADD SOON Microsoft.VisualStudio.Component.VC.Modules.x86.x64

#
# recommended or optional from Microsoft.VisualStudio.Workload.VCTools
#
# specific version
    # --add Microsoft.VisualStudio.Component.VC.14.38.17.8.x86.x64 \
    # --add Microsoft.VisualStudio.Component.VC.14.38.17.8.ATL \
    # --add Microsoft.VisualStudio.Component.VC.14.38.17.8.CLI.Support \
    # --add Microsoft.VisualStudio.Component.VC.14.38.17.8.MFC \
# latest ()
    # --add Microsoft.VisualStudio.Component.VC.ATL \
    # --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    # --add Microsoft.VisualStudio.Component.VC.CLI.Support \
    # --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 \ 

# Probably add soon
    # --add Microsoft.VisualStudio.Component.Vcpkg \

#   WINDOWS SDK:
#       Windows10SDK.20348 is apparently the final Windows 10 SDK (WAG/been this number for a while).
#       Windows10SDK.22621 appears to be a windows 11 SDK (not my typo) - still untested using this instead
#       of the windows 10 SDK... try next release... --LGP 2024-02-24
#ARG USE_WINSDK="--add Microsoft.VisualStudio.Component.Windows10SDK.20348"
ARG USE_WINSDK="--add Microsoft.VisualStudio.Component.Windows11SDK.22621"

# Attempt to save a little space - running out of space on github actions; but didn't materially help --LGP 2024-02-26
# ARG REMOVE_WINSDK="--remove Microsoft.VisualStudio.Component.Windows10SDK.10240 --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 --remove Microsoft.VisualStudio.Component.Windows81SDK"
ARG REMOVE_WINSDK=""

# Microsoft.VisualStudio.Workload.UniversalBuildTools appars to be around 10GB, and doesn't appear needed to build
# Stroika, and bigness causes issues with github actions running out of space, so lose this for now --LGP 2024-02-26
# ARG USE_UniversalBuildTools="--add Microsoft.VisualStudio.Workload.UniversalBuildTools"
ARG USE_UniversalBuildTools=""


# del stuff to minimize space left in the filesystem layer
SHELL ["cmd", "/S", "/C"]
RUN (%TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --add Microsoft.VisualStudio.Workload.MSBuildTools  \
    %USE_UniversalBuildTools% \
    --add Microsoft.VisualStudio.Workload.VCTools \
    --add Microsoft.VisualStudio.Component.VC.ASAN \
    --add Microsoft.VisualStudio.Component.VC.CLI.Support \
    --add Microsoft.VisualStudio.Component.VC.CMake.Project \ 
    --add Microsoft.VisualStudio.Component.VC.ATL \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 \ 
    %USE_WINSDK% \
    %REMOVE_WINSDK% \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0) \
    && (del /f /s /q "%TEMP%\*.*")

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygin-vs2k22
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
