#
# include (StroikaDependencies)
#
# The third-party libraries the configuration's Stroika uses, as imported targets - found as Stroika's build finds them,
# for its own (Library/Sources/Stroika/Foundation/CMakeLists.txt and Frameworks') and for a program using Stroika's package
# (StroikaConfig.cmake), whose Stroika targets name them. Sets, in the caller's scope:
#     STROIKA_FOUNDATION_DEPENDENCIES     what Foundation links with (PUBLIC)
#     STROIKA_FRAMEWORKS_DEPENDENCIES     what Frameworks links with besides Foundation (PUBLIC): googletest, for Frameworks/Test
# Each by its own CMake package, where it installs one; curl, lzma and sqlite install none, so they are taken from where
# they are installed (STROIKA_THIRD_PARTY_INSTALL_DIR), as Stroika::curl, Stroika::lzma and Stroika::sqlite. Needs the
# configuration's settings (Stroika-Configuration.cmake), then StroikaBuildSettings.
#
function (StroikaInstalledLibrary_ name library)
    if (TARGET Stroika::${name}) # found already: the package found twice
        return ()
    endif ()
    find_library (StroikaInstalledLibrary_${name}_ NAMES ${library} PATHS "${STROIKA_THIRD_PARTY_INSTALL_DIR}lib" NO_DEFAULT_PATH REQUIRED)
    add_library (Stroika::${name} STATIC IMPORTED GLOBAL)
    set_target_properties (Stroika::${name} PROPERTIES
        IMPORTED_LOCATION "${StroikaInstalledLibrary_${name}_}"
        INTERFACE_INCLUDE_DIRECTORIES "${STROIKA_THIRD_PARTY_INSTALL_DIR}include"
    )
endfunction ()

set (STROIKA_FOUNDATION_DEPENDENCIES)
set (STROIKA_FRAMEWORKS_DEPENDENCIES)

set (StroikaDependencies_zlib_)
if (STROIKA_FEATURE_ZLib STREQUAL "use")
    find_package (ZLIB CONFIG REQUIRED COMPONENTS static) # just static: Stroika's makefile removes the shared library
    set (StroikaDependencies_zlib_ ZLIB::ZLIBSTATIC)
elseif (STROIKA_FEATURE_ZLib STREQUAL "use-system")
    find_package (ZLIB REQUIRED) # CMake's FindZLIB: a system's zlib need not install a CMake package
    set (StroikaDependencies_zlib_ ZLIB::ZLIB)
endif ()
list (APPEND STROIKA_FOUNDATION_DEPENDENCIES ${StroikaDependencies_zlib_})
if (NOT STROIKA_FEATURE_zstd STREQUAL "no")
    find_package (zstd CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES zstd::libzstd_static)
endif ()
if (NOT STROIKA_FEATURE_OpenSSL STREQUAL "no")
    find_package (OpenSSL CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES OpenSSL::SSL OpenSSL::Crypto)
endif ()
if (NOT STROIKA_FEATURE_LibCurl STREQUAL "no" AND NOT TARGET Stroika::curl)
    StroikaInstalledLibrary_ (curl curl)
    set_target_properties (Stroika::curl PROPERTIES INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
    target_link_libraries (Stroika::curl INTERFACE OpenSSL::SSL OpenSSL::Crypto zstd::libzstd_static ${StroikaDependencies_zlib_})
    # and whatever else curl's build found and used (on a Mac, Homebrew's libidn2 and brotli, and macOS frameworks): its
    # libcurl.pc's static link requirements, as make's links take them - each .pc file's prefix where it is now
    # (--define-prefix), as each names where it was installed first, and Stroika's installation may be elsewhere
    find_package (PkgConfig REQUIRED)
    execute_process (
        COMMAND "${CMAKE_COMMAND}" -E env "PKG_CONFIG_PATH=${STROIKA_THIRD_PARTY_INSTALL_DIR}lib/pkgconfig" "${PKG_CONFIG_EXECUTABLE}" --define-prefix --static --libs libcurl
        OUTPUT_VARIABLE StroikaDependencies_curl_ OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY
    )
    separate_arguments (StroikaDependencies_curl_ UNIX_COMMAND "${StroikaDependencies_curl_}")
    string (REPLACE "-framework;" "-framework " StroikaDependencies_curl_ "${StroikaDependencies_curl_}") # each -framework and its name: one item
    target_link_libraries (Stroika::curl INTERFACE ${StroikaDependencies_curl_})
endif ()
if (NOT STROIKA_FEATURE_LibCurl STREQUAL "no")
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES Stroika::curl)
endif ()
if (NOT STROIKA_FEATURE_boost STREQUAL "no")
    # as Stroika's makefile builds it (link=static runtime-link=static) - else its package rejects that variant
    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_STATIC_RUNTIME ON)
    find_package (Boost CONFIG REQUIRED COMPONENTS chrono filesystem json thread)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES Boost::chrono Boost::filesystem Boost::json Boost::thread)
endif ()
if (NOT STROIKA_FEATURE_fmtlib STREQUAL "no")
    find_package (fmt CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES fmt::fmt)
endif ()
if (NOT STROIKA_FEATURE_libxml2 STREQUAL "no")
    find_package (LibXml2 CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES LibXml2::LibXml2)
endif ()
if (NOT STROIKA_FEATURE_Xerces STREQUAL "no")
    find_package (XercesC CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES XercesC::XercesC)
endif ()
if (NOT STROIKA_FEATURE_mongocxxdriver STREQUAL "no")
    find_package (mongocxx CONFIG REQUIRED)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES mongo::mongocxx_static)
endif ()
if (NOT STROIKA_FEATURE_LZMA STREQUAL "no")
    StroikaInstalledLibrary_ (lzma lzma)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES Stroika::lzma)
endif ()
if (NOT STROIKA_FEATURE_sqlite STREQUAL "no")
    StroikaInstalledLibrary_ (sqlite sqlite)
    list (APPEND STROIKA_FOUNDATION_DEPENDENCIES Stroika::sqlite)
endif ()

if (NOT STROIKA_FEATURE_GoogleTest STREQUAL "no")
    find_package (GTest CONFIG REQUIRED)
    list (APPEND STROIKA_FRAMEWORKS_DEPENDENCIES GTest::gtest)
endif ()
