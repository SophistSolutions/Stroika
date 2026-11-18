#
# OpenSSL's .pc files, with Visual Studio - which its install leaves out, as they are broken there - fixed as its Makefile
# fixes them, for pkg-config --msvc-syntax: the prefix relative to the file, and the libraries as -l names; and with
# UNIX line endings, as its Makefile's sed leaves them (its perl writes CRLF, whose CR pkg-config could keep in a value).
#     cmake -DSOURCE_DIR=<its sources, where its configure wrote them> -DBUILDS_DIR=<the configuration's Builds/> -P WindowsPc.cmake
#
file (GLOB pcs_ "${SOURCE_DIR}/*.pc")
file (MAKE_DIRECTORY "${BUILDS_DIR}lib/pkgconfig")
foreach (pc_ IN LISTS pcs_)
    file (READ "${pc_}" text_)
    string (REPLACE "\r" "" text_ "${text_}")
    string (REGEX REPLACE "(^|\n)prefix=[^\n]*" "\\1prefix=\${pcfiledir}/../.." text_ "${text_}")
    string (REPLACE "-lcrypto" "-llibcrypto" text_ "${text_}")
    string (REPLACE "-lssl" "-llibssl" text_ "${text_}")
    string (REPLACE "crypt32.lib" "-lcrypt32" text_ "${text_}")
    string (REPLACE "ws2_32.lib" "-lws2_32" text_ "${text_}")
    get_filename_component (name_ "${pc_}" NAME)
    file (CONFIGURE OUTPUT "${BUILDS_DIR}lib/pkgconfig/${name_}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
endforeach ()
