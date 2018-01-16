/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_h_
#define _Stroika_Foundation_Time_DateTime_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <ctime>
#include <locale>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Math/Common.h"

#include "Date.h"
#include "TimeOfDay.h"
#include "Timezone.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   Support various 64bit int (epoch time) types - even if time_t is 32-bit (such as on AIX).
 *              Be careful about overflow in underlying types like Date and TimeOfDay() however.
 *
 *      @todo   I'm not sure eCurrentLocale_WithZerosStripped is a good idea. Not sure if better
 *              to use separate format print arg or???
 *
 *      @todo   LCID stuff appears to be obsolete, and perhaps not supported by MSFT any longer. Consider
 *              de-supporting.
 *
 *      @todo   Current logic for TIMEZONE conversion is questionable. When we output to XML,
 *              we output the timezone offset of the currnet timezone and the daylight savingstime
 *              value as of that date. But when we read date - we ignore the offset (assume its localetime).
 *
 *      @todo   We sometimes store datetime internally as localetime, and sometimes as UTC. Maybe we
 *              should alway use UTC? If we use localtime and the timezone changes while we are running
 *              (timezone or DST) - we could get funky results.
 *
 *      @todo   (medium) Consider using strftime and strptime with %FT%T%z.
 *              Same format
 *              That doesn’t use std::locale()
 *              En.cppreference.com/w/cpp/io/manip/get_time
 *              istringstream xxx (“2011-feb…”)
 *              ss.imbue(std::locale() (“de-DE”));
 *              ss >> std::get_time(&t, “%FT%T%z”)
 *
 *      @todo   Maybe use wcsftime (buf, NEltsOf (buf), L"%I:%M %p", &temp);   or related for formatting dates/time?
 *
 *      @todo   Consider using strptime/strftime - and possibly use that to replace windows formatting APIs?
 *
 *      @todo   Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *      @todo   Consider replacing eXML with eISO8601_PF?  Not 100% sure they are the same. Maybe we should
 *              support BOTH here? Maybe where they differ doesn't matter for this class?
 *
 *      @todo   Error checking in conversions (date to string/Format/String2Date - should be doign THROWS on
 *              bad conversions I think - moistly an issue for the locale-based stuff. Now it maybe just
 *              silently returns empty date/time/etc. Better to except!
 *
 *      @todo   Future directions consider representing as big struct
 *          o   And maybe store cached string reps for common cases as optimization and
 *              store cached second-offset (mutable) for quick compares
 *          o   Note for timeofday it COULD be enhanced in the future to store TOD as
 *              fractional number of seconds. COULD use LINUX style struct with number of seconds and fixed
 *              point like number of nanoseconds (or some such)
 *
 *      @todo   (minor) Consider if DateTime stuff should cache locale () in some methods (static) –
 *              so can be re-used?? Performance tweek cuz current stuff doing new locale() does
 *              locking to bump refcount?
 */

namespace Stroika {
    namespace Foundation {
        namespace Time {

            using Characters::String;

            class Duration; // forward declare for Differnce ()

            /*
             *  Description:
             *
             *      DateTime is more than just a combination of Date, and Time. It also introduces the notion of TIMEZONE.
             *
             *      Timezone may be "unknown" (missing), or a Timezone object (@see Timezone).
             *
             *      'empty' concept:
             *          Treat it as DISTINCT from any other DateTime. However, when converting it to a number
             *          of seconds or days (JulienRep), treat empty as DateTime::kMin. For format routine,
             *          return empty string. And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN DateTime::kMin.
             *          This is a bit like the floating point concept of negative infinity.
             *
             *          This concept is the same as the Date::empty () concept.
             *
             *  <<<CONSIDERING MAYBE REQUIRING>>>
             *          Also note that if empty () - BOTH the date and timeofday parts of the DateTime must be empty, and
             *          the value of timezone is undefined.
             *  <<</CONSIDERING MAYBE REQUIRING>>>
             *
             *  \note   This type properties (kMin/kMax) can only be used after static initialization, and before
             *          static de-initializaiton.
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             *
             *  \note   In the DateTime code, time_t is always assumed to be in UTC, because according to http://en.cppreference.com/w/c/chrono/time_t:
             *              Although not defined by the C standard, this is almost always an integral 
             *              value holding the number of seconds (not counting leap seconds) 
             *              since 00:00, Jan 1 1970 UTC
             */
            class DateTime {
            public:
                /**
                 *  Construct a DateTime record with the given date and time value. Presume that these values
                 *  apply to the given (argument) timezone. This does not adjust the value of date/time for the given tz -
                 *  but just records that this datetime refers to the given timezone.
                 *
                 *  To change TO a target timezone, use AsUTC () or AsLocalTime ().
                 *
                 *  explicit DateTime (time_t unixEpochTime) noexcept
                 *      Creates a DateTime object in UTC, using UNIX Epoch time.
                 *
                 *  explicit DateTime (const FILETIME& fileTime, const Memory::Optional<Timezone>& tz = Timezone::kUTC) noexcept;
                 *      Most windows APIs return filetimes in UTC (or so it appears). Because of this,
                 *      our default interpretation of a FILETIME structure as as UTC.
                 *      Call DateTime (ft).AsLocalTime () to get the value returned in local time.
                 *
                 *  \note DateTime (time_t unixEpochTime) returns a datetime in UTC
                 */
                constexpr DateTime () noexcept;
                constexpr DateTime (const DateTime& src) = default;
                DateTime (DateTime&& src)                = default;
                constexpr DateTime (const Date& d) noexcept;
                DateTime (const DateTime& dt, const Date& updateDate) noexcept;
                DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept;
                constexpr DateTime (const Date& date, const TimeOfDay& timeOfDay, const Memory::Optional<Timezone>& tz = Timezone_kUnknown) noexcept;
                explicit DateTime (time_t unixEpochTime) noexcept;
                explicit DateTime (const tm& tmTime, const Memory::Optional<Timezone>& tz = Timezone_kUnknown) noexcept;
#if qPlatform_POSIX
                explicit DateTime (const timeval& tmTime, const Memory::Optional<Timezone>& tz = Timezone_kUnknown) noexcept;
                explicit DateTime (const timespec& tmTime, const Memory::Optional<Timezone>& tz = Timezone_kUnknown) noexcept;
#elif qPlatform_Windows
                explicit DateTime (const SYSTEMTIME& sysTime, const Memory::Optional<Timezone>& tz = Timezone::kLocalTime) noexcept;
                explicit DateTime (const FILETIME& fileTime, const Memory::Optional<Timezone>& tz = Timezone::kUTC) noexcept;
#endif

            public:
                /**
                 */
                nonvirtual DateTime& operator= (const DateTime& rhs) = default;
                nonvirtual DateTime& operator= (DateTime&& rhs) = default;

            public:
                /**
                 *  \brief  ParseFormat is a representation which a datetime can be transformed out of
                 *
                 *  eCurrentLocale
                 *      Note this is the current C++ locale, which may not be the same as the platform default locale.
                 *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
                 */
                enum class ParseFormat : uint8_t {
                    eCurrentLocale,
                    eISO8601,
                    eXML,

                    Stroika_Define_Enum_Bounds (eCurrentLocale, eXML)
                };

            public:
                /**
                 */
                static DateTime Parse (const String& rep, ParseFormat pf);
                static DateTime Parse (const String& rep, const locale& l);
#if qPlatform_Windows
                static DateTime Parse (const String& rep, LCID lcid);
#endif

            public:
                /**
                 *  If the date is empty - this DateTime is empty.
                 *
                 *  Timezone, and TimeOfDay are ignored for the purpose of 'empty' check (because empty tz means unknown, and empty tod
                 *  just means all day or time part unspecified).
                 */
                nonvirtual constexpr bool empty () const noexcept;

            public:
                /*
                 *  Return the current DateTime (in LocalTime)
                 */
                static DateTime Now () noexcept;

            public:
                /*
                 *  Return the current Date (in LocalTime - local timezone)
                 */
                static Date GetToday () noexcept;

            public:
                /*
                 * DateTime::kMin is the first date this DateTime class supports representing.
                 */
                [[deprecated ("use min ()")]] static const DateTime kMin;

            public:
                /*
                 * DateTime::kMin is the first date this DateTime class supports representing.
                 */
                [[deprecated ("use max ()")]] static const DateTime kMax;

            public:
                /**
                 *  DateTime::min () is the first date this DateTime class supports representing.
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
                static constexpr DateTime min ();

            public:
                /**
                 * DateTime::max () is the last date this DateTime class supports representing.
                 *
                 *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
                 */
                static constexpr DateTime max ();

            public:
                nonvirtual Memory::Optional<Timezone> GetTimezone () const noexcept;

            public:
                /**
                 *  Creates a new DateTime object known to be in localtime. If this DateTime is unknown, then the
                 * conversion is also unknown (but either treat Kind as localtime or UTC)
                 */
                nonvirtual DateTime AsLocalTime () const;

            public:
                /**
                 *  Creates a new DateTime object known to be in UTC. If this DateTime is unknown, then the
                 *  conversion is also unknown (but either treat Kind as localtime or UTC)
                 */
                nonvirtual DateTime AsUTC () const;

            public:
                /**
                 *  Convert the given datetime to a floating point offset in seconds based on the same reference as the tickcount.
                 *
                 *  \note   Time::GetTickCount () normally returns a positive number but if you go back in time before
                 *          its zero point, ToTickCount () will return negative numbers.
                 *
                 *  \par Example Usage
                 *      \code
                 *      Assert (Math::NearlyEquals (Time::GetTickCount (), DateTime::Now ().ToTickCount ());    // roughly true
                 *      \endcode
                 *
                 *  @see FromTickCount
                 */
                nonvirtual DurationSecondsType ToTickCount () const;

            public:
                /**
                 *  This is like a constructor, but with a more specific static name to avoid confusion with overloads.
                 *  This returns a datetime in localtime.
                 *
                 *  @see ToTickCount
                 */
                static DateTime FromTickCount (DurationSecondsType tickCount);

            public:
                /**
                 *  \brief  PrintFormat is a representation which a datetime can be transformed into
                 *
                 *  eCurrentLocale
                 *      Note this is the current C++ locale, which may not be the same as the platform default locale.
                 *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
                 *
                 *  eCurrentLocale_WithZerosStripped
                 *      eCurrentLocale_WithZerosStripped is eCurrentLocale, but with many cases of trailing zero's,
                 *      and sometimes leading zeros, stripped, so for example, 01:03:05 PM will become 1:03:05 PM,
                 *      and 04:06:00 PM will become 4:06 PM.
                 */
                enum class PrintFormat : uint8_t {
                    eCurrentLocale,
                    eISO8601,
                    eXML,
                    eCurrentLocale_WithZerosStripped,

                    eDEFAULT = eCurrentLocale_WithZerosStripped,

                    Stroika_Define_Enum_Bounds (eCurrentLocale, eCurrentLocale_WithZerosStripped)
                };

            public:
                /**
                 *  For formatPattern, see http://en.cppreference.com/w/cpp/locale/time_put/put
                 *  If only formatPattern specified, and no locale, use default (global) locale.
                 */
                nonvirtual String Format (PrintFormat pf = PrintFormat::eDEFAULT) const;
                nonvirtual String Format (const locale& l) const;
                nonvirtual String Format (const locale& l, const String& formatPattern) const;
                nonvirtual String Format (const String& formatPattern) const;
#if qPlatform_Windows
                nonvirtual String Format (LCID lcid) const;
#endif

            public:
                /**
                 *  @see Characters::ToString ()
                 */
                nonvirtual String ToString () const;

            public:
                /**
                 *  Returns number of days since this point - relative to NOW. Never less than zero.
                 */
                nonvirtual Date::JulianRepType DaysSince () const;

            public:
                /**
                 * Defined for
                 *      time_t
                 *      struct tm
                 *      struct timespec
                 *      SYSTEMTIME          (WINDOWS ONLY)
                 *      Date
                 *      String              (Format (PrintFormat::eDEFAULT))
                 *
                 *  NB: Intentionally NOT defined for TimeOfDay () - cuz it wouldn't make sense. A DateTime IS a Date, but its not a TimeOfDay. Time of day just
                 *  logically extends Date with extra (TOD) information.
                 *
                 *  \note  As<time_t> Returns seconds since midnight 1970 UTC (UNIX 'Epoch time')
                 */
                template <typename T>
                nonvirtual T As () const;

            public:
                /**
                 *  returns the Date part of the DateTime object (in this datetime's timezone).
                 */
                nonvirtual constexpr Date GetDate () const noexcept;

            public:
                /**
                 *  returns the TimeOfDay part of the DateTime object (in this datetime's timezone).
                 */
                nonvirtual constexpr TimeOfDay GetTimeOfDay () const noexcept;

            public:
                /**
                 *  Add the given amount of time to construct a new DateTime object. This funtion does NOT change 
                 *  the timezone value nor adjust for timezone issues. 
                 *
                 *  Add doesn't modify *this.
                 */
                nonvirtual DateTime Add (const Duration& d) const;

            public:
                /**
                 *  Add the given number of days to construct a new DateTime object. This funtion does NOT change 
                 *  the timezone value nor adjust for timezone issues. 
                 *
                 *  AddDays doesn't modify this.
                 */
                nonvirtual DateTime AddDays (int days) const;

            public:
                /**
                 *  Add the given number of seconds to construct a new DateTime object. This funtion does NOT change 
                 *  the timezone value nor adjust for timezone issues. 
                 *
                 *  AddSeconds doesn't modify this.
                 */
                nonvirtual DateTime AddSeconds (int64_t seconds) const;

            public:
                /**
                 *  Returns the difference between the two DateTime records. This can then be easily converted to 
                 *  seconds, or days, or whatever.
                 */
                nonvirtual Duration Difference (const DateTime& rhs) const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the 
                 *  purpose of this comparison function - see the notes about 'empty' in the class description.
                 *
                 *  Also note - if the datetimes differ in their GetTimeZone() value, they are not necessarily 
                 *  considered different. If either one is unknown, they will both be treated as the same timezone. 
                 *  Otherwise, they will BOTH be converted to GMT, and compared as GMT.
                 */
                nonvirtual int Compare (const DateTime& rhs) const;

            private:
                Memory::Optional<Timezone> fTimezone_;
                Date                       fDate_;
                TimeOfDay                  fTimeOfDay_;
            };
            template <>
            time_t DateTime::As () const;
            template <>
            tm DateTime::As () const;
#if qPlatform_POSIX
            template <>
            timespec DateTime::As () const;
#endif
#if qPlatform_Windows
            template <>
            SYSTEMTIME DateTime::As () const;
#endif
            template <>
            Date DateTime::As () const;

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator< (const DateTime& lhs, const DateTime& rhs);

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator<= (const DateTime& lhs, const DateTime& rhs);

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator== (const DateTime& lhs, const DateTime& rhs);

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator!= (const DateTime& lhs, const DateTime& rhs);

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator>= (const DateTime& lhs, const DateTime& rhs);

            /**
             *  operator indirects to DateTime::Compare()
             */
            bool operator> (const DateTime& lhs, const DateTime& rhs);

            /**
             *  Syntactic sugar on Add()
             */
            DateTime operator+ (const DateTime& lhs, const Duration& rhs);

            /**
            *  Syntactic sugar on Difference()
            */
            Duration operator- (const DateTime& lhs, const DateTime& rhs);

            /**
             *  Syntactic sugar on Add() or Difference()
             */
            DateTime operator- (const DateTime&, const Duration& rhs);
            Duration operator- (const DateTime&, const DateTime& rhs);
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Math {
            bool NearlyEquals (Time::DateTime l, Time::DateTime r);
            bool NearlyEquals (Time::DateTime l, Time::DateTime r, Time::DurationSecondsType epsilon);
            bool NearlyEquals (Time::DateTime l, Time::DateTime r, const Time::Duration& epsilon);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DateTime.inl"

#endif /*_Stroika_Foundation_Time_DateTime_h_*/
