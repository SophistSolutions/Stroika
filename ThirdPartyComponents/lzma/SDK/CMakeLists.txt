#
# The LZMA SDK's C 7z decoder as one library, as ThirdPartyComponents/lzma/Makefile builds it (with its Subdir-Makefile) -
# this file is copied in among the SDK's sources: the objects of the SDK's C/Util/7z/makefile.gcc, less that sample
# program (7zMain, 7zFile, 7zAlloc) - all Stroika uses (DataExchange::Archive::_7z::Reader); the headers Stroika includes,
# as lzma/7z.h and so on; and Stroika's lzma.pc.
#
cmake_minimum_required (VERSION 3.22)
project (lzma LANGUAGES C)

# as that makefile: PPMd-compressed archives too; no encoders
add_compile_definitions (Z7_PPMD_SUPPORT Z7_EXTRACT_ONLY)
add_library (lzma STATIC
    C/7zArcIn.c C/7zBuf.c C/7zBuf2.c C/7zCrc.c C/7zCrcOpt.c C/7zDec.c C/7zStream.c C/Bcj2.c C/Bra.c C/Bra86.c C/BraIA64.c
    C/CpuArch.c C/Delta.c C/Lzma2Dec.c C/LzmaDec.c C/Ppmd7.c C/Ppmd7Dec.c
)

install (TARGETS lzma ARCHIVE DESTINATION lib)
install (FILES C/7z.h C/7zCrc.h C/7zTypes.h DESTINATION include/lzma)
install (FILES "${LZMA_PC}" DESTINATION lib/pkgconfig)
