#
# Stroika's whole CMake build of one configuration, as make builds it: first the third-party components the configuration
# uses (ThirdPartyComponents/CMakeLists.txt - each installed into the configuration's Builds directory), then Stroika's
# libraries, tests and samples (Library/CMakeLists.txt), each a CMake project of its own, with its own build directory
# inside this one:
#     cmake -S Build/Superbuild -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#     cmake --build <build directory> -j8                         (a Visual Studio generator: --config Debug, or Release)
#     ctest --test-dir <build directory>/Stroika -j8 --output-on-failure
# Stroika's build directory, <build directory>/Stroika, is also the one to open in an IDE, once this has built the
# components. With a Makefiles generator, the inner builds share that -j's job slots.
#
cmake_minimum_required (VERSION 3.22)
project (StroikaSuperbuild LANGUAGES NONE)

if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
include (ExternalProject)
get_filename_component (sourceRoot_ "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# each step built every time, as make visits each directory: the project's own build decides what is out of date
ExternalProject_Add (ThirdPartyComponents
    SOURCE_DIR "${sourceRoot_}/ThirdPartyComponents"
    BINARY_DIR "${CMAKE_BINARY_DIR}/ThirdPartyComponents"
    CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    INSTALL_COMMAND ""
    BUILD_ALWAYS ON
)
ExternalProject_Add (Stroika
    SOURCE_DIR "${sourceRoot_}/Library"
    BINARY_DIR "${CMAKE_BINARY_DIR}/Stroika"
    CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    INSTALL_COMMAND ""
    BUILD_ALWAYS ON
    DEPENDS ThirdPartyComponents
)
