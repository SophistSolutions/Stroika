#
# One third-party component's Conan steps: cmake -P <its conan.cmake> (ThirdPartyComponents/CMakeLists.txt's
# StroikaComponentFromConan writes it), which sets what is below, then includes this. The component from Conan's cache,
# copied into the configuration's Builds directory (StroikaDeploy.py) - built there first, by its recipe, when the
# configuration has no package of it yet.
#
# Their output goes to a log in the configuration's IntermediateFiles, ThirdPartyComponents/<component>/: BuildRecipeLog.txt
# where Conan builds a package (this one, or a tool it builds with), else FetchLog.txt. Each is written afresh, its first
# line saying when; the other is left as it was, so a build's log outlives the fetches after it (make clean removes them).
# The build shows where the log is as the step starts (its COMMENT) and one line as it ends - with Conan's warnings, and on
# a failure the end of the log.
#
#   CONAN               conan
#   PACKAGE             the package, as a reference (stroika-zlib/1.3.2)
#   RECIPES             ThirdPartyComponents, where the recipes are
#   EXPORTS             the references of the recipes to export: its own, and its tools'
#   ARGUMENTS           Conan's arguments naming the package and the configuration (conan install's, and conan graph info's)
#   DEPLOY_ARGUMENTS    conan install's arguments copying it into the Builds directory
#   LOCK                the lock taken first, in Conan's home: Conan's cache is not safe for two Conan commands at once
#   CONAN_HOME          Conan's home
#   LOG_DIRECTORY       where its logs go
#   STAMP               touched when done
#

# MAKEFLAGS unset: Conan does not pass on make's job slots, so the component's own build takes CMAKE_BUILD_PARALLEL_LEVEL's
# jobs. Conan's output, to a log: without color, and unbuffered (Conan is a Python program), so the log grows as it runs.
unset (ENV{MAKEFLAGS})
unset (ENV{MFLAGS})
set (ENV{NO_COLOR} 1)
set (ENV{PYTHONUNBUFFERED} 1)

# Run again (below), as the log's writer: its first lines (STROIKA_CONAN_LOG_START_), then conan install
if (DEFINED STROIKA_CONAN_LOG_START_)
    execute_process (COMMAND "${CMAKE_COMMAND}" -E cat "${STROIKA_CONAN_LOG_START_}")
    execute_process (COMMAND "${CONAN}" install ${ARGUMENTS} ${DEPLOY_ARGUMENTS} COMMAND_ERROR_IS_FATAL ANY)
    return ()
endif ()

string (TIMESTAMP start_ "%s")
file (LOCK "${LOCK}" GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE locked_)
if (NOT locked_ EQUAL 0)
    message (STATUS "${PACKAGE}: waiting for another build's Conan steps (${LOCK})")
    file (LOCK "${LOCK}" GUARD PROCESS)
endif ()

set (exported_)
foreach (reference_ IN LISTS EXPORTS)
    string (REPLACE "/" ";" nameAndVersion_ "${reference_}")
    list (GET nameAndVersion_ 0 name_)
    list (GET nameAndVersion_ 1 version_)
    execute_process (COMMAND "${CONAN}" export "${RECIPES}" --name ${name_} --version ${version_}
        OUTPUT_VARIABLE output_ ERROR_VARIABLE output_ RESULT_VARIABLE result_)
    if (NOT result_ EQUAL 0)
        message (NOTICE "${output_}")
        message (FATAL_ERROR "${PACKAGE}: conan export ${reference_} failed")
    endif ()
    string (APPEND exported_ "${output_}")
endforeach ()

# Will Conan build anything? Asked as conan install is (ARGUMENTS: --build missing) - so which log this is
execute_process (COMMAND "${CONAN}" graph info ${ARGUMENTS} --format json
    OUTPUT_VARIABLE graph_ ERROR_VARIABLE output_ RESULT_VARIABLE result_)
if (NOT result_ EQUAL 0)
    message (NOTICE "${output_}")
    message (FATAL_ERROR "${PACKAGE}: conan graph info failed")
endif ()
if (graph_ MATCHES "\"binary\": *\"Build\"")
    set (built_ ON)
    set (log_ "${LOG_DIRECTORY}/BuildRecipeLog.txt")
else ()
    set (built_ OFF)
    set (log_ "${LOG_DIRECTORY}/FetchLog.txt")
endif ()

file (MAKE_DIRECTORY "${LOG_DIRECTORY}")
string (TIMESTAMP now_ "%Y-%m-%d %H:%M:%S")
get_filename_component (logStart_ "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
set (logStart_ "${logStart_}/conan-log-start.txt")
file (WRITE "${logStart_}" "${PACKAGE}: Conan's steps, ${now_}\n\n${exported_}\n")
execute_process (COMMAND "${CMAKE_COMMAND}" "-DSTROIKA_CONAN_LOG_START_=${logStart_}" -P "${CMAKE_SCRIPT_MODE_FILE}"
    OUTPUT_FILE "${log_}" ERROR_FILE "${log_}" RESULT_VARIABLE result_)
file (REMOVE "${logStart_}")
if (NOT result_ EQUAL 0)
    file (READ "${log_}" end_)
    string (LENGTH "${end_}" length_)
    if (length_ GREATER 6000)
        math (EXPR from_ "${length_} - 6000")
        string (SUBSTRING "${end_}" ${from_} -1 end_)
        string (FIND "${end_}" "\n" from_)
        math (EXPR from_ "${from_} + 1")
        string (SUBSTRING "${end_}" ${from_} -1 end_)
    endif ()
    # as it is - a FATAL_ERROR's text is wrapped and indented - and then the error, naming the log
    message (NOTICE "${PACKAGE}: the end of its log:\n${end_}")
    message (FATAL_ERROR "${PACKAGE}: Conan's steps failed - its log: ${log_}")
endif ()

# Conan's warnings - StroikaDeploy.py's among them - on the console too: WARN: after the scope, if any (a package's)
file (STRINGS "${log_}" warnings_ REGEX "^([^ ]+: )?WARN: ")
foreach (warning_ IN LISTS warnings_)
    message (NOTICE "${warning_}")
endforeach ()

# Conan's cache keeps the components' packages, not their builds (as make clean removes a component's own): their build
# folders, many GB for boost, removed once packaged - each package's b, beside its p, in a folder Conan names for the first
# five letters of its name. By CMake, not conan cache clean, whose python cannot remove a path longer than Windows' MAX_PATH
# (boost's documentation's, as of 1.92.0).
file (GLOB buildFolders_ LIST_DIRECTORIES true "${CONAN_HOME}/p/b/stroi*/b")
# none when nothing was built: file (REMOVE_RECURSE) with no paths is an error in CMake 3.28.3 (not 3.31.6)
if (buildFolders_)
    file (REMOVE_RECURSE ${buildFolders_})
endif ()
file (TOUCH "${STAMP}")

string (TIMESTAMP end_ "%s")
math (EXPR seconds_ "${end_} - ${start_}")
math (EXPR minutes_ "${seconds_} / 60")
math (EXPR seconds_ "${seconds_} % 60")
if (built_)
    message (STATUS "${PACKAGE}: built, ${minutes_}m${seconds_}s - ${log_}")
else ()
    message (STATUS "${PACKAGE}: from Conan's cache - ${log_}")
endif ()
