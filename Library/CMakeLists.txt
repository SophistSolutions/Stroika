#
# Stroika's libraries built with CMake - beside the make build, which this does not change (#1157). For one Stroika
# configuration, given its toolchain file:
#     cmake -S Library -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
# So far the Foundation library only, compiled against the third-party components make built for the configuration.
#
cmake_minimum_required (VERSION 3.22)
project (Stroika LANGUAGES C CXX)

# the configuration's Stroika settings, which ApplyConfiguration writes beside its toolchain file
if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")

# the third-party components make built for the configuration, ahead of any installed elsewhere
list (PREPEND CMAKE_PREFIX_PATH "${STROIKA_BUILDS_DIR}")

add_subdirectory (Sources/Stroika/Foundation)
