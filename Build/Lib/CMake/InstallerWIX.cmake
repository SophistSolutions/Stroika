#
# cmake -DEXE=<program> -DEXE_NAME=<file name> -DNAME=<name> -DPACKAGE=<package> -DSOURCE_DIR=<Installers directory>
#       -DARCH=<architecture> -DCONFIGURATION=<configuration> -DVERSION=<version> -DFOUR_DOT=<Win32 version>
#       -DCANDLE=<candle.exe> -DLIGHT=<light.exe> -DHEAT=<heat.exe> -DWORK_DIR=<directory> -DLOG=<log> -DOUTPUT=<.msi>
#       [-DSCRIPT=<script>] -P InstallerWIX.cmake
#
# A Windows installer (.msi) for the program, as Samples/Service/Installers/Makefile builds one with WIX: SOURCE_DIR's
# Installer.wix compiled (candle) - given ARCH, CONFIGURATION, VERSION (DISPLAY_FULL_VERSION), FOUR_DOT (WIX_4DOT_VERSION)
# and the program, copied into WORK_DIR as EXE_NAME (SERVICE_EXE) - and linked (light) with WIX's UI extension and
# SOURCE_DIR's Installer_en-us.wxl, run in SOURCE_DIR. SCRIPT, if given, is an installer's own step, run (in this script's
# scope) before that: it may add files for candle to compile (WXS_FILES) and directories for light to find files in
# (BIND_PATHS). Each command, and its output, goes to LOG.
#
file (REMOVE_RECURSE "${WORK_DIR}")
file (REMOVE "${LOG}")
file (MAKE_DIRECTORY "${WORK_DIR}")

# Run_ (<command>...): runs it in SOURCE_DIR, it and its output appended to LOG - its output then in output_, its exit
# status in result_
function (Run_)
    string (JOIN " " commandLine_ ${ARGN})
    file (APPEND "${LOG}" "${commandLine_}\n")
    execute_process (COMMAND ${ARGN} WORKING_DIRECTORY "${SOURCE_DIR}" OUTPUT_VARIABLE output_ ERROR_VARIABLE output_ RESULT_VARIABLE result_)
    file (APPEND "${LOG}" "${output_}")
    set (output_ "${output_}" PARENT_SCOPE)
    set (result_ "${result_}" PARENT_SCOPE)
endfunction ()

# RunOrFail_ (<command>...): Run_, and stop if it fails
function (RunOrFail_)
    Run_ (${ARGN})
    if (NOT result_ EQUAL 0)
        message (FATAL_ERROR "${ARGV0} failed (${result_}), building ${OUTPUT} - its output (${LOG}):\n${output_}")
    endif ()
endfunction ()

file (COPY_FILE "${EXE}" "${WORK_DIR}/${EXE_NAME}")
set (WXS_FILES)
set (BIND_PATHS)
if (SCRIPT)
    include ("${SCRIPT}")
endif ()

# candle's name for the architecture: x64, not x86_64
set (candleArch_ "${ARCH}")
if (ARCH STREQUAL "x86_64")
    set (candleArch_ x64)
endif ()
RunOrFail_ ("${CANDLE}" -v -arch ${candleArch_} "-dARCH=${ARCH}" "-dCONFIGURATION=${CONFIGURATION}" "-dDISPLAY_FULL_VERSION=${VERSION}"
    "-dWIX_4DOT_VERSION=${FOUR_DOT}" "-dSERVICE_EXE=${WORK_DIR}/${EXE_NAME}" Installer.wix ${WXS_FILES} -out "${WORK_DIR}/")

set (light_ "${LIGHT}")
set (sources_ Installer.wix ${WXS_FILES})
foreach (file_ IN LISTS sources_)
    get_filename_component (name_ "${file_}" NAME_WLE)
    list (APPEND light_ "${WORK_DIR}/${name_}.wixobj")
endforeach ()
foreach (directory_ IN LISTS BIND_PATHS)
    list (APPEND light_ -b "${directory_}")
endforeach ()
list (APPEND light_ -ext WixUIExtension -cultures:en-us -loc Installer_en-us.wxl -out "${OUTPUT}")
Run_ (${light_})
# light validates the .msi it built (ICE), with the Windows Installer service - which an account not an administrator cannot
# reach: then light fails, with LGHT0217 ("The Windows Installer Service could not be accessed") - its .msi good, just not
# validated. So, as Build/Scripts/RunWIXLight, build it again without validating (-sval), and say so.
if (NOT result_ EQUAL 0 AND output_ MATCHES "LGHT0217")
    message (WARNING "${OUTPUT}: built, but NOT VALIDATED (light -sval) - the Windows Installer service is not reachable from this "
        "account (typically, as it is not an administrator). Build it from an elevated (administrator) shell to validate it.")
    list (INSERT light_ 1 -sval)
    Run_ (${light_})
endif ()
if (NOT result_ EQUAL 0)
    message (FATAL_ERROR "light failed (${result_}), building ${OUTPUT} - its output (${LOG}):\n${output_}")
endif ()
