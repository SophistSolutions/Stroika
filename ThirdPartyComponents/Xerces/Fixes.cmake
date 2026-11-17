#
# cmake -DBUILDS_DIR=<the configuration's Builds directory, with a trailing /> -DPDB_DIR=<where Xerces' build leaves its
#       debug information> -DVISUAL_STUDIO=<ON or OFF> -DD=<d or nothing> -P Fixes.cmake
#
# After Xerces' install, as its Makefile does: xerces-c.pc's CFlags line gets the definitions a static library's user needs;
# with Visual Studio, its prefix and libdir become relative to it, the library's name xerces-c_3<d>, and the debug
# information the build leaves beside the library is copied beside the installed one.
#
set (pc_ "${BUILDS_DIR}lib/pkgconfig/xerces-c.pc")
file (READ "${pc_}" text_)
if (VISUAL_STUDIO)
    file (GLOB pdbs_ "${PDB_DIR}/xerces*.pdb")
    if (pdbs_)
        file (COPY ${pdbs_} DESTINATION "${BUILDS_DIR}lib")
    endif ()
    string (REGEX REPLACE "(^|\n)prefix=[^\n]*" "\\1prefix=\${pcfiledir}/../.." text_ "${text_}")
    string (REGEX REPLACE "(^|\n)libdir=[^\n]*" "\\1libdir=\${pcfiledir}/.." text_ "${text_}")
    string (REPLACE "-lxerces-c" "-lxerces-c_3${D}" text_ "${text_}")
endif ()
string (REGEX REPLACE "(^|\n)CFlags: " "\\1CFlags: -DXML_LIBRARY=1 -DXERCES_STATIC_LIBRARY=1" text_ "${text_}")
file (CONFIGURE OUTPUT "${pc_}" CONTENT "${text_}" @ONLY NEWLINE_STYLE UNIX)
