#
# include (StroikaBuildSettings)
#
# What a CMake project built with a Stroika configuration takes from it, after its project (): Stroika's own build
# (Library/CMakeLists.txt) and a program using Stroika's package (StroikaConfig.cmake) alike. Needs the configuration's
# settings (Stroika-Configuration.cmake).
#

# a Visual Studio configuration's flags are its toolchain file's for one build type - Debug where its assertions are on,
# else Release (CMAKE_CXX_FLAGS_DEBUG ...): so a single-configuration generator (Ninja) builds that one, whatever
# CMAKE_BUILD_TYPE held - project () has set it already for MSVC, to Debug (Windows-MSVC.cmake's CMAKE_BUILD_TYPE_INIT)
get_property (StroikaBuildSettings_multiConfig_ GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio" AND NOT StroikaBuildSettings_multiConfig_)
    if (STROIKA_ASSERTIONS_ENABLED)
        set (CMAKE_BUILD_TYPE Debug CACHE STRING "the configuration's build type" FORCE)
    else ()
        set (CMAKE_BUILD_TYPE Release CACHE STRING "the configuration's build type" FORCE)
    endif ()
endif ()

# the configuration's C++ standard, which its flags name too: else a package asking for a lower one (googletest, which
# Stroika::Frameworks brings) makes cmake add that standard's flag after the configuration's - overriding it where the
# compiler's default is lower (MSVC)
if (STROIKA_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD ${STROIKA_CXX_STANDARD})
    set (CMAKE_CXX_STANDARD_REQUIRED ON)
    set (CMAKE_CXX_EXTENSIONS OFF)
endif ()

# the third-party components installed for the configuration, ahead of any installed elsewhere
list (PREPEND CMAKE_PREFIX_PATH "${STROIKA_BUILDS_DIR}")
# and their include directories as -I, as make gives them, not -isystem: compilers search CPATH's directories ahead of
# -isystem ones - so given CPATH=/opt/homebrew/include (a common Mac shell setting), Homebrew's OpenSSL headers would win
# over the configuration's own
set (CMAKE_NO_SYSTEM_FROM_IMPORTED ON)
