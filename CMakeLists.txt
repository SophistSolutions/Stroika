#
# Stroika's whole CMake build of one configuration, as make builds it - one CMake project: its third-party components,
# built as it is configured (ThirdPartyComponents/CMakeLists.txt, each installed into the configuration's Builds
# directory, where the rest finds it), then Stroika's libraries, tools and documentation (Library/), the regression
# tests (Tests/) and the samples (Samples/). The libraries go where make puts them, with Stroika's CMake package, which
# a program built on Stroika uses - as the tests and the samples do, each built by itself (their CMakeLists.txt say
# how). So an IDE that reads CMake's presets (Visual Studio, VS Code's CMake Tools) sees every source file and program.
#
# Each configuration has a preset of its name (CMakeUserPresets.json, which ApplyConfiguration writes), giving its
# toolchain file, generator and build directory (IntermediateFiles/<configuration>/CMake):
#     cmake --preset <configuration>
#     cmake --build --preset <configuration> -j8 [--target libraries|tools|tests|samples|documentation|Test07|...]
#     ctest --preset <configuration>
#
cmake_minimum_required (VERSION 3.22)
# no languages yet: the third-party components are built first (below), with the configuration's compilers
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

# OFF: Stroika built against the components installed already (by make, or an earlier build)
option (STROIKA_BUILD_THIRD_PARTY_COMPONENTS "build the configuration's third-party components (else use those installed)" ON)
# OFF where only the libraries are wanted - an application's build of its Stroika (ApplyConfiguration)
option (STROIKA_BUILD_TESTS "build Stroika's regression tests (Tests/)" ON)
option (STROIKA_BUILD_SAMPLES "build Stroika's samples (Samples/)" ON)

# The third-party components, built now - as vcpkg builds a project's packages as it is configured - so that their
# packages are there to find: a CMake build of their own, in ThirdParty/ (short: ThirdPartyComponents/CMakeLists.txt
# says why), with this build's generator, and the jobs CMAKE_BUILD_PARALLEL_LEVEL gives (make's -jN sets it). Once
# built, up to date in moments; make configures this build each time, so checks them each time - and editing their
# recipes configures this build again (CMAKE_CONFIGURE_DEPENDS), wherever it is built from.
if (STROIKA_BUILD_THIRD_PARTY_COMPONENTS)
    set (thirdPartyDir_ "${CMAKE_BINARY_DIR}/ThirdParty")
    if (NOT EXISTS "${thirdPartyDir_}/CMakeCache.txt")
        set (generator_ -G "${CMAKE_GENERATOR}")
        if (CMAKE_GENERATOR_PLATFORM)
            list (APPEND generator_ -A "${CMAKE_GENERATOR_PLATFORM}")
        endif ()
        if (CMAKE_GENERATOR_TOOLSET)
            list (APPEND generator_ -T "${CMAKE_GENERATOR_TOOLSET}")
        endif ()
        if (CMAKE_GENERATOR_INSTANCE)
            list (APPEND generator_ "-DCMAKE_GENERATOR_INSTANCE=${CMAKE_GENERATOR_INSTANCE}")
        endif ()
        if (CMAKE_MAKE_PROGRAM AND NOT CMAKE_GENERATOR MATCHES "^Visual Studio")
            list (APPEND generator_ "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}")
        endif ()
        execute_process (
            COMMAND "${CMAKE_COMMAND}" ${generator_} -S "${CMAKE_SOURCE_DIR}/ThirdPartyComponents" -B "${thirdPartyDir_}"
                    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif ()
    set (buildArgs_)
    if (CMAKE_GENERATOR MATCHES "^Visual Studio")
        # the configuration's build type, as its preset builds it - and as many jobs as the machine has processors
        # (msbuild -m): msbuild has no job slots to share
        set (buildArgs_ --config Release --parallel)
        if (STROIKA_ASSERTIONS_ENABLED)
            set (buildArgs_ --config Debug --parallel)
        endif ()
    endif ()
    execute_process (COMMAND "${CMAKE_COMMAND}" --build "${thirdPartyDir_}" ${buildArgs_} COMMAND_ERROR_IS_FATAL ANY)
    set (thirdPartySources_ "${CMAKE_SOURCE_DIR}/ThirdPartyComponents")
    file (GLOB recipes_ "${thirdPartySources_}/*.cmake" "${thirdPartySources_}/*/*.cmake" "${thirdPartySources_}/CMakeLists.txt"
        "${thirdPartySources_}/*/CMakeLists.txt" "${thirdPartySources_}/*/*/CMakeLists.txt" "${thirdPartySources_}/*/Patches/*")
    set_property (DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${recipes_})
endif ()

enable_language (C)
enable_language (CXX)

# the configuration's build type, C++ standard and third-party components, as a program using Stroika's package takes
# them, and Stroika's CMake functions - for all the directories below
include (StroikaBuildSettings)
include (StroikaDependencies)
include (StroikaCopyFilesToExeDir)
include (StroikaEmbedFiles)
include (StroikaInstallers)
include (StroikaMakeVersionFile)
include (StroikaVersion)

enable_testing ()

# make's targets, each building that part: libraries, tools and documentation (Library/), tests (Tests/), samples
# (Samples/) - and third-party-components, built already, as this build is configured
add_subdirectory (Library)
if (STROIKA_BUILD_TESTS)
    add_subdirectory (Tests)
endif ()
if (STROIKA_BUILD_SAMPLES)
    add_subdirectory (Samples)
endif ()
add_custom_target (third-party-components)
