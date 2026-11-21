#
# The configuration's third-party components, built with CMake beside make (#1157). Each <name>/CMakeLists.txt builds
# one as its Makefile does: its source archive from Origs-Cache (downloaded there when missing), configured with the
# configuration's toolchain file, built, and installed into the configuration's Builds directory (STROIKA_BUILDS_DIR) -
# where Stroika's own CMake build finds it (the top CMakeLists.txt, which builds this as it is configured). For one
# configuration:
#     cmake -S ThirdPartyComponents -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#     CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build <build directory>
# The jobs: with a Makefiles generator, the components' own builds share make's job slots, as under make; with Ninja, they
# build one at a time, each with CMAKE_BUILD_PARALLEL_LEVEL's (make's -jN sets it). curl builds here on UNIX only, as
# Stroika uses it only there.
#
cmake_minimum_required (VERSION 3.22)
project (StroikaThirdPartyComponents LANGUAGES NONE)

# the configuration's Stroika settings, which ApplyConfiguration writes beside its toolchain file
if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")
if (NOT DEFINED STROIKA_PLATFORM_C_FLAGS)
    message (FATAL_ERROR "${configurationDir_}/Stroika-Configuration.cmake was written by an older Stroika: apply the configuration again - make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
endif ()
# Built by Conan, in its cache (conanfile.py): just the component STROIKA_THIRD_PARTY_ONLY, installed into
# STROIKA_THIRD_PARTY_PREFIX, not the configuration's Builds directory
if (DEFINED STROIKA_THIRD_PARTY_PREFIX)
    set (STROIKA_BUILDS_DIR "${STROIKA_THIRD_PARTY_PREFIX}")
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
    set (STROIKA_ORIGS_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/Origs-Cache")
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

#
# The components Conan keeps (StroikaComponentFromConan): built by their recipes here the first time a configuration needs
# one, and found in Conan's cache after that - by any build tree on the machine. For now these; in time, all of them.
#
set (STROIKA_THIRD_PARTY_FROM_CONAN zlib)
if (STROIKA_THIRD_PARTY_FROM_CONAN AND NOT DEFINED STROIKA_THIRD_PARTY_ONLY)
    find_program (STROIKA_CONAN conan REQUIRED)
    # The configuration, to Conan (conanfile.py's option "configuration"): a digest of its toolchain file and Stroika settings,
    # less what names this build tree or the configuration - their paths, its name and file - so the same configuration in
    # another tree finds its components in Conan's cache. And of its generator, which can change what a component installs:
    # mongo-cxx-driver's library names carry the Visual Studio toolset only under a Visual Studio generator.
    file (READ "${CMAKE_TOOLCHAIN_FILE}" toolchain_)
    file (READ "${configurationDir_}/Stroika-Configuration.cmake" settings_)
    string (REGEX REPLACE "#[^\n]*\n" "" toolchain_ "${toolchain_}") # their comments name the configuration
    string (REGEX REPLACE "#[^\n]*\n" "" settings_ "${settings_}")
    string (REGEX REPLACE "set\\(STROIKA_CONFIGURATION(_FILE)? [^\n]*\n" "" settings_ "${settings_}")
    cmake_path (GET configurationDir_ PARENT_PATH buildRoot_)
    cmake_path (GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH sourceRoot_)
    foreach (path_ IN ITEMS "${STROIKA_BUILDS_DIR}" "${buildRoot_}" "${sourceRoot_}")
        string (REPLACE "${path_}" "<tree>" toolchain_ "${toolchain_}")
        string (REPLACE "${path_}" "<tree>" settings_ "${settings_}")
    endforeach ()
    string (SHA256 STROIKA_CONAN_CONFIGURATION_DIGEST "${toolchain_}${settings_}${CMAKE_GENERATOR}|${CMAKE_GENERATOR_PLATFORM}|${CMAKE_GENERATOR_TOOLSET}")
    # its profile: just the OS building - which Conan needs, to write its scripts for its shell - as the components declare no
    # settings (so it is no part of a package's identity)
    set (os_ "${CMAKE_HOST_SYSTEM_NAME}")
    if (os_ STREQUAL "Darwin")
        set (os_ Macos)
    endif ()
    set (STROIKA_CONAN_PROFILE "${CMAKE_CURRENT_BINARY_DIR}/conan-profile")
    file (WRITE "${STROIKA_CONAN_PROFILE}" "# Stroika's third-party components declare no settings: the configuration is an option (conanfile.py)\n[settings]\nos=${os_}\n")
    # What a component's own build, in Conan's cache, uses as this one does (conanfile.py): cmake, its generator - and Visual
    # Studio's platform, toolset and instance, or the make program (Ninja's) - and, with a multi-configuration generator
    # (Visual Studio's), the build type
    set (STROIKA_CONAN_CONF "user.stroika:cmake=${CMAKE_COMMAND}" "user.stroika:generator=${CMAKE_GENERATOR}")
    foreach (setting_ IN ITEMS GENERATOR_PLATFORM GENERATOR_TOOLSET GENERATOR_INSTANCE MAKE_PROGRAM)
        if (CMAKE_${setting_} AND NOT (setting_ STREQUAL "MAKE_PROGRAM" AND CMAKE_GENERATOR MATCHES "^Visual Studio"))
            string (TOLOWER "${setting_}" name_)
            list (APPEND STROIKA_CONAN_CONF "user.stroika:${name_}=${CMAKE_${setting_}}")
        endif ()
    endforeach ()
    get_property (multiConfig_ GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (multiConfig_)
        list (APPEND STROIKA_CONAN_CONF "user.stroika:build_config=${STROIKA_THIRD_PARTY_BUILD_TYPE}")
    endif ()
    # the lock each component's Conan steps take, in Conan's home: so builds sharing its cache - which is not safe for two
    # Conan commands at once - take turns
    execute_process (COMMAND "${STROIKA_CONAN}" config home OUTPUT_VARIABLE conanHome_ OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    file (TO_CMAKE_PATH "${conanHome_}" conanHome_)
    set (STROIKA_CONAN_LOCK "${conanHome_}/stroika-third-party.lock")
endif ()

#
# StroikaComponentFromConan (<directory>)
#
# The component in <directory>, from Conan's cache, copied into the configuration's Builds directory (StroikaDeploy.py) -
# built into the cache first, by its recipe here (conanfile.py), when the configuration has no package of it there yet. A
# target of its name, as StroikaThirdPartyComponent makes, for the components built after it (DEPENDS); it runs Conan again
# only once what builds the component changes. Conan's commands are in a script (conan.cmake, in the component's binary
# directory), which takes Conan's lock first.
#
function (StroikaComponentFromConan directory)
    file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt" version_ REGEX "^set \\(version_ ")
    string (REGEX REPLACE "^set \\(version_ ([^) ]+)\\).*" "\\1" version_ "${version_}")
    set (package_ stroika-${directory})
    # what builds it, as conanfile.py exports it
    file (GLOB_RECURSE recipe_ LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*")
    list (FILTER recipe_ EXCLUDE REGEX "/${directory}/(Makefile$|CURRENT|ORIGS|ORIGs)")
    set (binaryDir_ "${CMAKE_CURRENT_BINARY_DIR}/${directory}")
    set (export_ "${STROIKA_CONAN}" export "${CMAKE_CURRENT_SOURCE_DIR}" --name ${package_} --version ${version_})
    set (install_ "${STROIKA_CONAN}" install --requires ${package_}/${version_} --build missing
        -o "${package_}/*:configuration=${STROIKA_CONAN_CONFIGURATION_DIGEST}" -pr:a "${STROIKA_CONAN_PROFILE}"
        -c "user.stroika:toolchain_file=${CMAKE_TOOLCHAIN_FILE}" -c "user.stroika:origs_cache=${STROIKA_ORIGS_CACHE}")
    foreach (conf_ IN LISTS STROIKA_CONAN_CONF)
        list (APPEND install_ -c "${conf_}")
    endforeach ()
    list (APPEND install_ --deployer "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" --deployer-folder "${STROIKA_BUILDS_DIR}"
        --output-folder "${binaryDir_}")
    # each argument a bracket argument in the script, so none needs quoting
    list (JOIN export_ "]==] [==[" export_)
    list (JOIN install_ "]==] [==[" install_)
    # written only when changed (file (CONFIGURE)), so the script's time says when it last did
    file (CONFIGURE OUTPUT "${binaryDir_}/conan.cmake" @ONLY CONTENT "\
# ${package_} from Conan's cache - built there first, if the configuration has no package of it yet - copied into the\n\
# configuration's Builds directory: cmake -P, by ThirdPartyComponents/CMakeLists.txt's StroikaComponentFromConan, which wrote it.\n\
# MAKEFLAGS unset: Conan does not pass on make's job slots, so the component's own build takes CMAKE_BUILD_PARALLEL_LEVEL's jobs\n\
unset (ENV{MAKEFLAGS})\n\
unset (ENV{MFLAGS})\n\
file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE locked_)\n\
if (NOT locked_ EQUAL 0)\n\
    message (STATUS [==[${package_}: waiting for another build's Conan steps (${STROIKA_CONAN_LOCK})]==])\n\
    file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS)\n\
endif ()\n\
execute_process (COMMAND [==[${export_}]==] COMMAND_ERROR_IS_FATAL ANY)\n\
execute_process (COMMAND [==[${install_}]==] COMMAND_ERROR_IS_FATAL ANY)\n\
file (TOUCH [==[${binaryDir_}/conan-stamp]==])\n\
")
    add_custom_command (OUTPUT "${binaryDir_}/conan-stamp"
        COMMAND "${CMAKE_COMMAND}" -P "${binaryDir_}/conan.cmake"
        DEPENDS "${binaryDir_}/conan.cmake" ${recipe_} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
                "${CMAKE_CURRENT_SOURCE_DIR}/RemoveFiles.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" "${CMAKE_TOOLCHAIN_FILE}" "${configurationDir_}/Stroika-Configuration.cmake"
        USES_TERMINAL
        VERBATIM
    )
    add_custom_target (${directory} ALL DEPENDS "${binaryDir_}/conan-stamp")
endfunction ()

#
# StroikaComponent (<directory> <feature>)
#
# The component in <directory>, where the configuration uses it (configure's --<feature> use): from Conan's cache, if it is
# one Conan keeps (STROIKA_THIRD_PARTY_FROM_CONAN), else built here. Building one for Conan (conanfile.py): just that one.
#
function (StroikaComponent directory feature)
    if (NOT STROIKA_FEATURE_${feature} STREQUAL "use")
        return ()
    endif ()
    if (DEFINED STROIKA_THIRD_PARTY_ONLY)
        if (directory STREQUAL STROIKA_THIRD_PARTY_ONLY)
            add_subdirectory (${directory})
        endif ()
    elseif (directory IN_LIST STROIKA_THIRD_PARTY_FROM_CONAN)
        StroikaComponentFromConan (${directory})
    else ()
        add_subdirectory (${directory})
    endif ()
endfunction ()

# the components the configuration uses - each after those it depends on
StroikaComponent (zlib ZLib)
StroikaComponent (zstd zstd)
StroikaComponent (StrawberryPerl StrawberryPerl)
StroikaComponent (openssl OpenSSL)
StroikaComponent (curl LibCurl)
StroikaComponent (boost boost)
StroikaComponent (mongo-cxx-driver mongocxxdriver)
StroikaComponent (libxml2 libxml2)
StroikaComponent (Xerces Xerces)
StroikaComponent (sqlite sqlite)
StroikaComponent (lzma LZMA)
StroikaComponent (fmtlib fmtlib)
StroikaComponent (GoogleTest GoogleTest)
StroikaComponent (WIX WIX)
