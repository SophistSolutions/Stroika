/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Configuration {

    /*
     ********************************************************************************
     *********************************** Version ************************************
     ********************************************************************************
     */
    constexpr Version::Version ()
        : fMajorVer{0}
        , fMinorVer{0}
        , fVerStage{VersionStage::eSTART}
        , fVerSubStage{0}
        , fFinalBuild{0}
    {
    }
    constexpr Version::Version (Binary32BitFullVersionType fullVersionNumber)
        : fMajorVer{static_cast<uint8_t> ((fullVersionNumber >> 24) & 0x8f)}
        , fMinorVer{static_cast<uint8_t> ((fullVersionNumber >> 16) & 0xff)}
        , fVerStage{static_cast<VersionStage> ((fullVersionNumber >> 13) & 0x07)}
        , fVerSubStage{static_cast<uint16_t> ((fullVersionNumber >> 1) & 0xfff)}
        , fFinalBuild{static_cast<bool> (fullVersionNumber & 1)}
    {
        Assert (fVerSubStage < kMaxVersionSubStage); // we got shift/mask right above
    }
    constexpr Version::Version (uint8_t majorVer, uint8_t minorVer, VersionStage verStage, uint16_t verSubStage, bool finalBuild)
        : fMajorVer{majorVer}
        , fMinorVer{minorVer}
        , fVerStage{verStage}
        , fVerSubStage{verSubStage}
        , fFinalBuild{finalBuild}
    {
        // @todo validate arg verSubStage < kMaxVersionSubStage
    }
    constexpr Binary32BitFullVersionType Version::AsFullVersionNum () const
    {
        // @todo validate arg verSubStage < kMaxVersionSubStage
        return Stroika_Make_FULL_VERSION (fMajorVer, fMinorVer, ((uint8_t)fVerStage), fVerSubStage, static_cast<int> (fFinalBuild));
    }
    inline Characters::String Version::ToString () const
    {
        return AsPrettyVersionString ();
    }
    constexpr bool Version::operator== (const Version& rhs) const
    {
        return make_signed_t<Binary32BitFullVersionType> (AsFullVersionNum ()) == make_signed_t<Binary32BitFullVersionType> (rhs.AsFullVersionNum ());
    }
    constexpr strong_ordering Version::operator<=> (const Version& rhs) const
    {
        return make_signed_t<Binary32BitFullVersionType> (AsFullVersionNum ()) <=> make_signed_t<Binary32BitFullVersionType> (rhs.AsFullVersionNum ());
    }

}
