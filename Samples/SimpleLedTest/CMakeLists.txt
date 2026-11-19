#
# SimpleLedTest, as its Makefile builds it - with Visual Studio: a Windows program, its resources compiled in
#
add_executable (SimpleLedTest WIN32 Sources/Test.cpp Sources/Test.rc)
set_source_files_properties (Sources/Test.rc PROPERTIES COMPILE_OPTIONS /l0x0409) # one argument: LedIt's CMakeLists.txt says why
target_link_libraries (SimpleLedTest PRIVATE Stroika::Frameworks)
StroikaCopyFilesToExeDir (SimpleLedTest)
