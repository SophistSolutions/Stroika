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
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Version::Version ()
                : fMajorVer (0)
                , fMinorVer (0)
                , fVerStage (VersionStage::eSTART)
                , fVerSubStage (0)
                , fFinalBuild (0)
            {
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Version::Version (FullVersionType fullVersionNumber)
                : fMajorVer ((fullVersionNumber >> 25) & 0x8f)
                , fMinorVer ((fullVersionNumber >> 17) & 0xff)
                , fVerStage (static_cast<VersionStage> ((fullVersionNumber >> 9) & 0xff))
                , fVerSubStage ((fullVersionNumber >> 1) & 0xff)
                , fFinalBuild (fullVersionNumber & 1)
            {
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Version::Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint8_t verSubStage, bool finalBuild)
                : fMajorVer (majorVer)
                , fMinorVer (minorVer)
                , fVerStage (verStage)
                , fVerSubStage (verSubStage)
                , fFinalBuild (finalBuild)
            {
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            FullVersionType  Version::AsFullVersionNum () const
            {
                return Stroika_Make_FULL_VERSION (fMajorVer, fMinorVer, ((uint8_t)fVerStage), fVerSubStage, static_cast<int> (fFinalBuild));
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            int Version::Compare (const Version& rhs) const
            {
                return make_signed<FullVersionType>::type (AsFullVersionNum ()) - make_signed<FullVersionType>::type (rhs.AsFullVersionNum ());
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator< (const Version& rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator<= (const Version& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator> (const Version& rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator>= (const Version& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator== (const Version& rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            bool    Version::operator!= (const Version& rhs) const
            {
                return Compare (rhs) != 0;
            }


#if     defined (__cplusplus)
        }
    }
}
#endif
#endif  /*_Stroika_Foundation_Configuration_Version_inl_*/
