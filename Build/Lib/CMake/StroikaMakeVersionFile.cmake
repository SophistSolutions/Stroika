#
# StroikaMakeVersionFile (<version file> <output file> [TYPE <type>] [NAMESPACE <namespace>])
#
# Writes the output file, a C++ header giving the version file's version, as make's Build/Scripts/MakeVersionFile
# <version file> <output file> <type> does. The type is
#     AppCPlusPlusVersion (the default): <namespace>::kVersion, a Stroika::Foundation::Common::Version - the namespace
#         defaults to the output file's name less its .h ('-' and '.' made '_');
#     StroikaLibVersion: the qStroika_Version_ macros, Stroika's own Stroika/Current-Version.h.
# The version file holds one version, as STROIKA_VERSION does (StroikaVersion.cmake) - 3.0d26x. Written when cmake
# configures, and again when the version file changes.
#
include (StroikaVersion)

function (StroikaMakeVersionFile versionFile outputFile)
    cmake_parse_arguments (PARSE_ARGV 2 arg "" "TYPE;NAMESPACE" "")
    StroikaReadVersion ("${versionFile}" v_)
    set (version_ "${v_}")
    set (major_ "${v__MAJOR}")
    set (minor_ "${v__MINOR}")
    set (stage_ "${v__STAGE}")
    set (subStage_ "${v__SUB_STAGE}")
    set (final_ "${v__FINAL}")
    set (finalBuild_ 0)
    if (final_)
        set (finalBuild_ 1)
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
