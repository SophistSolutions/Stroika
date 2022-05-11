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
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:2004
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:20H2
# ARG BASE_IMAGE=mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019  ## Referenced in https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2022
# ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2019                          ## ltsc2019 not working on github actions - LGP 2022-02-23

## Seems to work now on github actions using runner=Windows-2022 --LGP 2022-02-25
ARG BASE_IMAGE=mcr.microsoft.com/windows/servercore:ltsc2022

FROM ${BASE_IMAGE}



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
#       https://docs.microsoft.com/en-us/visualstudio/releases/2019/history
#
ARG VS_Latest=https://aka.ms/vs/16/release/vs_buildtools.exe
ARG VS_16_7_7=https://download.visualstudio.microsoft.com/download/pr/a319c7ec-a0bd-4619-b966-4c58a50f7c76/8ba9f0872faf7640709098adbd88f156902529de9032d214844bec363865d99e/vs_BuildTools.exe
ARG VS_16_9_6=https://download.visualstudio.microsoft.com/download/pr/e730a0bd-baf1-4f4c-9341-ca5a9caf0f9f/9a5f58f745e70806220238cb31d9da147462331eaa6ff61af1607052010b20e0/vs_BuildTools.exe
ARG VS_16_11_0=https://download.visualstudio.microsoft.com/download/pr/45dfa82b-c1f8-4c27-a5a0-1fa7a864ae21/75e7f5779a42dddabc647af82a7eae4bf1417484f0cd1ac9e8fd87cbe7450c39/vs_BuildTools.exe
ARG VS_16_11_1=https://download.visualstudio.microsoft.com/download/pr/9efbe138-ff42-4deb-95c9-1d78cdc1f98b/920981c883089c445a6a3a617396d089e7999437c1d70fc4629f557a75ac4fa5/vs_BuildTools.exe
ARG VS_16_11_3=https://download.visualstudio.microsoft.com/download/pr/22c17f05-944c-48dc-9f68-b1663f9df4cb/f3f6868ff82ea90b510c3ef76b8ee3ed2b559795da8dd80f3706fb8a7f7510d6/vs_BuildTools.exe
ARG VS_16_11_5=https://download.visualstudio.microsoft.com/download/pr/5a50b8ac-2c22-47f1-ba60-70d4257a78fa/4e0f5197da02b62b9fa48f05b55f2e206265785a6f0bab7235ef88fbdbe49e5e/vs_BuildTools.exe
ARG VS_16_11_7=https://download.visualstudio.microsoft.com/download/pr/c91ba3a2-4ed9-4ada-ac4a-01f62c9c86a9/5dc5c6649b2d35ab400df8536e8ee509304e48f560c431a264298feead70733c/vs_BuildTools.exe
ARG VS_16_11_8=https://download.visualstudio.microsoft.com/download/pr/b763973d-da6e-4025-834d-d8bc48e7d37f/4c9d3173a35956d1cf87e0fa8a9c79a0195e6e2acfe39f1ab92522d54a3bebb9/vs_BuildTools.exe
ARG VS_16_11_9=https://download.visualstudio.microsoft.com/download/pr/9a26f37e-6001-429b-a5db-c5455b93953c/f1c4f7b32e6da59b0a80c3a800d702211551738bcec68331aee1ab06d859be3d/vs_BuildTools.exe
ARG VS_16_11_10=https://download.visualstudio.microsoft.com/download/pr/791f3d28-7e20-45d9-9373-5dcfbdd1f6db/d5eabc3f4472d5ab18662648c8b6a08ea0553699819b88f89d84ec42d12f6ad7/vs_BuildTools.exe
ARG VS_16_11_11=https://download.visualstudio.microsoft.com/download/pr/73f91fcb-aa18-4bec-8c2f-8270acb22398/775c32ca5efcdc1e2227e52e943bb05bc8a7a9c1acacebb9d4ccc8496cc9906c/vs_BuildTools.exe
ARG VS_16_11_12=https://download.visualstudio.microsoft.com/download/pr/890a2f3f-4222-451c-b7ea-035d6c583dd7/11a323cd2efd2fc6ea81332e21904cfc703ca1ab80ff81b758877f99b5d7402d/vs_BuildTools.exe
ARG VS_16_11_13=https://download.visualstudio.microsoft.com/download/pr/d935ace6-0b55-4ef2-8ef2-7921ad9f3d3a/e2dec25f47d3abe13a0874e91d4eede0bfd67adc07d8bf23761b12e81c89bb81/vs_BuildTools.exe
ARG VS_16_11_14=https://download.visualstudio.microsoft.com/download/pr/f4685935-e4ae-4242-93bc-38dbe6477fb9/235d263c0b61c75da5c79938ad70e7fe67c83ed0d9bca7773ddc50a74c9dcc59/vs_BuildTools.exe

SHELL ["powershell"]
RUN Invoke-WebRequest "$env:VS_16_11_14" \
    -OutFile "$env:TEMP\vs_buildtools.exe" -UseBasicParsing
SHELL ["cmd", "/S", "/C"]
RUN %TEMP%\vs_buildtools.exe \
    --add Microsoft.VisualStudio.Workload.VCTools \
    --add Microsoft.VisualStudio.Workload.UniversalBuildTools \
    --add Microsoft.VisualStudio.Component.VC.ATLMFC \
    --includeRecommended \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
    --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
    --remove Microsoft.VisualStudio.Component.Windows81SDK \
    --quiet --wait --norestart --noUpdateInstaller \
    || IF "%ERRORLEVEL%"=="3010" EXIT 0


# Install MSYS
SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop'; $ProgressPreference = 'SilentlyContinue';"]
ARG MSYS_LatestRelease=https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.sfx.exe
ARG MSYS_20220118=https://github.com/msys2/msys2-installer/releases/download/2022-01-18/msys2-base-x86_64-20220118.sfx.exe
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; \
  Invoke-WebRequest -UseBasicParsing -uri "$env:MSYS_20220118" -OutFile msys2.exe; \
  .\msys2.exe -y -oC:\; \
  Remove-Item msys2.exe ; \
  function msys() { C:\msys64\usr\bin\bash.exe @('-lc') + @Args; } \
  msys ' '; \
  msys 'pacman --noconfirm -Syuu'; \
  msys 'pacman --noconfirm -Syuu'; \
  msys 'pacman --noconfirm -S base-devel git unzip p7zip doxygen mingw-w64-x86_64-jq'; \
  msys 'pacman --noconfirm -Scc';
# doxygen for generating docs - optional
# gdb for debugging - optional


# Set PATH variable (to include MSYS, and its mingw64 directory - for jq)
SHELL ["powershell"]
RUN  \
    $env:PATH =  $env:PATH + ';c:\msys64\usr\bin;c:\msys64\mingw64\bin;';   \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);


COPY Windows-MSYS-VS2k19/Getting-Started-With-Stroika.md c:/msys64/
COPY Windows-MSYS-VS2k19/.bash_profile c:/msys64/

# MSYS defaults to ignorning the inherited path - see https://www.msys2.org/wiki/MSYS2-introduction/
ENV MSYS2_PATH_TYPE=inherit

CMD ["bash.exe", "--login"]

# ### NOTE TO RUN
# ###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19
# ###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k19
# ###     see ScriptsLib/RunLocalWindowsDockerRegressionTests
