/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_h_
#define _Stroika_Foundation_Time_DateTime_h_    1

#include    "../StroikaPreComp.h"

#include    <climits>
#include    <ctime>
#include    <locale>
#include    <string>

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"

#include    "Date.h"
#include    "TimeOfDay.h"
#include    "Timezone.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   Only SetDate (const Date& d);/SetTimeOfDay are non-const methods. Consider making this const
 *              and just return a new date. Would matter from an API standpoint if the internl rep was BIGGER
 *              (and we did some kind of cache/ptr think like with GC'd langauges). BUt then I can always use
 *              SharedByValue<> template. Maybe a boondoggle?
 *
 *              This is an ESPECIALLY good idea since we've now done this for the 'Date' class.
 *
 *				<<< NOTE - DID DEPRECATE AND NEW CTOR 2014-09-21>>>
 *
 *
 *
 *      @todo   I think we either need to use constexpr for kMin/kMax and declare stuff in headers, or
 *              use ModuleInit<> code to assure proper construction order.
 *
 *              So far this doesnt appear to have caused problems by the DurationRange<> code refrences
 *              these constants at its module init time.
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            using   Characters::String;


            class   Duration;   // forward declare for Differnce ()


            /*
             *  Description:
             *
             *      DateTime is more than just a combination of Date, and Time. It also introduces the notion of TIMEZONE.
             *
             *      There are 3 possabilities for timezone - LOCALTIME, GMT, and UNKNOWN.
             *
             *      'empty' concept:
             *          Treat it as DISTINCT from any other DateTime. However, when converting it to a number
			 *			of seconds or days (JulienRep), treat empty as DateTime::kMin. For format routine,
			 *			return empty string. And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN DateTime::kMin.
             *          This is a bit like the floating point concept of negative infinity.
			 *
			 *			This concept is the same as the Date::empty () concept.
			 *
			 *	<<<CONSIDERING MAYBE REQUIRING>>>
			 *			Also note that if empty () - BOTH the date and timeofday parts of the DateTime must be empty, and
			 *			the value of timezone is undefined.
			 *	<<</CONSIDERING MAYBE REQUIRING>>>
             *
             *  \note   This type properties (kMin/kMax) can only be used after static initialization, and before
             *          static de-initializaiton.
             */
            class   DateTime {
            public:
                /**
                 *  Most of the time applications will utilize localtime. But occasionally its useful to use
                 *  different timezones, and our approach to this is to simply convert everything to GMT.
                 *
                 *  eUnknown - for the most part - is treated as if it were localtime (except with compare).
                 *  However - the "Kind" function returns Unknown in case your application wants to treat it
                 *  differently.
                 */
                enum class Timezone : uint8_t {
                    eLocalTime,
                    eUTC,
                    eUnknown,

                    Stroika_Define_Enum_Bounds (eLocalTime, eUnknown)
                };

            public:
                /**
                 *  Construct a DateTime record with the given date and time value. Presume that these values
                 *  apply to the given timezone. This does not adjust the value of date/time for the given tz -
                 *  but just records that this datetime refers to the given timezone.
                 *
                 *  To change TO a target timezone, use AsUTC () or AsLocalTime ().
                 */
                DateTime ();
                DateTime (const Date& d);
                DateTime (const DateTime& dt, const Date& updateDate);
                DateTime (const DateTime& dt, const TimeOfDay& updateTOD);
                DateTime (const Date& date, const TimeOfDay& timeOfDay, Timezone tz = Timezone::eUnknown);

            public:
                /**
                 *  UNIX epoch time is inheritently timezone independent. However, the tz argumet tells
                 *  the CTOR what timezone to assocaiate with the DateTime object once constructed, and if localtime
                 *  or unknown it will convert it to a localtime value.
                 */
                explicit DateTime (time_t unixTime, Timezone tz = Timezone::eUnknown);

                /*
                 */
                explicit DateTime (const tm& tmTime, Timezone tz = Timezone::eUnknown);

#if     qPlatform_POSIX
            public:
                explicit DateTime (const timespec& tmTime, Timezone tz = Timezone::eUnknown);
#endif

#if     qPlatform_Windows
            public:
                explicit DateTime (const SYSTEMTIME& sysTime, Timezone tz = Timezone::eLocalTime);

                /*
                 *  Most windows APIs return filetimes in UTC (or so it appears). Because of this,
                 *  our default interpretation of a FILETIME structure as as UTC.
                 *  Call DateTime (ft).AsLocalTime () to get the value returned in local time.
                 */
                explicit DateTime (const FILETIME& fileTime, Timezone tz = Timezone::eUTC);
#endif

            public:
                /**
                 *  \brief  ParseFormat is a representation which a datetime can be transformed out of
                 *
                 *  eCurrentLocale
                 *      Note this is the current C++ locale, which may not be the same as the platform default locale.
                 *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
                 */
                enum class  ParseFormat : uint8_t {
                    eCurrentLocale,
                    eISO8601,
                    eXML,

                    Stroika_Define_Enum_Bounds (eCurrentLocale, eXML)
                };

            public:
                static  DateTime    Parse (const String& rep, ParseFormat pf);
                static  DateTime    Parse (const String& rep, const locale& l);
#if     qPlatform_Windows
                static  DateTime    Parse (const String& rep, LCID lcid);
#endif

            public:
                /**
                 *  If the date is empty - this is empty (no date with a time doesn't make much sense as a
                 *  DateTime - use TimeOfDay by itself if thats what you want).
                 *  Timezone is ignored for the purpose of 'empty' check.
                 */
                nonvirtual  bool    empty () const;

            public:
                // Return the current DateTime (in LocalTime)
                static  DateTime    Now ();

                // Return the current Date (in LocalTime - local timezone)
                static  Date        GetToday ();

            public:
                /*
                 * DateTime::kMin is the first date this DateTime class supports representing.
                 */
                static  const   DateTime&    kMin;
                /*
                 * DateTime::kMin is the first date this DateTime class supports representing.
                 */
                static  const   DateTime&    kMax;

            public:
                nonvirtual  Timezone    GetTimezone () const;

            public:
                // Creates a new DateTime object known to be in localtime. If this DateTime is unknown, then the
                // conversion is also unknown (but either treat Kind as localtime or UTC)
                nonvirtual  DateTime    AsLocalTime () const;
                // Creates a new DateTime object known to be in UTC. If this DateTime is unknown, then the
                // conversion is also unknown (but either treat Kind as localtime or UTC)
                nonvirtual  DateTime    AsUTC () const;

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
                enum class  PrintFormat : uint8_t {
                    eCurrentLocale,
                    eISO8601,
                    eXML,
                    eCurrentLocale_WithZerosStripped,

                    Stroika_Define_Enum_Bounds (eCurrentLocale, eCurrentLocale_WithZerosStripped)

                    eDEFAULT    =   eCurrentLocale_WithZerosStripped,
                };

            public:
                nonvirtual  String Format (PrintFormat pf = PrintFormat::eDEFAULT) const;
                nonvirtual  String Format (const locale& l) const;

#if     qPlatform_Windows
                nonvirtual  String Format (LCID lcid) const;
#endif

            public:
                /**
                 *  Returns number of days since this point - relative to NOW. Never less than zero.
                 */
                nonvirtual  Date::JulianRepType DaysSince () const;

            public:
                /*
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
                 *  NB: This function result is TIMEZONE AGNOSTIC (always? at least for time_t - but assume for EVERYTHING unless documented otherwise) -- LGP 2012-05-01.
                 */
                template    <typename T>
                nonvirtual  T   As () const;


            public:
                nonvirtual  Date        GetDate () const;       // careful of timezone issues? (always in current timezone - I guess)

			public:
                nonvirtual  TimeOfDay   GetTimeOfDay () const;  // ditto

			public:
                _DeprecatedFunction_ (nonvirtual  void        SetDate (const Date& d), "d = DateTime (dt, d) - to be removed after v2.0a46");
                _DeprecatedFunction_ (nonvirtual  void        SetTimeOfDay (const TimeOfDay& tod), "dt = DateTime (dt, tod) - to be removed after v2.0a46");

            public:
                /*
                 * Add the given amount of time to construct a new DateTime object. This funtion does NOT change the timezone value nor adjust
                 * for timezone issues. It doesn't modify this.
                 */
                nonvirtual  DateTime    Add (const Duration& d) const;

                /*
                 * Add the given number of days to construct a new DateTime object. This funtion does NOT change the timezone value nor adjust
                 * for timezone issues. It doesn't modify this.
                 */
                nonvirtual  DateTime    AddDays (int days) const;
                /*
                 * Add the given number of seconds to construct a new DateTime object. This funtion does NOT change the timezone value nor adjust
                 * for timezone issues. It doesn't modify this.
                 */
                nonvirtual  DateTime    AddSeconds (time_t seconds) const;


            public:
                /**
                 *Returns the difference between the two DateTime records. This can then be easily converted to seconds, or days, or whatever
                 */
                nonvirtual  Duration    Difference (const DateTime& rhs) const;

            public:
                /**
                 *  Syntactic sugar on Add()
                 */
                nonvirtual  DateTime operator+ (const Duration& rhs) const;

            public:
                /**
                 *  Syntactic sugar on Add() or Difference()
                 */
                nonvirtual  DateTime operator- (const Duration& rhs) const;
                nonvirtual  Duration operator- (const DateTime& rhs) const;

            public:
                // Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
                // this comparison function - see the notes about 'empty' in the class description.
                //
                // Also note - if the datetimes differ in their GetTimeZone() value, they are not necessarily considered different. If either one is
                // unknown, they will both be treated as the same timezone. Otherwise, they will BOTH be converted to GMT, and compared as GMT.
                nonvirtual  int Compare (const DateTime& rhs) const;

            public:
                /**
                *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const DateTime& rhs)
                */
                nonvirtual  bool    operator< (const DateTime& rhs) const;
                nonvirtual  bool    operator<= (const DateTime& rhs) const;
                nonvirtual  bool    operator> (const DateTime& rhs) const;
                nonvirtual  bool    operator>= (const DateTime& rhs) const;
                nonvirtual  bool    operator== (const DateTime& rhs) const;
                nonvirtual  bool    operator!= (const DateTime& rhs) const;

            private:
                Timezone    fTimezone_;
                Date        fDate_;
                TimeOfDay   fTimeOfDay_;
            };
            /**
             *  Returns seconds since midnight 1970 (its independent of timezone). This is UNIX 'Epoch time'.
             */
            template    <>
            time_t  DateTime::As () const;
            template    <>
            tm  DateTime::As () const;
#if     qPlatform_POSIX
            template    <>
            timespec    DateTime::As () const;
#endif
#if     qPlatform_Windows
            template    <>
            SYSTEMTIME  DateTime::As () const;
#endif
            template    <>
            Date    DateTime::As () const;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DateTime.inl"

#endif  /*_Stroika_Foundation_Time_DateTime_h_*/
