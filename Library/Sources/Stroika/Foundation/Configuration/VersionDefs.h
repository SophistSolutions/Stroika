/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_VersionDefs_h_
#define _Stroika_Foundation_Configuration_VersionDefs_h_  1

#include    "../StroikaPreComp.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  This file defines the part of the version stuff that can be safely included anywhere - even in a
 *  non-C++ file (e.g. resource compiler).
 *
 * TODO:
 *      @todo   FIRST DRAFT
 */



#if     defined (__cplusplus)
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#endif




#if     defined (__cplusplus)
            /**
             */
            using   FullVersionType     =   uint32_t;
#endif


            /**
             */
// for use from .RC file - which cannot use C++
#define kStroika_Version_Stage_Dev              0x1
#define kStroika_Version_Stage_Alpha            0x2
#define kStroika_Version_Stage_Beta             0x3
#define kStroika_Version_Stage_ReleaseCandidate 0x4
#define kStroika_Version_Stage_Release          0x5

#define Stroika_Make_FULL_VERSION(_Major_,_Minor_,_Stage_,_SubStage_,_FinalBuild_) \
    ( (_Major_ << 25) | \
      (_Minor_ << 17) | \
      (_Stage_ << 9) | \
      (_SubStage_ << 1) | \
      (_FinalBuild_) \
    )



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
#include    "VersionDefs.inl"

#endif  /*_Stroika_Foundation_Configuration_VersionDefs_h_*/
