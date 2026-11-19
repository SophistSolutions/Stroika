add_executable (ArchiveUtility Sources/ArchiveUtility.cpp)
target_link_libraries (ArchiveUtility PRIVATE Stroika::Foundation)
StroikaCopyFilesToExeDir (ArchiveUtility)
