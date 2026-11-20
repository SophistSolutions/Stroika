#
# Stroika's libraries, as make builds them - part of Stroika's build (the top CMakeLists.txt, which has found the
# configuration's settings and third-party libraries). It builds the Foundation and Frameworks libraries, the tools
# (Tools/CMakeLists.txt) and the documentation (Documentation/CMakeLists.txt), and puts each where make puts it, in the
# configuration's Builds directory, with Stroika's CMake package, which a program built on Stroika uses.
#

# the libraries where make puts them, in the configuration's Builds directory (the tests and samples: their CMakeLists.txt) -
# a generator expression, though empty, keeps a Visual Studio generator from adding a directory for the build type
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}lib$<0:>")

# with a Visual Studio generator, a project's files compiled at once, as make -jN compiles them - up to N compiles in all,
# msbuild's -m:N (cmake --build's -jN): else msbuild compiles each project's files one at a time
if (CMAKE_GENERATOR MATCHES "^Visual Studio")
    set (CMAKE_VS_GLOBALS "UseMultiToolTask=true" "EnforceProcessCountAcrossBuilds=true")
endif ()

add_subdirectory (Sources/Stroika/Foundation)
add_subdirectory (Sources/Stroika/Frameworks)
# make's target libraries (as tools, documentation, tests and samples: each where its parts are)
add_custom_target (libraries)
add_dependencies (libraries stroika-foundation stroika-frameworks)

# the tools, Tools/
add_subdirectory (../Tools Tools)

# the documentation, Documentation/
add_subdirectory (../Documentation Documentation)

# Stroika's CMake package, for a program built on Stroika - find_package (Stroika): the libraries as Stroika::Foundation and
# Stroika::Frameworks, both as Stroika::Stroika (all of Stroika, one name for it, as packages commonly give: fmt::fmt), the
# tool Stroika::HTMLViewCompiler, and the functions in Build/Lib/CMake (StroikaConfig.cmake.in) - in the configuration's
# Builds directory, beside its third-party libraries' packages. It names the libraries and the tool where this build makes
# them, and the headers in the source tree.
add_library (stroika INTERFACE)
target_link_libraries (stroika INTERFACE stroika-frameworks) # and so Foundation
set_target_properties (stroika-foundation PROPERTIES EXPORT_NAME Foundation)
set_target_properties (stroika-frameworks PROPERTIES EXPORT_NAME Frameworks)
set_target_properties (stroika PROPERTIES EXPORT_NAME Stroika)
# and here by the package's names, for the tests and samples built in this build (their find_package skipped)
add_library (Stroika::Foundation ALIAS stroika-foundation)
add_library (Stroika::Frameworks ALIAS stroika-frameworks)
add_library (Stroika::Stroika ALIAS stroika)
set (packageDirectory_ "${STROIKA_BUILDS_DIR}lib/cmake/Stroika")
export (TARGETS stroika-foundation stroika-frameworks stroika HTMLViewCompiler NAMESPACE Stroika:: FILE "${packageDirectory_}/StroikaTargets.cmake")
set (sourceRoot_ "${CMAKE_SOURCE_DIR}/")
configure_file (StroikaConfig.cmake.in "${packageDirectory_}/StroikaConfig.cmake" @ONLY)
include (CMakePackageConfigHelpers)
StroikaReadVersion ("${CMAKE_SOURCE_DIR}/STROIKA_VERSION" version_)
write_basic_package_version_file ("${packageDirectory_}/StroikaConfigVersion.cmake" VERSION "${version__WIN32_FOUR_DOT}" COMPATIBILITY SameMinorVersion)
