/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *      @todo
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
                /**
                 *  The String CTOR overload may throw if it detects an ill-formatted version string.
                 */
                Version (FullVersionType fullVersionNumber = 0);
                explicit Version (const Characters::String& win32Version4DoTString);
                Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint8_t verSubStage, bool finalBuild = true);

                uint8_t     fMajorVer;
                uint8_t     fMinorVer;
                VersionStage fVerStage;
                uint8_t     fVerSubStage;
                bool        fFinalBuild;

                nonvirtual  FullVersionType         AsFullVersionNum () const;
                nonvirtual  Characters::String      AsWin32Version4DotString () const;
                nonvirtual  Characters::String      AsPrettyVersionString () const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                nonvirtual  int Compare (const Version& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Optional<T, TRAITS>& rhs)
                 */
                nonvirtual  bool    operator< (const Version& rhs) const;
                nonvirtual  bool    operator<= (const Version& rhs) const;
                nonvirtual  bool    operator> (const Version& rhs) const;
                nonvirtual  bool    operator>= (const Version& rhs) const;
                nonvirtual  bool    operator== (const Version& rhs) const;
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
