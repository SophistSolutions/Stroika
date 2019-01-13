# Building Stroika

# Common

Stroika is a C++ class library. For the most part, it&#39;s built make. However, internally, some of these make rules use Perl etc scripts.

# Required Tools and compatible compiler versions

Roughly, Stroika requires a modern C++ compiler, and gnu make to build.

### Stroika v2.0 {prior version}

- C++ 14 or later

### Stroika v2.1 {this version}

- C++17 or later

The details are below. But Stroika is regularly tested with gcc 7 and later, clang++6 or later, and visual studio 2017 (latest), and XCode 10.

# Quick Start

## For the very impatient

wget [https://github.com/SophistSolutions/Stroika/archive/V2.1-Release.tar.gz](https://github.com/SophistSolutions/Stroika/archive/V2.1-Release.tar.gz)

tar xf V2.1-Release.tar.gz

make --directory Stroika-2.1-Release all run-tests

## For the more patient (hints about what to try next)

- See Required Tools below (or wait for warnings during the build process)
- wget [https://github.com/SophistSolutions/Stroika/archive/V2.1-Release.tar.gz](https://github.com/SophistSolutions/Stroika/archive/V2.1-Release.tar.gz%20)

_or_

wget [https://github.com/SophistSolutions/Stroika/archive/v2.1d1.tar.gz](https://github.com/SophistSolutions/Stroika/archive/v2.1d1.tar.gz)

- tar xf V2.1-Release.tar.gz
- cd Stroika-2.1d1 (or whatever version extracted)
- make help

_Not needed, but gives some idea of make options_

- make check-tools

_Not needed, but tells you if you are missing anything critical_

- make default-configurations

_Not needed, but it&#39;s a springboard for setting up the configuration you want._

-
  - Review/edit ConfigurationFiles/Debug.xml
  - Or try something like
    - configure MyGCC7Config -assertions enable --trace2file enable --compiler-driver &#39;g++-7

_or_

-
  -
    - configure MyCPP17Test --assertions enable --trace2file enable _--cppstd-version-flag &#39;--std=c++17&#39;_

_or_

-
  -
    - configure --help
- ls ConfigurationFiles/ or make list-configurations
  - See what configurations you&#39;ve created. Edit files, add or delete.
- make list-configuration-tags
- make list- configurations TAG=&quot;Windows 32&quot;
- make all CONFIGURATION=_a-config_
  - make all targets for the configuration named _a-config_
- make all TAG=Unix
  - make all the configurations with tag UNIX
- make all

Builds everything for ALL configurations (in the folder ConfigurationFiles/)

- make run-tests

_Runs regression tests (optionally on remote machines, or with VALGRIND)_

# Required Tools

## Required for ALL platforms

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

## For MacOS

- XCode 10 or later
  - install from appstore,
  - Then from command line
    - xcode-select –install
    - Homebrew can be helpful (but use whatever package mgr you wish)
      - ruby -e &quot;$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/master/install](https://raw.githubusercontent.com/Homebrew/install/master/install))&quot;
      - to install apps with brew, use &quot;brew install APPNAME&quot;
    - brew install gnu-sed
    - brew install p7zip  (if building lzma)

## For UNIX

- Compiler
  - gcc 7 or later OR
    - Stroika v2.1 is currently tested with gcc7, and gcc8
  - llvm (clang++) 6 or later
    - Stroika v2.1 is currently tested with clang6, and clang7
- automake  (if building curl)
- libtool (gnu version) – (if building curl)

## For Windows

- Visual Studio.net 2017 (or later)
  - Currently tested with 15.8.6
- Cygwin

Including

-
  - dos2unix
  - unix2dos

## Third Party Components

These components automatically downloaded and built and integrated into Stroika (depending on configuration) or Stroika can be configured to use the system installed version of these components.

- boost
- curl
- openssl
- lzma SDK
- sqlite
- xerces
- zlib

# Build / Configuration Design

## Alternatives

We seriously considered a number of build systems, including cmake, ant, perl scripts, qmake, etc. They all weak, with ant possibly being the best alternative, except that it&#39;s heavily java oriented.

Just normal GNU make – appears to be the least bad alternative, so that&#39;s what we&#39;re doing.

## Configurations

The design is to make a set of configuration files – each stored in the ConfigurationFiles/ directory. Configurations are described by a single XML file. They can be generated using the configure script.

 configure --help

for more information.

A configuration comprises BOTH target platform (e.g. windows/linux), hardware (e.g. PowerPC, x86, x64, ARM), and any other options (debug build, enable assertions, support OpenSSL, etc).

This stands in contrast to several other systems (like Visual Studio.net) that treats platform and &#39;configuration&#39; as orthogonal choices.

Configuration files can be edited by hand.

@todo A FUTURE version of Stroika will have an XML Schema (XSD) to describe the configuration file format.

You can call

 make apply-configurations

to generate all the directories and files dependent on the defined configurations. Note – this is generally not necessary, and called automatically.

## Build Results

Intermediate files (objs etc) go into **IntermediateFiles/{CONFIGURATION-NAME}**. Final build products (libraries and executables) go into **Builds/{CONFIGURATION-NAME}**.



# Build Process

On any platform, building Stroika, and all is demo applications and regression tests is as simple as cd&#39;ing to the top-level directory, and typing make

## Special Targets

- make

Make with no arguments runs &#39;make help&#39;

- make help

Prints the names and details of the special targets

- make all

builds the stroika library, tests, demos, etc.

- make libraries

Builds just the Stroika libraries

- make samples

Builds the Stroika sample applications

- make run-tests

Builds Stroika, and all the regression tests, and runs the regression tests

- make project-files

Builds project files which can be used for things like visual studio (not needed)

- make check-tools

Checks if the tools needed to build Stroika are installed and in your path. This is done automatically, and generally not needed explicitly.

## Configuration arguments to make

All the make targets (e.g. all, libraries etc) take an OPTIONAL parameter CONFIGURATION. If specified, only that configuration is built. If omitted (or empty) – ALL configurations are built.

## Using Visual Studio.net

Visual Studio.net project and solution files are available for the Stroika demos, top-level project files, and regression tests. Once you have built your configuration files (see above), you can use the project files to build, test, extend and develop Stroika.

## Using QtCreator (on unix)

Run Library/Projects/QtCreator/CreateQtCreatorSymbolicLinks.sh to create project files at the top level of your Stroika directory. Then you can open that .creator file in qtCreator, and build and debug Stroika-based applications.

# Building For…

## Building for Raspberry Pi

To cross-compile for Raspberry pi,

- install some suitable cross compiler (in this example arm-linux-gnueabihf-g++-5)

On unubtu, sudo apt-get install g++-5-arm-linux-gnueabihf

- configure raspberrypi-gcc-7 --apply-default-debug-flags --trace2file enable --compiler-driver arm-linux-gnueabihf-g++-7 --cross-compiling true

Set cross-compiling true so that internal tests aren&#39;t run using the arm built executables.

Set –apply-default-release-flags instead of &#39;debug&#39; for a smaller faster executable.

--trace2file disable to disable tracefile utility, and enabled writes a debug log to /tmp.

- make CONFIGURATION=raspberrypi-gcc-7 all

This builds the samples, libraries etc to Builds/raspberrypi-gcc-7)

- make run-tests REMOTE=pi@myRasberryPi

This uses ssh to run the tests remotely on the argument machine (I setup a hostname in /etc/hosts for the mapping).

Using SSH, it&#39;s also helpful to setup ssh keys to avoid re-entering passwords

 [http://sshkeychain.sourceforge.net/mirrors/SSH-with-Keys-HOWTO/SSH-with-Keys-HOWTO-4.html](http://sshkeychain.sourceforge.net/mirrors/SSH-with-Keys-HOWTO/SSH-with-Keys-HOWTO-4.html)

# Common Errors

## Tar f **a** ilure

Errors about invalid parameters, and/or bad blocks can usually be fixed by installing a copy of gnu tar. We&#39;ve tested 1.27.

## cp: illegal option –

Install a copy of GNU cp

## Cannot find &#39;blah&#39; in Cygwin

If you are trying to install required components in Cygwin, and cannot find them in the Cygwin setup GUI, try:

cygcheck -p dos2unix