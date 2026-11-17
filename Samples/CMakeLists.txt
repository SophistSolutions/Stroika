#
# The samples, each built as its Makefile builds it: a program named for its directory, from its Sources/. Not here (make
# builds them): HTMLUI, a Quasar web app with its backend; Service's installers; and the Windows-only LedIt, LedLineIt and
# SimpleLedTest, with their resource files (and MFC).
#
add_subdirectory (AppSettings)
add_subdirectory (ArchiveUtility)
add_subdirectory (Containers)
add_subdirectory (DocumentDB)
add_subdirectory (Serialization)
add_subdirectory (Service)
add_subdirectory (SQL)
add_subdirectory (SSDPClient)
add_subdirectory (SSDPServer)
add_subdirectory (SystemPerformanceClient)
add_subdirectory (Traceroute)
add_subdirectory (WebServer)
add_subdirectory (WebService)
