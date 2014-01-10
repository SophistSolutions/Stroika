/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_StroikaVersion_h_
#define _Stroika_Foundation_Configuration_StroikaVersion_h_  1

#include    "../StroikaPreComp.h"

#if     defined (__cplusplus)
#include    "Common.h"
#endif
#include    "VersionDefs.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 * TODO:
 *
 */



#if     defined (__cplusplus)
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#endif


            /**
             *  Perhaps this should be autogenerated from some XML file - but this defines the current version of Stroika
             */
#define kStroika_Version_Major                  2
#define kStroika_Version_Minor                  0
#define kStroika_Version_MajorMinor             0x20
#define kStroika_Version_Stage                  kStroika_Version_Stage_Alpha
#define kStroika_Version_SubStage               17
#define kStroika_Version_FinalBuild             1

#define kStroika_Version_MajorMinorVersionString        "2.0"
#define kStroika_Version_ShortVersionString             "2.0a17"
#define kStroika_Version_FullVersion                    Stroika_Make_FULL_VERSION (kStroika_Version_Major, kStroika_Version_Minor, kStroika_Version_Stage, kStroika_Version_SubStage, kStroika_Version_FinalBuild)


#if     defined (__cplusplus)
        }
    }
}
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StroikaVersion.inl"

#endif  /*_Stroika_Foundation_Configuration_StroikaVersion_h_*/
