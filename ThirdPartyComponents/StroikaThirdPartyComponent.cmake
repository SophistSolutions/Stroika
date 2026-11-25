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

# STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS for a component's programs' links: built by Conan, on a Mac, the linker's debug
# map names each object a program links, by path - those of the static libraries in the directory the component is
# installed into too, in Conan's build folder, removed once packaged - so names them relative to it. (What a compile's debug
# information names: StroikaThirdPartyDebugPathFlags.)
set (STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS)
if (DEFINED STROIKA_THIRD_PARTY_PREFIX AND CMAKE_HOST_APPLE)
    string (REGEX REPLACE "/$" "" prefix_ "${STROIKA_THIRD_PARTY_PREFIX}")
    set (STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS "-Wl,-oso_prefix,${prefix_}/")
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
# StroikaThirdPartyDebugPathFlags (<variable> [SOURCES_ONLY] [MAP <directory> <deployed>])
#
# The compiler flags (a list) with which the calling directory's component, built by Conan, names in its debug information
# where its sources, and the headers it includes, will be once deployed into a configuration's Builds directory
# (StroikaDeploy.py) - each by STROIKA_THIRD_PARTY_DEPLOY_PLACEHOLDER, which deploying writes over with that directory, so
# that a debugger finds them with nothing set up; with MSVC, relative to that directory, which the debugger is given as its
# source path (MSVC's -pathmap cannot take the placeholder, below):
#   - its binary directory - StroikaThirdPartyComponent's src/ and build/: its sources, and what its build makes or fetches
#     there (mongo-c-driver) - as src/<directory> there, where conanfile.py packages them (STROIKA_THIRD_PARTY_SOURCES);
#   - its install directory, which holds the headers of the components it is built on, as that Builds directory, which
#     holds the same - but with SOURCES_ONLY, for a component built on none that compiles its flags in (OpenSSL, as its
#     version information): its libraries would name that directory, which deploying warns of;
#   - MAP's directory, in its binary directory, as <deployed> there (relative to it): boost's headers as include/boost,
#     where its install copied them, rather than packaged again.
# Empty where Conan does not build it. Where two match a path, MSVC takes the first, gcc and clang the last: so MAP's goes
# first, or last. -pathmap, not /pathmap: OpenSSL's Windows build turns each / in its flags into \.
#
function (StroikaThirdPartyDebugPathFlags variable)
    cmake_parse_arguments (PARSE_ARGV 1 arg "SOURCES_ONLY" "" "MAP")
    set (flags_)
    if (DEFINED STROIKA_THIRD_PARTY_PREFIX AND DEFINED STROIKA_THIRD_PARTY_DEPLOY_PLACEHOLDER)
        string (REGEX REPLACE "/$" "" prefix_ "${STROIKA_THIRD_PARTY_PREFIX}")
        cmake_path (GET CMAKE_CURRENT_SOURCE_DIR FILENAME directory_)
        set (maps_ "${CMAKE_CURRENT_BINARY_DIR}" "src/${directory_}")
        if (NOT arg_SOURCES_ONLY)
            list (APPEND maps_ "${prefix_}" ".")
        endif ()
        set (vs_ OFF)
        if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
            set (vs_ ON)
        endif ()
        if (arg_MAP)
            list (GET arg_MAP 0 from_)
            list (GET arg_MAP 1 to_)
            if (vs_)
                list (PREPEND maps_ "${CMAKE_CURRENT_BINARY_DIR}/${from_}" "${to_}")
            else ()
                list (APPEND maps_ "${CMAKE_CURRENT_BINARY_DIR}/${from_}" "${to_}")
            endif ()
        endif ()
        if (vs_)
            # -experimental:deterministic only for -pathmap, which MSVC takes only with it - else D9007, "'/pathmap:' requires
            # '/experimental:deterministic'; option ignored" (MSVC 19.44, 19.52). It also makes the compiler's output
            # deterministic: no time stamps, and a warning (C5048) where __DATE__ or __TIME__ is used. -pathmap matches a
            # directory in any letter case - and rewrites __FILE__ too.
            set (flags_ -experimental:deterministic)
        endif ()
        list (LENGTH maps_ n_)
        math (EXPR last_ "${n_} - 1")
        foreach (i_ RANGE 0 ${last_} 2)
            math (EXPR j_ "${i_} + 1")
            list (GET maps_ ${i_} from_)
            list (GET maps_ ${j_} to_)
            if (vs_)
                # Relative, not by the placeholder: -pathmap overruns __FILE__ where it makes it much longer - a directory short
                # beside the placeholder, as with a Conan home of 20 characters or so (CI's, c:/conan) - with C2001, C1004 or
                # C1001, a crash or a hang, as the lengths fall (MSVC 19.44, 19.52). A relative path is shorter than any
                # directory it replaces.
                file (TO_NATIVE_PATH "${from_}" from_)
                file (TO_NATIVE_PATH "${to_}" to_)
                list (APPEND flags_ "-pathmap:${from_}=${to_}")
            else ()
                set (deployed_ "${STROIKA_THIRD_PARTY_DEPLOY_PLACEHOLDER}")
                if (NOT to_ STREQUAL ".")
                    string (APPEND deployed_ "/${to_}")
                endif ()
                list (APPEND flags_ "-fdebug-prefix-map=${from_}=${deployed_}")
            endif ()
        endforeach ()
    endif ()
    set (${variable} "${flags_}" PARENT_SCOPE)
endfunction ()

