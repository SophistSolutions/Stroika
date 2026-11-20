#
# Stroika's documentation, as Documentation/Makefile builds it (BuildDocumentation.cmake): doxygen's HTML, as
# Stroika-Doxygen-Help.tar.gz in the Builds directory - one for all configurations, so built only when it is not there -
# given doxygen. Part of the build (all), as in make's all - but not on Windows, where doxygen takes hours: there the
# target documentation builds it.
#
find_program (STROIKA_DOXYGEN doxygen)
if (NOT STROIKA_DOXYGEN)
    message (STATUS "Documentation: not built, as no doxygen is installed")
    add_custom_target (documentation COMMAND "${CMAKE_COMMAND}" -E echo "Documentation: not built, as no doxygen is installed" VERBATIM)
    return ()
endif ()
StroikaReadVersion ("${CMAKE_SOURCE_DIR}/STROIKA_VERSION" version_)
string (REGEX REPLACE "/$" "" buildsDir_ "${STROIKA_BUILDS_DIR}")
cmake_path (GET buildsDir_ PARENT_PATH buildsDir_)
set (output_ "${buildsDir_}/Stroika-Doxygen-Help.tar.gz")
add_custom_command (
    OUTPUT "${output_}"
    COMMAND "${CMAKE_COMMAND}" "-DDOXYGEN=${STROIKA_DOXYGEN}" "-DVERSION=${version_}" "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/Doxygen"
            "-DOUTPUT=${output_}" -P "${CMAKE_CURRENT_SOURCE_DIR}/BuildDocumentation.cmake"
    COMMENT "Building Doxygen Documentation (${output_}; see ${CMAKE_CURRENT_BINARY_DIR}/Doxygen/BUILD.txt)"
    VERBATIM
)
set (all_ ALL)
if (CMAKE_HOST_WIN32)
    set (all_)
endif ()
add_custom_target (documentation ${all_} DEPENDS "${output_}")
