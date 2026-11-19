#
# HTMLUI's backend, as its Makefile builds it: the program HTMLUI, in Samples-HTMLUI/Backend, with its OpenAPI
# specification (../OpenAPI/api.json) embedded
#
StroikaMakeVersionFile ("${CMAKE_CURRENT_SOURCE_DIR}/../../../STROIKA_VERSION" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h")
add_executable (HTMLUI
    Sources/AppConfiguration.cpp Sources/IWSAPI.cpp Sources/Main.cpp Sources/Model.cpp Sources/OperationalStatistics.cpp
    Sources/Service.cpp Sources/WebServer.cpp Sources/WSImpl.cpp
)
target_include_directories (HTMLUI PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (HTMLUI PRIVATE Stroika::Frameworks)
set_target_properties (HTMLUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-HTMLUI/Backend$<0:>") # $<0:>: Samples/CMakeLists.txt
StroikaEmbedFiles (HTMLUI RELATIVE_TO ../OpenAPI api.json)
StroikaCopyFilesToExeDir (HTMLUI)
