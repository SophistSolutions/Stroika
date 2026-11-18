#
# HTMLUI, as its Makefile builds it: its backend, the program HTMLUI (Backend/), and the web app it serves
# (QuasarBasedHTMLApp/, into Samples-HTMLUI/html)
#
add_subdirectory (Backend)
add_subdirectory (QuasarBasedHTMLApp)

# beside the backend, html: a link to the web app it serves (from GetEXEDir ()/html), as its Makefile links it - or a copy,
# where Windows makes no symbolic link (without Developer Mode)
add_custom_target (HTMLUI-html-link ALL
    COMMAND "${CMAKE_COMMAND}" "-DTARGET=${STROIKA_BUILDS_DIR}Samples-HTMLUI/html" "-DLINK=${STROIKA_BUILDS_DIR}Samples-HTMLUI/Backend/html"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/Backend/LinkHTML.cmake"
    VERBATIM
)
add_dependencies (HTMLUI-html-link HTMLUI HTMLUI-html)
