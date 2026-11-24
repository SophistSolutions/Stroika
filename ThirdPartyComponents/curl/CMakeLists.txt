#
# libcurl, as its Makefile builds it: autoreconf, then its own configure, built among its sources - UNIX only, as there
#
set (version_ 8.23.0)

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    message (FATAL_ERROR "ThirdPartyComponents/curl: built only for UNIX, as its Makefile builds it")
endif ()

# the configuration's flags, as its Makefile gives them - with no sanitizer for a g++ before 12
set (cFlags_ "${CMAKE_C_FLAGS}")
execute_process (COMMAND "${CMAKE_C_COMPILER}" -dumpfullversion OUTPUT_VARIABLE compilerVersion_ OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process (COMMAND "${CMAKE_C_COMPILER}" --version OUTPUT_VARIABLE compilerBanner_ ERROR_QUIET)
if (NOT compilerBanner_ MATCHES "clang" AND compilerVersion_ VERSION_LESS 12)
    string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" cFlags_ "${cFlags_}")
endif ()
# and the debug information naming its sources, and the headers it includes, where they will be once deployed
StroikaThirdPartyDebugPathFlags (debugPathFlags_)
list (JOIN debugPathFlags_ " " debugPathFlags_)
string (APPEND cFlags_ " ${debugPathFlags_}")
# the configuration's include directories; then CPATH's, searched after everything else (a Mac shell's
# CPATH=/opt/homebrew/include would else put Homebrew's OpenSSL headers ahead of the configuration's), and CPATH unset
set (cppFlags_)
foreach (directory_ IN LISTS STROIKA_PLATFORM_INCLUDE_DIRECTORIES)
    string (APPEND cppFlags_ " -I${directory_}")
endforeach ()
string (REPLACE ":" ";" cpath_ "$ENV{CPATH}")
foreach (directory_ IN LISTS cpath_)
    if (directory_)
        string (APPEND cppFlags_ " -idirafter ${directory_}")
    endif ()
endforeach ()
list (JOIN STROIKA_LINK_OPTIONS " " ldFlags_)
string (REGEX REPLACE "(^| )--std=[^ ]*" "" ldFlags_ "${ldFlags_}")
# and, built by Conan, a program's debug map that does not name the directory it is installed into
# (StroikaThirdPartyComponent.cmake)
if (STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS)
    string (APPEND ldFlags_ " ${STROIKA_THIRD_PARTY_DEBUG_PATH_LINK_FLAGS}")
endif ()

set (configure_ --enable-static --disable-shared)
if (STROIKA_FEATURE_OpenSSL STREQUAL "no")
    list (APPEND configure_ --without-ssl)
else ()
    list (APPEND configure_ --with-ssl)
endif ()
if (STROIKA_FEATURE_ZLib STREQUAL "no")
    list (APPEND configure_ --without-zlib)
else ()
    list (APPEND configure_ --with-zlib)
endif ()
list (APPEND configure_ --without-librtmp --without-libssh2 --without-nghttp2 --without-libpsl --disable-smtp --disable-ldap --disable-ldaps
    --disable-manual --disable-rtsp --disable-gopher --disable-dict --disable-telnet --disable-pop3 --disable-imap --disable-ares)
if (STROIKA_ASSERTIONS_ENABLED)
    list (APPEND configure_ --enable-debug --disable-optimize --enable-curldebug)
else ()
    list (APPEND configure_ --disable-debug --enable-optimize --disable-curldebug)
endif ()
# brotli where the linker finds it - Homebrew's, on a Mac (LIBRARY_PATH) - for the configuration's target, as make checks
# (Build/Scripts/CheckForLibrary): a program linked with its compiler and flags, so a brotli built for another
# architecture (Homebrew's arm64, building x86_64; the build host's, cross-compiling) counts as none - not one curl's
# configure then takes, and its link fails. (C enabled here for that check: the third-party project enables no language.)
enable_language (C)
include (CheckLibraryExists)
set (CMAKE_REQUIRED_LINK_OPTIONS ${STROIKA_LINK_OPTIONS})
list (FILTER CMAKE_REQUIRED_LINK_OPTIONS EXCLUDE REGEX "^--std=")
string (REPLACE ":" ";" libraryPath_ "$ENV{LIBRARY_PATH}")
check_library_exists (brotlicommon BrotliGetDictionary "${libraryPath_}" STROIKA_CURL_BROTLICOMMON)
check_library_exists (brotlidec BrotliDecoderCreateInstance "${libraryPath_}" STROIKA_CURL_BROTLIDEC)
if (NOT STROIKA_CURL_BROTLICOMMON OR NOT STROIKA_CURL_BROTLIDEC)
    list (APPEND configure_ --without-brotli)
endif ()
# cross-compiled (the toolchain file says so: configure's --cross-compiling), as its Makefile builds it: configure told the
# target (--host) - else it runs what it builds, and stops - and, with OpenSSL, the CA certificates of a Linux target,
# where native configure finds them on Debian and Ubuntu (configure looks for the system's only building natively, and
# else libcurl trusts none: every verified HTTPS connection fails, error 60)
if (CMAKE_CROSSCOMPILING)
    execute_process (COMMAND "${CMAKE_C_COMPILER}" -dumpmachine OUTPUT_VARIABLE target_ OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    list (APPEND configure_ --host ${target_})
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT STROIKA_FEATURE_OpenSSL STREQUAL "no")
        list (APPEND configure_ --with-ca-bundle=/etc/ssl/certs/ca-certificates.crt --with-ca-path=/etc/ssl/certs)
    endif ()
endif ()
if (CMAKE_HOST_APPLE)
    list (APPEND configure_ "LIBS=-framework Foundation -framework SystemConfiguration")
endif ()

# make itself - not $(MAKE), which ExternalProject passes on only as a command's first word, not after cmake -E env
find_program (make_ make REQUIRED)
StroikaThirdPartyComponent (curl
    ARCHIVE curl-${version_}.tar.gz
    URLS https://curl.haxx.se/download/curl-${version_}.tar.gz http://curl.askapache.com/download/curl-${version_}.tar.gz
    BUILD_IN_SOURCE
    CONFIGURE_COMMAND autoreconf -fi
              COMMAND "${CMAKE_COMMAND}" -E env --unset=CPATH "PKG_CONFIG_PATH=${STROIKA_BUILDS_DIR}lib/pkgconfig" "ARCH=${STROIKA_ARCH}"
                      "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" "AR=${CMAKE_AR}" "RANLIB=${CMAKE_RANLIB}" "CFLAGS=${cFlags_}"
                      "CPPFLAGS=${cppFlags_}" "LDFLAGS=${ldFlags_}" ./configure "--prefix=${STROIKA_BUILDS_DIR}" ${configure_}
    BUILD_COMMAND "${CMAKE_COMMAND}" -E env --unset=CPATH ${make_}
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E env --unset=CPATH ${make_} install
    DEPENDS openssl zstd
)
