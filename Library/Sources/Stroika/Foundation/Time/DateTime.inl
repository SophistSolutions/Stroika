/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <chrono>

#include "Clock.h"

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
    template <Configuration::ITimePoint T>
    DateTime::DateTime (T timePoint) noexcept
        : DateTime{chrono::system_clock::to_time_t (Time::clock_cast<chrono::system_clock> (timePoint))}
    {
    }
    inline constexpr DateTime DateTime::kMin{Date::kMin, optional<TimeOfDay>{TimeOfDay::kMin}, Timezone::kUnknown};
    inline constexpr DateTime DateTime::kMax{Date::kMax, optional<TimeOfDay>{TimeOfDay::kMax}, Timezone::kUnknown};
    inline constexpr Date     DateTime::GetDate () const noexcept
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
#if !qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
    template <>
    time_t DateTime::As_Simple_ () const;
    template <>
    tm DateTime::As_Simple_ () const;
    template <>
    timespec DateTime::As_Simple_ () const;
#endif
    template <>
    inline Date DateTime::As_Simple_ () const
    {
        return fDate_;
    }
    template <>
    inline Characters::String DateTime::As_Simple_ () const
    {
        return this->Format ();
    }
    template <typename CLOCK_T, typename DURATION_T>
    time_point<CLOCK_T, DURATION_T> DateTime::As_TP_ () const
    {
        auto t = Time::clock_cast<CLOCK_T> (chrono::system_clock::from_time_t (this->As<time_t> ()));
        if constexpr (same_as<DURATION_T, chrono::system_clock::duration>) {
            return t;
        }
        else {
            return chrono::time_point_cast<DURATION_T> (t);
        }
    }
    template <typename T>
    inline T DateTime::As () const
#if !qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy
        requires (same_as<T, time_t> or same_as<T, struct tm> or same_as<T, struct timespec> or same_as<T, Date> or same_as<T, Characters::String> or
#if qPlatform_Windows
                  same_as<T, SYSTEMTIME> or
#endif
                  Configuration::ITimePoint<T>)
#endif
    {
        if constexpr (same_as<T, time_t> or same_as<T, struct tm> or same_as<T, struct timespec> or same_as<T, Date> or same_as<T, Characters::String>) {
            return As_Simple_<T> ();
        }
#if qPlatform_Windows
        else if constexpr (same_as<T, SYSTEMTIME>) {
            return AsSYSTEMTIME_ ();
        }
#endif
        else if constexpr (Configuration::ITimePoint<T>) {
            return As_TP_<typename T::clock, typename T::duration> ();
        }
    }
    inline Date DateTime::GetToday () noexcept
    {
        return Now ().GetDate ();
    }
    inline constexpr optional<Timezone> DateTime::GetTimezone () const noexcept
    {
        return fTimezone_;
    }
    inline strong_ordering DateTime::operator<=> (const DateTime& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline bool DateTime::operator== (const DateTime& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs) == 0;
    }

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
