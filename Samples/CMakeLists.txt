#
# The samples, each built as its Makefile builds it: a program named for its directory, from its Sources/, in
# Samples-<name> in the configuration's Builds directory. Not here (make builds them): HTMLUI, a Quasar web app with its
# backend; Service's installers; and the Windows-only LedIt, LedLineIt and SimpleLedTest, with their resource files (and MFC).
#
foreach (name_ IN ITEMS AppSettings ArchiveUtility Containers DocumentDB Serialization Service SQL SSDPClient SSDPServer
                        SystemPerformanceClient Traceroute WebServer WebService)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-${name_}$<0:>") # $<0:>: Library/CMakeLists.txt
    add_subdirectory (${name_})
endforeach ()
