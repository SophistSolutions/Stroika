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
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022                          ## ltsc2022 works fine, but use latest  
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2025                            

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
ARG VS_17_10_5=https://download.visualstudio.microsoft.com/download/pr/11917f82-1030-4dda-a768-f0da3c4623a9/9c46bb75e959df411b26cd8bc871a53b495010f4e868fcc509f9bb26756cb311/vs_BuildTools.exe
ARG VS_17_11_5=https://download.visualstudio.microsoft.com/download/pr/69e24482-3b48-44d3-af65-51f866a08313/471c9a89fa8ba27d356748ae0cf25eb1f362184992dc0bb6e9ccf10178c43c27/vs_BuildTools.exe
ARG VS_17_12_3=https://download.visualstudio.microsoft.com/download/pr/f2819554-a618-400d-bced-774bb5379965/cc7231dc668ec1fb92f694c66b5d67cba1a9e21127a6e0b31c190f772bd442f2/vs_BuildTools.exe
ARG VS_17_13_6=https://download.visualstudio.microsoft.com/download/pr/8fada5c7-8417-4239-acc3-bd499af09222/353141457abcc59eb9c38b2f30084e7271c6bcfb4e185466d98161bada905759/vs_BuildTools.exe
ARG VS_17_14_0=https://download.visualstudio.microsoft.com/download/pr/f50ab15d-99d5-43aa-b0b4-496b6cb1e574/8e8630e8383f68b7d2110c409dc7b2df6ae3ac92254612952eeb6a1e40207105/vs_BuildTools.exe
ARG VS_17_14_2=https://download.visualstudio.microsoft.com/download/pr/d99c7868-44a2-4254-9861-78e21c19bd34/fa923c074d0a62d965f4dac9020924afbfe452c6ee29b294e227acd1913c0452/vs_BuildTools.exe
ARG VS_17_14_7=https://download.visualstudio.microsoft.com/download/pr/13907dbe-8bb3-4cfe-b0ae-147e70f8b2f3/8e3a678471efe31f94d708b5ae2a5f78d2fed2b95e70079b157e6f3d6231efd7/vs_BuildTools.exe
ARG VS_17_14_8=https://download.visualstudio.microsoft.com/download/pr/f55fba7b-3f02-49b7-9aca-a075049a807d/54aaec8b9879acb7f54282f8442e81a620e4e79a2e0990572a3fab9de70c05d6/vs_BuildTools.exe
ARG VS_17_14_19=https://download.visualstudio.microsoft.com/download/pr/5011a9cc-e8ef-42cb-ad72-87de1031accc/6af0e8ee3a77b013160cb267d23547d248adc34e30e98edb42483214b332fa51/vs_BuildTools.exe


SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_17_14_19" \
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
