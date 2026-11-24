#
# StroikaCopyFilesToExeDir (<target>)
#
# Puts beside the target's executable the files the configuration says each executable needs there
# (STROIKA_COPY_FILES_TO_EXE_DIR - with MSVC's AddressSanitizer, its runtime DLL), as make does after each link
# (LinkTime_CopyFilesToEXEDir). One step copies them for all the executables a directory builds, before any of them links -
# so no two steps of a parallel build write the same file at once - and only when one is missing there, or has changed.
#
function (StroikaCopyFilesToExeDir target)
    if (NOT STROIKA_COPY_FILES_TO_EXE_DIR)
        return ()
    endif ()
    get_directory_property (copyTarget_ StroikaCopyFilesToExeDir_)
    if (NOT copyTarget_)
        # the copies, in the target's directory as CMake makes it: its RUNTIME_OUTPUT_DIRECTORY, else the binary directory -
        # and with a multi-configuration generator, a subdirectory for the build type, unless that is a generator expression
        get_target_property (directory_ ${target} RUNTIME_OUTPUT_DIRECTORY)
        if (NOT directory_)
            set (directory_ "${CMAKE_CURRENT_BINARY_DIR}")
        endif ()
        get_property (multiConfig_ GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if (multiConfig_ AND NOT directory_ MATCHES "\\$<")
            string (APPEND directory_ "/$<CONFIG>")
        endif ()
        set (copies_)
        foreach (file_ IN LISTS STROIKA_COPY_FILES_TO_EXE_DIR)
            cmake_path (GET file_ FILENAME name_)
            list (APPEND copies_ "${directory_}/${name_}")
        endforeach ()
        # its name kept short, as StroikaEmbedFiles' (Windows' MAX_PATH)
        set (copyTarget_ "${target}-copy")
        add_custom_command (OUTPUT ${copies_}
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>"
            COMMAND "${CMAKE_COMMAND}" -E copy ${STROIKA_COPY_FILES_TO_EXE_DIR} "$<TARGET_FILE_DIR:${target}>"
            DEPENDS ${STROIKA_COPY_FILES_TO_EXE_DIR}
            VERBATIM
        )
        add_custom_target ("${copyTarget_}" DEPENDS ${copies_})
        set_directory_properties (PROPERTIES StroikaCopyFilesToExeDir_ "${copyTarget_}")
    endif ()
    add_dependencies ("${target}" "${copyTarget_}")
endfunction ()
