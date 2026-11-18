StroikaMakeVersionFile ("${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h")
add_executable (WebServer Sources/WebServer.cpp)
target_include_directories (WebServer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (WebServer PRIVATE stroika-frameworks)
StroikaCopyFilesToExeDir (WebServer)

# and the files it serves, from html/ beside it - copied each build, as make does
add_custom_target (WebServer-html ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/sample-html-folder" "$<TARGET_FILE_DIR:WebServer>/html"
    VERBATIM
)
