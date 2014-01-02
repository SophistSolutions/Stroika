/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Version_inl_
#define _Stroika_Foundation_Configuration_Version_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if     defined (__cplusplus)
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#endif



            /*
             ********************************************************************************
             *********************************** Version ************************************
             ********************************************************************************
             */
            inline  Version::Version (FullVersionType fullVersionNumber)
                : fMajorVer ((fullVersionNumber >> 25) & 0x8f)
                , fMinorVer ((fullVersionNumber >> 17) & 0xff)
                , fVerStage (static_cast<VersionStage> ((fullVersionNumber >> 9) & 0xff))
                , fVerSubStage ((fullVersionNumber >> 1) & 0xff)
                , fFinalBuild (fullVersionNumber & 1)
            {
            }
            inline  Version::Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint8_t verSubStage, bool finalBuild)
                : fMajorVer (majorVer)
                , fMinorVer (minorVer)
                , fVerStage (verStage)
                , fVerSubStage (verSubStage)
                , fFinalBuild (finalBuild)
            {
            }
            inline  FullVersionType  Version::AsFullVersionNum () const
            {
                return Stroika_Make_FULL_VERSION (fMajorVer, fMinorVer, ((uint8_t)fVerStage), fVerSubStage, static_cast<int> (fFinalBuild));
            }


#if     defined (__cplusplus)
        }
    }
}
#endif
#endif  /*_Stroika_Foundation_Configuration_Version_inl_*/
