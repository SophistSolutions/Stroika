#
# Xerces-C, as its Makefile builds it
#
set (version_ 3.3.0)

set (args_ -Dnetwork:BOOL=OFF -Dmessage-loader=inmemory -DBUILD_SHARED_LIBS:BOOL=OFF)
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    list (APPEND args_ -Dtranscoder=windows)
    set (cxxExtra_ " -DWIN32")
    if ("-LTCG" IN_LIST STROIKA_LINK_OPTIONS)
        # with link-time code generation these warnings come at link time - kept with each object when given to the compiler
        string (APPEND cxxExtra_ " -wd4702 -wd4706")
    endif ()
    string (TOUPPER "${STROIKA_THIRD_PARTY_BUILD_TYPE}" buildType_)
    list (APPEND args_ "-DCMAKE_C_FLAGS_${buildType_}=${CMAKE_C_FLAGS_${buildType_}} -DWIN32" "-DCMAKE_CXX_FLAGS_${buildType_}=${CMAKE_CXX_FLAGS_${buildType_}}${cxxExtra_}")
else ()
    if (CMAKE_HOST_APPLE)
        list (APPEND args_ -Dtranscoder=iconv)
    else ()
        list (APPEND args_ -Dtranscoder=gnuiconv)
    endif ()
    # its CMakeLists.txt builds it as C++14 (its -std= after ours), so without what needs C++26: g++ refuses -freflection
    string (REGEX REPLACE "(^| )-freflection( |$)" "\\1" cxxFlags_ "${STROIKA_THIRD_PARTY_CXX_FLAGS}")
    list (APPEND args_ "-DCMAKE_C_FLAGS=${STROIKA_THIRD_PARTY_C_FLAGS} -pthread" "-DCMAKE_CXX_FLAGS=${cxxFlags_} -pthread")
endif ()

set (patches_ "${CMAKE_CURRENT_SOURCE_DIR}/Patches")
StroikaThirdPartyComponent (Xerces
    ARCHIVE xerces-c-${version_}.tar.gz
    URLS http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-${version_}.tar.gz
         http://apache.cs.utah.edu//xerces/c/3/sources/xerces-c-${version_}.tar.gz
         http://www.apache.org/dist/xerces/c/3/sources/xerces-c-${version_}.tar.gz
         http://www-us.apache.org/dist/xerces/c/3/sources/xerces-c-${version_}.tar.gz
    # (not its Makefile's two SilenceSanitizerWarningAboutMemcpyNullPtr patches: 3.3.0 has their fix, so neither applies)
    PATCH_COMMAND "${STROIKA_PATCH}" -p0 <SOURCE_DIR>/tests/src/Char16Test/Char16Test.cpp "${patches_}/Char16Test_IOSTREAMCHANGE.PATCH"
          COMMAND "${STROIKA_PATCH}" -p0 <SOURCE_DIR>/src/xercesc/util/XMLString.hpp "${patches_}/AvoidMemMoveOfZero.patch"
    CMAKE_ARGS ${args_}
)

# xerces-c.pc: the definitions a static library's user needs; with Visual Studio, also its directories relative to it,
# and the library's name - and the debug information its build leaves beside the library
set (visualStudio_ OFF)
set (d_)
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    set (visualStudio_ ON)
    if (STROIKA_ASSERTIONS_ENABLED)
        set (d_ d)
    endif ()
endif ()
ExternalProject_Add_Step (Xerces fixes DEPENDEES install
    COMMAND "${CMAKE_COMMAND}" "-DBUILDS_DIR=${STROIKA_BUILDS_DIR}" "-DPDB_DIR=<BINARY_DIR>/src/xerces-c.dir/${STROIKA_THIRD_PARTY_BUILD_TYPE}"
            "-DVISUAL_STUDIO=${visualStudio_}" "-DD=${d_}" -P "${CMAKE_CURRENT_SOURCE_DIR}/Fixes.cmake"
)
