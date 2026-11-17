#
# The Foundation library. Each directory's CMakeLists.txt lists its sources, as its Makefile does - keep the two in step
# while make builds it too.
#
add_library (stroika-foundation STATIC)

target_compile_definitions (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_DEFINITIONS})
target_compile_options (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_COMPILE_OPTIONS})
target_include_directories (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_INCLUDE_DIRECTORIES} ${STROIKA_PLATFORM_INCLUDE_DIRECTORIES})

#
# The third-party components the configuration uses - by their own CMake packages, where they install one; curl, lzma and
# sqlite install none, so they are taken from where Stroika's makefiles install them (STROIKA_BUILDS_DIR).
#
function (StroikaInstalledLibrary_ name library)
    find_library (StroikaInstalledLibrary_${name}_ NAMES ${library} PATHS "${STROIKA_BUILDS_DIR}lib" NO_DEFAULT_PATH REQUIRED)
    add_library (Stroika::${name} STATIC IMPORTED GLOBAL)
    set_target_properties (Stroika::${name} PROPERTIES
        IMPORTED_LOCATION "${StroikaInstalledLibrary_${name}_}"
        INTERFACE_INCLUDE_DIRECTORIES "${STROIKA_BUILDS_DIR}include"
    )
endfunction ()

if (STROIKA_FEATURE_ZLib STREQUAL "use")
    find_package (ZLIB CONFIG REQUIRED COMPONENTS static) # just static: Stroika's makefile removes the shared library
    set (zlib_ ZLIB::ZLIBSTATIC)
elseif (STROIKA_FEATURE_ZLib STREQUAL "use-system")
    find_package (ZLIB REQUIRED) # CMake's FindZLIB: a system's zlib need not install a CMake package
    set (zlib_ ZLIB::ZLIB)
endif ()
if (zlib_)
    target_link_libraries (stroika-foundation PUBLIC ${zlib_})
endif ()
if (NOT STROIKA_FEATURE_zstd STREQUAL "no")
    find_package (zstd CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC zstd::libzstd_static)
endif ()
if (NOT STROIKA_FEATURE_OpenSSL STREQUAL "no")
    find_package (OpenSSL CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif ()
if (NOT STROIKA_FEATURE_LibCurl STREQUAL "no")
    StroikaInstalledLibrary_ (curl curl)
    set_target_properties (Stroika::curl PROPERTIES INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
    target_link_libraries (Stroika::curl INTERFACE OpenSSL::SSL OpenSSL::Crypto zstd::libzstd_static ${zlib_})
    target_link_libraries (stroika-foundation PUBLIC Stroika::curl)
endif ()
if (NOT STROIKA_FEATURE_boost STREQUAL "no")
    # as Stroika's makefile builds it (link=static runtime-link=static) - else its package rejects that variant
    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_STATIC_RUNTIME ON)
    find_package (Boost CONFIG REQUIRED COMPONENTS chrono filesystem json thread)
    target_link_libraries (stroika-foundation PUBLIC Boost::chrono Boost::filesystem Boost::json Boost::thread)
endif ()
if (NOT STROIKA_FEATURE_fmtlib STREQUAL "no")
    find_package (fmt CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC fmt::fmt)
endif ()
if (NOT STROIKA_FEATURE_libxml2 STREQUAL "no")
    find_package (LibXml2 CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC LibXml2::LibXml2)
endif ()
if (NOT STROIKA_FEATURE_Xerces STREQUAL "no")
    find_package (XercesC CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC XercesC::XercesC)
endif ()
if (NOT STROIKA_FEATURE_mongocxxdriver STREQUAL "no")
    find_package (mongocxx CONFIG REQUIRED)
    target_link_libraries (stroika-foundation PUBLIC mongo::mongocxx_static)
endif ()
if (NOT STROIKA_FEATURE_LZMA STREQUAL "no")
    StroikaInstalledLibrary_ (lzma lzma)
    target_link_libraries (stroika-foundation PUBLIC Stroika::lzma)
endif ()
if (NOT STROIKA_FEATURE_sqlite STREQUAL "no")
    StroikaInstalledLibrary_ (sqlite sqlite)
    target_link_libraries (stroika-foundation PUBLIC Stroika::sqlite)
endif ()

add_subdirectory (Cache)
add_subdirectory (Characters)
add_subdirectory (Common)
add_subdirectory (Cryptography)
add_subdirectory (Database)
add_subdirectory (DataExchange)
add_subdirectory (Debug)
add_subdirectory (Execution)
add_subdirectory (IO)
add_subdirectory (Linguistics)
add_subdirectory (Math)
add_subdirectory (Memory)
add_subdirectory (Streams)
add_subdirectory (Time)
