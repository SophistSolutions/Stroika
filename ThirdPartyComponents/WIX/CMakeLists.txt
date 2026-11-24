#
# WIX, as its Makefile fetches it: the WiX Toolset's programs (candle, light, heat, ...) and extensions, which build Windows
# installers - installed as bin/WIX, where the samples' installers, and an application's, find them
# (Build/Lib/CMake/StroikaInstallers.cmake); not its documentation (doc), nor its SDK for writing custom actions (sdk)
#
set (version_ 314)
set (installDir_ "${STROIKA_BUILDS_DIR}bin/WIX")

StroikaThirdPartyComponent (WIX
    ARCHIVE wix${version_}-binaries.zip
    URLS https://github.com/wixtoolset/wix3/releases/download/wix${version_}1rtm/wix${version_}-binaries.zip
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND "${CMAKE_COMMAND}" -E true
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E copy_directory <SOURCE_DIR> "${installDir_}"
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${installDir_}/doc" "${installDir_}/sdk"
    NO_SOURCES
)
