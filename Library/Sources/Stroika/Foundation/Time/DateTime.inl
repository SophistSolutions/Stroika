/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"
#include "../Execution/ModuleInit.h"

namespace Stroika {
    namespace Foundation {
        namespace Time {

            /*
             ********************************************************************************
             ********************************** DateTime ************************************
             ********************************************************************************
             */
            inline constexpr DateTime::DateTime () noexcept
                : fTimezone_{Timezone::Unknown ()}
                , fDate_ ()
                , fTimeOfDay_ ()
            {
            }
            inline constexpr DateTime::DateTime (const Date& d) noexcept
                : fTimezone_{Timezone::Unknown ()}
                , fDate_ (d)
                , fTimeOfDay_ ()
            {
            }
            inline constexpr DateTime::DateTime (const DateTime& dt, const Date& updateDate) noexcept
                : fTimezone_ (dt.GetTimezone ())
                , fDate_ (updateDate)
                , fTimeOfDay_ (dt.GetTimeOfDay ())
            {
            }
            inline constexpr DateTime::DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept
                : fTimezone_ (dt.GetTimezone ())
                , fDate_ (dt.GetDate ())
                , fTimeOfDay_ (updateTOD)
            {
            }
            inline constexpr DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, const Memory::Optional<Timezone>& tz) noexcept
                : fTimezone_{tz}
                , fDate_{date}
                , fTimeOfDay_{timeOfDay}
            {
            }
            inline constexpr DateTime DateTime::min ()
            {
                return DateTime{Date::min (), TimeOfDay::min (), Timezone::Unknown ()};
            }
            inline constexpr DateTime DateTime::max ()
            {
                return DateTime{Date::max (), TimeOfDay::max (), Timezone::Unknown ()};
            }
            inline constexpr bool DateTime::empty () const noexcept
            {
                return fDate_.empty ();
            }
            inline constexpr Date DateTime::GetDate () const noexcept
            {
                return fDate_;
            }
            inline constexpr TimeOfDay DateTime::GetTimeOfDay () const noexcept
            {
                return fTimeOfDay_;
            }
            template <>
            inline Date DateTime::As () const
            {
                return fDate_;
            }
            inline Date DateTime::GetToday () noexcept
            {
                return Now ().GetDate ();
            }
            inline constexpr Memory::Optional<Timezone> DateTime::GetTimezone () const noexcept
            {
                return fTimezone_;
            }
            inline String DateTime::ToString () const
            {
                return Format ();
            }
            constexpr DateTime DateTime_kMin{Date::min (), TimeOfDay::min ()}; //[[deprecated ("use min ()")]]
            constexpr DateTime DateTime_kMax{Date::max (), TimeOfDay::max ()}; //[[deprecated ("use min ()")]]

            /*
             ********************************************************************************
             ************************** DateTime operators **********************************
             ********************************************************************************
             */
            inline bool operator< (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline bool operator<= (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline bool operator== (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline bool operator!= (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline bool operator>= (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline bool operator> (const DateTime& lhs, const DateTime& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Time_DateTime_inl_*/
