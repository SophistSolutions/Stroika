/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_h_
#define _Stroika_Foundation_Time_TimeOfDay_h_ 1

#include "../StroikaPreComp.h"

#include <climits>
#include <locale>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"
#include "../Execution/StringException.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Need DefaultNames<> for enums in TimeOfDay module
 *
 *      @todo   Consider having some way to support double as TimeOfDay (or maybe float). Dont want the
 *              complexity of the chrono code, but some of the power ;-). Not sure how to compromise.
 *
 *      @todo   I'm not sure eCurrentLocale_WithZerosStripped is a good idea. Not sure if better
 *              to use separate format print arg or???
 *
 *      @todo   Review how we use 'empty()'. Efficient, but might be cleaner design to use optional
 *              and in places that use TimeOfDay::empty () and lose the concept of empty here?
 *
 *      @todo   Consider getting rid of empty () method and empty state. Instead - in DateTime code -
 *               use optional<>
 *
 *              PROS:   probably more logical and typesafe; empty is kindof a 'sentinal' value used
 *                      typically used to represent not present
 *              CONS:   we use this clear/empty notion alot, so may involve many changes, and we have
 *                      a sensible fittable sentinal value handy.
 *
 *      @todo   LCID stuff appears to be obsolete, and perhaps not supported by MSFT any longer. Consider
 *              de-supporting.
 *
 *      @todo   Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *      @todo   (medium) Consider using strftime and strptime with %FT%T%z.
 *              Same format
 *              That doesn�t use std::locale()
 *              En.cppreference.com/w/cpp/io/manip/get_time
 *              istringstream xxx (�2011-feb��)
 *              ss.imbue(std::locale() (�de-DE�));
 *              ss >> std::get_time(&t, �%FT%T%z�)
 *
 *      @todo   Locale based parsing code seems quite poor. Haven't really evaluated locale-based
 *              print code (but I'm not optimistic). I'm not sure if I have it wrong, or if it just
 *              sucks (main issue is Vis Studio integration - doesn't appear to pay attention to
 *              local settings from regional settings control panel, and doesn't seem at all flexible
 *              about what it accepts). But also the %X output (again - at least for windows/vis studio)
 *              looks terrible - military format - full zero precision - even if not needed?).
 *              MAYBE try %EX is the locale's alternative time representation.
 *
 *      @todo   (minor) Consider if DateTime stuff should cache locale () in some methods (static) �
 *              so can be re-used?? Performance tweek cuz current stuff doing new locale() does
 *              locking to bump refcount?
 *
 */

namespace Stroika::Foundation::Time {

    using Characters::String;

    /**
     * Description:
     *      A time value - which is assumed to be within a given day - e.g 2:30 pm.
     *
     *      NB: this implies NO NOTION of timezone. Its a time relative to midnight of a given day.
     *
     *      TimeOfDay can also be (it is by default) the special sentinal value 'empty'.
     *  This is useful - with a DateTime object for example, to represent the idea that a
     *  time has not been specified.
     *
     *  \note   'empty' concept:
     *          Treat it as DISTINCT from any other time. However, when converting it to a
     *          number of seconds, treat empty as TimeOfDay::kMin. For format routine,
     *          return empty string. And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN
     *          TimeOfDay::kMin. This is a bit like the floating point concept of negative infinity.
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     *
     */
    class TimeOfDay {
    public:
        /**
         * NB: The maximum value in a TimeOfDay struct is one less than kMaxSecondsPerDay
         */
        static constexpr uint32_t kMaxSecondsPerDay = 60 * 60 * 24u; // nb: 86400: wont fit in uint16_t

