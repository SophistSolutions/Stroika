#
# StrawberryPerl, as its Makefile fetches it: a portable perl for Windows, just extracted - OpenSSL's Visual Studio build
# runs it (perl/bin, in its source directory)
#
set (version_ 5.42.3.1)
string (REPLACE "." "" compressedVersion_ "${version_}")

StroikaThirdPartyComponent (StrawberryPerl
    ARCHIVE strawberry-perl-${version_}-64bit-portable.zip
    URLS https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_${compressedVersion_}_64bit/strawberry-perl-${version_}-64bit-portable.zip
         https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/download/SP_${compressedVersion_}_64bit_UCRT/strawberry-perl-${version_}-64bit-portable.zip
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND "${CMAKE_COMMAND}" -E true
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E true
)
