/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/ModuleInit.h"

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ********************************** DateTime ************************************
     ********************************************************************************
     */
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    inline constexpr DateTime::DateTime () noexcept
        : fTimezone_{Timezone::Unknown ()}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    inline constexpr DateTime::DateTime (const Date& d) noexcept
        : fTimezone_{Timezone::Unknown ()}
        , fDate_{d}
    {
    }
    inline constexpr DateTime::DateTime (const DateTime& dt, const Date& updateDate) noexcept
        : fTimezone_ (dt.GetTimezone ())
        , fDate_ (updateDate)
        , fTimeOfDay_ (dt.GetTimeOfDay ().has_value () ? optional<TimeOfDay> (*dt.GetTimeOfDay ()) : nullopt)
    {
    }
    inline constexpr DateTime::DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept
        : fTimezone_ (dt.GetTimezone ())
        , fDate_ (dt.GetDate ())
        , fTimeOfDay_ (updateTOD)
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Require (not updateTOD.empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    inline constexpr DateTime::DateTime (const Date& date, const optional<TimeOfDay>& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay.has_value () ? optional<TimeOfDay>{*timeOfDay} : nullopt}
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Require (not timeOfDay.has_value () or not timeOfDay->empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    inline constexpr DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, const optional<Timezone>& tz) noexcept
        : fTimezone_{tz}
        , fDate_{date}
        , fTimeOfDay_{timeOfDay}
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Require (not timeOfDay.empty ()); // as of v2.1d4 - disallow passing in empty TOD, instead use optional
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
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
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        return fDate_.empty ();
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_END (4996);
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

    /*
     ********************************************************************************
     ************************** DateTime operators **********************************
     ********************************************************************************
     */
    inline bool operator< (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    inline bool operator<= (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    inline bool operator== (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    inline bool operator!= (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    inline bool operator>= (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    inline bool operator> (const DateTime& lhs, const DateTime& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }

}

namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat> DefaultNames<Stroika::Foundation::Time::DateTime::ParseFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::ParseFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::ParseFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::DateTime::ParseFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::DateTime::ParseFormat::eXML, L"XML"},
            {Stroika::Foundation::Time::DateTime::ParseFormat::eRFC1123, L"RFC-1123"},
        }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Stroika::Foundation::Time::DateTime::PrintFormat> DefaultNames<Stroika::Foundation::Time::DateTime::PrintFormat>::k{
        EnumNames<Stroika::Foundation::Time::DateTime::PrintFormat>::BasicArrayInitializer{{
            {Stroika::Foundation::Time::DateTime::PrintFormat::eCurrentLocale, L"Current-Locale"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eISO8601, L"ISO-8601"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eXML, L"XML"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eRFC1123, L"RFC-1123"},
            {Stroika::Foundation::Time::DateTime::PrintFormat::eCurrentLocale_WithZerosStripped, L"Current-Locale-With-Zeros-Stripped"},
        }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_END (4996)

}

#endif /*_Stroika_Foundation_Time_DateTime_inl_*/
