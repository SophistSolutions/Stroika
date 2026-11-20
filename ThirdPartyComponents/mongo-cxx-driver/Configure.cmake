#
# cmake -DARGUMENTS_FILE=<file setting arguments_> -DSOURCE_DIR=<its sources> -DBINARY_DIR=<its build directory>
#       -DVERSION=<its version, as 4.6.1> -P Configure.cmake
#
# mongo-cxx-driver's configure, as its Makefile runs it: a first pass, which fetches mongo-c-driver and may fail on its
# -Werror; that -Werror removed from mongo-c-driver's libmongoc CMakeLists.txt, and its /we4047 from its warnings (below);
# then the pass that must succeed.
#
include ("${ARGUMENTS_FILE}")
execute_process (COMMAND "${CMAKE_COMMAND}" ${arguments_} -S "${SOURCE_DIR}" -B "${BINARY_DIR}")
set (libmongoc_ "${BINARY_DIR}/_deps/mongo-c-driver-src/src/libmongoc/CMakeLists.txt")
if (EXISTS "${libmongoc_}")
    file (READ "${libmongoc_}" text_)
    string (REPLACE "-Werror -DCMAKE_CXX_LINK_EXECUTABLE" "-DCMAKE_CXX_LINK_EXECUTABLE" text_ "${text_}")
    file (WRITE "${libmongoc_}" "${text_}")
endif ()
# MSVC's C4047 a warning, not the error mongo-c-driver makes it (/we4047, its MongoC-Warnings.cmake): MSVC 19.52 (not
# 19.51) gives it wherever a pointer converts to bool - valid C, which 19.52 still converts correctly - as libbson and
# libmongoc do in several places (mongo-c-driver 2.5.6, which 4.6.1 fetches; unchanged in 2.5.9 and master, 2026-11-20):
#   validate.c(253): error C4047: 'function': 'bool' differs in levels of indirection from 'const char *const '
#   mongoc-handshake.c(419): error C4047: '=': 'bool' differs in levels of indirection from 'char *'
set (warnings_ "${BINARY_DIR}/_deps/mongo-c-driver-src/build/cmake/MongoC-Warnings.cmake")
if (VERSION VERSION_LESS_EQUAL 4.6.1 AND EXISTS "${warnings_}")
    file (READ "${warnings_}" text_)
    string (REPLACE " msvc:/we4047" "" text_ "${text_}")
    file (WRITE "${warnings_}" "${text_}")
endif ()
execute_process (COMMAND "${CMAKE_COMMAND}" ${arguments_} -S "${SOURCE_DIR}" -B "${BINARY_DIR}" RESULT_VARIABLE result_)
if (result_)
    message (FATAL_ERROR "mongo-cxx-driver's configure failed: ${result_}")
endif ()
