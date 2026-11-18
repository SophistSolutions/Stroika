#
# Stroika's whole CMake build of one configuration, as make builds it - compiling nothing itself, it runs two builds in
# turn: the third-party components the configuration uses (ThirdPartyComponents/CMakeLists.txt, each installed into the
# configuration's Builds directory), then Stroika's libraries, tests and samples (Library/CMakeLists.txt, which puts them
# there too, as make does). Each is a CMake project of its own, with its own build directory inside this one.
#
# Each configuration has a preset of its name (CMakeUserPresets.json, which ApplyConfiguration writes), giving its
# toolchain file, generator and build directory (IntermediateFiles/<configuration>/CMake):
#     cmake --preset <configuration>
#     CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build --preset <configuration> [--target libraries|tools|tests|samples|documentation|third-party-components]
#     ctest --preset <configuration> -j8
# The jobs (make's -jN sets CMAKE_BUILD_PARALLEL_LEVEL): with a Makefiles generator, the inner builds share make's job
# slots, as under make; with Ninja, each inner build - one at a time - takes CMAKE_BUILD_PARALLEL_LEVEL's (a -j would
# reach only this build: each inner one would take Ninja's default, a job per processor and two more); with msbuild, as
# many as the machine has processors (below).
#
cmake_minimum_required (VERSION 3.22)
project (Stroika LANGUAGES NONE)

if (NOT CMAKE_TOOLCHAIN_FILE)
    message (FATAL_ERROR "No configuration: use its preset (cmake --preset <configuration>), or give its toolchain file, -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake")
endif ()
include (ExternalProject)

# OFF: Stroika built against the components installed already (by make, or an earlier build)
option (STROIKA_BUILD_THIRD_PARTY_COMPONENTS "build the configuration's third-party components (else use those installed)" ON)

# Each step built every time, as make visits each directory: the project's own build decides what is out of date. In the
# terminal (USES_TERMINAL_BUILD): its output as it comes - under Ninja, not all at its end. Under a Visual Studio
# generator, with as many jobs as the machine has processors (msbuild -m): msbuild has no job slots to share, so a -j
# given this build reaches no step's
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
        USES_TERMINAL_BUILD ON
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
    USES_TERMINAL_BUILD ON
    ${dependsOn_}
    STEP_TARGETS configure
)

# make's targets that build part of it, each by its name: third-party-components, and the Stroika project's own targets
# libraries, tools, tests, samples and documentation - each built after that project's configure step (so after the
# components), with the outer build's job slots under make (its $(MAKE), as ExternalProject's own steps)
add_custom_target (third-party-components)
if (STROIKA_BUILD_THIRD_PARTY_COMPONENTS)
    add_dependencies (third-party-components ThirdPartyComponents)
endif ()
if (CMAKE_GENERATOR MATCHES "Makefiles")
    set (buildTarget_ "$(MAKE)" -C "${CMAKE_BINARY_DIR}/Stroika")
elseif (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (buildTarget_ "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/Stroika" --config $<CONFIG> --parallel --target)
else ()
    set (buildTarget_ "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/Stroika" --target)
endif ()
foreach (target_ libraries tools tests samples documentation)
    add_custom_target (${target_} COMMAND ${buildTarget_} ${target_} USES_TERMINAL)
    add_dependencies (${target_} Stroika-configure)
endforeach ()

# ctest here runs Stroika's regression tests, which its own build directory lists
enable_testing ()
set_property (DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_BINARY_DIR}/Stroika/Tests/CTestTestfile.cmake")
