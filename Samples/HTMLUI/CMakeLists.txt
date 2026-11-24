#
# HTMLUI, as its Makefile builds it: its backend, the program HTMLUI (Backend/), the web app it serves
# (QuasarBasedHTMLApp/, into Samples-HTMLUI/html), and its installers (Installers/, into Samples-HTMLUI). One of Stroika's
# samples (Samples/CMakeLists.txt), or a project by itself, built on Stroika's package - as the application Skel's HTMLUI
# template makes of it:
#     cmake -S Samples/HTMLUI -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#           -DCMAKE_PREFIX_PATH=<the configuration's Builds directory>
#
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    cmake_minimum_required (VERSION 3.22)
    project (HTMLUI LANGUAGES C CXX)
    find_package (Stroika REQUIRED)
endif ()

# its version, Stroika's - the backend's AppVersion.h, and its installers'
set (versionFile_ "${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION")

add_subdirectory (Backend)
add_subdirectory (QuasarBasedHTMLApp)
set (html_ "${STROIKA_BUILDS_DIR}Samples-HTMLUI/html") # the web app, as QuasarBasedHTMLApp/ builds it

# beside the backend, html: a link to the web app it serves (from GetEXEDir ()/html), as its Makefile links it - or a copy,
# where Windows makes no symbolic link (without Developer Mode); made again when the web app is built again (its stamp)
set (linkStamp_ "${CMAKE_CURRENT_BINARY_DIR}/html-link.stamp")
add_custom_command (OUTPUT "${linkStamp_}"
    COMMAND "${CMAKE_COMMAND}" "-DTARGET=${html_}" "-DLINK=${STROIKA_BUILDS_DIR}Samples-HTMLUI/Backend/html"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/Backend/LinkHTML.cmake"
    COMMAND "${CMAKE_COMMAND}" -E touch "${linkStamp_}"
    DEPENDS "${webAppStamp_}" "${CMAKE_CURRENT_SOURCE_DIR}/Backend/LinkHTML.cmake"
    VERBATIM
)
add_custom_target (HTMLUI-html-link ALL DEPENDS "${linkStamp_}")
add_dependencies (HTMLUI-html-link HTMLUI HTMLUI-html)

# its installers: the backend, and beside it the web app it serves (the .msi's, Installers/AddWebAppToWIX.cmake adds) - so
# they are built again after it
StroikaInstallers (HTMLUI NAME Stroika-Sample-HTMLUI PACKAGE stroika-sample-htmlui VERSION_FILE "${versionFile_}"
    OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-HTMLUI" FILES "${html_}" DEPENDS HTMLUI-html "${webAppStamp_}"
    WIX_PROGRAM_NAME HTMLUI.exe WIX_SCRIPT Installers/AddWebAppToWIX.cmake WIX_ARGS "-DHTML_DIR=${html_}")
