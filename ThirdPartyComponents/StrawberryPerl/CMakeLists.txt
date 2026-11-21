#
# StrawberryPerl, as its Makefile fetches it: a portable perl for Windows - its perl directory installed as
# StrawberryPerl/perl, which OpenSSL's Visual Studio build runs (perl/bin); not the C toolchain beside it (c)
#
set (version_ 5.42.3.1)
string (REPLACE "." "" compressedVersion_ "${version_}")

StroikaThirdPartyComponent (StrawberryPerl
    ARCHIVE strawberry-perl-${version_}-64bit-portable.zip
    URLS https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_${compressedVersion_}_64bit/strawberry-perl-${version_}-64bit-portable.zip
         https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_${compressedVersion_}_64bit_UCRT/strawberry-perl-${version_}-64bit-portable.zip
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND "${CMAKE_COMMAND}" -E true
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E copy_directory <SOURCE_DIR>/perl "${STROIKA_BUILDS_DIR}StrawberryPerl/perl"
)
