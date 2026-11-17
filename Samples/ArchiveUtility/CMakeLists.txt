add_executable (ArchiveUtility Sources/ArchiveUtility.cpp)
target_link_libraries (ArchiveUtility PRIVATE stroika-foundation)
StroikaCopyFilesToExeDir (ArchiveUtility)