    public:
        /**
         * If value out of range - pinned to kMax.
         * We normalize to be within a given day (seconds since midnight)
         *
         *  For the TimeOfDay, we allow out of range values and pin/accumulate. But you can still never have a time of day >= kMaxSecondsPerDay.
         *  And the first hour (1pm) is hour 0, so TimeOfDay (2, 0, 0) is 3am.
         *
         *  \req argument time-of-day (in seconds or hours/minutes/seconds) is in valid range for one day
         *  \req t < kMaxSecondsPerDay
         *  \req hour < 24
         *  \req minute < 60
         *  \req seconds < 60
         */
        constexpr TimeOfDay ();
        constexpr TimeOfDay (const TimeOfDay&) = default;
        constexpr TimeOfDay (TimeOfDay&& src)  = default;
        constexpr explicit TimeOfDay (uint32_t t);
        explicit constexpr TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds);

    public:
        /**
         */
        nonvirtual TimeOfDay& operator= (const TimeOfDay&) = default;

    public:
        /**
         *  \brief  ParseFormat is a representation which a TimeOfDay can be transformed out of
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
         *  Always produces a valid legal TimeOfDay, or throws an exception
         */
        static TimeOfDay Parse (const String& rep, ParseFormat pf);
        static TimeOfDay Parse (const String& rep, const locale& l);
#if qPlatform_Windows
        static TimeOfDay Parse (const String& rep, LCID lcid);
#endif

    public:
        /**
         *  TimeOfDay::kMin is the first date this TimeOfDay class supports representing.
         *
         *  @see constexpr TimeOfDay::min ()
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
        static const TimeOfDay kMin;
#else
        static constexpr TimeOfDay kMin{0};
#endif

    public:
        /**
         *  TimeOfDay::kMax is the last date this TimeOfDay class supports representing. This is a legal TimeOfDay, and
         *  not like 'end' - one past the last legal value.
         *
         *  @see constexpr TimeOfDay::max ()
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
#if qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
        static const TimeOfDay kMax;
#else
        static constexpr TimeOfDay kMax{kMaxSecondsPerDay - 1};
#endif

    public:
        /**
         *  min () is the first date this TimeOfDay class supports representing.
         *
         *  @see constexpr TimeOfDay::kMin
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr TimeOfDay min ();

    public:
        /**
         *  max () is the last date this TimeOfDay class supports representing. This is a legal TimeOfDay, and
         *  not like 'end' - one past the last legal value.
         *
         *  @see constexpr TimeOfDay::kMax
         *
         *  \note see https://stroika.atlassian.net/browse/STK-635 for static constexpr data member kMin/kMax issue
         */
        static constexpr TimeOfDay max ();

    public:
        class FormatException;

    public:
        /**
         * In special case of empty - this also returns 0
         *
         *  \ensure {return} < kMaxSecondsPerDay
         */
        nonvirtual constexpr uint32_t GetAsSecondsCount () const; // seconds since StartOfDay (midnight)

    public:
        /**
         */
        nonvirtual constexpr bool empty () const;

    public:
        /**
         */
        nonvirtual void ClearSecondsField ();

    public:
        /**
         * returns 0..23
         */
        nonvirtual uint8_t GetHours () const;

    public:
        /**
         *  returns 0..59
         */
        nonvirtual uint8_t GetMinutes () const;

    public:
        /**
         *  returns 0..59
         */
        nonvirtual uint8_t GetSeconds () const;

    public:
        /**
         *  \brief  PrintFormat is a representation which a TimeOfDay can be transformed into
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
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
         *  this comparison function - see the notes about 'empty' in the class description.
         */
        nonvirtual int Compare (const TimeOfDay& rhs) const;

    private:
        uint32_t fTime_;
    };

    class TimeOfDay::FormatException : public Execution::StringException {
    public:
        FormatException ();

    public:
        /**
         */
        static const FormatException kThe;
    };

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator< (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator<= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator== (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator!= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator>= (TimeOfDay lhs, TimeOfDay rhs);

    /**
     *  operator indirects to TimeOfDay::Compare()
     */
    bool operator> (TimeOfDay lhs, TimeOfDay rhs);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TimeOfDay.inl"

#endif /*_Stroika_Foundation_Time_TimeOfDay_h_*/
