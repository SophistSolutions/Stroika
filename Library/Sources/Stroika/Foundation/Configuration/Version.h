/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Version_h_
#define _Stroika_Foundation_Configuration_Version_h_  1

#include    "../StroikaPreComp.h"

#include    <cstdint>

#include    "../Characters/String.h"

#include    "VersionDefs.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   RETHINK MAPPING TO FULL_VERSION_NUMBER type? VersionStage only needs 3 bits (??)
 *              But maybe useful compat with windows format?
 *
 *      @todo   Consider if explicit Version(FullVersionType) CTOR should be FromFullVersionType
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            enum    class   VersionStage {
                Dev = kStroika_Version_Stage_Dev,
                Alpha = kStroika_Version_Stage_Alpha,
                Beta = kStroika_Version_Stage_Beta,
                ReleaseCandidate = kStroika_Version_Stage_ReleaseCandidate,
                Release = kStroika_Version_Stage_Release,

                Stroika_Define_Enum_Bounds(Dev, Release)
            };


            /**
             */
            struct  Version {
            public:
                /**
                 */
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                Version ();
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Version (FullVersionType fullVersionNumber);
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint8_t verSubStage, bool finalBuild = true);


            public:
                /**
                 *  FromWin32Version4DoTString may throw if it detects an ill-formatted version string.
                 */
                static  Version FromWin32Version4DotString (const Characters::String& win32Version4DotString);


            public:
                /**
                 *  FromPrettyVersionString may throw if it detects an ill-formatted version string.
                 */
                static  Version FromPrettyVersionString (const Characters::String& prettyVersionString);

            public:
                uint8_t         fMajorVer;
                uint8_t         fMinorVer;
                VersionStage    fVerStage;
                uint8_t         fVerSubStage;
                bool            fFinalBuild;

            public:
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  FullVersionType         AsFullVersionNum () const;

            public:
                nonvirtual  Characters::String      AsWin32Version4DotString () const;

            public:
                nonvirtual  Characters::String      AsPrettyVersionString () const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  int Compare (const Version& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Optional<T, TRAITS>& rhs)
                 */
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator< (const Version& rhs) const;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator<= (const Version& rhs) const;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator> (const Version& rhs) const;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator>= (const Version& rhs) const;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator== (const Version& rhs) const;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                nonvirtual  bool    operator!= (const Version& rhs) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Version.inl"

#endif  /*_Stroika_Foundation_Configuration_Version_h_*/
