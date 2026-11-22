#
# libxml2, as its Makefile builds it
#
set (version_ 2.15.4)

# No catalogs (which map a resource's name to where to load it from): Stroika's XML providers resolve what a schema imports
# themselves (Schema's resolver). libxml2 would read its default catalog from a path compiled in - the directory it was
# installed to, which a program run elsewhere does not have, and on Windows any user could create.
# No tests: never run here - and without catalogs, libxml2 2.15.4's tests still need xmlcatalog, so its configure fails.
set (args_ -DLIBXML2_WITH_ICONV=OFF -DLIBXML2_WITH_HTTP=OFF -DLIBXML2_WITH_LZMA=OFF -DLIBXML2_WITH_PYTHON=OFF -DLIBXML2_WITH_CATALOG=OFF
           -DLIBXML2_WITH_TESTS=OFF -DBUILD_SHARED_LIBS=OFF)
if (STROIKA_FEATURE_ZLib STREQUAL "no")
    list (APPEND args_ -DLIBXML2_WITH_ZLIB=OFF)
endif ()
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio" AND "-LTCG" IN_LIST STROIKA_LINK_OPTIONS)
    set (cFlags_ "${CMAKE_C_FLAGS_RELEASE}")
    # MSVC 19.52 (not 19.51) for x86, with link-time code generation (-GL), builds libxml2's list of a schema's identity
    # constraints wrong - so validating against a schema with an xs:key or xs:unique fails (Tests/34):
    #   Internal error: xmlSchemaIDCRegisterMatchers, Could not find an augmented IDC item for an IDC definition.
    # libxml2 built without -GL there (still -O2). Any change near the code (a fprintf, one file at -Od) hides it.
    if (STROIKA_ARCH STREQUAL "x86")
        execute_process (COMMAND "${CMAKE_C_COMPILER}" OUTPUT_QUIET ERROR_VARIABLE banner_)
        if (banner_ MATCHES "Version ([0-9]+\\.[0-9]+)" AND CMAKE_MATCH_1 VERSION_EQUAL 19.52)
            string (REGEX REPLACE "(^| )-GL( |$)" "\\1" cFlags_ "${cFlags_}")
        endif ()
    endif ()
    # with link-time code generation these warnings come at link time - kept with each object when given to the compiler
    list (APPEND args_ "-DCMAKE_C_FLAGS_RELEASE=${cFlags_} -wd4701 -wd4702 -wd4703 -wd4706 -wd4789")
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
