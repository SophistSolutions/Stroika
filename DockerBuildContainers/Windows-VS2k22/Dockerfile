ARG BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-msys

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

# # From hints on https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
#    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
#    --remove Microsoft.VisualStudio.Component.Windows81SDK \
#    --quiet --wait --norestart --noUpdateInstaller --nocache modify \

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
ARG VS_17_5_1=https://download.visualstudio.microsoft.com/download/pr/07db0e25-01f0-4ac0-946d-e03196d2cc8b/c200c70c03378c5d45f16746a1ab6470bb9894f2492ef753aaeff2545c050e96/vs_BuildTools.exe
ARG VS_17_5_2=https://download.visualstudio.microsoft.com/download/pr/3b6ddbf8-a147-4435-a494-52ca53c1322f/5b5fe89fa8b28b8f6623fbce6b8688c469759b24c1dada428c8e5227a83cbb57/vs_BuildTools.exe
ARG VS_17_5_4=https://download.visualstudio.microsoft.com/download/pr/0bb9a5f5-5481-4efe-92ab-cca29a90fa5e/adbfb904ddfc115ae7df00098df92d4e545a5eb062ffea8a93f7b8df8d509ff3/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_5_4" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --includeRecommended \
    --add Microsoft.VisualStudio.Workload.VCTools \
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools \
    --add Microsoft.VisualStudio.Component.VC.ATL \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
    --remove Microsoft.VisualStudio.Component.Windows81SDK \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0
# Since Visual Studio.net 17.5.0, adding Microsoft.VisualStudio.Component.VC.ATLMFC above causes install to fail,
# But adding here by itself is fine.
RUN (%TEMP%\vs_buildtools.exe \
    --installPath "%ProgramFiles%\Microsoft Visual Studio\2022\BuildTools" \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0) \
    && (del /f /s /q "C:\$Recycle.Bin")

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygin-vs2k22
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
