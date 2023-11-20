/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_ 1

#include <chrono>
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Time {

    namespace Private_ {
        // From https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
        // EXPECIALLY read commentary...
        template <typename DstTimePointT, typename SrcTimePointT, typename DstClockT = typename DstTimePointT::clock, typename SrcClockT = typename SrcTimePointT::clock>
        DstTimePointT clock_cast_0th (const SrcTimePointT tp)
        {
            const auto src_now = SrcClockT::now ();
            const auto dst_now = DstClockT::now ();
            return dst_now + chrono::duration_cast<typename DstClockT::duration> (tp - src_now);
        }
        namespace detail {
            template <typename DurationT, typename ReprT = typename DurationT::rep>
            constexpr DurationT max_duration () noexcept
            {
                return DurationT{std::numeric_limits<ReprT>::max ()};
            }
            template <typename DurationT>
            constexpr DurationT abs_duration (const DurationT d) noexcept
            {
                return DurationT{(d.count () < 0) ? -d.count () : d.count ()};
            }
        }
        template <typename DstTimePointT, typename SrcTimePointT, typename DstDurationT = typename DstTimePointT::duration, typename SrcDurationT = typename SrcTimePointT::duration,
                  typename DstClockT = typename DstTimePointT::clock, typename SrcClockT = typename SrcTimePointT::clock>
        DstTimePointT clock_cast_2nd (const SrcTimePointT tp, const SrcDurationT tolerance = std::chrono::nanoseconds{100}, const int limit = 4)
        {
            Assert (limit > 0);
            auto itercnt = 0;
            auto src_now = SrcTimePointT{};
            auto dst_now = DstTimePointT{};
            auto epsilon = detail::max_duration<SrcDurationT> ();
            do {
                const auto src_before  = SrcClockT::now ();
                const auto dst_between = DstClockT::now ();
                const auto src_after   = SrcClockT::now ();
                const auto src_diff    = src_after - src_before;
                const auto delta       = detail::abs_duration (src_diff);
                if (delta < epsilon) {
                    src_now = src_before + src_diff / 2;
                    dst_now = dst_between;
                    epsilon = delta;
                }
                if (++itercnt >= limit)
                    break;
            } while (epsilon > tolerance);
            return dst_now + chrono::duration_cast<typename DstClockT::duration> (tp - src_now);
        }
        template <typename DstTimePointT, typename SrcTimePointT>
        inline DstTimePointT my_clock_cast (const SrcTimePointT tp)
        {
            using namespace std::chrono;
            using DstClockT = typename DstTimePointT::clock;
            using SrcClockT = typename SrcTimePointT::clock;
            // @todo find better way to check if should use clock_cast or my_private_clock_cast
            // clang-format off
            #if __cpp_lib_chrono >= 201907L
            if constexpr (
                (same_as<DstClockT, system_clock> or same_as<DstClockT, utc_clock> or same_as<DstClockT, gps_clock> or same_as<DstClockT, file_clock> or  same_as<DstClockT, tai_clock>)
                and (same_as<SrcClockT, system_clock> or same_as<SrcClockT, utc_clock> or same_as<SrcClockT, gps_clock> or same_as<SrcClockT, file_clock> or  same_as<SrcClockT, tai_clock>)
               ) {
                return clock_cast<DstClockT> (tp);
            }
            else {
                //return clock_cast_0th<DstTimePointT> (tp);
                return clock_cast_2nd<DstTimePointT> (tp);
            }
            #else
                //return clock_cast_0th<DstTimePointT> (tp);
                return clock_cast_2nd<DstTimePointT> (tp);
            #endif
            // clang-format on
        }
    }

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
        : DateTime{chrono::system_clock::to_time_t (Private_::my_clock_cast<chrono::system_clock::time_point> (timePoint))}
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
    template <>
    inline time_point<chrono::system_clock, chrono::duration<double>> DateTime::As_TP_ () const
    {
        return chrono::system_clock::from_time_t (this->As<time_t> ());
    }
    template <typename CLOCK_T, typename DURATION_T>
    time_point<CLOCK_T, DURATION_T> DateTime::As_TP_ () const
    {
        return Private_::my_clock_cast<time_point<CLOCK_T, DURATION_T>> (this->As_TP_<chrono::system_clock, chrono::duration<double>> ());
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
