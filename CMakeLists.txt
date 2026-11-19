#
# Stroika's whole CMake build of one configuration, as make builds it - compiling nothing itself, it runs these builds in
# turn: the third-party components the configuration uses (ThirdPartyComponents/CMakeLists.txt, each installed into the
# configuration's Builds directory), then Stroika's libraries and tools (Library/CMakeLists.txt, which puts them there
# too, as make does, with Stroika's CMake package), then - once the libraries are built - the regression tests and the
# samples (Tests/CMakeLists.txt and Samples/CMakeLists.txt, each built on that package, as a program using Stroika builds).
# Each is a CMake project of its own, with its own build directory inside this one.
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

# The configuration as applied must be current: else the build stops, saying how (StroikaCheckConfiguration)
get_filename_component (configurationDir_ "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
include ("${configurationDir_}/Stroika-Configuration.cmake")
list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Build/Lib/CMake")
include (StroikaCheckConfiguration)
StroikaCheckConfiguration ("${CMAKE_TOOLCHAIN_FILE}")

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

# A target of the Stroika project's built by itself, after its configure step (so after the components) - with the outer
# build's job slots under make (its $(MAKE), as ExternalProject's own steps)
if (CMAKE_GENERATOR MATCHES "Makefiles")
    set (buildTarget_ "$(MAKE)" -C "${CMAKE_BINARY_DIR}/Stroika")
elseif (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (buildTarget_ "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/Stroika" --config $<CONFIG> --parallel --target)
else ()
    set (buildTarget_ "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/Stroika" --target)
endif ()

# Its libraries first, a step of their own (Stroika-libraries), which the tests and samples need - not all of it
ExternalProject_Add_Step (Stroika libraries
    COMMAND ${buildTarget_} libraries
    DEPENDEES configure
    DEPENDERS build
    ALWAYS ON
    USES_TERMINAL ON
)
ExternalProject_Add_StepTargets (Stroika libraries)

# The regression tests and the samples, each a project built on Stroika's package, which finds it in the configuration's
# Builds directory (as a program using Stroika would) - make's targets tests and samples
foreach (directory_ Tests Samples)
    string (TOLOWER "${directory_}" target_)
    ExternalProject_Add (${target_}
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/${directory_}"
        BINARY_DIR "${CMAKE_BINARY_DIR}/${directory_}"
        CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}" "-DCMAKE_PREFIX_PATH=${STROIKA_BUILDS_DIR}"
        ${buildCommand_}
        INSTALL_COMMAND ""
        BUILD_ALWAYS ON
        USES_TERMINAL_BUILD ON
        DEPENDS Stroika-libraries
    )
endforeach ()

# make's targets that build part of it, each by its name: third-party-components, libraries, tests and samples (above),
# and the Stroika project's own targets tools and documentation
add_custom_target (third-party-components)
if (STROIKA_BUILD_THIRD_PARTY_COMPONENTS)
    add_dependencies (third-party-components ThirdPartyComponents)
endif ()
add_custom_target (libraries)
add_dependencies (libraries Stroika-libraries)
foreach (target_ tools documentation)
    add_custom_target (${target_} COMMAND ${buildTarget_} ${target_} USES_TERMINAL)
    add_dependencies (${target_} Stroika-configure)
endforeach ()

# ctest here runs Stroika's regression tests, which their own build directory lists - once that is configured (make tests,
# or all): before, ctest finds none, rather than stopping at the missing list
enable_testing ()
file (CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/StroikaTests.cmake" CONTENT "include (\"${CMAKE_BINARY_DIR}/Tests/CTestTestfile.cmake\" OPTIONAL)\n")
set_property (DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_BINARY_DIR}/StroikaTests.cmake")
