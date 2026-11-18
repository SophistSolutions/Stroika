#
# HTMLUI's own step in building its .msi (StroikaInstallers' WIX_SCRIPT: run within Build/Lib/CMake/InstallerWIX.cmake),
# given HTML_DIR, its web app - as its Makefile builds the .msi: the DLLs beside the program (with MSVC's AddressSanitizer,
# its runtime) installed beside it (INSTALLDIR), and the web app in its html directory (HTML_DIR_REF) - a page saying
# "empty" where there is no web app (built without node) - each harvested by heat into a fragment of its own
# (SampleHTMLUI_dlls_comp, SampleHTMLUI_html_comp; EmptyDLLs.wix, where there are no DLLs).
#
get_filename_component (exeDir_ "${EXE}" DIRECTORY)
file (GLOB dlls_ "${exeDir_}/*.dll")
file (MAKE_DIRECTORY "${WORK_DIR}/dlls")
if (dlls_)
    file (COPY ${dlls_} DESTINATION "${WORK_DIR}/dlls")
endif ()
file (MAKE_DIRECTORY "${WORK_DIR}/html")
file (COPY "${HTML_DIR}/" DESTINATION "${WORK_DIR}/html")
file (GLOB html_ "${WORK_DIR}/html/*")
if (NOT html_)
    file (WRITE "${WORK_DIR}/html/index.html" "<html><body>empty</body></html>\n")
endif ()

RunOrFail_ ("${HEAT}" dir "${WORK_DIR}/html" -nologo -o "${WORK_DIR}/html.wxs" -scom -frag -srd -sreg -gg
    -cg SampleHTMLUI_html_comp -dr HTML_DIR_REF)
if (dlls_)
    RunOrFail_ ("${HEAT}" dir "${WORK_DIR}/dlls" -nologo -o "${WORK_DIR}/dlls.wxs" -scom -frag -srd -sreg -gg
        -cg SampleHTMLUI_dlls_comp -dr INSTALLDIR)
else ()
    file (COPY_FILE "${SOURCE_DIR}/EmptyDLLs.wix" "${WORK_DIR}/dlls.wxs")
endif ()
list (APPEND WXS_FILES "${WORK_DIR}/html.wxs" "${WORK_DIR}/dlls.wxs")
list (APPEND BIND_PATHS "${WORK_DIR}/html" "${WORK_DIR}/dlls")
