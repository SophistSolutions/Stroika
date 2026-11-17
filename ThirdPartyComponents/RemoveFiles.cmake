#
# cmake -DDIRECTORY=<directory> "-DGLOBS=<glob>|<glob>..." -P RemoveFiles.cmake
#
# Removes the files in the directory that match any of the globs - '|' between them, as a ';' would split the argument.
#
string (REPLACE "|" ";" globs_ "${GLOBS}")
foreach (glob_ IN LISTS globs_)
    file (GLOB files_ LIST_DIRECTORIES false "${DIRECTORY}/${glob_}")
    if (files_)
        file (REMOVE ${files_})
    endif ()
endforeach ()
