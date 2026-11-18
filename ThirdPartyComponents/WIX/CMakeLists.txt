#
# WIX, as its Makefile fetches it: the WiX Toolset's programs (candle, light, heat) that build Windows installers - just
# extracted; the samples' installers run them from its source directory (Build/Lib/CMake/StroikaInstallers.cmake)
#
set (version_ 314)

StroikaThirdPartyComponent (WIX
    ARCHIVE wix${version_}-binaries.zip
    URLS https://github.com/wixtoolset/wix3/releases/download/wix${version_}1rtm/wix${version_}-binaries.zip
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND "${CMAKE_COMMAND}" -E true
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E true
)
