#
# cmake -DDOXYGEN=<doxygen> -DVERSION=<version> -DWORK_DIR=<directory> -DOUTPUT=<.tar.gz> -P BuildDocumentation.cmake
#
# Stroika's documentation, as Documentation/Makefile builds it: doxygen run in Doxygen/ with its Stroika-Library.cfg - given
# VERSION as its PROJECT_NUMBER (for the PROJECT_NUMBER.cfg it includes, which make writes beside it) and WORK_DIR as its
# OUTPUT_DIRECTORY; its output in WORK_DIR/BUILD.txt - then the HTML it makes, as Stroika-Doxygen-Help/, in OUTPUT.
#
set (doxygenDir_ "${CMAKE_CURRENT_LIST_DIR}/Doxygen")
file (REMOVE_RECURSE "${WORK_DIR}")
file (MAKE_DIRECTORY "${WORK_DIR}")
cmake_path (GET OUTPUT PARENT_PATH outputParent_)
file (MAKE_DIRECTORY "${outputParent_}")
file (READ "${doxygenDir_}/Stroika-Library.cfg" configuration_)
string (REPLACE "@INCLUDE = \"PROJECT_NUMBER.cfg\"" "PROJECT_NUMBER = \"${VERSION}\"" configuration_ "${configuration_}")
# OUTPUT_DIRECTORY relative to doxygen's working directory, where it can be (else, on another drive, as given): a doxygen
# built for MSYS2 or Cygwin takes C:/... for a relative path, putting its working directory in front (as of doxygen 1.18.0)
cmake_path (RELATIVE_PATH WORK_DIR BASE_DIRECTORY "${doxygenDir_}" OUTPUT_VARIABLE outputDirectory_)
if (outputDirectory_ STREQUAL "")
    set (outputDirectory_ "${WORK_DIR}")
endif ()
file (WRITE "${WORK_DIR}/Doxyfile" "${configuration_}\nOUTPUT_DIRECTORY = \"${outputDirectory_}\"\n")
execute_process (COMMAND "${DOXYGEN}" "${WORK_DIR}/Doxyfile" WORKING_DIRECTORY "${doxygenDir_}" OUTPUT_FILE "${WORK_DIR}/BUILD.txt"
    ERROR_FILE "${WORK_DIR}/BUILD.txt" RESULT_VARIABLE result_)
if (NOT result_ EQUAL 0)
    message (FATAL_ERROR "doxygen failed (${result_}) - its output: ${WORK_DIR}/BUILD.txt")
endif ()
file (RENAME "${WORK_DIR}/html" "${WORK_DIR}/Stroika-Doxygen-Help")
execute_process (COMMAND "${CMAKE_COMMAND}" -E tar czf "${OUTPUT}.part" --format=gnutar Stroika-Doxygen-Help WORKING_DIRECTORY "${WORK_DIR}"
    COMMAND_ERROR_IS_FATAL ANY)
file (RENAME "${OUTPUT}.part" "${OUTPUT}")
