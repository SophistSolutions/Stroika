#
# HTMLUI's backend, as its Makefile builds it: the program HTMLUI, in Samples-HTMLUI/Backend, with its OpenAPI
# specification (../OpenAPI/api.json) embedded
#
StroikaMakeVersionFile ("${versionFile_}" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h") # (../CMakeLists.txt)
add_executable (HTMLUI
    Sources/AppConfiguration.cpp Sources/IWSAPI.cpp Sources/Main.cpp Sources/Model.cpp Sources/OperationalStatistics.cpp
    Sources/Service.cpp Sources/WebServer.cpp Sources/WSImpl.cpp
)
target_include_directories (HTMLUI PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (HTMLUI PRIVATE Stroika::Frameworks)
# (a generator expression, though empty, keeps a Visual Studio generator from adding a directory for the build type)
set_target_properties (HTMLUI PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-HTMLUI/Backend$<0:>")
StroikaEmbedFiles (HTMLUI RELATIVE_TO ../OpenAPI api.json)
StroikaCopyFilesToExeDir (HTMLUI)
