#
# googletest, as its Makefile builds it
#
set (version_ 1.18.0)

set (args_ -DCMAKE_CXX_STANDARD=20) # else Windows links fail (char8_t)
if (DEFINED STROIKA_THIRD_PARTY_CXX_FLAGS)
    # and so without what needs C++26: g++ refuses -freflection otherwise
    string (REGEX REPLACE "(^| )-freflection( |$)" "\\1" cxxFlags_ "${STROIKA_THIRD_PARTY_CXX_FLAGS}")
    list (APPEND args_ "-DCMAKE_CXX_FLAGS=${cxxFlags_}")
endif ()

StroikaThirdPartyComponent (GoogleTest
    ARCHIVE googletest-${version_}.tar.gz
    URLS https://github.com/google/googletest/archive/refs/tags/v${version_}.tar.gz
    CMAKE_ARGS ${args_}
)

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # gtest_main.pc's directories, relative to it
    ExternalProject_Add_Step (GoogleTest windowsFixes DEPENDEES install
        COMMAND "${CMAKE_COMMAND}" "-DPC=${STROIKA_BUILDS_DIR}lib/pkgconfig/gtest_main.pc" -P "${CMAKE_CURRENT_SOURCE_DIR}/WindowsFixes.cmake"
    )
endif ()
