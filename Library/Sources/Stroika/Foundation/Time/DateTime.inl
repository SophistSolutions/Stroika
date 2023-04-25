/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    inline constexpr DateTime            DateTime::kMin{Date::kMin, optional<TimeOfDay>{TimeOfDay::kMin}, Timezone::kUnknown};
    inline constexpr DateTime            DateTime::kMax{Date::kMax, optional<TimeOfDay>{TimeOfDay::kMax}, Timezone::kUnknown};
    inline constexpr Date                DateTime::GetDate () const noexcept { return fDate_; }
    inline constexpr optional<TimeOfDay> DateTime::GetTimeOfDay () const noexcept { return fTimeOfDay_; }
    inline DateTime           DateTime::Parse (const String& rep, const locale& l) { return Parse (rep, l, kDefaultParseFormats); }
    inline optional<DateTime> DateTime::ParseQuietly (const String& rep, const String& formatPattern, size_t* consumedCharacters)
    {
        return ParseQuietly (rep, locale{}, formatPattern, consumedCharacters);
    }
    inline optional<DateTime> DateTime::ParseQuietly (const String& rep, const locale& l, const String& formatPattern, size_t* consumedCharacters)
    {
        if (rep.empty ()) [[unlikely]] {
            return nullopt; // Stroika v2.1 erroiously threw FormatException::kThe here - missing the word 'Quietly'
        }
        return ParseQuietly_ (rep.As<wstring> (), use_facet<time_get<wchar_t>> (l), formatPattern, consumedCharacters);
    }
    template <>
    inline Date DateTime::As () const
    {
        return fDate_;
    }
    inline Date                         DateTime::GetToday () noexcept { return Now ().GetDate (); }
    inline constexpr optional<Timezone> DateTime::GetTimezone () const noexcept { return fTimezone_; }
    inline strong_ordering DateTime::operator<=> (const DateTime& rhs) const { return ThreeWayComparer{}(*this, rhs); }
    inline bool DateTime::operator== (const DateTime& rhs) const { return ThreeWayComparer{}(*this, rhs) == 0; }

    /*
     ********************************************************************************
     ************************* DateTime::ThreeWayComparer ***************************
     ********************************************************************************
     */
    constexpr DateTime::ThreeWayComparer::ThreeWayComparer (bool coerceToCommonTimezone)
        : fCoerceToCommonTimezone{coerceToCommonTimezone}
    {
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::LocaleIndependentFormat> DefaultNames<Stroika::Foundation::Time::DateTime::LocaleIndependentFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::LocaleIndependentFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::LocaleIndependentFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::DateTime::LocaleIndependentFormat::eRFC1123, L"RFC-1123"},
        }}};
    template <>
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::NonStandardPrintFormat> DefaultNames<Stroika::Foundation::Time::DateTime::NonStandardPrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::NonStandardPrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::NonStandardPrintFormat::eCurrentLocale_WithZerosStripped,
             L"Current-Locale-With-Zeros-Stripped"},
        }}};
}

namespace Stroika::Foundation::Traversal::RangeTraits {
    inline constexpr Time::DateTime Default<Time::DateTime>::kLowerBound{Time::DateTime::kMin};
    inline constexpr Time::DateTime Default<Time::DateTime>::kUpperBound{Time::DateTime::kMax};
}

#endif /*_Stroika_Foundation_Time_DateTime_inl_*/
