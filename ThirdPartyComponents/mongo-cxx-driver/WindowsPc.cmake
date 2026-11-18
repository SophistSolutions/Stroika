#
# mongoc2-static.pc, with Visual Studio, fixed as its Makefile fixes it: its Libs gives zstd's library by its path, as
# -l<path>/zstd.lib (as of r4.6.1 - mongo-c-driver 2.5.6), which pkg-config cannot take - so libzstd is required instead.
# Only when it does: so this does nothing once mongo-c-driver writes it right. Written with UNIX line endings, as its
# Makefile's sed leaves them.
#     cmake -DPC=<the installed mongoc2-static.pc> -P WindowsPc.cmake
#
file (READ "${PC}" text_)
if (text_ MATCHES "-l[^ ]*zstd\\.lib")
    string (REPLACE "\r" "" text_ "${text_}")
    string (REGEX REPLACE "-l[^ ]*/zstd\\.lib" "" text_ "${text_}")
    string (REGEX REPLACE "(Requires: [^\n]*)" "\\1, libzstd" text_ "${text_}")
    file (CONFIGURE OUTPUT "${PC}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
endif ()
