#
# zlib, as its Makefile builds it
#
set (version_ 1.3.2)

set (args_ "-DINSTALL_PKGCONFIG_DIR=${STROIKA_BUILDS_DIR}lib/pkgconfig")
if (STROIKA_BUILD_PLATFORM STREQUAL "Unix")
    list (APPEND args_ -DCMAKE_SKIP_INSTALL_RPATH=YES -DCMAKE_SKIP_RPATH=YES)
endif ()
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    list (APPEND args_ -DZLIB_BUILD_STATIC=ON)
endif ()

StroikaThirdPartyComponent (zlib
    ARCHIVE zlib-${version_}.tar.gz
    URLS https://github.com/madler/zlib/archive/v${version_}.tar.gz
         http://zlib.net/zlib-${version_}.tar.gz
         "http://prdownloads.sourceforge.net/libpng/zlib-${version_}.tar.gz?download"
    CMAKE_ARGS ${args_}
    BUILD_TARGETS zlibstatic zlib # not its examples
)

if (STROIKA_BUILD_PLATFORM STREQUAL "Unix")
    # no shared library: what links against one in Builds/<configuration>/lib finds it there only at link time (curl's
    # build, for one)
    ExternalProject_Add_Step (zlib removeSharedLibrary DEPENDEES install
        COMMAND "${CMAKE_COMMAND}" "-DDIRECTORY=${STROIKA_BUILDS_DIR}lib" "-DGLOBS=libz.so*|libz*.dylib" -P "${CMAKE_CURRENT_SOURCE_DIR}/../RemoveFiles.cmake"
    )
endif ()
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # the debug information zlib's build leaves beside its libraries, and a .pc file naming the static library
    set (d_)
    if (STROIKA_ASSERTIONS_ENABLED)
        set (d_ d)
    endif ()
    ExternalProject_Add_Step (zlib windowsFixes DEPENDEES install
        COMMAND "${CMAKE_COMMAND}" "-DFROM=${STROIKA_THIRD_PARTY_OUTPUT_DIR}" "-DLIB_DIR=${STROIKA_BUILDS_DIR}lib" "-DD=${d_}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/WindowsFixes.cmake"
    )
endif ()
