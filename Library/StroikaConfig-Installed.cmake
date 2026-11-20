#
# Stroika's CMake package as installed (Library/CMakeLists.txt), as StroikaConfig.cmake: for a program built on Stroika,
# with the configuration's toolchain file - beside this file: its compilers and flags, which must be Stroika's - and the
# installation in CMAKE_PREFIX_PATH:
#     cmake -DCMAKE_TOOLCHAIN_FILE=<installation>/lib/cmake/Stroika/Toolchain.cmake -DCMAKE_PREFIX_PATH=<installation> ...
#     find_package (Stroika REQUIRED)
#     target_link_libraries (MyApp PRIVATE Stroika::Stroika)
# It gives what the package in Stroika's build gives (StroikaConfig.cmake.in), all from the installation: the headers in
# include/, the libraries in lib/, Stroika::HTMLViewCompiler in bin/, and the configuration's third-party components, each
# where its own install put it.
#

# the configuration's settings (STROIKA_*), which Stroika's functions and the finding of its third-party libraries use, as
# Stroika's build applied them - but its third-party components are here, in the installation; and as an installation has
# no Builds directory, an application's programs, which Skel's put there (STROIKA_BUILDS_DIR), go in its own build directory
include ("${CMAKE_CURRENT_LIST_DIR}/Stroika-Configuration.cmake")
get_filename_component (STROIKA_THIRD_PARTY_INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
string (APPEND STROIKA_THIRD_PARTY_INSTALL_DIR "/")
set (STROIKA_BUILDS_DIR "${CMAKE_BINARY_DIR}/")
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
# and its build type, C++ standard and third-party components, as Stroika's own build takes them
include (StroikaBuildSettings)

if (TARGET Stroika::Foundation) # found already, here
    return ()
endif ()
include (StroikaDependencies)
include ("${CMAKE_CURRENT_LIST_DIR}/StroikaTargets.cmake")
include (StroikaCopyFilesToExeDir)
include (StroikaEmbedFiles)
include (StroikaInstallers)
include (StroikaMakeVersionFile)
