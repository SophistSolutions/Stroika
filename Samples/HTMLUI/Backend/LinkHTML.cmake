#
# cmake -DTARGET=<the web app's directory> -DLINK=<the link to make to it> -P LinkHTML.cmake
#
# LINK made a symbolic link to TARGET (made, if missing), as HTMLUI's Makefile makes it (ln -s) - or, where none can be made
# (Windows without Developer Mode), a copy of it.
#
file (MAKE_DIRECTORY "${TARGET}")
file (REMOVE_RECURSE "${LINK}")
file (CREATE_LINK "${TARGET}" "${LINK}" SYMBOLIC RESULT result_)
if (NOT result_ EQUAL 0)
    file (COPY "${TARGET}/" DESTINATION "${LINK}")
endif ()
