#
# lzma - the LZMA SDK's 7z decoder - as its Makefile builds it: the SDK has no CMake build of its own, so SDK/CMakeLists.txt
# is copied in among its sources
#
set (version_ 26.06)
string (REPLACE "." "" archive_ "lzma${version_}")

# the configuration's C flags, as its Makefile gives them - its own, so without -DNDEBUG - less any sanitizer and -flto
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    string (TOUPPER "${STROIKA_THIRD_PARTY_BUILD_TYPE}" buildType_)
    set (flagsVariable_ CMAKE_C_FLAGS_${buildType_})
else ()
    set (flagsVariable_ CMAKE_C_FLAGS)
endif ()
string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" flags_ "${${flagsVariable_}}")
string (REGEX REPLACE "(^| )-flto( |$)" "\\1" flags_ "${flags_}")

StroikaThirdPartyComponent (lzma
    ARCHIVE ${archive_}.7z
    URLS https://github.com/ip7z/7zip/releases/download/${version_}/${archive_}.7z
         https://sourceforge.net/projects/sevenzip/files/LZMA%20SDK/${archive_}.7z
    PATCH_COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/SDK/CMakeLists.txt" <SOURCE_DIR>/CMakeLists.txt
    CMAKE_ARGS "-D${flagsVariable_}=${flags_}" "-DLZMA_PC=${CMAKE_CURRENT_SOURCE_DIR}/lzma.pc"
)
