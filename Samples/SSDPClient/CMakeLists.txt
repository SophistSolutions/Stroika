add_executable (SSDPClient Sources/SSDPClient.cpp)
target_link_libraries (SSDPClient PRIVATE stroika-frameworks)
StroikaCopyFilesToExeDir (SSDPClient)
