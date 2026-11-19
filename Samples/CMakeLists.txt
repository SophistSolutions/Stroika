#
# Stroika's samples, built as a program built on Stroika builds - a CMake project of its own, using Stroika's package
# (find_package (Stroika), Library/StroikaConfig.cmake.in), which a configuration's build writes into its Builds directory.
# For one configuration, once its libraries are built (the top CMakeLists.txt builds this after them, as its target samples):
#     cmake -S Samples -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#           -DCMAKE_PREFIX_PATH=<the configuration's Builds directory>
# Each is built as its Makefile builds it: a program named for its directory, from its Sources/, in Samples-<name> in the
# configuration's Builds directory - the ones Samples/Makefile builds for the configuration: LedIt and LedLineIt given MFC
# (ATLMFC), SimpleLedTest with Visual Studio. HTMLUI is its backend and the web app it serves, and HTMLUI and Service
# build installers too (their CMakeLists.txt say how).
#
cmake_minimum_required (VERSION 3.22)
project (StroikaSamples LANGUAGES C CXX)

find_package (Stroika REQUIRED)

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
