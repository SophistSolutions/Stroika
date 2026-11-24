StroikaMakeVersionFile ("${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h")
add_executable (WebServer Sources/WebServer.cpp)
target_include_directories (WebServer PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (WebServer PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (WebServer)

# and the files it serves, from html/ beside it (CMAKE_RUNTIME_OUTPUT_DIRECTORY: Samples/CMakeLists.txt) - each copied
# only when it changes, or is missing there
file (GLOB_RECURSE htmlFiles_ CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sample-html-folder" "${CMAKE_CURRENT_SOURCE_DIR}/sample-html-folder/*")
set (copies_)
foreach (file_ IN LISTS htmlFiles_)
    set (copy_ "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/html/${file_}")
    add_custom_command (OUTPUT "${copy_}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/sample-html-folder/${file_}" "${copy_}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/sample-html-folder/${file_}"
        VERBATIM
    )
    list (APPEND copies_ "${copy_}")
endforeach ()
add_custom_target (WebServer-html ALL DEPENDS ${copies_})
