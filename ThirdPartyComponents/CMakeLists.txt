#
# The configuration's third-party components, built with CMake beside make (#1157). Each <name>/CMakeLists.txt builds one
# as its Makefile does: its source archive from Origs-Cache (downloaded there when missing), configured with the
# configuration's toolchain file, built, and installed into the configuration's Builds directory (STROIKA_BUILDS_DIR) -
# where Stroika's own CMake build (Library/CMakeLists.txt) finds it. For one configuration:
#     cmake -S ThirdPartyComponents -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#     cmake --build <build directory> -j8
# With a Makefiles generator, the components' own builds share that -j's job slots, as under make. OpenSSL, boost and
# mongo-cxx-driver build here on UNIX only - with Visual Studio, their Makefiles build them - as does curl, which Stroika
# uses only on UNIX.
#
cmake_minimum_required (VERSION 3.22)
project (StroikaThirdPartyComponents LANGUAGES NONE)

# the configuration's Stroika settings, which ApplyConfiguration writes beside its toolchain file
if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")

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

set (STROIKA_ORIGS_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/Origs-Cache")
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
    if (arg_BUILD_IN_SOURCE)
        list (APPEND passOn_ BUILD_IN_SOURCE ON)
    endif ()
    if (EXISTS "${STROIKA_ORIGS_CACHE}/${arg_ARCHIVE}")
        set (download_ URL "${STROIKA_ORIGS_CACHE}/${arg_ARCHIVE}")
    else ()
        set (download_ URL ${arg_URLS} DOWNLOAD_DIR "${STROIKA_ORIGS_CACHE}" DOWNLOAD_NAME "${arg_ARCHIVE}")
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
    ExternalProject_Add (${name} ${download_} ${configure_}
        BUILD_COMMAND ${build_}
        INSTALL_COMMAND ${install_}
        LOG_DOWNLOAD ON LOG_CONFIGURE ON LOG_BUILD ON LOG_INSTALL ON LOG_OUTPUT_ON_FAILURE ON
        ${passOn_}
    )
endfunction ()

# the components the configuration uses (configure's --<component> use) - each after those it depends on
if (STROIKA_FEATURE_ZLib STREQUAL "use")
    add_subdirectory (zlib)
endif ()
if (STROIKA_FEATURE_zstd STREQUAL "use")
    add_subdirectory (zstd)
endif ()
if (STROIKA_FEATURE_OpenSSL STREQUAL "use")
    add_subdirectory (openssl)
endif ()
if (STROIKA_FEATURE_LibCurl STREQUAL "use")
    add_subdirectory (curl)
endif ()
if (STROIKA_FEATURE_boost STREQUAL "use")
    add_subdirectory (boost)
endif ()
if (STROIKA_FEATURE_mongocxxdriver STREQUAL "use")
    add_subdirectory (mongo-cxx-driver)
endif ()
if (STROIKA_FEATURE_libxml2 STREQUAL "use")
    add_subdirectory (libxml2)
endif ()
if (STROIKA_FEATURE_Xerces STREQUAL "use")
    add_subdirectory (Xerces)
endif ()
if (STROIKA_FEATURE_sqlite STREQUAL "use")
    add_subdirectory (sqlite)
endif ()
if (STROIKA_FEATURE_LZMA STREQUAL "use")
    add_subdirectory (lzma)
endif ()
if (STROIKA_FEATURE_fmtlib STREQUAL "use")
    add_subdirectory (fmtlib)
endif ()
if (STROIKA_FEATURE_GoogleTest STREQUAL "use")
    add_subdirectory (GoogleTest)
endif ()
