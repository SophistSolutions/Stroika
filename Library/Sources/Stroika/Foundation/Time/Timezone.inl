/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_inl_
#define _Stroika_Foundation_Time_Timezone_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
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
                static_assert (sizeof (Timezone) == sizeof (uint32_t)); // make sure full struct as small as possible
            }
            inline constexpr Timezone::Timezone (BiasInMinutesFromUTCType biasInMinutesFromUTC)
                : fTZ_ (TZ_::eFixedOffsetBias)
                , fBiasInMinutesFromUTC_ (biasInMinutesFromUTC)
            {
                static_assert (sizeof (Timezone) == sizeof (uint32_t)); // make sure full struct as small as possible
            }
            inline constexpr bool Timezone::operator== (const Timezone& rhs) const
            {
                return fTZ_ == rhs.fTZ_ and fBiasInMinutesFromUTC_ == rhs.fBiasInMinutesFromUTC_;
            }
            inline constexpr bool Timezone::operator!= (const Timezone& rhs) const
            {
                return fTZ_ != rhs.fTZ_ or fBiasInMinutesFromUTC_ != rhs.fBiasInMinutesFromUTC_;
            }
#if !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr const Timezone                   Timezone::kUTC{Timezone::TZ_::eUTC};
            constexpr const Timezone                   Timezone::kLocalTime{Timezone::TZ_::eLocalTime};
            constexpr const Memory::Optional<Timezone> Timezone::kUnknown{};
#endif
        }
    }
}
#endif /*_Stroika_Foundation_Time_Timezone_inl_*/
