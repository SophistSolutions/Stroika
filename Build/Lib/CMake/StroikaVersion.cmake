#
# StroikaReadVersion (<version file> <prefix>)
#
# Reads the version file's version into variables of the caller's, each as make's Build/Scripts/ExtractVersionInformation
# gives that part (its name for it in brackets). The file holds one version, as STROIKA_VERSION does: major.minor, a stage
# (a, b, d or r; none or '.' for a release) and its number, then x unless it is a final build:
#     <prefix>                    the version (FullVersionString)                                  3.0d26x
#     <prefix>_MAJOR              (Major)                                                          3
#     <prefix>_MINOR              (Minor)                                                          0
#     <prefix>_STAGE              Dev, Alpha, Beta, ReleaseCandidate or Release (Stage)            Dev
#     <prefix>_SUB_STAGE          the stage's number, 0 if none (SubStage)                         26
#     <prefix>_FINAL              true for a final build, else false (Final)                       false
#     <prefix>_DECORATED_STAGE    the stage as written, its number and the x (DecoratedStageInfo)  d26x
#     <prefix>_WIN32_FOUR_DOT     as Common::Version::AsWin32Version4DotString (Win32FourDot)      3.0.32.52
# cmake runs again when the version file changes (but in a script, cmake -P).
#
function (StroikaReadVersion versionFile prefix)
    # listed once, normalized: a file Ninja sees twice, under two spellings (a/../b and b), fails a Ninja build ("defined as
    # an output multiple times")
    cmake_path (ABSOLUTE_PATH versionFile NORMALIZE)
    if (NOT CMAKE_SCRIPT_MODE_FILE)
        get_property (configureDepends_ DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY CMAKE_CONFIGURE_DEPENDS)
        if (NOT versionFile IN_LIST configureDepends_)
            set_property (DIRECTORY "${CMAKE_SOURCE_DIR}" APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${versionFile}")
        endif ()
    endif ()
    file (READ "${versionFile}" version_)
    string (STRIP "${version_}" version_)
    if (NOT version_ MATCHES "^([0-9]+)\\.([0-9]+)([abdr.]?)([0-9]*)(x?)$")
        message (FATAL_ERROR "${versionFile}: not a version: ${version_}")
    endif ()
    set (major_ "${CMAKE_MATCH_1}")
    set (minor_ "${CMAKE_MATCH_2}")
    set (stageLetter_ "${CMAKE_MATCH_3}")
    set (subStage_ "${CMAKE_MATCH_4}")
    set (x_ "${CMAKE_MATCH_5}")
    if (subStage_ STREQUAL "")
        set (subStage_ 0)
    endif ()
    if (x_ STREQUAL "x")
        set (final_ false)
        set (finalBit_ 0)
    else ()
        set (final_ true)
        set (finalBit_ 1)
    endif ()
    # each stage, and its number in a Win32 version (Common::VersionStage)
    if (stageLetter_ STREQUAL "a")
        set (stage_ Alpha)
        set (stageNumber_ 2)
    elseif (stageLetter_ STREQUAL "b")
        set (stage_ Beta)
        set (stageNumber_ 3)
    elseif (stageLetter_ STREQUAL "d")
        set (stage_ Dev)
        set (stageNumber_ 1)
    elseif (stageLetter_ STREQUAL "r")
        set (stage_ ReleaseCandidate)
        set (stageNumber_ 4)
    else ()
        set (stage_ Release)
        set (stageNumber_ 5)
    endif ()
    math (EXPR thirdOctet_ "(${stageNumber_} << 5) | (${subStage_} >> 7)")
    math (EXPR fourthOctet_ "((${subStage_} & 0x7f) << 1) | ${finalBit_}")
    set (${prefix} "${version_}" PARENT_SCOPE)
    set (${prefix}_MAJOR "${major_}" PARENT_SCOPE)
    set (${prefix}_MINOR "${minor_}" PARENT_SCOPE)
    set (${prefix}_STAGE "${stage_}" PARENT_SCOPE)
    set (${prefix}_SUB_STAGE "${subStage_}" PARENT_SCOPE)
    set (${prefix}_FINAL "${final_}" PARENT_SCOPE)
    set (${prefix}_DECORATED_STAGE "${stageLetter_}${subStage_}${x_}" PARENT_SCOPE)
    set (${prefix}_WIN32_FOUR_DOT "${major_}.${minor_}.${thirdOctet_}.${fourthOctet_}" PARENT_SCOPE)
endfunction ()
