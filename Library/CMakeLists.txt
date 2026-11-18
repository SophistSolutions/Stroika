#
# Stroika's libraries built with CMake, as make builds them (#1157). For one Stroika configuration, given its toolchain file:
#     cmake -S Library -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
# (or the top CMakeLists.txt, by the configuration's preset: its third-party components, then this). It builds the
# Foundation and Frameworks libraries, the tools (Tools/CMakeLists.txt), the regression tests (which ctest runs) and the
# samples (Samples/CMakeLists.txt says which), against the third-party components installed for the configuration - by
# make, or by ThirdPartyComponents/CMakeLists.txt - and puts each where make puts it, in the configuration's Builds
# directory.
#
cmake_minimum_required (VERSION 3.22)
project (Stroika LANGUAGES C CXX)

# the configuration's Stroika settings, which ApplyConfiguration writes beside its toolchain file
if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")

# the configuration's C++ standard, which its flags name too: else a package asking for a lower one (googletest) makes
# cmake add that standard's flag after the configuration's - overriding it where the compiler's default is lower (MSVC)
if (STROIKA_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD ${STROIKA_CXX_STANDARD})
    set (CMAKE_CXX_STANDARD_REQUIRED ON)
    set (CMAKE_CXX_EXTENSIONS OFF)
endif ()

# the third-party components installed for the configuration, ahead of any installed elsewhere
list (PREPEND CMAKE_PREFIX_PATH "${STROIKA_BUILDS_DIR}")
# and their include directories as -I, as make gives them, not -isystem: compilers search CPATH's directories ahead of
# -isystem ones - so given CPATH=/opt/homebrew/include (a common Mac shell setting), Homebrew's OpenSSL headers would win
# over the configuration's own
set (CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../Build/Lib/CMake")
include (StroikaCopyFilesToExeDir)
include (StroikaEmbedFiles)
include (StroikaMakeVersionFile)

# the libraries where make puts them, in the configuration's Builds directory (the tests and samples: their CMakeLists.txt) -
# a generator expression, though empty, keeps a Visual Studio generator from adding a directory for the build type
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}lib$<0:>")

# with a Visual Studio generator, a project's files compiled at once, as make -jN compiles them - up to N compiles in all,
# msbuild's -m:N (cmake --build's -jN): else msbuild compiles each project's files one at a time
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (CMAKE_VS_GLOBALS "UseMultiToolTask=true" "EnforceProcessCountAcrossBuilds=true")
endif ()

add_subdirectory (Sources/Stroika/Foundation)
add_subdirectory (Sources/Stroika/Frameworks)

# the tools, Tools/
add_subdirectory (../Tools Tools)

# the regression tests, Tests/NN
enable_testing ()
add_subdirectory (../Tests Tests)

# the samples, Samples/<name>
add_subdirectory (../Samples Samples)
