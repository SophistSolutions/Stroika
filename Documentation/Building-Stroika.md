# Building Stroika {#Building-Stroika}

---

## Common

Stroika is a C++ class library. The only fully supported build environment for Stroika is GNU Make. Once you have that setup, you can build through your favorite IDE. There are other build tools required, but make auto-detects these and warns what is missing.

This build process is cross-platform. It supports cross-compiling, and builds on MacOS (XCode),  Linux (via gcc, or clang) and Windows (visual studio.net). Stroika also directly supports the IDEs Visual Studio.Net, and Visual Studio code.

---

## <a name="Quick-Start">Quick Start</a>

If you have a POSIX build environment (else see below, docker environments or installing development tools):

```bash
git clone https://github.com/SophistSolutions/Stroika.git Stroika-Dev
```

or

```bash
wget https://github.com/SophistSolutions/Stroika/archive/v3-Release.tar.gz
tar xf v3-Release.tar.gz && mv Stroika-3-Release Stroika-Dev
```

followed by:

```bash
cd Stroika-Dev
Build/Scripts/MakeBuildRoot
make all run-tests
```

MakeBuildRoot, run once with no arguments in the top of the tree, makes the build root there - IntermediateFiles/, with
the products in Builds/ (to build somewhere else: [Relocating build output](#relocating-build-output)). make stops
without one, printing the line that makes it.

Or, for a depending on your system, for a quicker turnaround, perhaps:
```bash
make all run-tests CONFIGURATION=Debug -j10
```


If you have a relatively standard POSIX like c++ build environement, you maybe done at this point. If you got errors, or want to know more, read on.

### Build with IDE

Run `make project-files` first (`make all`, as suggested above, does it too): it makes the default configurations, if there are none, and writes each one's IDE settings - without them an IDE lists no configurations. Visual Studio then opens Stroika's top directory (File > Open > Folder) and builds it with CMake, from each configuration's preset - see [Using Visual Studio](#using-visual-studio). VS Code has a workspace, [Workspaces/VSCode/](../Workspaces/VSCode/).

### Build with Docker

If you are missing any components and just want a quick environment to test that has all the right build components installed, you can use the pre-built docker containers:

UNIX:

```bash
docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2604-small
cat Getting-Started-With-Stroika.md
```

Windows:

```bash
docker run -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k26
cat Getting-Started-With-Stroika.md
```

Or - a couple other docker run variants that may help with windows
```bash
docker run --mount type=bind,source=c:/Sandbox,target=c:/Sandbox -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k26
docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k26
```


### **_Note_**

It takes a while to build all of Stroika (10-20 minutes per configuration), so adding -j10 (or so) helps a lot.

---

## More Details on Getting Started

### Install required tools

This mostly conists of the standard UNIX build tools.

Roughly:
- c++ compiler supporting C++20 or later - see
  [Supported Platforms and Compilers](SupportedPlatformsAndCompilers.md)
- bash (sh)
- make (gnu make), patch, pkg-config, realpath, sed, tar, tr, wget, unzip
- python3, as `/usr/bin/python3` (on Windows, MSYS2's or Cygwin's own - not a native Windows one)
- 7za, cmake (optionally - for building some optional components)
- perl, to build OpenSSL - but not on Windows, where Stroika fetches Strawberry Perl for it


You may find it helpful to review the instructions in [Installing-Required-Tools.md](./Installing-Required-Tools.md) which layout details per development platform.

### Things to try (explore)

First make the build root, if you have not - every make needs one:

- `Build/Scripts/MakeBuildRoot`

  `Build/Scripts/MakeBuildRoot --help` lists its options; [Relocating build output](#relocating-build-output) says more.

- `make help`

  Not needed, but gives some idea of make options.

- `make check-prerequisite-tools`

  Not needed, but tells you if you are missing anything critical.

- `make default-configurations`

  Not needed, but it&#39;s a springboard for setting up the configuration you want.

  Review IntermediateFiles/ConfigurationFiles/Debug.xml or any of the other default configuration files

---

## The **configure** script

Like many non-trivial C/C++ libraries, you run configure to establish some build parameters, before invoking make. But unlike most such configuration systems, Stroika creates 'named' configurations, and facilitates building multiple such named configurations at once.

Each configuration is stored in a file named \${CONFIGNAME}.xml in the build root's `ConfigurationFiles/` (by default
`IntermediateFiles/ConfigurationFiles/` - see [Relocating build output](#relocating-build-output)).

- Examples of generating configurations
  - `./configure Debug-x86 --config-tag Windows --config-tag x86 --arch x86 --apply-default-debug-flags`
  - `./configure Debug --config-tag Unix --apply-default-debug-flags`
  - `./configure clang++-20-release-libstdc++ --config-tag Unix --compiler-driver clang++20 --apply-default-release-flags --stdlib libstdc++ --trace2file enable`
  - `CXX=clang++ ./configure Debug-clang --config-tag Unix --apply-default-debug-flags`
  - `./configure g++-valgrind-debug-SSLPurify --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --sanitize none;`

### Configuration File Format

Simple XML format (@todo provide XSD).
The command-line used to generate the configuration is the first element of the XML file, so its easy to regenerate the configuration with whatever slight variation you wish.

#### Example Configuration file

```xml
<!--This file autogenerated by the configure command: see ConfigureCommandLine, modify it, and re-run-->

<Configuration>
    <!--Overall-->
    <ConfigureCommandLine>configure Debug-x86_64 --config-tag Windows --config-tag x86_64 --build-by-default Windows --arch x86_64 --apply-default-debug-flags</ConfigureCommandLine>
    <BuildPlatform>VisualStudio.Net-2022</BuildPlatform>
    <BuildToolsRoot>C:/Program Files/Microsoft Visual Studio/2022/Community</BuildToolsRoot>
    <TargetPlatforms>Windows</TargetPlatforms>
    <ARCH>x86_64</ARCH>
    <ConfigTags>Windows x86_64</ConfigTags>
    <BuildByDefault>Windows</BuildByDefault>

    <!--Tools-->
    <AS>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/ml64.exe</AS>
    <CC>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/cl.exe</CC>
    <CXX>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/cl.exe</CXX>
    <CMAKE>C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe</CMAKE>
    <LIBTOOL>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/lib.exe</LIBTOOL>
    <LINKER>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/link.exe</LINKER>
    <MIDL>C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/midl.exe</MIDL>
    <RC>C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/rc.exe</RC>

    <!--Features Enabled-->
    <qFeatureFlag_ATLMFC>use-system</qFeatureFlag_ATLMFC>
    <qFeatureFlag_boost>use</qFeatureFlag_boost>
    <qFeatureFlag_LibCurl>no</qFeatureFlag_LibCurl>
    <qFeatureFlag_fmtlib>no</qFeatureFlag_fmtlib>
    <qFeatureFlag_GoogleTest>use</qFeatureFlag_GoogleTest>
    <qFeatureFlag_LZMA>use</qFeatureFlag_LZMA>
    <qFeatureFlag_mongocxxdriver>use</qFeatureFlag_mongocxxdriver>
    <qFeatureFlag_OpenSSL>use</qFeatureFlag_OpenSSL>
    <qFeatureFlag_sqlite>use</qFeatureFlag_sqlite>
    <qFeatureFlag_StrawberryPerl>use</qFeatureFlag_StrawberryPerl>
    <qFeatureFlag_WinHTTP>use-system</qFeatureFlag_WinHTTP>
    <qFeatureFlag_WIX>use</qFeatureFlag_WIX>
    <qFeatureFlag_Xerces>no</qFeatureFlag_Xerces>
    <qFeatureFlag_libxml2>use</qFeatureFlag_libxml2>
    <qFeatureFlag_ZLib>use</qFeatureFlag_ZLib>
    <qFeatureFlag_zstd>use</qFeatureFlag_zstd>
    <AssertionsEnabled>1</AssertionsEnabled>
    <SanitizerFlags>address </SanitizerFlags>

    <!--MISC-->
    <CrossCompiling>false</CrossCompiling>
    <ExtraMakeDefines>
    </ExtraMakeDefines>
    <Foundation_PkgConfigNames>libxml-2.0 libmongocxx-static openssl lzma sqlite zlib libzstd gtest_main boost</Foundation_PkgConfigNames>

    <!--Platform Settings-->
    <VSVARS_WindowsSdkDir>C:/Program Files (x86)/Windows Kits/10</VSVARS_WindowsSdkDir>
    <VSVARS_WindowsSDKVersion>10.0.26100.0</VSVARS_WindowsSDKVersion>
    <VSVARS_VSINSTALLDIR>C:/Program Files/Microsoft Visual Studio/2022/Community</VSVARS_VSINSTALLDIR>
    <VSVARS_VCToolsInstallDir>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207</VSVARS_VCToolsInstallDir>
    <VSVARS_MSVC_RUNTIME_LIBRARY>MultiThreadedDebug</VSVARS_MSVC_RUNTIME_LIBRARY>
    <TOOLS_PATH_ADDITIONS>C:/Program Files (x86)/HTML Help Workshop;C:/Program Files (x86)/Microsoft SDKs/Windows/v10.0A/bin/NETFX 4.8 Tools/x64/;C:/Program Files (x86)/Microsoft Visual Studio/Shared/Common/VSPerfCollectionTools/vs2019/;C:/Program Files (x86)/Microsoft Visual Studio/Shared/Common/VSPerfCollectionTools/vs2019/x64;C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64;C:/Program Files (x86)/Windows Kits/10/bin/x64;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/FSharp/Tools;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/TestWindow;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/Linux/bin/ConnectionManagerExe;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/VC/VCPackages;C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/Tools/;C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64;C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/bin/Roslyn;C:/Program Files/Microsoft Visual Studio/2022/Community/Team Tools/DiagnosticsHub/Collector;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg;C:/Windows/Microsoft.NET/Framework64/v4.0.30319;C:/tools/msys64/mingw64/bin)</TOOLS_PATH_ADDITIONS>
    <VSVARS_PLATFORM_INCLUDES_PATH>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/ATLMFC/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/ucrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/um;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/shared;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/winrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/cppwinrt;C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um</VSVARS_PLATFORM_INCLUDES_PATH>

    <!--Compiling-->
    <PLATFORM_CPPFLAGS_NOTINCLUDES>-D_UNICODE -DUNICODE -D_WINDOWS -D_DEBUG</PLATFORM_CPPFLAGS_NOTINCLUDES>
    <PLATFORM_INCLUDES_PATH>C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/ATLMFC/include;C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/ucrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/um;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/shared;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/winrt;C:/Program Files (x86)/Windows Kits/10/include/10.0.26100.0/cppwinrt;C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/include/um;C:/Sandbox/Stroika/DevRoot/Builds/Debug-x86_64/include/</PLATFORM_INCLUDES_PATH>
    <PLATFORM_CFLAGS>-EHsc -nologo -GR -Gd -W4 -Zc:inline -FC -bigobj -RTCsu -GS -Oy- -Od -MTd -Z7 -fsanitize=address</PLATFORM_CFLAGS>
    <PLATFORM_CXXFLAGS>-std:c++latest -EHsc -nologo -GR -Gd -W4 -Zc:inline -FC -bigobj -RTCsu -GS -Oy- -Od -MTd -Z7 -fsanitize=address</PLATFORM_CXXFLAGS>
    <FOUNDATION_CPPFLAGS_NOTINCLUDES>-DqStroika_Foundation_Debug_AssertionsChecked=1 -DqStroika_HasComponent_ATLMFC=1 -DqStroika_HasComponent_boost=1 -DqStroika_HasComponent_libcurl=0 -DqStroika_HasComponent_fmtlib=0 -DqStroika_HasComponent_googletest=1 -DqStroika_HasComponent_LZMA=1 -DqStroika_HasComponent_OpenSSL=1 -DqStroika_HasComponent_sqlite=1 -DqStroika_HasComponent_mongocxxdriver=1 -DqStroika_HasComponent_WinHTTP=1 -DqStroika_HasComponent_xerces=0 -DqStroika_HasComponent_libxml2=1 -DqStroika_HasComponent_zlib=1 -DqStroika_HasComponent_zstd=1 -DqStroika_Foundation_Debug_TraceToFile=1 -DqStroika_Foundation_Debug_DefaultTracingOn=1</FOUNDATION_CPPFLAGS_NOTINCLUDES>
    <FOUNDATION_INCLUDES_PATH>C:/Sandbox/Stroika/DevRoot/Library/Sources/</FOUNDATION_INCLUDES_PATH>

    <!--Linking-->
    <IncludeDebugSymbolsInLibraries>1</IncludeDebugSymbolsInLibraries>
    <IncludeDebugSymbolsInExecutables>1</IncludeDebugSymbolsInExecutables>
    <Platform_LinkerArgs_LibPath>-LIBPATH:"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/ATLMFC/lib/x64" -LIBPATH:"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/lib/x64" -LIBPATH:"C:/Program Files (x86)/Windows Kits/NETFXSDK/4.8/lib/um/x64" -LIBPATH:"C:/Program Files (x86)/Windows Kits/10/lib/10.0.26100.0/ucrt/x64" -LIBPATH:"C:/Program Files (x86)/Windows Kits/10/lib/10.0.26100.0/um/x64" </Platform_LinkerArgs_LibPath>
    <Platform_LinkerArgs_LibDependencies>urlmon.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib </Platform_LinkerArgs_LibDependencies>
    <Platform_LinkerArgs_ExtraPrefix>-DEBUG:FULL -noexp -noimplib -nologo -MACHINE:x64</Platform_LinkerArgs_ExtraPrefix>
    <Platform_LinkerArgs_ExtraSuffix></Platform_LinkerArgs_ExtraSuffix>
    <LinkTime_CopyFilesToEXEDir>"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/clang_rt.asan_dynamic-x86_64.dll" "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/HostX64/x64/clang_rt.asan_dynamic-x86_64.pdb"</LinkTime_CopyFilesToEXEDir>

    <!--Runtime/Testing-->
    <RunPrefix></RunPrefix>

    <!--Compiler/Tool Bug Workarounds/Defines-->

    <!--VERSIONS-->
        <!--MSVC++ version: 194435222-->
</Configuration>
```

### Configuration Basic Concepts

- Each configuration has a name (e.g. Debug, Release-clang-7, Debug-raspberry-pi, etc)
- Each configuration can have multiple 'tags' - like Unix, Windows, x86, arm, etc - which can be used to build sets of configurations
- Each configuration defines CFLAGS and CXXFLAGS and (explain others) variables used in a regular makefile. You define (on the command line) variables (like 'assertions') which are used to generate to generate a bunch of other variables which appear in configuration files.

### Configure Command-line reference

```text
Help for making Stroika:
Targets:
    all:                         -    Builds everything
    check:                       -    Checks everything was built properly
    clean:
    clobber:
    clobber-stroika:                       -    clean + destroy Stroika targets (libs etc) - so all of stroika will be rebuilt next make (but doesnt destroy third-party-components)
    distclean:
    reconfigure:                 -    Rebuild configuration files from the command-lines that built them before
    libraries:                   -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-components)
    library-clobber:             -    Clobber library and things that depend on it (i.e. all but configurations and third-party-components)
    project-files:               -    Writes each configuration's IDE settings - its CMake preset (Visual Studio's Open Folder
                                      reads it) and VS Code's - making the default configurations first, if there are none
    project-files-vs-code:       -    Builds project files for visual studio code
    tests:
    format-code:                 -    Run Scripts/FormatCode (clang-format) on source code, and update it to conform to Stroika code formatting standards
    samples:
    documentation:
    third-party-components:
    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost;
                                      [VALGRIND=memcheck, etc] to run with valgrind
                                      (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)
                                      EG: VALGRIND_SUPPRESSIONS="Valgrind-MemCheck-Common.supp" make VALGRIND=memcheck run-tests
    skel-test:                   -    Builds the libraries, then a new Skel application on them - as a package manager's Stroika - and runs it
    apply-configurations:        -    Force re-creation implied files / links for any configurations in the Configurations
                                      folder (not needed, automatic)
    default-configurations:      -    Creates the default configurations in Configurations folder; see ./configure -help for environment variables
                                      e.g. EXTRA_CONFIGURE_ARGS='--openssl-extraargs purify --block-allocation disable' make default-configurations
                                      OR EXTRA_CONFIGURE_ARGS='--build-platform VisualStudio.Net-2022' make default-configurations
                                      OR PLATFORM='VisualStudio.Net-2022' make default-configurations
    list-configurations:         -    prints all available configurations (each can be used as arg to CONFIGURATION= make lines)
    list-configuration-tags:     -    prints a list of all configurtion tags (configuration tags impute groups of configurations)
    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed.
Special Variables:               -    Extra params you can pass to the make line that may help...
    CONFIGURATION=XYZ            -    Causes only the configuration XYZ to have the targetted goal applied (e.g. make all CONFIGURATION=Debug)
    CONFIGURATION_TAGS=TAGS      -    Causes only the configurations with tags TAGS to have the targetted goal applied (e.g. make all CONFIGURATION_TAGS=Windows)
                                      NB: TAGS is a handy shortcut for CONFIGURAITON_TAGS, so you can say make all TAGS="Windows 64"
    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging
    WRITE_PREPROCESSOR_OUTPUT=1  -    Write next to each object file a corresponding .i (preprocessor output) file; useful to reproduce and narrow compiler bugs (for report especially)
    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make
    QUICK_BUILD=1                -    Defaults=1, but if =1, skip some optional build steps (like openssl tests, CURRENT folders; may skip detecting changes in libraries etc; used for some CI testing)
    TEST_FAILURES_CAUSE_FAILED_MAKE=0
                                      only applies to make run-tests, and prevents test failures from stopping make (like make -k on run-tests)
    EXTRA_CONFIGURE_ARGS=        -    really proceeded by configure (only when its invoked); but handy to override values (eg --all-available-third-party-components --boost no)
```

### Amending a configuration

Configuration files should **not** be edited by hand. Instead, the current command line is the first element of the configuration file: take that as a starting point and amend it as needed, and re-run **make apply-configurations** or **make CONFIGURATION=X apply-configuration**.

### Environment variables that affect generation of configuration

- CC, CXX, PLATFORM, ARCH, AS, AR, RANLIB, STRIP

The reason this is so important, is that it allows an external build system like bitbake, or node-gyp, etc to define parameters for a build, and easily generate appropriate configurations.

### Printing configure variables from a script

`Build/Scripts/GetConfigurationParameter CONFIGURATION PARAMETER` prints one element of `ConfigurationFiles/CONFIGURATION.xml`:
PARAMETER is the element's name, as that file shows it.

#### Examples

- `./Build/Scripts/GetConfigurationParameter Debug ARCH`

  x86_64

- `./Build/Scripts/GetConfigurationParameter Debug LINKER`

  g++

- `./Build/Scripts/GetConfigurationParameter Release PLATFORM_CXXFLAGS`

  --std=c++20 -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-psabi -O3 -g -flto

- defining sample configurations
  ~~~bash
  make default-configuration

  make default-configurations DEFAULT_CONFIGURATION_ARGS='--openssl use --OpenSSL-ExtraArgs purify --block-allocation disable --valgrind enable'

  ./configure DEFAULT_CONFIG

  ./configure gcc-with-malloc-guard --malloc-guard true

  ./configure DefaultConfiguration --apply-default-debug-flag
  ./configure DefaultConfiguration --trace2file enable
  ./configure DefaultConfiguration --assertions enable --trace2file enable

  # Profiler
  ./configure DefaultConfiguration --assertions disable --trace2file disable --cpp-optimize-flag -O3 --pg --lto enable

  # Sanitizer explicit
  ./configure g++-debug-sanitize_address --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,address,undefined --trace2file enable

  ### For Windows to use particular version of Windows SDK
  configure DefaultConfiguration --c-define '\#define WINVER 0x600' --c-define '\#define _WIN32_WINNT 0x600' --c-define '\#define _WIN32_WINDOWS 0x600'
  ~~~


---

## .pc files (pkgconfig)

Stroika's build process leverages .pc (pkgconfig) files. It uses (requires) certain libraries via pkgconfig
and it provides pkgconfig files for the foundation and frameworks libraries.

 - Builds/${CONFIGURATION}/lib/pkgconfig/stroika-platform.pc
   
   Settings for std c++ library, sdk (e.g. windows), etc. This is used in building third-party-components to assure they reference a consistent version of std c++ etc libs
 - Builds/${CONFIGURATION}/lib/pkgconfig/stroika-foundation.pc
   
   Stroika-specific pkg-config file the Stroika Foundation library. This contains defines controlling features Stroika was 
     built with, and requires for the libraries Stroika was configured to depend upon
 - Builds/${CONFIGURATION}/lib/pkgconfig/stroika-frameworks.pc
   
   Stroika-specific pkg-config file the Stroika Frameworks (UPNP, Test, WebServices, etc) library. This contains little except a requires of the stroika-foundation librar.


---

## Static Linking vs. Dynamic Libraries

Stroika itself is only provided as a static library. This is because static libraries are much simpler, better for optimizing, and more flexible about configuration of the particular build options desired.

I cannot rule out ever providing a dynamic link option/feature, but I see exceedingly little point to it.

Of course, you can still use dynamic (shared library) linking however you wish for any components you build (like you can build shared libraries with Stroika), and obviously for most operatiing systems, the libraries your apps must like to are shared libraries.

But this bias towards static linking is reflected in all the defaults and samples provided with Stroika.

---

## Third Party Components

Stroika builds on, and neatly integrates functionality from several third-party compoenents. These components are automatically downloaded and built and integrated into Stroika (depending on configuration) or Stroika can be configured to use the system installed version of these components.

- StrawberryPerl (special case, just a hack to be able to build openssl)
- boost
- curl
- openssl
- lzma SDK
- sqlite
- WIX
- xerces
- zlib

---

## Build Results

Intermediate files (objs etc) go into

- **IntermediateFiles/{CONFIGURATION-NAME}**.

Final build products (libraries and executables) go into

- **Builds/{CONFIGURATION-NAME}**.

and the configurations themselves into **IntermediateFiles/ConfigurationFiles/{CONFIGURATION-NAME}.xml** - all in the
Stroika tree, unless you choose another build root.

### Relocating build output

Usually you run `Build/Scripts/MakeBuildRoot` once, with no arguments, in the top directory of what you build - Stroika's
tree, or an application's - and it makes **IntermediateFiles/** there, the build root, with the products in **Builds/**,
also there. `make` (but `make help`) and `configure` stop without a build root, printing the line that makes it.

The build root is what CMake's `-B` names: the directory that says where everything else is. It holds the intermediate
files, the configurations (**ConfigurationFiles/**), and **DirectoryLayout.mk**, which says where the products go.

To build somewhere else - on a faster disk, off a shared or network path, or to keep the checkout clean - name all three
directories: `--top-root` (the project to build: the directory you would otherwise run it in), `--intermediate-files-root`
(the build root) and `--builds-root` (where the products go). Nothing in between is accepted, and MakeBuildRoot reads no
environment variables. The paths may be relative (to the current directory); DirectoryLayout.mk records them absolute.
Then build there:

~~~bash
Build/Scripts/MakeBuildRoot --top-root . --intermediate-files-root /fastdisk/stroika-out/IntermediateFiles/ --builds-root /fastdisk/stroika-out/Builds/
cd /fastdisk/stroika-out/IntermediateFiles/
make default-configurations
make CONFIGURATION=Debug all -j8
~~~

Instead of building in the build root, you can set **StroikaIntermediateFilesRoot** to it - in the environment, or on the
make command line; `configure` and the other build scripts, run by hand, follow it too.

The makefiles' variables for these directories - StroikaIntermediateFilesRoot, StroikaConfigurationFilesDir and
StroikaBuildsRoot - are documented, with the other directories a build uses, at the top of
`Build/Lib/Make/SharedMakeVariables-Default.mk`.

- `make clobber` empties the build root, but keeps its configurations and DirectoryLayout.mk; `make distclean` deletes
  it all - run MakeBuildRoot again to build after it.
- Give each source tree its own build root: make stops when a build root made for another tree is used.
- On Windows, DirectoryLayout.mk writes the paths `C:/...`, which MSYS2, Cygwin and the native tools all read. Keep them
  short: boost's build puts one `-I` per sublibrary on each command line, so a long path can exceed cmd.exe's 8191
  characters - and a configuration's name is part of that path; make's own build (Makefile-Legacy) warns when its build
  directory's Windows path passes 95 characters, the CMake build does not
  ([issue 769](https://github.com/SophistSolutions/Stroika/issues/769)).
- Still written in the source tree: the third-party components' downloaded archives (`ThirdPartyComponents/Origs-Cache`),
  the configurations' CMake presets (`CMakeUserPresets.json`), and the IDE files (`.vscode/`, `Workspaces/`), which also
  still assume the default directories - and, by make's own build (Makefile-Legacy), the components' extracted sources
  (`ThirdPartyComponents/*/CURRENT`).
- An application made by `Skel` has its build root in its own top directory by default - IntermediateFiles/, with the
  products in Builds/ - shared by its output and Stroika's. In that directory:
  `ThirdPartyComponents/Stroika/StroikaRoot/Build/Scripts/MakeBuildRoot`
- Before Stroika v3.0d26 the configurations were in ConfigurationFiles/ at the top of the tree: MakeBuildRoot copies any
  there into the build root it makes, and `make distclean` deletes that old directory too.

Before Stroika v3.0d26, `Build/Scripts/MakeBuildRoot <ROOT>` relocated the output by replacing those three directories
with directory symbolic links into ROOT. On Windows such a link cannot be made to work dependably: a
symbolic link created without `SeCreateSymbolicLinkPrivilege` is marked UNTRUSTED, and Redirection Guard then stops native
tools - MSVC's `cl.exe` and `link.exe` among them - from traversing it (`STATUS_UNTRUSTED_MOUNT_POINT`); a directory cannot
be hard linked, and a junction is blocked by the same mitigation. See
[issue 1075](https://github.com/SophistSolutions/Stroika/issues/1075) and
[issue 1170](https://github.com/SophistSolutions/Stroika/issues/1170).

---

## The Build folder

**Build/** holds everything the build system itself needs - as distinct from the Stroika library
source (**Library/**), the tools built with it (**Tools/**), the regression tests (**Tests/**), and
the example applications (**Samples/**).

Its subfolders divide by **role** - *how* a file is consumed - rather than by topic. That is
deliberate: the most useful thing to know about a build file is whether something executes it,
includes it, or reads it as data.

| Folder | Role |
| :-- | :-- |
| [Build/Scripts/](../Build/Scripts/) | Standalone programs, invoked directly by the Makefiles and by `configure` |
| [Build/Lib/Make/](../Build/Lib/Make/) | `.mk` fragments, `include`d by other makefiles |
| [Build/Lib/Python/](../Build/Lib/Python/) | `.py` modules, `import`ed by the Python scripts |
| [Build/Shared/](../Build/Shared/) | Data read at build time, rather than code |
| [Build/Tools/Src/](../Build/Tools/Src/) | Source for host utilities needed *before* Stroika can be built |
| [Build/Docker/](../Build/Docker/) | Build-VM and dev container definitions |

Note that these paths are effectively public, not merely internal to Stroika: an application built
against Stroika `include`s the makefile fragments from **Build/Lib/Make/** in its own makefiles, and
if it was generated by `Skel` it began as a copy of the templates in **Build/Shared/**. Renaming
things here is a breaking change for applications built against Stroika.

### Build/Scripts

Programs run as commands - `$(StroikaSourceRoot)Build/Scripts/PrintProgressLine ...`, not
`bash PrintProgressLine`. Consequently **they must keep their executable bit**; losing it breaks the
build on UNIX while still appearing to work on Windows, since MSYS/Cygwin synthesize the mode.

Most are small build-plumbing helpers (`GetConfigurationParameter`, `DetectedHostOS`,
`PrintProgressLine`, ...) - see [Printing configure variables from a script](#printing-configure-variables-from-a-script).
A few are meant to be run by hand:

- `Skel` - scaffold a new Stroika-based application; see [Building your own application](#building-your-own-application)
- `FormatCode` - what `make format-code` runs (clang-format)
- `RegressionTests`, `RunLocalWSLRegressionTests`, `RunRemoteRegressionTests` - test drivers
- `RunInDockerEnvironment` - start a dev container with useful options preset
- `MakeBuildRoot` - make a build root out of the source tree; see [Relocating build output](#relocating-build-output)

Two that callers pick between deliberately: **`MakeFileLink`** and **`MakeDirectoryLink`** create an
alias for a file / for a directory. They are separate because the two cases need different
mechanisms and, on Windows, have different answers to whether they can work at all - and because
Windows stamps file-vs-directory into a symbolic link at creation time, so when the target does not
exist yet only the caller knows which was meant. The older `MakeSymbolicLink` still works, warns,
and forwards.

### Build/Lib

Libraries pulled *into* other build files, and never executed - which is why, unlike **Build/Scripts/**,
nothing here is marked executable.

**Build/Lib/Make/** holds `SharedMakeVariables-Default.mk` (variables) and
`SharedBuildRules-Default.mk` (rules). **Include them in that order** - the rules file needs variables
the other defines, and warns if you get it backwards. `Makefile-CMake-Common.mk` adds the `CMAKE_...`
variables used by the cmake-based third-party component builds. `Makefile-Common.mk` is deprecated (it
just forwards to `SharedMakeVariables-Default.mk`).

### Build/Shared

Currently `Skel-Templates/` - the application skeletons `Skel` copies out and substitutes into.

### Build/Tools/Src - versus the top-level Tools/

These two are easy to confuse:

- **Build/Tools/Src/** - host utilities the build needs *before* Stroika exists, and which therefore
  cannot use it: `realpath.cpp` (a GNU-`realpath` stand-in, for MacOS; the top-level `Makefile`
  compiles it ad hoc) and `vswhere/` (fetches Microsoft&#39;s Visual Studio locator; nothing runs it -
  `make -C Build/Tools/Src/vswhere` by hand). Neither is part of the normal build.
- **Tools/** - tools built *with* Stroika, by the normal build, into **Builds/{CONFIGURATION}/bin/**.
  They depend on the Foundation/Frameworks and mirror **Library/**&#39;s layout. Currently
  `HTMLViewCompiler`, which `SharedBuildRules-Default.mk` invokes to compile `.swsp` files.

The dividing line is just: **can it use Stroika?** If yes, it belongs in the top-level **Tools/**. If
it has to run before Stroika can be built, it belongs under **Build/**.

### Note on the pre-3.0d24 layout

Before Stroika v3.0d24, the scripts, makefile includes and templates all lived in one top-level
`ScriptsLib/` folder, and the container definitions in a top-level `DockerBuildContainers/`. A
top-level `ScriptsLib/` remains, containing only deprecation shims - each entry warns and forwards to
its new home - so existing application makefiles keep working. Don&#39;t reference it from new code.

---

## Build Process

On any platform, building Stroika, and all is demo applications and regression tests is as simple as cd&#39;ing to the top-level directory, and typing make

make runs Stroika's CMake build (since Stroika v3.0d26: CMakeLists.txt), one build directory per configuration - `IntermediateFiles/<configuration>/CMake` - with the configuration's CMake preset (`CMakeUserPresets.json`, written as the configuration is applied - by any make target for it, or `make apply-configurations`). So `cmake --preset Debug` then `cmake --build --preset Debug -j8` builds as `make CONFIGURATION=Debug all -j8` does, and `ctest --preset Debug` runs its tests. It is one CMake project, whose configure step builds the configuration's third-party components first; so an IDE that reads the presets - Visual Studio's File > Open > Folder on the top directory, VS Code's CMake Tools - builds from them too, and sees every source file, test and sample (to debug one, say). make applies a configuration again whenever it has changed (re-run configure, say, or pull a newer Stroika); without make, the build stops instead, saying how. A configuration's CMake generator is configure's `--cmake-generator` (ninja, msbuild or make; by default ninja with Visual Studio, else make), which make prints as the build starts. Until Stroika v3.0d26 ships, `make -f Makefile-Legacy <target>` is make's own build of before, kept as a fallback.

### Special Targets

- `make`

  Make with no arguments runs &#39;make help&#39;

- `make help`

  Prints the names and details of the special targets

- `make all`

  Builds the stroika library, tests, demos, etc.

- `make libraries`

  Builds just the Stroika libraries (and the third-party components they need)

- `make tools`, `make tests`, `make samples`, `make documentation`, `make third-party-components`

  Builds that part (and what it needs): the tools (HTMLViewCompiler), the regression tests, the sample applications (and their installers), the documentation (given doxygen), the configuration's third-party components

- `make run-tests`
- `make CONFIGURATION=zyx run-tests REMOTE='lewis@raspberrypi'`
- `make CONFIGURATION=abc run-tests VALGRIND=memcheck`

  Builds the regression tests, and runs them, one at a time (ctest), showing each test's whole output. If REMOTE= is specified, each test is copied to the target machine with ssh, and run there (helpful for when cross-compiling - which, without REMOTE=, skips them). VALGRIND= is used to run memcheck, or leakcheck on the given configuration.

- `make clean`, `make library-clobber`, `make clobber`

  clean removes what was built but not installed - Stroika's own build, and the third-party components' sources and build trees - but not the libraries and programs built, nor what the components installed (so they are not built again). library-clobber: Stroika's own build afresh next time, the components' build kept. clobber removes all that was built.

- make `project-files`

  Writes each configuration's IDE settings - its CMake preset (`CMakeUserPresets.json`, which Visual Studio's Open Folder reads) and VS Code's (`.vscode/c_cpp_properties.json`) - making the default configurations first, if there are none. `make all` does it too.

- `make check-prerequisite-tools`

  Checks if the tools needed to build Stroika are installed and in your path. This is done automatically, and generally not needed explicitly.

- `make apply-configurations`

  To generate all the directories and files dependent on the defined configurations. Note – this is generally not necessary, and called automatically.

### CONFIGURATION arguments to make

All the make targets (e.g. all, libraries etc) take an OPTIONAL parameter CONFIGURATION. If specified, only that configuration is built. If omitted (or empty) – ALL configurations are built.

### TAGS arguments to make

This allows for building (or clobbering or whatever) a related family of configurations. For example

- `make TAGS=Windows`

  runs all the windows builds.

- `make TAGS=Unix` all

  runs all the UNIX compiles

- `make TAGS="Windows x86" all`

  This builds all the configurations with BOTH the Windows and x86 tag (so all the 32-bit x86 builds)

- `make TAGS="Unix arm" list-configurations`

  This doesn't build anything, but just lists all the matching configurations.

- `make list-configurations`
- `make list-configuration-tags`
- `make list-configurations TAGS="Windows"`
- `make list-configurations TAGS="Windows x86_64"`

### Other Make variables that can be helpful

- ECHO_BUILD_LINES=1 makes the output of make noisier, so you can run particular builds by hand, or debug the make process (make's own build: make -f Makefile-Legacy).
- MAKE_INDENT_LEVEL=N, is useful when calling the stroika make process as part of another larger make process, to get its output indended.

### Cross-Compiling

#### Building for Raspberry Pi

To cross-compile for Raspberry pi,

- install some suitable cross compiler (in this example arm-linux-gnueabihf-g++-9)

On unubtu, sudo apt-get install g++-11-arm-linux-gnueabihf

- configure raspberrypi-gcc-11 --apply-default-debug-flags --trace2file enable --compiler-driver arm-linux-gnueabihf-g++-11 --cross-compiling true

Set cross-compiling true so that internal tests aren&#39;t run using the arm built executables.

Set –apply-default-release-flags instead of &#39;debug&#39; for a smaller faster executable.

--trace2file disable to disable tracefile utility, and enabled writes a debug log to /tmp.

- make CONFIGURATION=raspberrypi-gcc-9 all

This builds the samples, libraries etc to Builds/raspberrypi-gcc-9)

- make run-tests REMOTE=pi@myRasberryPi

This uses ssh to run the tests remotely on the argument machine (I setup a hostname in /etc/hosts for the mapping).

Using SSH, it&#39;s also helpful to setup ssh keys to avoid re-entering passwords

[http://sshkeychain.sourceforge.net/mirrors/SSH-with-Keys-HOWTO/SSH-with-Keys-HOWTO-4.html](http://sshkeychain.sourceforge.net/mirrors/SSH-with-Keys-HOWTO/SSH-with-Keys-HOWTO-4.html)

---

## Windows gotchas

Three that cost real time to diagnose, because none of them is hinted at by the error.

**MSYS2 parallel builds can HANG - `msys2-runtime` 3.6.10 regression.** `make -jN` under MSYS2 stops
part way through, at any stage, and never recovers. This is not a Stroika bug; it is an unfixed
regression in `msys-2.0.dll`. The signature is that **no compiler processes remain and no new files
appear** - several `make.exe`/`sh.exe` persist, but no `cl.exe` does, and nothing has been written
under `IntermediateFiles/` or `Builds/` for minutes. CPU is misleading (the wedged `make` may sit at
0% or spin at ~2%), so check for compilers and file writes instead. **Ctrl+C will not break it.**

Recover by closing the terminal tab - that tears down the whole wedged process group - or with
`taskkill /F /IM make.exe /T` and then the same for `sh.exe`. Re-running make **resumes where it
stopped**, so nothing already built is lost; a few such kicks will usually get a build through.

| `msys2-runtime` | |
|---|---|
| <= 3.6.9-2 | not affected - what a working box should be on |
| 3.6.10-1 ... 3.6.10-4 | affected |

The build warns when it sees an affected runtime, from `Build/Scripts/CheckPrerequisiteTools` - so
`make check-prerequisite-tools` says so, as does any build whose `IntermediateFiles/` has been
clobbered. It is stamp-gated though, so an established tree that has already been checked will NOT
re-warn on its own; run `make check-prerequisite-tools` after a `pacman -Syu`.

Workarounds, cheapest first:

1. Keep stdout/stderr off a console, which only the top-level make needs since every sub-make and
   compiler inherits it: `set -o pipefail; make CONFIGURATION=Debug all -j8 2>&1 | tee build.txt`
   (`pipefail` matters - without it the pipeline returns `tee`'s status and a failed build reports
   success).
2. Lower `-jN`. The bug is specific to parallel builds.
3. Copy `msys-2.0.dll` from a 3.6.9 MSYS2 install over `msys64/usr/bin/msys-2.0.dll` - a single-file
   swap, not a package downgrade. It costs you the 3.6.10 fixes, including the slow-paste one.

Whether Cygwin shares this bug has NOT been measured, so do not assume it is the escape hatch.
See [issue #1169](https://github.com/SophistSolutions/Stroika/issues/1169).


**"Permission denied" on a build product, with a permissive DACL.** Check for a Mandatory Integrity
Control label before assuming an ACL or ownership problem:

```bash
icacls Builds/Debug/Samples-HTMLUI/*.msi    # look for: Mandatory Label\High Mandatory Level:(NW)
```

A file is stamped High only if an elevated process wrote it - in practice, test-installing a built
`.msi` in place, since `msiexec /i` (or double-clicking Install) elevates and labels the file it
installs *from*. "No-Write-Up" then blocks every Medium-integrity process from writing or deleting
it, so `make distclean` fails no matter what the DACL says. Being in Administrators does not help:
UAC leaves an ordinary shell at Medium integrity too.

Fix from an elevated shell with `icacls "<path>" /setintegritylevel Medium`, or just delete the file.
Avoid it by copying a built `.msi` somewhere like `%TEMP%` before test-installing, so the elevated
`msiexec` never touches anything inside the checkout.

**MSYS2 resolves HOME two different ways.** bash's own `$HOME` is normally `C:\Users\<account>`, but
`ssh.exe` and git's bundled ssh resolve home through nsswitch to `C:\msys64\home\<account>` instead.
If something depending on dotfiles, `known_hosts` or keys mysteriously cannot see them, check both.

## Integration with IDEs

### Using Visual Studio

First run `make project-files` (or `make all`): it makes the default configurations, if there are none, and writes each one's CMake preset (`CMakeUserPresets.json`) - without the presets Visual Studio lists no configurations. Then open Stroika's top directory (File > Open > Folder) and pick a configuration in the toolbar's configure-preset box. Each configuration's CMake project holds every source file, test and sample, to build, run, debug and browse; its first configure builds its third-party components, which takes a while (View > Output, CMake). The debugger shows Stroika's types through `Library/Sources/Stroika/Foundation/Debug/Stroika-Foundation.natvis`, which each program's link puts in its .pdb.

An application built on Stroika opens the same way - `make project-files` in it first, then its folder - with its own CMakeLists.txt and preset, as Skel makes them.


### Using Visual Studio Code

Visual Studio Code works well with Stroika. Just open the workspace file Workspaces/VSCode/Stroika.code-workspace.
The workspsace contains pre-built 'tasks' to build Stroika (run makefiles).

To use the builtin 'tasks.json' - you may need to install the VSCode extension "command variable".

Also, you will need to have tools like cygwin or MSYS (see [Installing-Required-Tools.md](./Installing-Required-Tools.md)) in your path. One good way todo this is to 
add to your settings.json:
  ~~~json
    "terminal.integrated.profiles.windows": {
      "cygwin-bash": {
        "path": "C:\\tools\\cygwin\\bin\\bash.exe",
        "args": [
          "-l", "-i"
        ],
      },
      "msys-bash": {
        "path": "C:\\tools\\msys64\\usr\\bin\\bash.exe",
        "env": {"MSYSTEM": "MSYS", "CHERE_INVOKING": "1"},
        "args": [
          "-l", "-i"
        ],
      },
    },
    "terminal.integrated.defaultProfile.windows": "cygwin-msys",
  ~~~

  The 'defaultProfile' is the one used by the tasks.json for executing commands like make all, make clean etc from vscode's build menu (SHIFT-Cntrl-B)


---

## Building your own application

To some degree, this is simple, and you can just copy/munge one of the sample applications. But the paths in the makefile aren't simple to update. So Stroika provides
a 'Skel' utility, that builds a skeleton application.

  ~~~bash
  ./Build/Scripts/Skel --appRoot ../myApp
  cd ../myApp
  make CONFIGURATION=Debug all -j10
  ~~~

Use './Build/Scripts/Skel --help' to see options

---

## Integrating with CMake

Stroika itself builds with CMake (since v3.0d26 - see Build Process, above), and writes a CMake package for each configuration (`lib/cmake/Stroika` in its Builds directory), for a program built on Stroika with CMake:

  ~~~cmake
  find_package (Stroika REQUIRED)
  add_executable (MyApp Sources/Main.cpp)
  target_link_libraries (MyApp PRIVATE Stroika::Stroika)
  ~~~

configured with the configuration's toolchain file - Stroika's compilers and flags, which a program linking Stroika must share - and its Builds directory where CMake looks for packages:

  ~~~bash
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<Stroika>/IntermediateFiles/Debug/Toolchain.cmake -DCMAKE_PREFIX_PATH=<Stroika>/Builds/Debug
  ~~~

`Stroika::Stroika` is both of Stroika's libraries - `Stroika::Foundation`, and `Stroika::Frameworks`, which brings Foundation with it - with the third-party libraries they use. `Stroika::HTMLViewCompiler` compiles a .swsp file to C++, and the package defines Stroika's CMake functions (Build/Lib/CMake): `StroikaMakeVersionFile`, `StroikaCopyFilesToExeDir`, `StroikaInstallers` and `StroikaEmbedFiles`.

Stroika's regression tests and samples are part of Stroika's own build, linking its libraries there (each test `Stroika::Frameworks`, each sample `Stroika::Foundation` or `Stroika::Frameworks`); `Tests/CMakeLists.txt` and `Samples/CMakeLists.txt` can each also build by themselves, on the package.

An application Skel makes builds so too, on the Stroika in its `ThirdPartyComponents/Stroika/StroikaRoot` (a git submodule), whose build writes the application's preset (`CMakeUserPresets.json`). Made with `Skel --stroikaSource none`, it has no Stroika of its own - it builds on one built elsewhere, as a package manager would supply one - and that Stroika writes its preset:

  ~~~bash
  <Stroika>/Build/Scripts/ApplyConfiguration --application-root ../myApp Debug
  cd ../myApp && cmake --preset Debug && cmake --build --preset Debug
  ~~~

`make skel-test` does that with a new application, and runs it - what CI checks the package with.

---

## But Why? Build / Configuration Design

### Alternatives

We seriously considered a number of build systems, including cmake, ant, perl scripts, qmake, etc. They all had substantial weaknesses, with ant possibly being the best alternative, except that it&#39;s heavily java oriented. Maybe for c++ cmake would have been the best?

But just plain GNU make – appears to be a nearly universally available alternative, very standard and simple, so that&#39;s what we did, until Stroika v3.0d26 moved the build itself to CMake (keeping make's targets, and the configure script).

But - even with just plain make, you need some sort of configure script to establish what compiler options will be defined. Again, lots of different alternatives here, but in the end I decided to just build custom scripts which build a very simple XML configuration declaration, and which drives the make process by #included 'config' makefile.

---

## Common Errors (FAQ)

- When installing a new version of some compilers like Visual Studio, you must rebuild
  your configuration files with
  ~~~
  make reconfigure
  ~~~
  since the configuration file may contain absolute paths (with version info) to particular compilers or libraries.

- Tar failure

  Errors about invalid parameters, and/or bad blocks can usually be fixed by installing a copy of gnu tar. We&#39;ve tested 1.27.

- cp: illegal option –

  Install a copy of GNU cp

- Cannot find &#39;blah&#39; in Cygwin

  If you are trying to install required components in Cygwin, and cannot find them in the Cygwin setup GUI, try:

  cygcheck -p 7z

- rm: cannot remove '.../boost/libs/beast/test/extern/zlib-1.2.11/contrib/dotzlib': Device or resource busy

  This appears to be some weird issue with visual studio code. Quit vs code, and try again.

- On raspberry pi

  > /tmp/Test43: /lib/arm-linux-gnueabihf/libc.so.6: version `GLIBC_2.28' not found (required by /tmp/Test43)

  - fix by

    - sudo vi /etc/apt/sources.list"
    - temporarily add

      ```bash
      #tmphack to load GLIBC_2 2.28
      deb http://raspbian.raspberrypi.org/raspbian/ buster main contrib non-free rpi
      ```

    - `sudo apt-get update`
    - `apt-cache policy libc6`
    - `sudo apt-get install libc6=2.28-5+rpi1`
    - undo edit to /etc/apt/sources.list (or comment out addition)
    - sudo apt-get update

    NOTE - if you area dealing later versions some variation on this will likely work. Select the next debian release past your current one for the /etc/apt/sources.list addition and use apt-cache policy to find an available version.

- On MacOS

  - Brew issues

    ```sh
    ld: library not found for -lidn2
    ```

    - brew install libidn2

    - and maybe also add to .zshenv

    ```sh
    export CPATH=/opt/homebrew/include
    export LIBRARY_PATH=/opt/homebrew/lib
    ```

  - realpath issues
    ~~~sh
      realpath: illegal option -- -
      usage: realpath [-q] [path ...]
    ~~~

    missing, or insufficient realpath on macos.
    ~~~sh
    make install-realpath
    ~~~



- Under Docker

  ```text
  ==7192==LeakSanitizer has encountered a fatal error.
  ==7192==HINT: For debugging, try setting environment variable LSAN_OPTIONS=verbosity=1:log_threads=1
  ==7192==HINT: LeakSanitizer does not work under ptrace (strace, gdb, etc)
  ```

  OR

  ```text
  warning: Error disabling address space randomization: Operation not permitted
  warning: Could not trace the inferior process.
  warning: ptrace: Operation not permitted
  ```

  run the docker container (docker run or docker exec line) - with:
  `--security-opt seccomp=unconfined`

  - See also
    [../Build/Scripts/RunInDockerEnvironment](../Build/Scripts/RunInDockerEnvironment)
    for more hints on developing flags with docker containers.

- VSCode
  - command 'extension.commandvariable.file.content' not found

    Caused by missing VSCode extension

    - Install the "Command Variable" Extension 
      - shortcut Ctrl+Shift+X
      - Search for "Command Variable"
      - click install

  - command 'cpptools.activeConfigName' not found

    Caused by missing VSCode extension

    - Install the "C/C++ for Visual Studio Code" Extension 
      - shortcut Ctrl+Shift+X
      - Search for "C/C++ for Visual Studio Code"
      - click install

  - when building in vscode, error message from vscode:
    "Unexpected token 'U', "Unknown" is not valid JSON"

    - fix with

      ~~~bash
      make project-files
      ~~~
