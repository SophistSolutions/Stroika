# Installing Development Environment Tools

This documetation is incomplete, and really has little to do with Stroika, but has been frequently requested, so I'm adding some brief docs on this.

Note also - much of this (in perhaps more detail) can be gleaned (programatically) from the reading the Docker files
- [../DockerBuildContainers/ReadMe.md](../DockerBuildContainers/ReadMe.md)

---

## What all build platforms require (overview)

- c++ compiler supporting C++17 or later
- make (gnu make)
- patch
- perl
- pkg-config
- realpath
- sed
- tar
- tr
- wget
- 7za (if building with LZMA SDK – the default)
- unzip

---

## For MacOS

- XCode (12 or later)
  - install from appstore
  - Then from command line
    - xcode-select –install
    - Homebrew can be helpful (but use whatever package mgr you wish)
      - ruby -e &quot;\$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/master/install](https://raw.githubusercontent.com/Homebrew/install/master/install))&quot;
      - to install apps with brew, use &quot;brew install APPNAME&quot;
    - brew install gnu-sed
    - brew install p7zip (if building lzma)

---

## Windows

Building with Stroika requires

- a C++ compiler (suggested Visual Studio.net 2022, but 2017, 2019 also supported)
- a set of unix-like build tools (suggested MSYS OR Cygwin - no need for both)

### Visual Studio.net 2022

- Download and run [https://aka.ms/vs/17/release/vs_community.exe](https://aka.ms/vs/17/release/vs_community.exe)
- Install the C++ related workloads
  - Desktop Development with C++ (suggested/sufficient to get started)
- On the Individual Components page
  - select MFC (latest or whatever platform/toolsets you are targetting) if you want to build the LedIt, LedLineIt, and ActiveLedIt samples

### Cygwin

#### Via https://cygwin.com/

- Download and install [https://cygwin.com/setup-x86_64.exe](https://cygwin.com/setup-x86_64.exe)
- Add packages
  - make
  - p7zip
  - git  (not used by Stroika, but by these instructions)
  - wget
  - patch
  - pkg-config
  - automake
  - dos2unix
  - unzip

#### Via Chocolatey

- [https://chocolatey.org/](https://chocolatey.org/)
  TBD

### MSYS

Note - we've only tested building (from MSYS) using Visual Studio. We've not yet integrated building with MINGW.

- See <https://www.msys2.org/>
- download and run installer - e.g. <https://github.com/msys2/msys2-installer/releases/download/2021-11-30/msys2-x86_64-20211130.exe>
- pacman -Syu
  
  Note - this closes the shell window when its done, so start a new shell window when it completes
- pacman --noconfirm -S --needed base-devel git unzip p7zip

---

## For UNIX

- Compiler
  - gcc 8 or later OR
    - Stroika v2.1 is currently tested with gcc-8, gcc-9, gcc-10, and gcc-11
  - llvm (clang++) 6 or later
    - Stroika v2.1 is currently tested with clang6, clang-7, clang-8, clang-9, clang-10, clang-11, clang-12
- automake (if building curl)
- libtool (gnu version) – (if building curl)


---

## Return to Building Stroika instructions

- [Building Stroika.md#Quick-Start](Building%20Stroika.md#Quick-Start)
