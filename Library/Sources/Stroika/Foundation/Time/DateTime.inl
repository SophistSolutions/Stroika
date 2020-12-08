/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ********************************** DateTime ************************************
     ********************************************************************************
     */
    inline constexpr DateTime::DateTime (const Date& d) noexcept
        : fTimezone_{Timezone::kUnknown}
        , fDate_{d}
    {
    }
    inline constexpr DateTime::DateTime (const DateTime& dt, const Date& updateDate) noexcept
        : fTimezone_{dt.GetTimezone ()}
        , fDate_{updateDate}
        , fTimeOfDay_{dt.GetTimeOfDay ().has_value () ? optional<TimeOfDay> (*dt.GetTimeOfDay ()) : nullopt}
    {
    }
    inline constexpr DateTime::DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept
        : fTimezone_{dt.GetTimezone ()}
        , fDate_{dt.GetDate ()}
        , fTimeOfDay_{updateTOD}
    {
    }
    inline constexpr DateTime::DateTime (const Date& date, const optional<TimeOfDay>& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay.has_value () ? optional<TimeOfDay>{*timeOfDay} : nullopt}
    {
    }
    inline constexpr DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay}
    {
    }
    inline constexpr DateTime DateTime::kMin{Date::kMin, optional<TimeOfDay>{TimeOfDay::kMin}, Timezone::kUnknown};
    inline constexpr DateTime DateTime::min ()
    {
        return DateTime{Date::kMin, optional<TimeOfDay>{TimeOfDay::kMin}, Timezone::kUnknown};
    }
    inline constexpr DateTime DateTime::kMax{Date::kMax, optional<TimeOfDay>{TimeOfDay::kMax}, Timezone::kUnknown};
    inline constexpr DateTime DateTime::max ()
    {
        return DateTime{Date::kMax, optional<TimeOfDay>{TimeOfDay::kMax}, Timezone::kUnknown};
    }
    inline constexpr Date DateTime::GetDate () const noexcept
    {
        return fDate_;
    }
    inline constexpr optional<TimeOfDay> DateTime::GetTimeOfDay () const noexcept
    {
        return fTimeOfDay_;
    }
    inline DateTime DateTime::Parse (const String& rep, const locale& l)
    {
        return Parse (rep, l, kDefaultParseFormats);
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
#if __cpp_impl_three_way_comparison >= 201907
    inline strong_ordering DateTime::operator<=> (const DateTime& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline bool DateTime::operator== (const DateTime& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs) == 0;
    }
#endif

    /*
     ********************************************************************************
     ************************* DateTime::ThreeWayComparer ***************************
     ********************************************************************************
     */
    constexpr DateTime::ThreeWayComparer::ThreeWayComparer (bool coerceToCommonTimezone)
        : fCoerceToCommonTimezone{coerceToCommonTimezone}
    {
    }

    /*
     ********************************************************************************
     ************************** DateTime operators **********************************
     ********************************************************************************
     */
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator< (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    inline bool operator<= (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    inline bool operator== (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) == 0;
    }
    inline bool operator!= (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) != 0;
    }
    inline bool operator>= (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    inline bool operator> (const DateTime& lhs, const DateTime& rhs)
    {
        return DateTime::ThreeWayComparer{}(lhs, rhs) > 0;
    }
#endif

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat> DefaultNames<Stroika::Foundation::Time::DateTime::ParseFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::ParseFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::DateTime::ParseFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::DateTime::ParseFormat::eRFC1123, L"RFC-1123"},
        }}};
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::PrintFormat> DefaultNames<Stroika::Foundation::Time::DateTime::PrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::PrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::PrintFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eRFC1123, L"RFC-1123"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eCurrentLocale_WithZerosStripped, L"Current-Locale-With-Zeros-Stripped"},
        }}};
}

namespace Stroika::Foundation::Traversal::RangeTraits {
    inline constexpr Time::DateTime DefaultRangeTraits<Time::DateTime>::kLowerBound{Time::DateTime::kMin};
    inline constexpr Time::DateTime DefaultRangeTraits<Time::DateTime>::kUpperBound{Time::DateTime::kMax};
}

#endif /*_Stroika_Foundation_Time_DateTime_inl_*/
