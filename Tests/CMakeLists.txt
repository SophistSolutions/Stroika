#
# The regression tests: Tests/NN, each its Test.cpp built as TestNN, linked with the Frameworks library (and googletest's
# main). ctest runs each through Tests/Scripts/RunTest, as make run-tests does - so RunTest's REMOTE=, VALGRIND= and so on
# work the same way:
#     ctest --test-dir <build directory> -j8 --output-on-failure
# (with a Visual Studio generator, -C and the configuration's build type too: -C Debug, or -C Release).
#
if (NOT STROIKA_FEATURE_GoogleTest STREQUAL "no")
    find_package (GTest CONFIG REQUIRED) # its targets are visible only below where it is found
endif ()

# each test where make puts it, Tests/ in the configuration's Builds directory (for $<0:>: Library/CMakeLists.txt)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Tests$<0:>")

# RunTest's environment, as make run-tests gives it: the configuration's command prefix, where Stroika's sources are, and
# where a PerformanceDump.txt that a test writes goes (the configuration's Builds directory)
cmake_path (GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH sourceRoot_)
set (environment_ "RunPrefix=${STROIKA_RUN_PREFIX}" "StroikaSourceRoot=${sourceRoot_}/" "StroikaPlatformTargetBuildDir=${STROIKA_BUILDS_DIR}")
if (WIN32)
    # and the tools RunTest uses (sed, tr ...), beside its bash: on PATH in an MSYS2 or Cygwin shell, not in PowerShell's
    cmake_path (GET STROIKA_BASH PARENT_PATH bashDirectory_)
endif ()

file (GLOB tests_ RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/[0-9][0-9]")
foreach (n_ IN LISTS tests_)
    add_executable (Test${n_} ${n_}/Test.cpp)
    target_link_libraries (Test${n_} PRIVATE stroika-frameworks)
    if (NOT STROIKA_FEATURE_GoogleTest STREQUAL "no")
        target_link_libraries (Test${n_} PRIVATE GTest::gtest_main)
    endif ()
    StroikaCopyFilesToExeDir (Test${n_})
    add_test (NAME Test${n_} COMMAND "${STROIKA_BASH}" "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/RunTest" ${n_} $<TARGET_FILE:Test${n_}>)
    set_tests_properties (Test${n_} PROPERTIES ENVIRONMENT "${environment_}")
    if (WIN32)
        set_tests_properties (Test${n_} PROPERTIES ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${bashDirectory_}")
    endif ()
endforeach ()

# the test files Test34 embeds
add_subdirectory (34)
