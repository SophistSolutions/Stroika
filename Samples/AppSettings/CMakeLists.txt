add_executable (AppSettings Sources/AppConfiguration.cpp Sources/Main.cpp Sources/MoreConfiguration.cpp)
target_link_libraries (AppSettings PRIVATE Stroika::Foundation)
StroikaCopyFilesToExeDir (AppSettings)
