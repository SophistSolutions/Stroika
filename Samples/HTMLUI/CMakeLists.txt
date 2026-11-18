#
# HTMLUI, as its Makefile builds it: its backend, the program HTMLUI (Backend/), the web app it serves
# (QuasarBasedHTMLApp/, into Samples-HTMLUI/html), and its installers (Installers/, into Samples-HTMLUI)
#
add_subdirectory (Backend)
add_subdirectory (QuasarBasedHTMLApp)
set (html_ "${STROIKA_BUILDS_DIR}Samples-HTMLUI/html") # the web app, as QuasarBasedHTMLApp/ builds it

# beside the backend, html: a link to the web app it serves (from GetEXEDir ()/html), as its Makefile links it - or a copy,
# where Windows makes no symbolic link (without Developer Mode)
add_custom_target (HTMLUI-html-link ALL
    COMMAND "${CMAKE_COMMAND}" "-DTARGET=${html_}" "-DLINK=${STROIKA_BUILDS_DIR}Samples-HTMLUI/Backend/html"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/Backend/LinkHTML.cmake"
    VERBATIM
)
add_dependencies (HTMLUI-html-link HTMLUI HTMLUI-html)

# its installers: the backend, and beside it the web app it serves (the .msi's, Installers/AddWebAppToWIX.cmake adds) - so
# they are built again after it
StroikaInstallers (HTMLUI NAME Stroika-Sample-HTMLUI PACKAGE stroika-sample-htmlui VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../STROIKA_VERSION"
    OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}Samples-HTMLUI" FILES "${html_}" DEPENDS HTMLUI-html "${webAppStamp_}"
    WIX_PROGRAM_NAME HTMLUI.exe WIX_SCRIPT Installers/AddWebAppToWIX.cmake WIX_ARGS "-DHTML_DIR=${html_}")
