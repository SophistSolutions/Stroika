#
# cmake -DBUILDS_DIR=<the configuration's Builds directory, with a trailing /> -DD=<d or nothing> -P WindowsFixes.cmake
#
# After libxml2's install with Visual Studio, as its Makefile does: the library libxml2s<d>.lib as xml2.lib too, and
# libxml-2.0.pc's includedir the configuration's include directory.
#
file (COPY_FILE "${BUILDS_DIR}lib/libxml2s${D}.lib" "${BUILDS_DIR}lib/xml2.lib" ONLY_IF_DIFFERENT)
set (pc_ "${BUILDS_DIR}lib/pkgconfig/libxml-2.0.pc")
file (READ "${pc_}" text_)
string (REGEX REPLACE "(^|\n)includedir=[^\n]*" "\\1includedir=${BUILDS_DIR}include" text_ "${text_}")
file (CONFIGURE OUTPUT "${pc_}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
