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

namespace Stroika::Foundation::Time {

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
        , fTimeOfDay_ (dt.GetTimeOfDay ().has_value () ? *dt.GetTimeOfDay () : TimeOfDay{})
    {
    }
    inline constexpr DateTime::DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept
        : fTimezone_ (dt.GetTimezone ())
        , fDate_ (dt.GetDate ())
        , fTimeOfDay_ (updateTOD)
    {
        Require (not updateTOD.empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
    }
    inline constexpr DateTime::DateTime (const Date& date, const optional<TimeOfDay>& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay.has_value () ? *timeOfDay : TimeOfDay{}}
    {
        Require (not timeOfDay.has_value () or not timeOfDay->empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
    }
    inline constexpr DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay}
    {
        Require (not timeOfDay.empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
    }
    inline constexpr DateTime DateTime::min ()
    {
        return DateTime{Date::min (), optional<TimeOfDay>{TimeOfDay::min ()}, Timezone::Unknown ()};
    }
    inline constexpr DateTime DateTime::max ()
    {
        return DateTime{Date::max (), optional<TimeOfDay>{TimeOfDay::max ()}, Timezone::Unknown ()};
    }
    inline constexpr bool DateTime::empty () const noexcept
    {
        return fDate_.empty ();
    }
    inline constexpr Date DateTime::GetDate () const noexcept
    {
        return fDate_;
    }
    inline constexpr optional<TimeOfDay> DateTime::GetTimeOfDay () const noexcept
    {
        return fTimeOfDay_.empty () ? optional<TimeOfDay>{} : fTimeOfDay_;
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
    inline constexpr optional<Timezone> DateTime::GetTimezone () const noexcept
    {
        return fTimezone_;
    }

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

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Stroika::Foundation::Time::DateTime::ParseFormat> : EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat> {
        static constexpr EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat> k{
            EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat>::BasicArrayInitializer{
                {
                    {Stroika::Foundation::Time::DateTime::ParseFormat::eCurrentLocale, L"Current-Locale"},
                    {Stroika::Foundation::Time::DateTime::ParseFormat::eISO8601, L"ISO-8601"},
                    {Stroika::Foundation::Time::DateTime::ParseFormat::eXML, L"XML"},
                    {Stroika::Foundation::Time::DateTime::ParseFormat::eRFC1123, L"RFC-1123"},
                }}};
        DefaultNames ()
            : EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_Time_DateTime_inl_*/
