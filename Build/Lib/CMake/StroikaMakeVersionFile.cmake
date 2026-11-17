#
# StroikaMakeVersionFile (<version file> <output file> [TYPE <type>] [NAMESPACE <namespace>])
#
# Writes the output file, a C++ header giving the version file's version, as make's Build/Scripts/MakeVersionFile
# <version file> <output file> <type> does. The type is
#     AppCPlusPlusVersion (the default): <namespace>::kVersion, a Stroika::Foundation::Common::Version - the namespace
#         defaults to the output file's name less its .h ('-' and '.' made '_');
#     StroikaLibVersion: the qStroika_Version_ macros, Stroika's own Stroika/Current-Version.h.
# The version file holds one version, as STROIKA_VERSION does: major.minor, a stage (a, b, d or r; none or '.' for a
# release) and its number, then x unless it is a final build - 3.0d26x. Written when cmake configures, and again when the
# version file changes.
#
function (StroikaMakeVersionFile versionFile outputFile)
    cmake_parse_arguments (PARSE_ARGV 2 arg "" "TYPE;NAMESPACE" "")
    # cmake runs again when the version file changes - listed once, normalized: a file Ninja sees twice, under two spellings
    # (a/../b and b), fails a Ninja build ("defined as an output multiple times")
    cmake_path (ABSOLUTE_PATH versionFile NORMALIZE)
    get_property (configureDepends_ DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY CMAKE_CONFIGURE_DEPENDS)
    if (NOT versionFile IN_LIST configureDepends_)
        set_property (DIRECTORY "${CMAKE_SOURCE_DIR}" APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${versionFile}")
    endif ()
    file (READ "${versionFile}" version_)
    string (STRIP "${version_}" version_)
    if (NOT version_ MATCHES "^([0-9]+)\\.([0-9]+)([abdr.]?)([0-9]*)(x?)$")
        message (FATAL_ERROR "${versionFile}: not a version: ${version_}")
    endif ()
    set (major_ "${CMAKE_MATCH_1}")
    set (minor_ "${CMAKE_MATCH_2}")
    set (stage_ "${CMAKE_MATCH_3}")
    set (subStage_ "${CMAKE_MATCH_4}")
    set (final_ true)
    set (finalBuild_ 1)
    if (CMAKE_MATCH_5 STREQUAL "x")
        set (final_ false)
        set (finalBuild_ 0)
    endif ()
    if (stage_ STREQUAL "a")
        set (stage_ Alpha)
    elseif (stage_ STREQUAL "b")
        set (stage_ Beta)
    elseif (stage_ STREQUAL "d")
        set (stage_ Dev)
    elseif (stage_ STREQUAL "r")
        set (stage_ ReleaseCandidate)
    else ()
        set (stage_ Release)
    endif ()
    if (subStage_ STREQUAL "")
        set (subStage_ 0)
    endif ()
    if (NOT arg_TYPE OR arg_TYPE STREQUAL "AppCPlusPlusVersion")
        if (arg_NAMESPACE)
            set (namespace_ "${arg_NAMESPACE}")
        else ()
            get_filename_component (namespace_ "${outputFile}" NAME)
            string (REGEX REPLACE "(.)\\.h$" "\\1" namespace_ "${namespace_}")
            string (REGEX REPLACE "[-.]" "_" namespace_ "${namespace_}")
        endif ()
        string (REGEX REPLACE "[-:]" "_" guard_ "__${namespace_}__")
        file (CONFIGURE OUTPUT "${outputFile}" CONTENT [[
#ifndef @guard_@
#define @guard_@ 1
#include "Stroika/Foundation/StroikaPreComp.h"
#include "Stroika/Foundation/Common/Version.h"
namespace @namespace_@ {
constexpr Stroika::Foundation::Common::Version  kVersion = Stroika::Foundation::Common::Version (@major_@, @minor_@, Stroika::Foundation::Common::VersionStage::@stage_@, @subStage_@, @final_@);
}
#endif /*@guard_@*/
]] @ONLY NEWLINE_STYLE UNIX)
    elseif (arg_TYPE STREQUAL "StroikaLibVersion")
        file (CONFIGURE OUTPUT "${outputFile}" CONTENT [[
#define qStroika_Version_Major                  @major_@
#define qStroika_Version_Minor                  @minor_@
#define qStroika_Version_MajorMinor             0x@major_@@minor_@
#define qStroika_Version_Stage                  qStroika_Version_Stage_@stage_@
#define qStroika_Version_SubStage               @subStage_@
#define qStroika_Version_FinalBuild             @finalBuild_@
#define qStroika_Version_MajorMinorVersionString        "@major_@.@minor_@"
#define qStroika_Version_ShortVersionString             "@version_@"
#define qStroika_Version_FullVersion                    Stroika_Make_FULL_VERSION (qStroika_Version_Major, qStroika_Version_Minor, qStroika_Version_Stage, qStroika_Version_SubStage, qStroika_Version_FinalBuild)
/* DEPRECATED since Stroika v3.0d25 - use the qStroika_Version_ names above ('q' names a macro - see Design-Overview.md) */
#define kStroika_Version_Major qStroika_Version_Major
#define kStroika_Version_Minor qStroika_Version_Minor
#define kStroika_Version_MajorMinor qStroika_Version_MajorMinor
#define kStroika_Version_Stage qStroika_Version_Stage
#define kStroika_Version_SubStage qStroika_Version_SubStage
#define kStroika_Version_FinalBuild qStroika_Version_FinalBuild
#define kStroika_Version_MajorMinorVersionString qStroika_Version_MajorMinorVersionString
#define kStroika_Version_ShortVersionString qStroika_Version_ShortVersionString
#define kStroika_Version_FullVersion qStroika_Version_FullVersion
]] @ONLY NEWLINE_STYLE UNIX)
    else ()
        message (FATAL_ERROR "StroikaMakeVersionFile: TYPE ${arg_TYPE} - AppCPlusPlusVersion or StroikaLibVersion")
    endif ()
endfunction ()
