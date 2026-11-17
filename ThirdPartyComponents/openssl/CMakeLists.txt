#
# OpenSSL, as its Makefile builds it: configured by its own perl scripts, built among its sources
#
set (version_ 4.0.3)
set (base_ openssl-${version_})

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    message (FATAL_ERROR "ThirdPartyComponents/openssl: no Visual Studio build here (StrawberryPerl, nmake) - its Makefile builds it")
endif ()

# its target, where config would otherwise take the host's (cross-compiling) - and on a Mac, the configuration's architecture
set (configArgs_)
if (STROIKA_ARCH STREQUAL "armhf")
    list (APPEND configArgs_ linux-generic32)
elseif (STROIKA_ARCH STREQUAL "aarch64")
    list (APPEND configArgs_ linux-aarch64)
elseif (CMAKE_HOST_APPLE)
    list (APPEND configArgs_ darwin64-${STROIKA_ARCH}-cc)
endif ()
list (APPEND configArgs_ -no-shared)
if (STROIKA_OPENSSL_EXTRA_ARGS MATCHES "purify")
    list (APPEND configArgs_ -DPURIFY)
endif ()
list (APPEND configArgs_ "--prefix=${STROIKA_BUILDS_DIR}" -no-tests -no-docs --libdir=lib)

# Its configure takes the compiler and flags from the environment: the configuration's, but no sanitizer (libcurl's build
# then fails against it) and no -flto (it broke cross-compiled ARM builds, with gcc 8).
string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" cFlags_ "${CMAKE_C_FLAGS}")
string (REGEX REPLACE "(^| )-flto( |$)" "\\1" cFlags_ "${cFlags_}")
string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" cxxFlags_ "${CMAKE_CXX_FLAGS}")
string (REGEX REPLACE "(^| )-flto( |$)" "\\1" cxxFlags_ "${cxxFlags_}")
list (JOIN STROIKA_LINK_OPTIONS " " ldFlags_)
string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" ldFlags_ "${ldFlags_}")

if (CMAKE_GENERATOR MATCHES "Makefiles")
    set (make_ "$(MAKE)") # sharing make's job slots
else ()
    find_program (make_ make REQUIRED)
endif ()
StroikaThirdPartyComponent (openssl
    ARCHIVE ${base_}.tar.gz
    URLS https://github.com/openssl/openssl/releases/download/${base_}/${base_}.tar.gz
         https://www.openssl.org/source/${base_}.tar.gz
         http://www.openssl.org/source/${base_}.tar.gz
         http://www.openssl.org/source/old/3.1/${base_}.tar.gz
         http://www.openssl.org/source/old/3.2/${base_}.tar.gz
         http://www.openssl.org/source/old/3.3/${base_}.tar.gz
         http://www.openssl.org/source/old/3.4/${base_}.tar.gz
         ftp://mirror.switch.ch/mirror/openssl/source/${base_}.tar.gz
         http://artfiles.org/openssl.org/source/${base_}.tar.gz
         https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/${base_}.tar.gz
    BUILD_IN_SOURCE
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E env --unset=MAKEFLAGS "CC=${CMAKE_C_COMPILER}" "AR=${CMAKE_AR}" "RANLIB=${CMAKE_RANLIB}"
                      "MACHINE=${STROIKA_ARCH}" "CFLAGS=${cFlags_}" "CXXFLAGS=${cxxFlags_}" "LDFLAGS=${ldFlags_}" ./config ${configArgs_}
    BUILD_COMMAND ${make_} all
    INSTALL_COMMAND ${make_} -j1 install_sw install_ssldirs
    DEPENDS zlib
)
