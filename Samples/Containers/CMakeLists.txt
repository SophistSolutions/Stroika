add_executable (Containers Sources/Collection.cpp Sources/Main.cpp Sources/Mapping.cpp Sources/Sequence.cpp Sources/Set.cpp)
target_link_libraries (Containers PRIVATE Stroika::Foundation)
StroikaCopyFilesToExeDir (Containers)
