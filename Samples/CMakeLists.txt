#
# The samples, each built as its Makefile builds it: a program named for its directory, from its Sources/, in
# Samples-<name> in the configuration's Builds directory - the ones Samples/Makefile builds for the configuration: LedIt
# and LedLineIt given MFC (ATLMFC), SimpleLedTest with Visual Studio. HTMLUI is its backend and the web app it serves, and
# HTMLUI and Service build installers too (their CMakeLists.txt say how).
#
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
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-${name_}$<0:>") # $<0:>: Library/CMakeLists.txt
    add_subdirectory (${name_})
endforeach ()
