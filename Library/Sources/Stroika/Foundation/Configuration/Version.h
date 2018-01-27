/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika {
    namespace Foundation {
        namespace Configuration {

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
                uint8_t      fMajorVer;
                uint8_t      fMinorVer;
                VersionStage fVerStage;
                uint16_t     fVerSubStage;
                bool         fFinalBuild;

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
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString () const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                nonvirtual constexpr int Compare (const Version& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Optional<T, TRAITS>& rhs)
                 */
                nonvirtual constexpr bool operator< (const Version& rhs) const;
                nonvirtual constexpr bool operator<= (const Version& rhs) const;
                nonvirtual constexpr bool operator> (const Version& rhs) const;
                nonvirtual constexpr bool operator>= (const Version& rhs) const;
                nonvirtual constexpr bool operator== (const Version& rhs) const;
                nonvirtual constexpr bool operator!= (const Version& rhs) const;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Version.inl"

#endif /*_Stroika_Foundation_Configuration_Version_h_*/
