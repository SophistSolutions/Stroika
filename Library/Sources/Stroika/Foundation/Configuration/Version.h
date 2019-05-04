/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Version_h_
#define _Stroika_Foundation_Configuration_Version_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Characters/String.h"

#include "VersionDefs.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Better document/enforce limits on range of values for version#. Note - we have some extra bits because
 *              verStage only requires 3 (so 8+8+3+8_1 used, leaving 4).
 *
 *      @todo   RETHINK MAPPING TO FULL_VERSION_NUMBER type? VersionStage only needs 3 bits (??)
 *              But maybe useful compat with windows format?
 *
 *      @todo   Consider if explicit Version(Binary32BitFullVersionType) CTOR should be FromBinary32BitFullVersionType
 */

namespace Stroika::Foundation::Configuration {

    /**
     */
    enum class VersionStage {
        Dev              = kStroika_Version_Stage_Dev,
        Alpha            = kStroika_Version_Stage_Alpha,
        Beta             = kStroika_Version_Stage_Beta,
        ReleaseCandidate = kStroika_Version_Stage_ReleaseCandidate,
        Release          = kStroika_Version_Stage_Release,

        Stroika_Define_Enum_Bounds (Dev, Release)
    };

    /**
     *  In Stroika, we represent a version# as (higher sort order priority first):
     *      MAJOR (uint8_t)
     *      MINOR (uint8_t)
     *      VersionStage
     *      VERSION-SubStage (uint8_t)
     *      finalBuild(bool)
     *
     *  We provide support to automatically map this notion to a 32-bit version# which microsoft uses.
     *  Note - this mapping is not totally 1-1, and doesn't correspond to any documented version# strategy defined
     *  by MSFT (as near as I can tell they have none - its just 4 bytes for them).
     *
     *  @see Stroika_Make_FULL_VERSION for the mapping
     */
    struct Version {
    public:
        static constexpr uint16_t kMaxVersionSubStage = (1 << 12) - 1;

    public:
        /**
         *  \req verSubStage <= kMaxVersionSubStage
         */
        constexpr Version ();
        constexpr explicit Version (Binary32BitFullVersionType fullVersionNumber);
        constexpr Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint16_t verSubStage, bool finalBuild = true);

    public:
        /**
         *  FromWin32Version4DoTString may throw if it detects an ill-formatted version string.
         */
        static Version FromWin32Version4DotString (const Characters::String& win32Version4DotString);

    public:
        /**
         *  FromPrettyVersionString may throw if it detects an ill-formatted version string.
         */
        static Version FromPrettyVersionString (const Characters::String& prettyVersionString);

    public:
        uint8_t      fMajorVer;    // 8 bits
        uint8_t      fMinorVer;    // 8 bits
        VersionStage fVerStage;    // 3 bits
        uint16_t     fVerSubStage; // 12 bits
        bool         fFinalBuild;  // 1 bit

    public:
        /**
         */
        nonvirtual constexpr Binary32BitFullVersionType AsFullVersionNum () const;

    public:
        /**
         */
        nonvirtual Characters::String AsWin32Version4DotString () const;

    public:
        /**
         */
        nonvirtual Characters::String AsPrettyVersionString () const;

    public:
        /**
         *  Returns "1.0" for example.
         */
        nonvirtual Characters::String AsMajorMinorString () const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

    public:
        struct ThreeWayComparer;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
         */
        [[deprecated ("in Stroika v2.1d24 - use ThreeWayComparer{} () instead")]] constexpr int Compare (const Version& rhs) const;

#if __cpp_lib_three_way_comparison >= 201711
    public:
        constexpr std::strong_ordering operator<=> (const Version& rhs)
#endif
    };

    /**
     */
    struct Version::ThreeWayComparer {
        constexpr int operator() (const Version& lhs, const Version& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Version::ThreeWayComparer ()
     */
#if __cpp_lib_three_way_comparison < 201711
    constexpr bool operator< (const Version& lhs, const Version& rhs);
    constexpr bool operator<= (const Version& lhs, const Version& rhs);
    constexpr bool operator== (const Version& lhs, const Version& rhs);
    constexpr bool operator!= (const Version& lhs, const Version& rhs);
    constexpr bool operator>= (const Version& lhs, const Version& rhs);
    constexpr bool operator> (const Version& lhs, const Version& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Version.inl"

#endif /*_Stroika_Foundation_Configuration_Version_h_*/
