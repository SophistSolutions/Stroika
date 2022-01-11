/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        : fTZ_{tz}
        , fBiasInMinutesFromUTC_{0}
    {
    }
    inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC)
        : fTZ_{TZ_::eFixedOffsetBias}
        , fBiasInMinutesFromUTC_{biasInMinutesFromUTC}
    {
        Require (kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC));
    }
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    constexpr Timezone Timezone::kUTC{TZ_::eUTC};
#else
    inline constexpr Timezone Timezone::kUTC{TZ_::eUTC};
#endif
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    constexpr Timezone Timezone::kLocalTime{TZ_::eLocalTime};
#else
    inline constexpr Timezone Timezone::kLocalTime{TZ_::eLocalTime};
#endif
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    constexpr optional<Timezone> Timezone::kUnknown{nullopt};
#else
    inline constexpr optional<Timezone> Timezone::kUnknown{nullopt};
#endif
    inline make_signed_t<time_t> Timezone::GetBiasFromUTC (const Date& date, const TimeOfDay& tod) const
    {
        // Cast to avoid warning, but no chance of overflow cuz GetBiasInMinutesFromUTC range restricted and time_t at least 4 bytes
        return static_cast<make_signed_t<time_t>> (60) * GetBiasInMinutesFromUTC (date, tod);
    }

}

#endif /*_Stroika_Foundation_Time_Timezone_inl_*/
