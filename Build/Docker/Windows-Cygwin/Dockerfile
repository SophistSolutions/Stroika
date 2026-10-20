# Usually specify this from commandline
ARG BASE_IMAGE=sophistsolutionsinc/stroika-buildvm-windows-vs2k22

FROM ${BASE_IMAGE}

# install choco (from https://chocolatey.org/install#completely-offline-install)
SHELL ["powershell"]

RUN \
    Set-ExecutionPolicy Bypass -Scope Process -Force; \
    $ErrorActionPreference = 'Stop'; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))


# Since 2025-11-25 apx - this seems to be needed to get choco to install without errors about permissions
RUN icacls 'C:\ProgramData\chocolatey' /grant 'Everyone:(OI)(CI)F' /T

# chocoInstall throws unless choco exits 0 AND every package named is then installed (choco list): choco can fail and
# still exit 0, saying it installed everything - as of 2.7.4 a package whose metadata it could not fetch (a 504 from the
# feed) is dropped with a warning, and no failed result. A failed RUN leaves no cached layer, so the next build retries.
RUN \
    function chocoInstall() { \
        choco install -y @Args; $rc = $LASTEXITCODE; \
        $have = choco list --limit-output | ForEach-Object { ($_ -split '\|')[0] }; \
        $missing = $Args | Where-Object { $_ -notlike '-*' -and $_ -notin $have }; \
        if (($rc -notin @(0, 1641, 3010)) -or $missing) { \
            type C:\ProgramData\chocolatey\logs\chocolatey.log; \
            throw ('choco install failed (exit code ' + $rc + '), not installed: ' + $missing) \
        } \
    }; \
    chocoInstall cygwin cyg-get

# Add cygwin to the path
RUN  \
    $env:PATH =  $env:PATH + ';c:\tools\cygwin\bin'; \
    [Environment]::SetEnvironmentVariable('PATH', $env:PATH, [EnvironmentVariableTarget]::Machine);

# Set HOME so we know where to copy bash_profile (seems to default to "" before we've actually run cygwin)
RUN  \
    [Environment]::SetEnvironmentVariable('HOME', 'c:\tools\cygwin\home', [EnvironmentVariableTarget]::Machine);

# Install a bunch of components - mostly that Stroika builds will find handy
# doxygen for generating docs - optional
# gdb for debugging - optional
# xxd for 'resource' strategy (currently just in Tests - as of 2023-12-16)
# cyg-get's exit code tells nothing - it starts cygwin's setup and discards its result - so check that each command the
# build needs is there (p7zip's is 7z). No double quotes in the bash script: PowerShell 5.1 passes them to bash.exe broken.
RUN \
    cyg-get default make git wget patch pkg-config automake dos2unix unzip p7zip doxygen jq xxd python3; \
    C:\tools\cygwin\bin\bash.exe -c 'PATH=/usr/bin; for c in make git wget patch pkg-config automake dos2unix unzip 7z doxygen jq xxd python3; do command -v $c > /dev/null || { echo cyg-get did not install: $c; exit 1; }; done'; \
    if ($LASTEXITCODE -ne 0) { throw 'cyg-get did not install everything' }

COPY Windows-Cygwin/Getting-Started-With-Stroika.md ./tools/cygwin/home
COPY Windows-Cygwin/.bash_profile ./tools/cygwin/home

ARG INSTALL_QUASAR=0
# Node 26, the same as Windows-MSYS's - see why there. Pinned only because nodejs-lts is still 24: once 26 is LTS
# (expected late 2026-10), go back to the nodejs-lts line. chocoInstall is the one above, for cygwin.
#   if ($env:INSTALL_QUASAR -eq 1) { chocoInstall nodejs-lts --force }
RUN \
    function chocoInstall() { \
        choco install -y @Args; $rc = $LASTEXITCODE; \
        $have = choco list --limit-output | ForEach-Object { ($_ -split '\|')[0] }; \
        $missing = $Args | Where-Object { $_ -notlike '-*' -and $_ -notin $have }; \
        if (($rc -notin @(0, 1641, 3010)) -or $missing) { \
            type C:\ProgramData\chocolatey\logs\chocolatey.log; \
            throw ('choco install failed (exit code ' + $rc + '), not installed: ' + $missing) \
        } \
    }; \
    if ($env:INSTALL_QUASAR -eq 1) { chocoInstall nodejs --version=26.12.0 --force }
SHELL ["cmd", "/S", "/C"]
RUN IF %INSTALL_QUASAR%==1 ( npm i -g npm @quasar/cli )

# Note, tried using ENTRYPOINT, but this doesn't seem to work reasonably with docker, 
# and much worse with windows (in part due to available shell commands)
CMD ["bash.exe", "--login"]

### NOTE TO RUN
###     docker run -v c:\Sandbox:c:\Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
###     see Build/Scripts/RunLocalWindowsDockerRegressionTests
