add_executable (SSDPServer Sources/SSDPServer.cpp)
target_link_libraries (SSDPServer PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (SSDPServer)
