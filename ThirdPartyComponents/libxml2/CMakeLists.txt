#
# libxml2, as its Makefile builds it
#
set (version_ 2.15.4)

set (args_ -DLIBXML2_WITH_ICONV=OFF -DLIBXML2_WITH_HTTP=OFF -DLIBXML2_WITH_LZMA=OFF -DLIBXML2_WITH_PYTHON=OFF -DBUILD_SHARED_LIBS=OFF)
if (STROIKA_FEATURE_ZLib STREQUAL "no")
    list (APPEND args_ -DLIBXML2_WITH_ZLIB=OFF)
endif ()
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio" AND "-LTCG" IN_LIST STROIKA_LINK_OPTIONS)
    # with link-time code generation these warnings come at link time - kept with each object when given to the compiler
    list (APPEND args_ "-DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE} -wd4701 -wd4702 -wd4703 -wd4706 -wd4789")
endif ()

StroikaThirdPartyComponent (libxml2
    ARCHIVE libxml2-${version_}.tar.gz
    URLS https://github.com/GNOME/libxml2/archive/refs/tags/v${version_}.tar.gz
    CMAKE_ARGS ${args_}
    DEPENDS zlib
)

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # the library as xml2.lib too, and libxml-2.0.pc's includedir the configuration's include directory
    set (d_)
    if (STROIKA_ASSERTIONS_ENABLED)
        set (d_ d)
    endif ()
    ExternalProject_Add_Step (libxml2 windowsFixes DEPENDEES install
        COMMAND "${CMAKE_COMMAND}" "-DBUILDS_DIR=${STROIKA_BUILDS_DIR}" "-DD=${d_}" -P "${CMAKE_CURRENT_SOURCE_DIR}/WindowsFixes.cmake"
    )
endif ()
