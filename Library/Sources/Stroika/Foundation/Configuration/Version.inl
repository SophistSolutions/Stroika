/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Version_inl_
#define _Stroika_Foundation_Configuration_Version_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if defined(__cplusplus)
namespace Stroika::Foundation::Configuration {
#endif

    /*
     ********************************************************************************
     *********************************** Version ************************************
     ********************************************************************************
     */
    inline constexpr Version::Version ()
        : fMajorVer (0)
        , fMinorVer (0)
        , fVerStage (VersionStage::eSTART)
        , fVerSubStage (0)
        , fFinalBuild (0)
    {
    }
    inline constexpr Version::Version (Binary32BitFullVersionType fullVersionNumber)
        : fMajorVer ((fullVersionNumber >> 24) & 0x8f)
        , fMinorVer ((fullVersionNumber >> 16) & 0xff)
        , fVerStage (static_cast<VersionStage> ((fullVersionNumber >> 13) & 0x07))
        , fVerSubStage ((fullVersionNumber >> 1) & 0xff)
        , fFinalBuild (fullVersionNumber & 1)
    {
        if (fVerStage < VersionStage::eSTART) {
            // @todo throw but be careful about includes
        }
        if (fVerStage > VersionStage::eLAST) {
            // @todo throw but be careful about includes
        }
        // @todo check any other overflows
    }
    inline constexpr Version::Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint16_t verSubStage, bool finalBuild)
        : fMajorVer (majorVer)
        , fMinorVer (minorVer)
        , fVerStage (verStage)
        , fVerSubStage (verSubStage)
        , fFinalBuild (finalBuild)
    {
    }
    inline constexpr Binary32BitFullVersionType Version::AsFullVersionNum () const
    {
        return Stroika_Make_FULL_VERSION (fMajorVer, fMinorVer, ((uint8_t)fVerStage), fVerSubStage, static_cast<int> (fFinalBuild));
    }
    inline Characters::String Version::ToString () const
    {
        return AsPrettyVersionString ();
    }
    inline constexpr int Version::Compare (const Version& rhs) const
    {
        return make_signed_t<Binary32BitFullVersionType> (AsFullVersionNum ()) - make_signed_t<Binary32BitFullVersionType> (rhs.AsFullVersionNum ());
    }
    inline constexpr bool Version::operator< (const Version& rhs) const
    {
        return Compare (rhs) < 0;
    }
    inline constexpr bool Version::operator<= (const Version& rhs) const
    {
        return Compare (rhs) <= 0;
    }
    inline constexpr bool Version::operator> (const Version& rhs) const
    {
        return Compare (rhs) > 0;
    }
    inline constexpr bool Version::operator>= (const Version& rhs) const
    {
        return Compare (rhs) >= 0;
    }
    inline constexpr bool Version::operator== (const Version& rhs) const
    {
        return Compare (rhs) == 0;
    }
    inline constexpr bool Version::operator!= (const Version& rhs) const
    {
        return Compare (rhs) != 0;
    }

#if defined(__cplusplus)
}
#endif

#endif /*_Stroika_Foundation_Configuration_Version_inl_*/
