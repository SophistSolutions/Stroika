/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
            };


            /**
             */
            struct  Version {
                Version (FullVersionType fullVersionNumber = 0);
                Version (const Characters::String& win32Version4DoTString);
                Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint8_t verSubStage, bool finalBuild = true);

                uint8_t     fMajorVer;
                uint8_t     fMinorVer;
                VersionStage fVerStage;
                uint8_t     fVerSubStage;
                bool        fFinalBuild;

                nonvirtual  FullVersionType         AsFullVersionNum () const;
                nonvirtual  Characters::String      AsWin32Version4DotString () const;
                nonvirtual  Characters::String      AsPrettyVersionString () const;
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
