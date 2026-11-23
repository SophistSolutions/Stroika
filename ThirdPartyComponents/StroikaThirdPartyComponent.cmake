#
# How Stroika builds a third-party component: the settings from the configuration that each component's own build is
# given, and StroikaThirdPartyComponent, which each <name>/CMakeLists.txt calls. Included by ThirdPartyComponents/
# CMakeLists.txt, which builds the configuration's components, and by Conan's build of one (conanfile.py) - so this file
# and the component's directory are what Conan keeps a component for: editing them builds it again in Conan's cache, and
# editing ThirdPartyComponents/CMakeLists.txt (which components, how Conan runs) does not.
#

# the configuration's Stroika settings, which ApplyConfiguration writes beside its toolchain file
if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")
if (NOT DEFINED STROIKA_PLATFORM_C_FLAGS)
    message (FATAL_ERROR "${configurationDir_}/Stroika-Configuration.cmake was written by an older Stroika: apply the configuration again - make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
endif ()
# Built by Conan, in its cache (conanfile.py): installed into STROIKA_THIRD_PARTY_PREFIX, not the configuration's Builds
# directory
if (DEFINED STROIKA_THIRD_PARTY_PREFIX)
    set (STROIKA_BUILDS_DIR "${STROIKA_THIRD_PARTY_PREFIX}")
endif ()

# STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS for a component's compiles, and STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS for its
# programs' links: built by Conan, debug information names the directory the component is installed into - where the
# components it is built on are, in Conan's build folder, removed once packaged - as ".", so that a package names that
# directory only where it is compiled in, which StroikaDeploy.py warns of. The compiler's debug information names it as
# the include directory of those components; on a Mac, the linker's debug map names each object a program links, by path -
# those of the static libraries there too. With Visual Studio, where the configuration's toolchain file gives the flags, a
# recipe that gives its own adds these (mongo-cxx-driver's).
set (STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS)
set (STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS)
if (DEFINED STROIKA_THIRD_PARTY_PREFIX)
    string (REGEX REPLACE "/$" "" prefix_ "${STROIKA_THIRD_PARTY_PREFIX}")
    if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
        # /experimental:deterministic only for /pathmap, which MSVC takes only with it - else D9007, "'/pathmap:' requires
        # '/experimental:deterministic'; option ignored" (MSVC 19.44, 19.52). It also makes the compiler's output
        # deterministic: no time stamps, and a warning (C5048) where __DATE__ or __TIME__ is used. /pathmap matches the
        # directory in any letter case.
        file (TO_NATIVE_PATH "${prefix_}" nativePrefix_)
        set (STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS "/experimental:deterministic \"/pathmap:${nativePrefix_}=.\"")
    else ()
        set (STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS "-fdebug-prefix-map=${prefix_}=.")
        if (CMAKE_HOST_APPLE)
            set (STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS "-Wl,-oso_prefix,${prefix_}/")
        endif ()
    endif ()
endif ()

if (POLICY CMP0135)
    cmake_policy (SET CMP0135 NEW) # files extracted from an archive get the time of extraction
endif ()
include (ExternalProject)

# What each component's own CMake build is given, as Build/Lib/Make/Makefile-CMake-Common.mk gives it: the configuration's
# toolchain file, and its Builds directory to install into. A Visual Studio configuration's build type follows its
# assertions; any other's is None, with the configuration's flags - and -DNDEBUG where its assertions are off.
set (STROIKA_THIRD_PARTY_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}" "-DCMAKE_INSTALL_PREFIX=${STROIKA_BUILDS_DIR}" -DCMAKE_VERBOSE_MAKEFILE=1)
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    list (APPEND STROIKA_THIRD_PARTY_CMAKE_ARGS -DCMAKE_POLICY_DEFAULT_CMP0091=NEW)
    if (STROIKA_ASSERTIONS_ENABLED)
        set (STROIKA_THIRD_PARTY_BUILD_TYPE Debug)
        list (APPEND STROIKA_THIRD_PARTY_CMAKE_ARGS -DCMAKE_POLICY_DEFAULT_CMP0141=NEW)
    else ()
        set (STROIKA_THIRD_PARTY_BUILD_TYPE Release)
    endif ()
    # STROIKA_THIRD_PARTY_OUTPUT_DIR: where a component's build leaves what it builds - with msbuild, in a subdirectory for
    # the build type; with a single-configuration generator (Ninja), which is given the build type (zstd then adds its own
    # debug checks, DEBUGLEVEL=1), in its build directory
    get_property (multiConfig_ GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (multiConfig_)
        set (STROIKA_THIRD_PARTY_OUTPUT_DIR "<BINARY_DIR>/${STROIKA_THIRD_PARTY_BUILD_TYPE}")
    else ()
        set (STROIKA_THIRD_PARTY_OUTPUT_DIR "<BINARY_DIR>")
        list (APPEND STROIKA_THIRD_PARTY_CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${STROIKA_THIRD_PARTY_BUILD_TYPE}")
    endif ()
else ()
    set (STROIKA_THIRD_PARTY_BUILD_TYPE None)
    set (STROIKA_THIRD_PARTY_C_FLAGS "${CMAKE_C_FLAGS}")
    set (STROIKA_THIRD_PARTY_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    if (NOT STROIKA_ASSERTIONS_ENABLED)
        string (APPEND STROIKA_THIRD_PARTY_C_FLAGS " -DNDEBUG")
        string (APPEND STROIKA_THIRD_PARTY_CXX_FLAGS " -DNDEBUG")
    endif ()
    list (APPEND STROIKA_THIRD_PARTY_CMAKE_ARGS -DCMAKE_BUILD_TYPE=None "-DCMAKE_C_FLAGS=${STROIKA_THIRD_PARTY_C_FLAGS}" "-DCMAKE_CXX_FLAGS=${STROIKA_THIRD_PARTY_CXX_FLAGS}")
    # STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS as the build type's flags, not in CMAKE_<LANG>_FLAGS: a component may compile
    # those in - mongo-c-driver sends them to the server, in its handshake - and they name the directory
    if (STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS)
        list (APPEND STROIKA_THIRD_PARTY_CMAKE_ARGS "-DCMAKE_C_FLAGS_NONE=${STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS}" "-DCMAKE_CXX_FLAGS_NONE=${STROIKA_THIRD_PARTY_DEBUG_PATH_FLAGS}")
    endif ()
endif ()

if (NOT DEFINED STROIKA_ORIGS_CACHE) # given for a component Conan builds (conanfile.py): the source tree's
    set (STROIKA_ORIGS_CACHE "${CMAKE_CURRENT_LIST_DIR}/Origs-Cache")
endif ()
set (STROIKA_PKG_CONFIG_PATH "${STROIKA_BUILDS_DIR}lib/pkgconfig")

# each component's own CMake build uses this project's generator (and platform, and toolset)
set (STROIKA_THIRD_PARTY_GENERATOR_ARGS -G "${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR_PLATFORM)
    list (APPEND STROIKA_THIRD_PARTY_GENERATOR_ARGS -A "${CMAKE_GENERATOR_PLATFORM}")
endif ()
if (CMAKE_GENERATOR_TOOLSET)
    list (APPEND STROIKA_THIRD_PARTY_GENERATOR_ARGS -T "${CMAKE_GENERATOR_TOOLSET}")
endif ()

# patch, for the components with patches - on Windows, beside the bash the configuration uses (MSYS2's or Cygwin's)
cmake_path (GET STROIKA_BASH PARENT_PATH bashDirectory_)
find_program (STROIKA_PATCH patch HINTS "${bashDirectory_}" REQUIRED)

#
# StroikaVisualStudioCommand (<variable> <name> <script>)
#
# With Visual Studio: <script> - its lines separated by \n - run in the configuration's Visual Studio environment, as make's
# Build/Scripts/RunArgumentsWithCommonBuildVars runs a command, for a component built by tools of its own (nmake, b2): it
# writes <name>.bat in the calling directory's binary directory - vcvarsall.bat for the configuration's architecture, then
# each line, stopping at the first that fails - and sets <variable> to the command that runs it. One string, not a list:
# a line may hold a ; (set "PATH=...;%PATH%").
#
function (StroikaVisualStudioCommand variable name script)
    if (STROIKA_ARCH STREQUAL "x86")
        set (arch_ x64_x86)
    else ()
        set (arch_ x64)
    endif ()
    file (TO_NATIVE_PATH "${STROIKA_BUILD_TOOLS_ROOT}/VC/Auxiliary/Build/vcvarsall.bat" vcvarsall_)
    string (REPLACE "\n" "\nif errorlevel 1 exit /b 1\n" script_ "${script}")
    # NoDefaultCurrentDirectoryInExePath unset: else cmd runs nothing in the current directory by its bare name, as
    # vcvarsall.bat runs vswhere.exe, and boost's bootstrap.bat its own scripts. pushd/popd: vcvarsall.bat cd's to
    # %VSCMD_START_DIR% if that is set (and older Visual Studios to %USERPROFILE%\Source)
    file (CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.bat" @ONLY NEWLINE_STYLE CRLF
        CONTENT "@echo off\nset NoDefaultCurrentDirectoryInExePath=\npushd %TEMP%\ncall \"${vcvarsall_}\" ${arch_} > nul\npopd\n${script_}\nif errorlevel 1 exit /b 1\n")
    # its path with /, not \: a step's log script (LOG_CONFIGURE ...) holds the command in a CMake string, unescaped
    set (${variable} cmd /E:ON /C "${CMAKE_CURRENT_BINARY_DIR}/${name}.bat" PARENT_SCOPE)
endfunction ()

#
# StroikaThirdPartyComponent (<name> ARCHIVE <file> URLS <url>... [SOURCE_SUBDIR <directory>] [PATCH_COMMAND <command>...]
#                             [CMAKE_ARGS <arg>...] [BUILD_TARGETS <target>...] [DEPENDS <component>...]
#                             [BUILD_IN_SOURCE] [CONFIGURE_COMMAND <command>...] [BUILD_COMMAND <command>...]
#                             [INSTALL_COMMAND <command>...])
#
# The component <name>, from <file> in Origs-Cache (downloaded there from the first of the URLs that works, when missing),
# configured with STROIKA_THIRD_PARTY_CMAKE_ARGS and CMAKE_ARGS, built (just BUILD_TARGETS, when given), and installed -
# after each component it DEPENDS on that the configuration uses, as its own build finds them installed. One that is not a
# CMake project gives its own CONFIGURE_COMMAND, BUILD_COMMAND and INSTALL_COMMAND, and BUILD_IN_SOURCE where it builds
# among its sources. SOURCE_SUBDIR, PATCH_COMMAND and those are ExternalProject_Add's. Each step's output goes to a log file
# in the component's build directory, shown when the step fails.
#
function (StroikaThirdPartyComponent name)
    cmake_parse_arguments (PARSE_ARGV 1 arg "BUILD_IN_SOURCE" "ARCHIVE;SOURCE_SUBDIR"
        "URLS;PATCH_COMMAND;CMAKE_ARGS;BUILD_TARGETS;DEPENDS;CONFIGURE_COMMAND;BUILD_COMMAND;INSTALL_COMMAND")
    if (arg_UNPARSED_ARGUMENTS)
        message (FATAL_ERROR "StroikaThirdPartyComponent (${name}): unknown arguments ${arg_UNPARSED_ARGUMENTS}")
    endif ()
    set (passOn_)
    foreach (component_ IN LISTS arg_DEPENDS)
        if (TARGET ${component_}) # one the configuration uses: added before this one
            list (APPEND passOn_ DEPENDS ${component_})
        endif ()
    endforeach ()
    if (arg_PATCH_COMMAND)
        list (APPEND passOn_ PATCH_COMMAND ${arg_PATCH_COMMAND})
    endif ()
    # Its directories, short, in the calling directory's binary directory - src, build, stamp, tmp, not ExternalProject's
    # <name>-prefix/src/<name>-build and so on: with Visual Studio, boost's b2 puts its 62 include directories, absolute, on
    # one rc command line, which cmd.exe cuts at 8191 characters (#769) - so its sources' path may be no longer than under
    # make (IntermediateFiles/<configuration>/CMake/ThirdParty/boost/src, beside make's .../ThirdPartyComponents/boost); and
    # mongo-c-driver's deepest files come near Windows' MAX_PATH.
    list (APPEND passOn_ PREFIX "${CMAKE_CURRENT_BINARY_DIR}" SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src"
        STAMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/stamp" TMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/tmp")
    if (arg_BUILD_IN_SOURCE)
        list (APPEND passOn_ BUILD_IN_SOURCE ON)
    else ()
        list (APPEND passOn_ BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
    endif ()
    # its archive, downloaded into Origs-Cache when missing - here, not by its download step, which is so the same whether or
    # not the archive was there: a step defined differently from the last build runs again, and every step after it
    set (archive_ "${STROIKA_ORIGS_CACHE}/${arg_ARCHIVE}")
    foreach (url_ IN LISTS arg_URLS)
        if (EXISTS "${archive_}")
            break ()
        endif ()
        message (STATUS "${name}: downloading ${url_}")
        file (DOWNLOAD "${url_}" "${archive_}.part" STATUS status_ TLS_VERIFY ON)
        list (GET status_ 0 code_)
        if (code_ EQUAL 0)
            file (RENAME "${archive_}.part" "${archive_}")
        else ()
            file (REMOVE "${archive_}.part")
            list (GET status_ 1 message_)
            message (STATUS "${name}: ${url_}: ${message_}")
        endif ()
    endforeach ()
    if (NOT EXISTS "${archive_}")
        message (FATAL_ERROR "${name}: ${arg_ARCHIVE} is not in Origs-Cache, and none of its URLs gave it: ${arg_URLS}")
    endif ()
    if (arg_CONFIGURE_COMMAND)
        set (configure_ CONFIGURE_COMMAND ${arg_CONFIGURE_COMMAND})
    else ()
        # its own CMake configure - with the configuration's pkgconfig directory in PKG_CONFIG_PATH, as make's builds have it
        # (Configuration.mk exports it), so its pkg-config checks find the components already built (mongo-c-driver's zstd)
        set (source_ <SOURCE_DIR>)
        if (arg_SOURCE_SUBDIR)
            string (APPEND source_ "/${arg_SOURCE_SUBDIR}")
        endif ()
        set (configure_ CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E env "PKG_CONFIG_PATH=${STROIKA_PKG_CONFIG_PATH}"
            "${CMAKE_COMMAND}" ${STROIKA_THIRD_PARTY_GENERATOR_ARGS} ${STROIKA_THIRD_PARTY_CMAKE_ARGS} ${arg_CMAKE_ARGS} -S ${source_} -B <BINARY_DIR>)
    endif ()
    if (arg_BUILD_COMMAND)
        set (build_ ${arg_BUILD_COMMAND})
    elseif (CMAKE_GENERATOR MATCHES "Makefiles")
        set (build_ "$(MAKE)" ${arg_BUILD_TARGETS}) # sharing make's job slots, as each Makefile's +cd ... && cmake --build does
    else ()
        set (build_ "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${STROIKA_THIRD_PARTY_BUILD_TYPE})
        foreach (target_ IN LISTS arg_BUILD_TARGETS)
            list (APPEND build_ --target ${target_})
        endforeach ()
    endif ()
    if (arg_INSTALL_COMMAND)
        set (install_ ${arg_INSTALL_COMMAND})
    else ()
        set (install_ "${CMAKE_COMMAND}" --install <BINARY_DIR> --config ${STROIKA_THIRD_PARTY_BUILD_TYPE})
    endif ()
    # Its build step in the terminal (USES_TERMINAL_BUILD): under Ninja, in its console pool - so one component builds at a
    # time, with the build's jobs (CMAKE_BUILD_PARALLEL_LEVEL), not several at once, each with Ninja's default (a job per
    # processor and two more)
    ExternalProject_Add (${name} URL "${archive_}" ${configure_}
        BUILD_COMMAND ${build_}
        INSTALL_COMMAND ${install_}
        USES_TERMINAL_BUILD ON
        LOG_DOWNLOAD ON LOG_CONFIGURE ON LOG_BUILD ON LOG_INSTALL ON LOG_OUTPUT_ON_FAILURE ON
        ${passOn_}
    )
endfunction ()
