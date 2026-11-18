#
# Stroika's whole CMake build of one configuration, as make builds it - compiling nothing itself, it runs two builds in
# turn: the third-party components the configuration uses (ThirdPartyComponents/CMakeLists.txt, each installed into the
# configuration's Builds directory), then Stroika's libraries, tests and samples (Library/CMakeLists.txt, which puts them
# there too, as make does). Each is a CMake project of its own, with its own build directory inside this one.
#
# Each configuration has a preset of its name (CMakeUserPresets.json, which ApplyConfiguration writes), giving its
# toolchain file, generator and build directory (IntermediateFiles/<configuration>/CMake):
#     cmake --preset <configuration>
#     cmake --build --preset <configuration> -j8
#     ctest --preset <configuration> -j8
# With a Makefiles generator, the inner builds share that -j's job slots, as under make.
#
cmake_minimum_required (VERSION 3.22)
project (Stroika LANGUAGES NONE)

if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: use its preset (cmake --preset <configuration>), or give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
include (ExternalProject)

# OFF: Stroika built against the components installed already (by make, or an earlier build)
option (STROIKA_BUILD_THIRD_PARTY_COMPONENTS "build the configuration's third-party components (else use those installed)" ON)

# Each step built every time, as make visits each directory: the project's own build decides what is out of date. Under a
# Visual Studio generator, with as many jobs as the machine has processors (msbuild -m): msbuild has no job slots to share,
# so a -j given this build reaches no step's
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (buildCommand_ BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config $<CONFIG> --parallel)
endif ()
set (dependsOn_)
if (STROIKA_BUILD_THIRD_PARTY_COMPONENTS)
    ExternalProject_Add (ThirdPartyComponents
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/ThirdPartyComponents"
        BINARY_DIR "${CMAKE_BINARY_DIR}/ThirdParty" # short: ThirdPartyComponents/CMakeLists.txt says why

        CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        ${buildCommand_}
        INSTALL_COMMAND ""
        BUILD_ALWAYS ON
    )
    set (dependsOn_ DEPENDS ThirdPartyComponents)
endif ()
ExternalProject_Add (Stroika
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Library"
    BINARY_DIR "${CMAKE_BINARY_DIR}/Stroika"
    CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    ${buildCommand_}
    INSTALL_COMMAND ""
    BUILD_ALWAYS ON
    ${dependsOn_}
)

# ctest here runs Stroika's regression tests, which its own build directory lists
enable_testing ()
set_property (DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_BINARY_DIR}/Stroika/Tests/CTestTestfile.cmake")
