/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
    inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC) noexcept
        : fTZ_{TZ_::eFixedOffsetBias}
        , fBiasInMinutesFromUTC_{biasInMinutesFromUTC}
    {
        Require (kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC));
    }
    inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC, DataExchange::ValidationStrategy validationStrategy)
        : fTZ_{TZ_::eFixedOffsetBias}
        , fBiasInMinutesFromUTC_{biasInMinutesFromUTC}
    {
        if (validationStrategy == DataExchange::ValidationStrategy::eThrow) {
            if (not kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC)) {
                Execution::Throw (Execution::Exception<runtime_error>{"invalid timezone offset"sv});
            }
        }
        Require (kBiasInMinutesFromUTCTypeValidRange.Contains (biasInMinutesFromUTC));
    }

    /**
     *  Returns Timezone object in UTC timezone.
     */
    inline constexpr Timezone Timezone::kUTC{TZ_::eUTC};

    /**
     *  Returns Timezone object in localtime timezone.
     *
     *  \note - LocalTime is a STICKY property. This does NOT return the FIXED OFFSET for the current local-time, but rather a special
     *          Timezone which always references that global current timezone.
     */
    inline constexpr Timezone Timezone::kLocalTime{TZ_::eLocalTime};

#if qCompilerAndStdLib_reflection_static_member_type_instantiated_while_incomplete_Buggy
    namespace Private_ {
        inline constexpr optional<Timezone> kUnknownTimezone_{nullopt};
    }
    inline constexpr const optional<Timezone>& Timezone::kUnknown = Private_::kUnknownTimezone_;
#else
    inline constexpr optional<Timezone> Timezone::kUnknown{nullopt};
#endif

    inline make_signed_t<time_t> Timezone::GetBiasFromUTC (const Date& date, const TimeOfDay& tod) const
    {
        // Cast to avoid warning, but no chance of overflow cuz GetBiasInMinutesFromUTC range restricted and time_t at least 4 bytes
        return static_cast<make_signed_t<time_t>> (60) * GetBiasInMinutesFromUTC (date, tod);
    }

}
