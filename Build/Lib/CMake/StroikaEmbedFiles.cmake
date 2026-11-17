#
# StroikaEmbedFiles (<target> <file>...)
#
# Each file (relative to the calling directory) embedded as bytes - <name>.embed, an xxd -i style C array (EmbedFile.cmake),
# generated in the calling directory's binary directory, which the target's sources may then include. Bytes, never a C++
# string literal: MSVC is passed no /utf-8, so non-ASCII in someone else's file would be read as cp1252.
#
set (StroikaEmbedFileScript_ "${CMAKE_CURRENT_LIST_DIR}/EmbedFile.cmake")

function (StroikaEmbedFiles target)
    set (outputs_)
    foreach (file_ IN LISTS ARGN)
        get_filename_component (name_ "${file_}" NAME)
        set (output_ "${CMAKE_CURRENT_BINARY_DIR}/${name_}.embed")
        add_custom_command (
            OUTPUT "${output_}"
            COMMAND "${CMAKE_COMMAND}" "-DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/${file_}" "-DNAME=${file_}" "-DOUTPUT=${output_}" -P "${StroikaEmbedFileScript_}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${file_}" "${StroikaEmbedFileScript_}"
            VERBATIM
        )
        list (APPEND outputs_ "${output_}")
    endforeach ()
    # a custom command's outputs drive only targets of its own directory: so this directory's own target, which the target
    # (from another directory) depends on
    file (RELATIVE_PATH directory_ "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    string (MAKE_C_IDENTIFIER "${target}-embed-${directory_}" embedTarget_)
    add_custom_target ("${embedTarget_}" DEPENDS ${outputs_})
    add_dependencies ("${target}" "${embedTarget_}")
    target_include_directories ("${target}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction ()
