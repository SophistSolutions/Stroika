#
# zstd, as its Makefile builds it
#
set (version_ 1.5.7)

set (args_ -DZSTD_BUILD_STATIC=ON -DZSTD_BUILD_SHARED=OFF) # static only: else, given both, a Mac's linker takes the .dylib
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    list (APPEND args_ -UCMAKE_CXX_COMPILER) # its library is C - and a Visual Studio build fails, given one
endif ()
if (CMAKE_CXX_FLAGS MATCHES "-flto")
    list (APPEND args_ -DC_FLAG_WA_NOEXECSTACK=FALSE) # else each LTO link warns that it drops the -Wa options
endif ()

StroikaThirdPartyComponent (zstd
    ARCHIVE zstd-${version_}.tar.gz
    URLS https://github.com/facebook/zstd/releases/download/v${version_}/zstd-${version_}.tar.gz
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS ${args_}
)

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # zstd's CMake package names the library zstd_static.lib, its .pc file zstd.lib: both
    ExternalProject_Add_Step (zstd copyLibrary DEPENDEES install
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${STROIKA_BUILDS_DIR}lib/zstd_static.lib" "${STROIKA_BUILDS_DIR}lib/zstd.lib"
    )
endif ()
