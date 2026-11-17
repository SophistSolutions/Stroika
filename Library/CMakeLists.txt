#
# Stroika's libraries built with CMake - beside the make build, which this does not change (#1157). For one Stroika
# configuration, given its toolchain file:
#     cmake -S Library -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
# It builds the Foundation and Frameworks libraries, the regression tests (which ctest runs) and the samples
# (Samples/CMakeLists.txt says which), against the third-party components make built for the configuration.
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

# the third-party components make built for the configuration, ahead of any installed elsewhere
list (PREPEND CMAKE_PREFIX_PATH "${STROIKA_BUILDS_DIR}")
# and their include directories as -I, as make gives them, not -isystem: compilers search CPATH's directories ahead of
# -isystem ones - so given CPATH=/opt/homebrew/include (a common Mac shell setting), Homebrew's OpenSSL headers would win
# over the configuration's own
set (CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../Build/Lib/CMake")
include (StroikaCopyFilesToExeDir)
include (StroikaEmbedFiles)
include (StroikaMakeVersionFile)

add_subdirectory (Sources/Stroika/Foundation)
add_subdirectory (Sources/Stroika/Frameworks)

# the regression tests, Tests/NN
enable_testing ()
add_subdirectory (../Tests Tests)

# the samples, Samples/<name>
add_subdirectory (../Samples Samples)
