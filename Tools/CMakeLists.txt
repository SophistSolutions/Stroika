#
# The tools built with Stroika, each as its Makefile builds it, into bin/ in the configuration's Builds directory:
# HTMLViewCompiler, which compiles a .swsp file to C++
#
add_executable (HTMLViewCompiler Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/HTMLViewCompiler.cpp)
target_link_libraries (HTMLViewCompiler PRIVATE stroika-foundation)
set_target_properties (HTMLViewCompiler PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}bin$<0:>") # $<0:>: Library/CMakeLists.txt
StroikaCopyFilesToExeDir (HTMLViewCompiler)
