/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_inl_
#define _Stroika_Foundation_Time_Timezone_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation {
    namespace Time {

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
        inline make_signed_t<time_t> Timezone::GetOffset (const Date& date, const TimeOfDay& tod) const
        {
            return 60 * GetBiasInMinutesFromUTCType (date, tod);
        }
        inline constexpr bool Timezone::operator== (const Timezone& rhs) const
        {
            return fTZ_ == rhs.fTZ_ and fBiasInMinutesFromUTC_ == rhs.fBiasInMinutesFromUTC_;
        }
        inline constexpr bool Timezone::operator!= (const Timezone& rhs) const
        {
            return fTZ_ != rhs.fTZ_ or fBiasInMinutesFromUTC_ != rhs.fBiasInMinutesFromUTC_;
        }
    }
}
#endif /*_Stroika_Foundation_Time_Timezone_inl_*/
