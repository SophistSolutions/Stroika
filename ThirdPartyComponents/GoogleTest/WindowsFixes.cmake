#
# cmake -DPC=<installed gtest_main.pc> -P WindowsFixes.cmake
#
# After googletest's install with Visual Studio, as its Makefile does: gtest_main.pc's libdir and includedir, relative to
# the .pc file.
#
file (READ "${PC}" text_)
string (REGEX REPLACE "(^|\n)libdir=[^\n]*" "\\1libdir=\${pcfiledir}/.." text_ "${text_}")
string (REGEX REPLACE "(^|\n)includedir=[^\n]*" "\\1includedir=\${pcfiledir}/../../include" text_ "${text_}")
file (CONFIGURE OUTPUT "${PC}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
