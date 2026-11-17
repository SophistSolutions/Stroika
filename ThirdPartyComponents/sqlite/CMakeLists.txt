#
# sqlite, as its Makefile builds it - from its amalgamation, which has no CMake build of its own: Amalgamation/CMakeLists.txt
# is copied in among its sources
#
set (version_ 3540100)

StroikaThirdPartyComponent (sqlite
    ARCHIVE sqlite-amalgamation-${version_}.zip
    URLS https://www.sqlite.org/2026/sqlite-amalgamation-${version_}.zip
         https://www.sqlite.org/2025/sqlite-amalgamation-${version_}.zip
         https://www.sqlite.org/2024/sqlite-amalgamation-${version_}.zip
         https://www.sqlite.org/2023/sqlite-amalgamation-${version_}.zip
    PATCH_COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/Amalgamation/CMakeLists.txt" <SOURCE_DIR>/CMakeLists.txt
    CMAKE_ARGS "-DSTROIKA_CONFIGURATION_FILE=${configurationDir_}/Stroika-Configuration.cmake" "-DSQLITE_PC=${CMAKE_CURRENT_SOURCE_DIR}/sqlite.pc"
)
