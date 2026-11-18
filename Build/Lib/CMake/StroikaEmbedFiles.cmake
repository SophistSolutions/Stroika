#
# StroikaEmbedFiles (<target> [RELATIVE_TO <directory>] <file>...)
#
# Each file (relative to the directory - by default the calling directory) embedded as bytes - <name>.embed, an xxd -i
# style C array (EmbedFile.cmake) named for the file as given, which xxd would be run from that directory - generated in
# the calling directory's binary directory, which the target's sources may then include. Bytes, never a C++ string
# literal: MSVC is passed no /utf-8, so non-ASCII in someone else's file would be read as cp1252.
#
set (StroikaEmbedFileScript_ "${CMAKE_CURRENT_LIST_DIR}/EmbedFile.cmake")

function (StroikaEmbedFiles target)
    cmake_parse_arguments (PARSE_ARGV 1 arg "" "RELATIVE_TO" "")
    set (from_ "${CMAKE_CURRENT_SOURCE_DIR}")
    if (arg_RELATIVE_TO)
        cmake_path (ABSOLUTE_PATH arg_RELATIVE_TO BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE from_)
    endif ()
    set (outputs_)
    foreach (file_ IN LISTS arg_UNPARSED_ARGUMENTS)
        get_filename_component (name_ "${file_}" NAME)
        set (output_ "${CMAKE_CURRENT_BINARY_DIR}/${name_}.embed")
        add_custom_command (
            OUTPUT "${output_}"
            COMMAND "${CMAKE_COMMAND}" "-DINPUT=${from_}/${file_}" "-DNAME=${file_}" "-DOUTPUT=${output_}" -P "${StroikaEmbedFileScript_}"
            DEPENDS "${from_}/${file_}" "${StroikaEmbedFileScript_}"
            VERBATIM
        )
        list (APPEND outputs_ "${output_}")
    endforeach ()
    # a custom command's outputs drive only targets of its own directory: so this directory's own target, which the target
    # (from another directory) depends on - named for the directory's last part only, as a Visual Studio generator puts the
    # name twice in the path of each such project's build state file, under this binary directory: one longer than 260
    # characters (Windows' MAX_PATH) fails the next build (MSB3501)
    get_filename_component (directory_ "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    string (MAKE_C_IDENTIFIER "${target}-embed-${directory_}" embedTarget_)
    set (n_ 2)
    while (TARGET "${embedTarget_}")
        string (MAKE_C_IDENTIFIER "${target}-embed-${directory_}-${n_}" embedTarget_)
        math (EXPR n_ "${n_} + 1")
    endwhile ()
    add_custom_target ("${embedTarget_}" DEPENDS ${outputs_})
    add_dependencies ("${target}" "${embedTarget_}")
    target_include_directories ("${target}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
endfunction ()
