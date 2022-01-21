# Installing Development Environment Tools

This documetation is incomplete, and really has little to do with Stroika, but it was requested, so I'm adding some brief docs on this.

Note also - much of this (in perhaps more detail) can be gleaned (programatically) from the reading the Docker files
- [../DockerBuildContainers/ReadMe.md](../DockerBuildContainers/ReadMe.md)

## Windows

Building with Stroika requires

- a C++ compiler
- a set of unix-like build tools

### Visual Studio.net 2022

(you can use Visual Studio.net 2019 or 2017 as well).
- Download and run [https://aka.ms/vs/17/release/vs_community.exe](https://aka.ms/vs/17/release/vs_community.exe)
- Install the C++ related workloads
  - Desktop Development with C++ (suggested/sufficient to get started)

### Cygwin

- TBD

### MSYS

Note - we've only tested building (from MSYS) using Visual Studio. We've not yet integrated building with MINGW.

- See <https://www.msys2.org/>
- download and run installer - e.g. <https://github.com/msys2/msys2-installer/releases/download/2021-11-30/msys2-x86_64-20211130.exe>
- pacman -Syu
  
  Note - this closes the shell window when its done, so start a new shell window when it completes
- pacman --noconfirm -S --needed base-devel git unzip p7zip


# Return to Building Stroika instructions

- [Building Stroika.md#Quick-Start](Building%20Stroika.md#Quick-Start)
