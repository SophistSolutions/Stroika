# Installing Development Environment Tools {#Installing-Required-Tools}

This documetation is incomplete, and really has little to do with Stroika, but has been frequently requested, so I'm adding some brief docs on this.

Note also - much of this (in perhaps more detail) can be gleaned (programatically) from the reading the Docker files
- [../Build/Docker/](../Build/Docker/)

Or better yet, can be avoided entirely by just using the pre-built docker containers [Quick-Start](./Quick-Start.md).

---

## What all build platforms require (overview)

- c++ compiler supporting C++20 or later
- conan (Conan 2, <https://conan.io>): builds the third-party components, keeping each in its cache - so a configuration's
  components are built once on a machine, whatever build tree needs them. If it fails at once with "table recipes has 4
  columns but 5 values were supplied", its home - the directory `conan config home` prints - was made by an early Conan 2
  (before 2.0.14), which a current one cannot read: move that directory aside, and Conan makes a new one
- make (gnu make)
- patch
- perl (to build OpenSSL - but not on Windows, where Stroika fetches Strawberry Perl for it)
- pkg-config
- python3, as `/usr/bin/python3` (on Windows, MSYS2's or Cygwin's own - not a native Windows one)
- realpath
- sed
- tar
- tr
- wget
- 7za (if building with LZMA SDK – the default)
- unzip

---

## For MacOS

- XCode (14 or later)
  - install from appstore
  - Then from command line
    - xcode-select –install
    - Homebrew can be helpful (but use whatever package mgr you wish)
      - ruby -e &quot;\$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/master/install](https://raw.githubusercontent.com/Homebrew/install/master/install))&quot;
      - to install apps with brew, use &quot;brew install APPNAME&quot;
    - brew install gnu-sed
    - brew install p7zip (if building lzma)
    - brew install conan

---

## Windows

Building with Stroika requires

- a C++ compiler (suggested Visual Studio.net 2022, Visual Studio.Net 2026 or later)
- a set of unix-like build tools (suggested MSYS OR Cygwin - no need for both)

### Visual Studio.net 2026

- Download and run [https://visualstudio.microsoft.com/downloads/](https://visualstudio.microsoft.com/downloads/)
- Install the C++ related workloads
  - Desktop Development with C++ (suggested/sufficient to get started)
- On the Individual Components page
  - select MFC (latest or whatever platform/toolsets you are targetting) if you want to build the LedIt and LedLineIt samples

### Visual Studio.net 2022

- Download and run [https://aka.ms/vs/17/release/vs_community.exe](https://aka.ms/vs/17/release/vs_community.exe)
- Install the C++ related workloads
  - Desktop Development with C++ (suggested/sufficient to get started)
- On the Individual Components page
  - select MFC (latest or whatever platform/toolsets you are targetting) if you want to build the LedIt and LedLineIt samples

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
  - unzip
  - python3

#### Via Chocolatey

- [https://chocolatey.org/](https://chocolatey.org/)
  

  This assumes you've first installed choco (see above link)

  ~~~
  choco install cygwin cyg-get
  cyg-get default make git wget patch pkg-config automake unzip p7zip doxygen python3
  ~~~

### MSYS

Note - we've only tested building (from MSYS) using Visual Studio. We've not yet integrated building with MINGW.

- See <https://www.msys2.org/>
- download and run installer - e.g. <https://github.com/msys2/msys2-installer/releases/download/2021-11-30/msys2-x86_64-20211130.exe>
  
  OR
  use 
  ~~~
  choco install msys2
  ~~~

- pacman -Syu
  
  Note - this closes the shell window when its done, so start a new shell window when it completes
- pacman --noconfirm -S --needed base-devel git unzip p7zip doxygen python

  python is MSYS2's own (`/usr/bin/python3`), which Stroika's Python build scripts run - not
  mingw-w64-x86_64-python, a native Windows one.

### Conan (MSYS or Cygwin)

Conan's own Windows build - a native conan.exe, which MSYS and Cygwin both run - from
<https://github.com/conan-io/conan/releases> (conan-VERSION-windows-x86_64.zip), unpacked into C:\Program Files\Conan, which
goes on the Windows PATH. In PowerShell, run as administrator:

~~~
Expand-Archive conan-VERSION-windows-x86_64.zip -DestinationPath 'C:\Program Files\Conan'
[Environment]::SetEnvironmentVariable('PATH', [Environment]::GetEnvironmentVariable('PATH', 'Machine') + ';C:\Program Files\Conan', 'Machine')
~~~

Cygwin inherits the Windows PATH. MSYS does only with MSYS2_PATH_TYPE=inherit - otherwise add to its ~/.bashrc:

~~~
export PATH="$PATH:/c/Program Files/Conan"
~~~

Not pip's Conan, in MSYS2's mingw64 python: an old one left there (/mingw64/bin/conan.exe) is found first, and stops working
when that python is upgraded.

---

## For UNIX

- Compiler
  - gcc 11 or later OR
    - Stroika v3 is currently tested with gcc-11 thru gcc-14
  - llvm (clang++) 14 or later
    - Stroika v3 is currently tested with clang-14 thrue clang-20
    - libc++ - if used - must be v 14 or later due to concept/chrono support
- automake (if building curl)
- libtool (gnu version) – (if building curl)
- conan: on Debian and Ubuntu, which package none, Conan's own .deb from <https://github.com/conan-io/conan/releases>
  ~~~sh
  wget https://github.com/conan-io/conan/releases/download/VERSION/conan-VERSION-$(dpkg --print-architecture).deb
  sudo apt-get install ./conan-VERSION-*.deb
  ~~~

---

## Other useful tools

- clang-format
  ~~~sh
  npm install -g clang-format
  ~~~

---

## Return to Building Stroika instructions

- [Building-Stroika.md#Quick-Start](Building%20Stroika.md#Quick-Start)
