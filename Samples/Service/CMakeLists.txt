StroikaMakeVersionFile ("${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h")
add_executable (Service Sources/Main.cpp Sources/Service.cpp)
target_include_directories (Service PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (Service PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (Service)
StroikaInstallers (Service NAME Stroika-SampleService PACKAGE stroika-sampleservice VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION"
    OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-Service" WIX_PROGRAM_NAME Stroika-SampleService-Service.exe)
