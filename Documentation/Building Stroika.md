# Building Stroika

## Common
---------

Stroika is a C++ class library. The only fully supported build environment for Stroika is GNU Make. Once you have that setup, you can build through your favorite IDE.

## Quick Start
---------

    wget https://github.com/SophistSolutions/Stroika/archive/V2.1-Release.tar.gz
    tar xf V2.1-Release.tar.gz && mv Stroika-2.1-Release Stroika-Dev

or

    git clone https://github.com/SophistSolutions/Stroika.git Stroika-Dev
then
  
    make --directory Stroika-Dev all run-tests

  If you have a relatively standard POSIX like c++ build environement, you maybe done at this point. If you got errors, or want to know more, read on.

## More Details on Getting Started
----------------------------------

### Install required tools

 This mostly conists of GNU make, and perl (see details below depending on your platform).

#### Required for ALL platforms
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

#### For MacOS
- XCode 10 or later
  - install from appstore,
  - Then from command line
    - xcode-select –install
    - Homebrew can be helpful (but use whatever package mgr you wish)
      - ruby -e &quot;$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/master/install](https://raw.githubusercontent.com/Homebrew/install/master/install))&quot;
      - to install apps with brew, use &quot;brew install APPNAME&quot;
    - brew install gnu-sed
    - brew install p7zip  (if building lzma)

#### For Windows
- Visual Studio.net 2017 (or later)
  - Currently tested with 15.8.6
- Cygwin
   
   Including
  - dos2unix
  - unix2dos

#### For UNIX
- Compiler
  - gcc 7 or later OR
    - Stroika v2.1 is currently tested with gcc7, and gcc8
  - llvm (clang++) 6 or later
    - Stroika v2.1 is currently tested with clang6, and clang7
- automake  (if building curl)
- libtool (gnu version) – (if building curl)



### Things to try/explore
- `make help`
   
   Not needed, but gives some idea of make options.

- `make check-prerequisite-tools`

    Not needed, but tells you if you are missing anything critical.

- `make default-configurations`

   Not needed, but it&#39;s a springboard for setting up the configuration you want.

   Review ConfigurationFiles/Debug.xml or any of the other default configuration files

## The **configure** script
---------

 - Examples

   - `./configure Debug-U-32 --config-tag Windows --config-tag 32 --arch x86 --apply-default-debug-flags`
   - `./configure Debug --config-tag Unix --apply-default-debug-flags`
   - `./configure clang++-6-release-libstdc++ --config-tag Unix --compiler-driver clang++-6.0 --apply-default-release-flags --stdlib libstdc++ --trace2file enable`
   - `CXX=clang++ ./configure Debug-clang --config-tag Unix --apply-default-debug-flags`


### Configuration File Format

  Simple XML format (@todo provide XSD).
  The command-line used to generate the configuration is the first element of the XML file, so its easy to regenerate the configuration with whatever slight variation you wish.

### Configuration Basic Concepts

- Each configuraiton has a name (e.g. Debug, Release-clang-7, Debug-raspberry-pi, Release-Centos-6, etc)
- Each configuration can have multiple 'tags' - like Unix, Windows, x86, arm, etc - which can be used to build sets of configurations
- each configuration defines CFLAGS and CXXFLAGS and (explain others) variables used in a regular makefile. You define (on the command line) variables (like 'assertions') which are used to generate to generate a bunch of other variables which appear in configuration files.

### Sample default build rules (just to provide context for the defined configuration variables)

### Command-line reference
~~~~
  configure CONFIGURATION-NAME [OPTIONS]* where options can be:
            --arch {ARCH}                                   /* for high level architecture - first section of gcc -machine - e.g. x86, x86_64, arm - usually auto-detected */
            [--config-tag {TAG-NAME}]*                      /* Add TAG-NAME to the list of tags associated with this configuration (for now limit one). Maybe repeated */
            --platform {PLATFORM}                           /* Specifies the ProjectPlatformSubdir (Unix, VisualStudio.Net-2017, VisualStudio.Net-2019) - usually auto-detected */
            --assertions { enable|disable|default }         /* Enables/disable assertion feature (setting qDebug) */
            --block-allocation { enable|disable|default }   /* Enables/disable block-allocation (a feature that improves performance, but messes up valgrind) */
            --valgrind { enable|disable|default }           /* Enables/disable valgrind-specific runtime code (so far only needed for clean helgrind use) */
            --GLIBCXX_DEBUG { enable|disable|default }      /* Enables/Disables GLIBCXX_DEBUG (G++-specific) */
            --cppstd-version-flag {FLAG}                    /* DEPRECATED - use -cppstd-version (without --std part */
            --cppstd-version {FLAG}                         /* Sets can be c++17, or c++2a
            --stdlib {LIB}                                  /* libc++ (clang lib), libstdc++ (gcc and often clang)
            --ActivePerl {use|no}                           /* Enables/disables use of ActivePerl (Windows Only) - JUST USED TO BUILD OPENSSL for Windows*/
            --LibCurl {build-only|use|use-system|no}        /* Enables/disables use of LibCurl for this configuration [default TBD]*/
            --boost {build-only|use|use-system|no}          /* Enables/disables use of boost for this configuration [default use] */
            --OpenSSL {build-only|use|use-system|no}        /* Enables/disables use of OpenSSL for this configuration [default use] */
            --OpenSSL-ExtraArgs { purify? }                 /* Optionally configure extra OpenSSL features (see Stroika/OpenSSL makefile) */
            --WinHTTP {use-system|no}                       /* Enables/disables use of WinHTTP for this configuration [default use-system on windows, and no otherwise] */
            --ATLMFC {use-system|no}                        /* Enables/disables use of ATLMFC for this configuration [default use-system on windows, and no otherwise] */
            --Xerces {build-only|use|use-system|no}         /* Enables/disables use of Xerces for this configuration [default use] */
            --sqlite {build-only|use|use-system|no}         /* Enables/disables use of sqlite for this configuration [default use] */
            --ZLib {build-only|use|use-system|no}           /* Enables/disables use of ZLib for this configuration [default use] */
            --WIX {use|use-system|no}                       /* Enables/disables use of WIX (Windows Only) - to build windows installers*/
            --lzma {build-only|use|use-system|no}           /* Enables/disables use of LZMA SDK for this configuration [default use] */
            --trace2file { enable|disable|default }         /* Enables/disable trace2file feature */
            --static-link-gccruntime { enable|disable }     /* Enables/disable gcc runtime static link (only applies if gcc family compiler) */
            --cpp-optimize-flag  {FLAG}                     /* Sets $COPTIMIZE_FLAGS (empty str means none, -O2 is typical for optimize) - UNIX ONLY */
            --c-define {ARG}                                /* Define C++ pre-processor define for the given configuration: arg appears as a line in Stroika-Configuration.h
                                                               (e.g. --c-define '\#define qCompilerAndStdLib_quick_exit_Buggy 1')*/
            --make-define {ARG}                             /* Define makefile define for the given configuration: text of arg appears as line in Configuration.mk */
            --compiler-driver {ARG}                         /* default is gcc */
            --ar {ARG}                                      /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ar */
            --as {ARG}                                      /* default is 'as' on unix, and retrieved from visual studio on visual studio */
            --ranlib {ARG}                                  /* default is undefined, but if compiler-driver is gcc or g++, this is gcc-ranlib */
            --strip {ARG}                                   /* sets program to do stripping; default is undefined, but for POSIX, defaults to strip */
            --compiler-warning-args {ARG}                   /* Sets variable with compiler warnings flags */
            --append-compiler-warning-args {ARG}            /* Appends ARG to 'compiler warning flags */
            --append-CFLAGS {ARG}                           /* Appends ARG to CFLAGS */
            --remove-CFLAGS {ARG}                           /* Remove ARG from CFLAGS (including default added args; processed after all adds applied) */
            --replace-all-CFLAGS {ARG}                      /* OVERRIDES DEFAULTS- and sets CFLAGS to just these values */
            --append-CXXFLAGS {ARG}                         /* Appends ARG to CXXFLAGS */
            --remove-CXXFLAGS {ARG}                         /* Remove ARG from CXXFLAGS (including default added args; processed after all adds applied) */
            --replace-all-CXXFLAGS {ARG}                    /* OVERRIDES DEFAULTS- and sets CXXFLAGS to just these values */
            --append-CPPFLAGS {ARG}                         /* alias for append-CFLAGS AND append-CXXFLAGS */
            --extra-linker-args {ARG}                       /* Sets variable with extra args for linker */
            --append-extra-prefix-linker-args {ARG}         /* Appends ARG to 'extra prefix linker args */
            --append-extra-suffix-linker-args {ARG}         /* Appends ARG to 'extra suffix linker args */
            --append-extra-compiler-and-linker-args {ARG}   /* Appends ARG to 'extra compiler' and 'extra linker' args */
            --includes-path {ARG}                           /* Sets INCLUDES_PATH variable (: separated, since unix standard and allows spaces) */
            --append-includes-path {ARG}                    /* Appends ARG to 'INCLUDES_PATH */
            --libs-path {ARG}                               /* Sets LIBS_PATH variable (':' separated, since unix standard and allows spaces) */
            --append-libs-path {ARG}                        /* Appends ARG to 'LIBS_PATH */
            --lib-dependencies {ARG}                        /* Sets LIB_DEPENDENCIES variable (space separated) */
            --append-lib-dependencies {ARG}                 /* Appends ARG to 'LIB_DEPENDENCIES */
            --run-prefix {ARG}                              /* Sets variable RUN_PREFIX with stuff injected before run for built executables,
                                                               such as LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.3 */
            --append-run-prefix {ARG}                       /* Appends ARG to 'extra linker */
            --pg {ARG}                                      /* Turn on -pg option (profile for UNIX/gcc platform) on linker/compiler */
            --lto { enable|disable }                        /* Turn on link time code gen on linker/compiler (for now only gcc/unix stack) */
            --cross-compiling {true|false}                  /* Defaults generally to false, but set explicitly to control if certain tests will be run */
            --apply-default-debug-flags                     /*  */
            --apply-default-release-flags                   /*  */
            --only-if-has-compiler                          /* Only generate this configuration if the compiler appears to exist (test run)*/
            --debug-symbols {true|false}                    /* --debug-symbols-lib AND --debug-symbols-exe at the same time */
            --debug-symbols-lib {true|false}                /* defaults to true, but can be disabled if makes compile/link/etc too big/slow */
            --debug-symbols-exe {true|false}                /* defaults to true, but can be disabled if makes compile/link/etc too big/slow */
            --malloc-guard {true|false}                     /* defaults to false (for now experimental and only works with GCC) */
            --runtime-stack-check {true|false}              /* gcc -fstack-protector-all */
            --sanitize {none|thread|address|undefined|leak} /* if arg none, reset to none, else adds arg to sanitized feature (gcc/clang only) -
                                                               any arg you can pass to -fsanitize=XXXX */
                                                            /* see https://gcc.gnu.org/onlinedocs/gcc-6.1.0/gcc.pdf (search -fsanitize=; eg. --sanitize address,undefined */
            --no-sanitize {thread|vptr|etc...}              /* any from --sanitize or all */

Configure's behavior is also influenced by the following environment variables:
            CC, CXX, PLATFORM, ARCH, AS, AR, RANLIB, STRIP; these just simpulate adding the obvoius associated argument to configure
~~~~

### Environment variables that affect generation of configuration

- CC, CXX, PLATFORM, ARCH, AS, AR, RANLIB, STRIP

The reason this is so important, is that it allows an external build system like bitbake, or node-gyp, etc to define parameters for a build, and easily generate appropriate configurations.

### Printing configure variables from a script

#### Example
 
 - `./ScriptsLib/GetConfigurationParameter Debug-U-32 CFLAGS`

 - `./ScriptsLib/GetConfigurationParameter Debug-U-32 Linker`




## @todo organize this next section
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

We seriously considered a number of build systems, including cmake, ant, perl scripts, qmake, etc. They all had substantial weaknesses, with ant possibly being the best alternative, except that it&#39;s heavily java oriented. Maybe for c++ cmake would have been the best?

But just plain GNU make – appears to be the least bad alternative, so that&#39;s what we&#39;re doing.

But - even with just plain make, you need some sort of configure script to establish what compiler options will be defined. Again, lots of different alternatives here, but in the end I decided to just build custom scripts which build a very simple XML configuration declaration, and which drives the make process by #included 'config' makefile.

## Configurations

The design is to make a set of configuration files – each stored in the `ConfigurationFiles/` directory. Configurations are described by a single XML file. They can be generated using the configure script.

`configure --help`

for more information.

A configuration comprises BOTH target platform (e.g. windows/linux), hardware (e.g. PowerPC, x86, x64, ARM), and any other options (debug build, enable assertions, support OpenSSL, etc).

This stands in contrast to several other systems (like Visual Studio.net) that treats platform and &#39;configuration&#39; as orthogonal choices.

### Amending a configuration
Configuration files should **not** be edited by hand. Instead, the current command line is the first element of the configuration file: take that as a starting point and ammend it as needed, and re-run.

 - @todo A FUTURE version of Stroika will have an XML Schema (XSD) to describe the configuration file format.

You can call

 `make apply-configurations`

to generate all the directories and files dependent on the defined configurations. Note – this is generally not necessary, and called automatically.

## Build Results

Intermediate files (objs etc) go into **IntermediateFiles/{CONFIGURATION-NAME}**. Final build products (libraries and executables) go into **Builds/{CONFIGURATION-NAME}**.



# Build Process

On any platform, building Stroika, and all is demo applications and regression tests is as simple as cd&#39;ing to the top-level directory, and typing make

## Special Targets

- `make`

Make with no arguments runs &#39;make help&#39;

- `make help`

Prints the names and details of the special targets

- `make all`

builds the stroika library, tests, demos, etc.

- `make libraries`

Builds just the Stroika libraries

- `make samples`

Builds the Stroika sample applications

- make `run-tests`

Builds Stroika, and all the regression tests, and runs the regression tests

- make `project-files`

Builds project files which can be used for things like visual studio (not needed)

- make check-tools

Checks if the tools needed to build Stroika are installed and in your path. This is done automatically, and generally not needed explicitly.

## Configuration arguments to make

All the make targets (e.g. all, libraries etc) take an OPTIONAL parameter CONFIGURATION. If specified, only that configuration is built. If omitted (or empty) – ALL configurations are built.

## Using Visual Studio.net

Visual Studio.net project and solution files are available for the Stroika demos, top-level project files, and regression tests. Once you have built your configuration files (see above), you can use the project files to build, test, extend and develop Stroika.

## Using QtCreator (on unix)

Run Library/Projects/QtCreator/CreateQtCreatorSymbolicLinks.sh to create project files at the top level of your Stroika directory. Then you can open that .creator file in qtCreator, and build and debug Stroika-based applications.

### Cross-Compiling

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

## Common Errors
----------------

## Tar failure

Errors about invalid parameters, and/or bad blocks can usually be fixed by installing a copy of gnu tar. We&#39;ve tested 1.27.

## cp: illegal option –

Install a copy of GNU cp

## Cannot find &#39;blah&#39; in Cygwin

If you are trying to install required components in Cygwin, and cannot find them in the Cygwin setup GUI, try:

cygcheck -p dos2unix