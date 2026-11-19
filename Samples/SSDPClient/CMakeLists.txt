add_executable (SSDPClient Sources/SSDPClient.cpp)
target_link_libraries (SSDPClient PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (SSDPClient)
