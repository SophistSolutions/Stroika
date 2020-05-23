/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_inl_
#define _Stroika_Foundation_Time_Timezone_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     *********************************** Timezone ***********************************
     ********************************************************************************
     */
    inline constexpr Timezone::Timezone (TZ_ tz) noexcept
        : fTZ_ (tz)
        , fBiasInMinutesFromUTC_ (0)
    {
    }
    inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC)
        : fTZ_ (TZ_::eFixedOffsetBias)
        , fBiasInMinutesFromUTC_ (biasInMinutesFromUTC)
    {
        Require (kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC));
    }
    inline constexpr Timezone Timezone::UTC ()
    {
        return Timezone{TZ_::eUTC};
    }
    inline constexpr Timezone Timezone::LocalTime ()
    {
        return Timezone{TZ_::eLocalTime};
    }
    inline constexpr optional<Timezone> Timezone::Unknown ()
    {
        return {};
    }
    inline make_signed_t<time_t> Timezone::GetBiasFromUTC (const Date& date, const TimeOfDay& tod) const
    {
        // Cast to avoid warning, but no chance of overflow cuz GetBiasInMinutesFromUTC range restricted and time_t at least 4 bytes
        return static_cast<make_signed_t<time_t>> (60) * GetBiasInMinutesFromUTC (date, tod);
    }
#if __cpp_impl_three_way_comparison < 201907
    inline constexpr bool Timezone::operator== (const Timezone& rhs) const
    {
        return fTZ_ == rhs.fTZ_ and fBiasInMinutesFromUTC_ == rhs.fBiasInMinutesFromUTC_;
    }
    inline constexpr bool Timezone::operator!= (const Timezone& rhs) const
    {
        return fTZ_ != rhs.fTZ_ or fBiasInMinutesFromUTC_ != rhs.fBiasInMinutesFromUTC_;
    }
    inline constexpr bool Timezone::operator< (const Timezone& rhs) const
    {
        if (fTZ_ != rhs.fTZ_) {
            return fTZ_ < rhs.fTZ_;
        }
        return fBiasInMinutesFromUTC_ < rhs.fBiasInMinutesFromUTC_;
    }
#endif

}

#endif /*_Stroika_Foundation_Time_Timezone_inl_*/
