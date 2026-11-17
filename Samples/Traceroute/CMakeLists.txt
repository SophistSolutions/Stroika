add_executable (Traceroute Sources/Traceroute.cpp)
target_link_libraries (Traceroute PRIVATE stroika-frameworks)
StroikaCopyFilesToExeDir (Traceroute)
