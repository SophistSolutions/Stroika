#
# StroikaCopyFilesToExeDir (<target>)
#
# Puts beside the target's executable the files the configuration says each executable needs there
# (STROIKA_COPY_FILES_TO_EXE_DIR - with MSVC's AddressSanitizer, its runtime DLL), as make does after each link
# (LinkTime_CopyFilesToEXEDir). One step copies them for all the executables a directory builds, before any of them links -
# so no two steps of a parallel build write the same file at once.
#
function (StroikaCopyFilesToExeDir target)
    if (NOT STROIKA_COPY_FILES_TO_EXE_DIR)
        return ()
    endif ()
    get_directory_property (copyTarget_ StroikaCopyFilesToExeDir_)
    if (NOT copyTarget_)
        set (copyTarget_ "${target}-CopyFilesToExeDir")
        add_custom_target ("${copyTarget_}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${target}>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${STROIKA_COPY_FILES_TO_EXE_DIR} "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
        set_directory_properties (PROPERTIES StroikaCopyFilesToExeDir_ "${copyTarget_}")
    endif ()
    add_dependencies ("${target}" "${copyTarget_}")
endfunction ()
