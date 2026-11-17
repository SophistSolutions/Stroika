#
# sqlite's amalgamation, built as ThirdPartyComponents/sqlite/Makefile builds it - this file is copied in among its sources:
# the library sqlite, and its shell, sqlite (linked as make links a Stroika program); sqlite3.h installed as
# sqlite/sqlite3.h, and Stroika's sqlite.pc.
#
cmake_minimum_required (VERSION 3.22)
project (sqlite LANGUAGES C CXX)

# the configuration's settings (its assertions, link options and libraries) - ThirdPartyComponents/CMakeLists.txt gives the file
include ("${STROIKA_CONFIGURATION_FILE}")

add_compile_definitions (SQLITE_OMIT_LOAD_EXTENSION SQLITE_ENABLE_JSON1)
# sqlite's own checks while Stroika's are on - but not under Valgrind or ThreadSanitizer, which they upset (#133)
if (STROIKA_ASSERTIONS_ENABLED AND NOT "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" MATCHES "qStroika_FeatureSupported_Valgrind|-fsanitize=[^ ]*thread")
    add_compile_definitions (SQLITE_ENABLE_API_ARMOR SQLITE_DEBUG)
endif ()
include (CheckCCompilerFlag)
check_c_compiler_flag (-Wmisleading-indentation hasMisleadingIndentationWarning_)
if (hasMisleadingIndentationWarning_)
    add_compile_options (-Wno-misleading-indentation)
endif ()

add_library (sqlite STATIC sqlite3.c)
add_executable (sqlite-shell shell.c)
set_target_properties (sqlite-shell PROPERTIES OUTPUT_NAME sqlite LINKER_LANGUAGE CXX)
target_link_libraries (sqlite-shell PRIVATE sqlite ${STROIKA_LINK_LIBRARIES})
target_link_options (sqlite-shell PRIVATE ${STROIKA_LINK_OPTIONS})

install (TARGETS sqlite ARCHIVE DESTINATION lib)
install (TARGETS sqlite-shell RUNTIME DESTINATION bin)
install (FILES sqlite3.h DESTINATION include/sqlite)
install (FILES "${SQLITE_PC}" DESTINATION lib/pkgconfig)
