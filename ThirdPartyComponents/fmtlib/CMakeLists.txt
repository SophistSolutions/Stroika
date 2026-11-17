#
# fmtlib, as its Makefile builds it - used only where the compiler has no usable <format> (configure's NeedsFmtLib_)
#
set (version_ 12.2.0)

StroikaThirdPartyComponent (fmtlib
    ARCHIVE fmtlib-${version_}.zip
    URLS https://github.com/fmtlib/fmt/archive/refs/tags/${version_}.zip
    CMAKE_ARGS -DFMT_TEST=NO
)
