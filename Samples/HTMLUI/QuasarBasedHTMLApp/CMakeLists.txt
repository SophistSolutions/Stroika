#
# HTMLUI's web app, as its Makefile builds it (BuildWebApp.cmake), into Samples-HTMLUI/html - again whenever its sources
# change - given node 22 or later, npm and quasar; else, as HTMLUI's Makefile, a warning, and html left empty. Its build
# touches a stamp file, which the calling directory gets as webAppStamp_ (HTMLUI's .msi holds the web app).
#
set (html_ "${STROIKA_BUILDS_DIR}Samples-HTMLUI/html")
find_program (STROIKA_NODE NAMES node)
find_program (STROIKA_NPM NAMES npm.cmd npm) # on Windows, npm.cmd (beside it, npm is a shell script)
find_program (STROIKA_QUASAR NAMES quasar.cmd quasar)
set (missing_)
if (NOT STROIKA_NODE)
    set (missing_ "no nodejs installed")
else ()
    execute_process (COMMAND "${STROIKA_NODE}" -v OUTPUT_VARIABLE nodeVersion_ OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    string (REGEX REPLACE "^v([0-9]+).*" "\\1" nodeMajor_ "${nodeVersion_}")
    if (NOT nodeMajor_ MATCHES "^[0-9]+$" OR nodeMajor_ LESS 22)
        set (missing_ "nodejs is older than 22 (${nodeVersion_})")
    elseif (NOT STROIKA_NPM)
        set (missing_ "no npm installed")
    elseif (NOT STROIKA_QUASAR)
        set (missing_ "no quasar installed")
    endif ()
endif ()
if (missing_)
    message (WARNING "Samples/HTMLUI: skipping its web app (html), as ${missing_}")
    # a stamp of another name than the web app's: so once node is there, the web app is built, though its sources are older
    set (stamp_ "${CMAKE_CURRENT_BINARY_DIR}/no-web-app.stamp")
    add_custom_command (
        OUTPUT "${stamp_}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${html_}"
        COMMAND "${CMAKE_COMMAND}" -E touch "${stamp_}"
        VERBATIM
    )
    add_custom_target (HTMLUI-html ALL DEPENDS "${stamp_}")
    set (webAppStamp_ "${stamp_}" PARENT_SCOPE)
    return ()
endif ()

# its sources - as its Makefile copies them - and where it builds them: short, for npm's node_modules on Windows
file (GLOB_RECURSE sources_ CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*" "${CMAKE_CURRENT_SOURCE_DIR}/public/*")
file (GLOB topFiles_ CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.env*" "${CMAKE_CURRENT_SOURCE_DIR}/*.js" "${CMAKE_CURRENT_SOURCE_DIR}/*.ts"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.mjs" "${CMAKE_CURRENT_SOURCE_DIR}/*.json" "${CMAKE_CURRENT_SOURCE_DIR}/*.html" "${CMAKE_CURRENT_SOURCE_DIR}/.prettierrc")
set (workDir_ "${CMAKE_BINARY_DIR}/HTMLUI-html")
set (stamp_ "${CMAKE_CURRENT_BINARY_DIR}/web-app.stamp")
add_custom_command (
    OUTPUT "${stamp_}"
    COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}" "-DWORK_DIR=${workDir_}" "-DHTML_DIR=${html_}"
            "-DNPM=${STROIKA_NPM}" -P "${CMAKE_CURRENT_SOURCE_DIR}/BuildWebApp.cmake"
    COMMAND "${CMAKE_COMMAND}" -E touch "${stamp_}"
    DEPENDS ${sources_} ${topFiles_} "${CMAKE_CURRENT_SOURCE_DIR}/BuildWebApp.cmake"
    COMMENT "Building HTMLUI's web app - npm i, npm run build (logs in ${workDir_})"
    VERBATIM
)
add_custom_target (HTMLUI-html ALL DEPENDS "${stamp_}")
set (webAppStamp_ "${stamp_}" PARENT_SCOPE)
