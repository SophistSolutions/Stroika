#
# cmake -DFROM=<zlib's build directory, its configuration's subdirectory> -DLIB_DIR=<installed lib directory> -DD=<d or nothing>
#       -P WindowsFixes.cmake
#
# After zlib's install with Visual Studio, as its Makefile does: copies the debug information zlib's build leaves beside
# its libraries (z<d>.pdb, zs<d>.pdb), where there is any, and makes zlib.pc name the static library, zs<d>.
#
foreach (pdb_ IN ITEMS z${D}.pdb zs${D}.pdb)
    if (EXISTS "${FROM}/${pdb_}")
        file (COPY "${FROM}/${pdb_}" DESTINATION "${LIB_DIR}")
    endif ()
endforeach ()
set (pc_ "${LIB_DIR}/pkgconfig/zlib.pc")
file (READ "${pc_}" text_)
string (REGEX REPLACE "(^|\n)libdir=[^\n]*" "\\1libdir=\${prefix}/lib" text_ "${text_}")
string (REGEX REPLACE "(^|\n)Libs:[^\n]*" "\\1Libs: -L\${libdir} -lzs${D}" text_ "${text_}")
file (CONFIGURE OUTPUT "${pc_}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
