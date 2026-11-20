#
# Stroika's samples, each built as a program built on Stroika is - part of Stroika's build (the top CMakeLists.txt;
# make's target samples), or a project by itself, using Stroika's package (find_package (Stroika),
# Library/StroikaConfig.cmake.in), which a configuration's build writes into its Builds directory - once the
# configuration's libraries are built:
#     cmake -S Samples -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#           -DCMAKE_PREFIX_PATH=<the configuration's Builds directory>
# Each is built as its Makefile builds it: a program named for its directory, from its Sources/, in Samples-<name> in the
# configuration's Builds directory - the ones Samples/Makefile builds for the configuration: LedIt and LedLineIt given MFC
# (ATLMFC), SimpleLedTest with Visual Studio. HTMLUI is its backend and the web app it serves, and HTMLUI and Service
# build installers too (their CMakeLists.txt say how).
#
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required (VERSION 3.22)
    project (StroikaSamples LANGUAGES C CXX)
    find_package (Stroika REQUIRED)
endif ()

# with a Visual Studio generator, a project's files compiled at once (Library/CMakeLists.txt says why)
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (CMAKE_VS_GLOBALS "UseMultiToolTask=true" "EnforceProcessCountAcrossBuilds=true")
endif ()

set (samples_ AppSettings ArchiveUtility Containers DocumentDB HTMLUI)
if (STROIKA_FEATURE_ATLMFC STREQUAL "use-system")
    list (APPEND samples_ LedIt LedLineIt)
endif ()
list (APPEND samples_ Serialization)
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    list (APPEND samples_ SimpleLedTest)
endif ()
list (APPEND samples_ Service SQL SSDPClient SSDPServer SystemPerformanceClient Traceroute WebServer WebService)
foreach (name_ IN LISTS samples_)
    # a generator expression, though empty, keeps a Visual Studio generator from adding a directory for the build type
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-${name_}$<0:>")
    add_subdirectory (${name_})
endforeach ()

# make's target samples, which builds what all builds of them: each target here and below, but those left out of all
set (targets_)
set (directories_ "${CMAKE_CURRENT_SOURCE_DIR}")
while (directories_)
    list (POP_FRONT directories_ directory_)
    get_property (subdirectories_ DIRECTORY "${directory_}" PROPERTY SUBDIRECTORIES)
    list (APPEND directories_ ${subdirectories_})
    get_property (directoryTargets_ DIRECTORY "${directory_}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach (target_ IN LISTS directoryTargets_)
        get_target_property (excluded_ ${target_} EXCLUDE_FROM_ALL)
        if (NOT excluded_)
            list (APPEND targets_ ${target_})
        endif ()
    endforeach ()
endwhile ()
add_custom_target (samples)
add_dependencies (samples ${targets_})
