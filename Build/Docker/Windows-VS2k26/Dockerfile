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
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2025                            ## now works but requres runs_on: windows-2025 in github actions

FROM ${BASE_IMAGE}


#
# VS versions
#       https://learn.microsoft.com/en-us/visualstudio/releases/2026/release-history
#
ARG VS_18_6_2=https://download.visualstudio.microsoft.com/download/pr/471ad3d6-cb2b-4d53-8edf-a9eeade096a5/3cfa3f8fc957f406dc35be713959a85c10b23b9ed9a865626550e8cf676d96a8/vs_BuildTools.exe


SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_18_6_2" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing

#
# Where to look for list of these available components?
#   o   visual studio installer tool
#       'export' function and GUI to select.
#       C:\Users\lewis\OneDrive\Documents\.vsconfig
#   o   WebPage
#       https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=visualstudio&viewFallbackFrom=vs-2026
#

# According to https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=visualstudio&viewFallbackFrom=vs-2026
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
#ARG USE_WINSDK="--add Microsoft.VisualStudio.Component.Windows11SDK.22621"
ARG USE_WINSDK="--add Microsoft.VisualStudio.Component.Windows11SDK.26100"

# Attempt to save a little space - running out of space on github actions; but didn't materially help --LGP 2024-02-26
# ARG REMOVE_WINSDK="--remove Microsoft.VisualStudio.Component.Windows10SDK.10240 --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 --remove Microsoft.VisualStudio.Component.Windows81SDK"
ARG REMOVE_WINSDK=""

# Microsoft.VisualStudio.Workload.UniversalBuildTools appars to be around 10GB, and doesn't appear needed to build
# Stroika, and bigness causes issues with github actions running out of space, so lose this for now --LGP 2024-02-26
# ARG USE_UniversalBuildTools="--add Microsoft.VisualStudio.Workload.UniversalBuildTools"
ARG USE_UniversalBuildTools=""


# del stuff to minimize space left in the filesystem layer
SHELL ["cmd", "/S", "/C"]
# RUN  (wmic logicaldisk get deviceid, freespace, size, description)
RUN (%TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\18\BuildTools" \
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
    || IF "%ERRORLEVEL%"=="3010" EXIT 0)
    
# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygin-vs2k26
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k26
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
