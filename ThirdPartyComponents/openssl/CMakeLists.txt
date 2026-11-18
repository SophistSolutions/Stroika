#
# OpenSSL, as its Makefile builds it: configured by its own perl scripts, built among its sources - with Visual Studio, by
# nmake, in Visual Studio's environment
#
set (version_ 4.0.3)
set (base_ openssl-${version_})

set (configArgs_ -no-shared)
if (STROIKA_OPENSSL_EXTRA_ARGS MATCHES "purify")
    list (APPEND configArgs_ -DPURIFY)
endif ()
list (APPEND configArgs_ -no-tests -no-docs)

# Its configure takes the compiler flags from the environment: the configuration's, as make gives them (no preprocessor
# definitions), but no sanitizer (libcurl's build then fails against it) and no -flto (it broke cross-compiled ARM builds,
# with gcc 8)
foreach (language_ IN ITEMS C CXX)
    string (REGEX REPLACE "(^| )-fsanitize=[^ ]*" "" flags_ "${STROIKA_PLATFORM_${language_}_FLAGS}")
    string (REGEX REPLACE "(^| )-flto( |$)" "\\1" ${language_}Flags_ "${flags_}")
endforeach ()
set (ldFlags_)
foreach (option_ IN LISTS STROIKA_LINK_OPTIONS)
    if (NOT option_ MATCHES "^-fsanitize=")
        string (REGEX REPLACE "^(-[A-Za-z]+:)(.* .*)$" "\\1\"\\2\"" option_ "${option_}") # -LIBPATH:"C:/Program Files/...", as make's
        string (APPEND ldFlags_ " ${option_}")
    endif ()
endforeach ()
string (STRIP "${ldFlags_}" ldFlags_)

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # its Visual Studio target (debug- where assertions are on), and no assembler (#665)
    if (STROIKA_ARCH STREQUAL "x86")
        set (target_ VC-WIN32)
    else ()
        set (target_ VC-WIN64A)
    endif ()
    if (STROIKA_ASSERTIONS_ENABLED)
        string (PREPEND target_ debug-)
    endif ()
    string (REGEX REPLACE "/$" "" prefix_ "${STROIKA_BUILDS_DIR}")
    list (APPEND configArgs_ -no-asm "--prefix=${prefix_}")
    list (JOIN configArgs_ " " configArgs_)
    # warnings it would otherwise give where Stroika links it, with link-time code generation, silenced
    if (STROIKA_LINK_OPTIONS MATCHES "-LTCG")
        string (APPEND CFlags_ " -wd4703 -wd4724")
    endif ()
    if (CXXFlags_ MATCHES "(^| )-GL( |$)")
        string (APPEND CFlags_ " -wd4701 -wd4702 -wd4706")
        string (APPEND CXXFlags_ " -wd4701 -wd4702 -wd4706")
    endif ()
    set (environment_)
    set (dependsOn_)
    if (STROIKA_FEATURE_StrawberryPerl STREQUAL "use")
        # the StrawberryPerl component's perl (else the one on PATH), whose OpenSSL scripts take \ for / in the flags
        ExternalProject_Get_Property (StrawberryPerl SOURCE_DIR)
        file (TO_NATIVE_PATH "${SOURCE_DIR}/perl/bin" perlBin_)
        string (APPEND environment_ "set \"PATH=${perlBin_};%PATH%\"\n")
        string (REPLACE "/" "\\" CFlags_ "${CFlags_}")
        set (dependsOn_ StrawberryPerl)
    endif ()
    string (APPEND environment_ "set \"CFLAGS=${CFlags_}\"\nset \"CXXFLAGS=${CXXFlags_}\"\nset \"LDFLAGS=${ldFlags_}\"\n")
    StroikaVisualStudioCommand (configure_ configure "${environment_}perl Configure ${target_} ${configArgs_}")
    StroikaVisualStudioCommand (build_ build "${environment_}nmake")
    # its install takes the ossl_static.pdb a -Zi build makes - not the configuration's -Z7: an empty one, removed after
    file (TO_NATIVE_PATH "${prefix_}/lib/ossl_static.pdb" installedPdb_)
    StroikaVisualStudioCommand (install_ install "${environment_}if not exist ossl_static.pdb type nul > ossl_static.pdb
nmake install_sw install_ssldirs INSTALLTOP=${prefix_} OPENSSLDIR=${prefix_} ENGINESDIR=${prefix_}
del ossl_static.pdb
if exist \"${installedPdb_}\" del \"${installedPdb_}\"")
    StroikaThirdPartyComponent (openssl
        ARCHIVE ${base_}.tar.gz
        URLS https://github.com/openssl/openssl/releases/download/${base_}/${base_}.tar.gz
             https://www.openssl.org/source/${base_}.tar.gz
             http://www.openssl.org/source/${base_}.tar.gz
             ftp://mirror.switch.ch/mirror/openssl/source/${base_}.tar.gz
             http://artfiles.org/openssl.org/source/${base_}.tar.gz
             https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/${base_}.tar.gz
        BUILD_IN_SOURCE
        CONFIGURE_COMMAND ${configure_}
        BUILD_COMMAND ${build_}
        INSTALL_COMMAND ${install_}
                COMMAND "${CMAKE_COMMAND}" -DSOURCE_DIR=<SOURCE_DIR> "-DBUILDS_DIR=${STROIKA_BUILDS_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/WindowsPc.cmake"
        DEPENDS zlib ${dependsOn_}
    )
    return ()
endif ()

# its target, where config would otherwise take the host's (cross-compiling) - and on a Mac, the configuration's architecture
set (targetArgs_)
if (STROIKA_ARCH STREQUAL "armhf")
    set (targetArgs_ linux-generic32)
elseif (STROIKA_ARCH STREQUAL "aarch64")
    set (targetArgs_ linux-aarch64)
elseif (CMAKE_HOST_APPLE)
    set (targetArgs_ darwin64-${STROIKA_ARCH}-cc)
endif ()
list (APPEND configArgs_ "--prefix=${STROIKA_BUILDS_DIR}" --libdir=lib)

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
                      "MACHINE=${STROIKA_ARCH}" "CFLAGS=${CFlags_}" "CXXFLAGS=${CXXFlags_}" "LDFLAGS=${ldFlags_}" ./config ${targetArgs_} ${configArgs_}
    BUILD_COMMAND ${make_} all
    INSTALL_COMMAND ${make_} -j1 install_sw install_ssldirs
    DEPENDS zlib
)
