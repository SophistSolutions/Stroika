#
# OpenSSL's .pc files, as its install put them, with the prefix relative to the file (as WindowsPc.cmake does): they name the
# prefix it was configured with - its default (CMakeLists.txt) - not where it was installed.
#     cmake -DPC_DIR=<the directory its install put them in> -P UnixPc.cmake
#
file (GLOB pcs_ "${PC_DIR}/*.pc")
if (NOT pcs_)
    message (FATAL_ERROR "No .pc files in ${PC_DIR}")
endif ()
foreach (pc_ IN LISTS pcs_)
    file (READ "${pc_}" text_)
    if (NOT text_ MATCHES "(^|\n)prefix=")
        message (FATAL_ERROR "${pc_} names no prefix")
    endif ()
    string (REGEX REPLACE "(^|\n)prefix=[^\n]*" "\\1prefix=\${pcfiledir}/../.." text_ "${text_}")
    file (WRITE "${pc_}" "${text_}")
endforeach ()
