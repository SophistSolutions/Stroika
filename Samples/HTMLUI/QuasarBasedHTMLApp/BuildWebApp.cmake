#
# cmake -DSOURCE_DIR=<QuasarBasedHTMLApp> -DWORK_DIR=<where to build it> -DHTML_DIR=<Samples-HTMLUI/html> -DNPM=<npm> -P BuildWebApp.cmake
#
# HTMLUI's web app, built as its Makefile builds it: its sources copied to WORK_DIR, built there by npm (npm i, then npm
# run build - each one's output in a log there), and what that builds (dist/spa) copied to HTML_DIR.
#
file (REMOVE_RECURSE "${WORK_DIR}")
file (MAKE_DIRECTORY "${WORK_DIR}")
file (GLOB items_ RELATIVE "${SOURCE_DIR}" "${SOURCE_DIR}/.env*" "${SOURCE_DIR}/*.js" "${SOURCE_DIR}/*.ts" "${SOURCE_DIR}/*.mjs"
    "${SOURCE_DIR}/*.json" "${SOURCE_DIR}/*.html" "${SOURCE_DIR}/.prettierrc")
foreach (item_ IN LISTS items_ ITEMS public src Makefile)
    if (EXISTS "${SOURCE_DIR}/${item_}")
        file (COPY "${SOURCE_DIR}/${item_}" DESTINATION "${WORK_DIR}")
    endif ()
endforeach ()

function (Run_ log)
    execute_process (COMMAND ${ARGN} WORKING_DIRECTORY "${WORK_DIR}" OUTPUT_FILE "${WORK_DIR}/${log}" ERROR_FILE "${WORK_DIR}/${log}"
        RESULT_VARIABLE result_)
    if (NOT result_ EQUAL 0)
        file (READ "${WORK_DIR}/${log}" text_)
        string (LENGTH "${text_}" length_)
        if (length_ GREATER 2000)
            math (EXPR start_ "${length_} - 2000")
            string (SUBSTRING "${text_}" ${start_} -1 text_)
        endif ()
        message (FATAL_ERROR "HTMLUI's web app: ${ARGN} failed (${result_}) - ${WORK_DIR}/${log} ends:\n${text_}")
    endif ()
endfunction ()

Run_ (NPM-Install.txt "${NPM}" i)
file (REMOVE "${WORK_DIR}/.env") # its build ignores a .env - development builds' only
Run_ (Build.txt "${NPM}" run build)
file (REMOVE_RECURSE "${HTML_DIR}")
file (COPY "${WORK_DIR}/dist/spa/" DESTINATION "${HTML_DIR}")
