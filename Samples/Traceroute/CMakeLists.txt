add_executable (Traceroute Sources/Traceroute.cpp)
target_link_libraries (Traceroute PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (Traceroute)