#
# StroikaThirdPartyDebugPathProjectInclude (<variable> [MAP <directory> <deployed>])
#
# For a component's own CMake build: -DCMAKE_PROJECT_INCLUDE=<file>, a file giving each project () of its build
# StroikaThirdPartyDebugPathFlags as compile options - not in its CMAKE_<LANG>_FLAGS, which a component may compile in
# (mongo-c-driver sends them to the server, in its handshake). Empty where Conan does not build it.
#
function (StroikaThirdPartyDebugPathProjectInclude variable)
    StroikaThirdPartyDebugPathFlags (flags_ ${ARGN})
    set (result_)
    if (flags_)
        set (content_)
        foreach (flag_ IN LISTS flags_)
            string (APPEND content_ "add_compile_options ([==[$<$<COMPILE_LANGUAGE:C,CXX>:${flag_}>]==])\n")
        endforeach ()
        file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/DebugPaths.cmake" "${content_}")
        set (result_ "-DCMAKE_PROJECT_INCLUDE=${CMAKE_CURRENT_BINARY_DIR}/DebugPaths.cmake")
    endif ()
    set (${variable} "${result_}" PARENT_SCOPE)
endfunction ()

#
# StroikaThirdPartyComponent (<name> ARCHIVE <file> URLS <url>... [SOURCE_SUBDIR <directory>] [PATCH_COMMAND <command>...]
#                             [CMAKE_ARGS <arg>...] [BUILD_TARGETS <target>...] [DEPENDS <component>...]
#                             [BUILD_IN_SOURCE] [CONFIGURE_COMMAND <command>...] [BUILD_COMMAND <command>...]
#                             [INSTALL_COMMAND <command>...] [PACKAGE_SOURCES <pattern>... | NO_SOURCES])
#
# The component <name>, from <file> in Origs-Cache (downloaded there from the first of the URLs that works, when missing),
# configured with STROIKA_THIRD_PARTY_CMAKE_ARGS and CMAKE_ARGS, built (just BUILD_TARGETS, when given), and installed -
# after each component it DEPENDS on that the configuration uses, as its own build finds them installed. One that is not a
# CMake project gives its own CONFIGURE_COMMAND, BUILD_COMMAND and INSTALL_COMMAND, and BUILD_IN_SOURCE where it builds
# among its sources - and, its own flags, StroikaThirdPartyDebugPathFlags. SOURCE_SUBDIR, PATCH_COMMAND and those are
# ExternalProject_Add's. Each step's output goes to a log file in the component's build directory, shown when the step fails.
#
# Built by Conan, with the configuration's STROIKA_THIRD_PARTY_SOURCES, its package carries its sources - those of its
# binary directory's files (src/ and build/) that PACKAGE_SOURCES matches, by default its C and C++ files - for its debug
# information to name once deployed (StroikaThirdPartyDebugPathFlags): conanfile.py copies them, as package-sources.txt
# in its binary directory lists the patterns. NO_SOURCES for a tool, or binaries, it does not compile (StrawberryPerl, WIX).
#
function (StroikaThirdPartyComponent name)
    cmake_parse_arguments (PARSE_ARGV 1 arg "BUILD_IN_SOURCE;NO_SOURCES" "ARCHIVE;SOURCE_SUBDIR"
        "URLS;PATCH_COMMAND;CMAKE_ARGS;BUILD_TARGETS;DEPENDS;CONFIGURE_COMMAND;BUILD_COMMAND;INSTALL_COMMAND;PACKAGE_SOURCES")
    if (arg_UNPARSED_ARGUMENTS)
        message (FATAL_ERROR "StroikaThirdPartyComponent (${name}): unknown arguments ${arg_UNPARSED_ARGUMENTS}")
    endif ()
    file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/package-sources.txt")
    if (DEFINED STROIKA_THIRD_PARTY_PREFIX AND STROIKA_THIRD_PARTY_SOURCES AND NOT arg_NO_SOURCES)
        set (patterns_ ${arg_PACKAGE_SOURCES})
        if (NOT patterns_)
            foreach (extension_ IN ITEMS c cc cpp cxx h hh hpp hxx ipp inl inc tcc s S asm)
                list (APPEND patterns_ "src/*.${extension_}" "build/*.${extension_}")
            endforeach ()
        endif ()
        list (JOIN patterns_ "\n" patterns_)
        file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/package-sources.txt" "${patterns_}\n")
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
        # and its debug information naming its sources, and the headers it includes, where they will be once deployed
        StroikaThirdPartyDebugPathProjectInclude (projectInclude_)
        set (configure_ CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E env "PKG_CONFIG_PATH=${STROIKA_PKG_CONFIG_PATH}"
            "${CMAKE_COMMAND}" ${STROIKA_THIRD_PARTY_GENERATOR_ARGS} ${STROIKA_THIRD_PARTY_CMAKE_ARGS} ${projectInclude_} ${arg_CMAKE_ARGS}
            -S ${source_} -B <BINARY_DIR>)
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
    # Its build step in the terminal (USES_TERMINAL_BUILD): under Ninja, in its console pool, whose output is passed on as
    # it comes - Ninja holds back any other step's until it ends - so the component's log (ConanSteps.cmake) grows as it
    # builds. Its steps' output is not kept in logs of their own (LOG_BUILD and the rest): it goes to that log.
    ExternalProject_Add (${name} URL "${archive_}" ${configure_}
        BUILD_COMMAND ${build_}
        INSTALL_COMMAND ${install_}
        USES_TERMINAL_BUILD ON
        ${passOn_}
    )
endfunction ()
