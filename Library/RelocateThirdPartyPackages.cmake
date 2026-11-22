#
# Run by cmake --install (Library/CMakeLists.txt), once the third-party components are copied from the configuration's
# Builds directory (StroikaBuildsDir_) into the installation: a CMake package's targets file that names a file there by its
# absolute path - as mongo-c-driver's names its zstd library - names it relative to the installation instead, as the
# rest of each such file does (_IMPORT_PREFIX, which it computes from where it is). The other files naming the Builds
# directory - boost's packages, which use it only while it is where they are, and pkg-config files (pkg-config's
# --define-prefix) - are left as they are.
#
file (GLOB_RECURSE files_ "${CMAKE_INSTALL_PREFIX}/lib/cmake/*.cmake" "${CMAKE_INSTALL_PREFIX}/cmake/*.cmake")
foreach (file_ ${files_})
    file (READ "${file_}" text_)
    if (text_ MATCHES "_IMPORT_PREFIX")
        string (REPLACE "${StroikaBuildsDir_}" "\${_IMPORT_PREFIX}/" relocated_ "${text_}")
        if (NOT relocated_ STREQUAL text_)
            message (STATUS "Relocating: ${file_}")
            file (WRITE "${file_}" "${relocated_}")
        endif ()
    endif ()
endforeach ()
