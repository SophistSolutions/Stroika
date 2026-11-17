#
# cmake -DARGUMENTS_FILE=<file setting arguments_> -DSOURCE_DIR=<its sources> -DBINARY_DIR=<its build directory>
#       -P Configure.cmake
#
# mongo-cxx-driver's configure, as its Makefile runs it: a first pass, which fetches mongo-c-driver and may fail on its
# -Werror; that -Werror removed from mongo-c-driver's libmongoc CMakeLists.txt; then the pass that must succeed.
#
include ("${ARGUMENTS_FILE}")
execute_process (COMMAND "${CMAKE_COMMAND}" ${arguments_} -S "${SOURCE_DIR}" -B "${BINARY_DIR}")
set (libmongoc_ "${BINARY_DIR}/_deps/mongo-c-driver-src/src/libmongoc/CMakeLists.txt")
if (EXISTS "${libmongoc_}")
    file (READ "${libmongoc_}" text_)
    string (REPLACE "-Werror -DCMAKE_CXX_LINK_EXECUTABLE" "-DCMAKE_CXX_LINK_EXECUTABLE" text_ "${text_}")
    file (WRITE "${libmongoc_}" "${text_}")
endif ()
execute_process (COMMAND "${CMAKE_COMMAND}" ${arguments_} -S "${SOURCE_DIR}" -B "${BINARY_DIR}" RESULT_VARIABLE result_)
if (result_)
    message (FATAL_ERROR "mongo-cxx-driver's configure failed: ${result_}")
endif ()
