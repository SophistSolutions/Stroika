add_executable (Serialization Sources/Main.cpp Sources/ObjectVariantMapper.cpp)
target_link_libraries (Serialization PRIVATE stroika-foundation)
StroikaCopyFilesToExeDir (Serialization)
