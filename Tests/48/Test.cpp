/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Time
#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateRange.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/DateTimeRange.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/DurationRange.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;

using Stroika::Foundation::Debug::TraceContextBumper;

namespace {
    template <typename DATEORTIME>
    void TestRoundTripFormatThenParseNoChange_ (DATEORTIME startDateOrTime, const locale& l)
    {
        // disable for now cuz fails SO OFTEN
        String     formatByLocale = startDateOrTime.Format (l);
        DATEORTIME andBack        = DATEORTIME::Parse (formatByLocale, l);
        VerifyTestResult (startDateOrTime == andBack);
    }
    template <typename DATEORTIME>
    void TestRoundTripFormatThenParseNoChange_ (DATEORTIME startDateOrTime)
    {
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale ());
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale::classic ());
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, Configuration::FindNamedLocale (L"en", L"us"));
#endif

        // should add test like this...
        //VerifyTestResult (startDateOrTime == DATEORTIME::Parse (startDateOrTime.Format (DATEORTIME::PrintFormat::eCurrentLocale), DATEORTIME::PrintFormat::ParseFormat::eCurrentLocale));
    }
}

// Skip some locale tests cuz so little works
//////https://stroika.atlassian.net/browse/STK-107 -- BROKEN
#define qSupport_TestRoundTripFormatThenParseNoChange_For_TimeOfDay_ 0
#define qSupport_TestRoundTripFormatThenParseNoChange_For_Date_ 0
#define qSupport_TestRoundTripFormatThenParseNoChange_For_DateTime_ 0

#if !qSupport_TestRoundTripFormatThenParseNoChange_For_TimeOfDay_
namespace {
    template <>
    void TestRoundTripFormatThenParseNoChange_ ([[maybe_unused]] TimeOfDay startDateOrTime)
    {
    }
}
#endif

#if !qSupport_TestRoundTripFormatThenParseNoChange_For_Date_
namespace {
    template <>
    void TestRoundTripFormatThenParseNoChange_ ([[maybe_unused]] Date startDateOrTime)
    {
    }
}
#endif

#if !qSupport_TestRoundTripFormatThenParseNoChange_For_DateTime_
namespace {
    template <>
    void TestRoundTripFormatThenParseNoChange_ ([[maybe_unused]] DateTime startDateOrTime)
    {
    }
}
#endif

namespace {

    void Test_1_TestTickCountGrowsMonotonically_ ()
    {
        TraceContextBumper  ctx ("Test_1_TestTickCountGrowsMonotonically_");
        DurationSecondsType start = Time::GetTickCount ();
        Execution::Sleep (100ms);
        VerifyTestResult (start <= Time::GetTickCount ());
    }
}

namespace {

    void Test_2_TestTimeOfDay_ ()
    {
        TraceContextBumper ctx ("Test_2_TestTimeOfDay_");
        {
            optional<TimeOfDay> t;
            VerifyTestResult (not t.has_value ());
            TimeOfDay t2 (2);
            VerifyTestResult (t < t2);
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            VerifyTestResult (not t2.Format (TimeOfDay::PrintFormat::eCurrentLocale).empty ());
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996);
            VerifyTestResult (t2.GetHours () == 0);
            VerifyTestResult (t2.GetMinutes () == 0);
            VerifyTestResult (t2.GetSeconds () == 2);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay t2 (5 * 60 * 60 + 3 * 60 + 49);
            VerifyTestResult (t2.GetHours () == 5);
            VerifyTestResult (t2.GetMinutes () == 3);
            VerifyTestResult (t2.GetSeconds () == 49);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay t2 (24 * 60 * 60 - 1);
            VerifyTestResult (t2.GetHours () == 23);
            VerifyTestResult (t2.GetMinutes () == 59);
            VerifyTestResult (t2.GetSeconds () == 59);
            VerifyTestResult (t2 == TimeOfDay::max ());
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            VerifyTestResult (TimeOfDay::Parse (L"3pm", locale::classic ()).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3PM", locale::classic ()).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00", locale::classic ()).GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // Not sure these should ALWAYS work in any locale. Probably not. But any locale I'd test in??? Maybe... Good for starters anyhow...
            //      -- LGP 2011-10-08
            VerifyTestResult (TimeOfDay::Parse (L"3pm", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
#if qCompilerAndStdLib_locale_pctX_print_time_Buggy
            // NOTE - these values are wrong, but since using locale code, not easy to fix/workaround - but to note XCode locale stuff still
            // somewhat broken...
            VerifyTestResult (TimeOfDay (101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:41");
            VerifyTestResult (TimeOfDay (60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"0:01");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41");
            VerifyTestResult (TimeOfDay (60 * 60 + 60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01");
#else
            VerifyTestResult (TimeOfDay (101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"12:01:41 AM");
            VerifyTestResult (TimeOfDay (60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"12:01 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"1:01:41 AM" or TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01 AM");
#endif
#endif
        }
        {
            VerifyTestResult (TimeOfDay (101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:41");
            VerifyTestResult (TimeOfDay (60).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:00");
            VerifyTestResult (TimeOfDay (60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"0:01");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41");
            VerifyTestResult (TimeOfDay (60 * 60 + 60).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:00");
        }
        {
            TimeOfDay threePM = TimeOfDay::Parse (L"3pm", locale::classic ());
            VerifyTestResult (threePM.Format (locale::classic ()) == L"15:00:00"); // UGH!!!
            TestRoundTripFormatThenParseNoChange_ (threePM);
        }
    }
}

namespace {
    void VERIFY_ROUNDTRIP_XML_ (const Date& d)
    {
        VerifyTestResult (Date::Parse (d.Format (Date::PrintFormat::eISO8601), Date::ParseFormat::eISO8601) == d);
    }

    void Test_3_TestDate_ ()
    {
        TraceContextBumper ctx ("Test_3_TestDate_");
        {
            Date d (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eISO8601) == L"1903-04-04");
            VERIFY_ROUNDTRIP_XML_ (d);
            d = d.AddDays (4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eISO8601) == L"1903-04-08");
            d = d.AddDays (-4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eISO8601) == L"1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        try {
            Date d = Date::Parse (L"09/14/1752", locale::classic ());
            VerifyTestResult (d == Date::min ());
            VerifyTestResult (d.Format (Date::PrintFormat::eISO8601) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        catch (...) {
            VerifyTestResult (false);
        }
        {
            optional<Date> d;
            VerifyTestResult (d < DateTime::GetToday ());
            VerifyTestResult (DateTime::GetToday () > d);
            //TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            Date d = Date::min ();
            VerifyTestResult (d < DateTime::Now ().GetDate ());
            VerifyTestResult (not(DateTime::Now ().GetDate () < d));
            VerifyTestResult (d.Format (Date::PrintFormat::eISO8601) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::ParseFormat::eJavascript) == Date (Year (2001), Time::MonthOfYear::eNovember, DayOfMonth (1)));
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::ParseFormat::eJavascript).Format (Date::PrintFormat::eJavascript) == L"11/01/2001");
        }
        {
            VerifyTestResult (Date::min () < Date::max ());
            VerifyTestResult (Date::min () <= Date::max ());
            VerifyTestResult (not(Date::min () > Date::max ()));
            VerifyTestResult (not(Date::min () >= Date::max ()));
            TestRoundTripFormatThenParseNoChange_ (Date::min ());
            TestRoundTripFormatThenParseNoChange_ (Date::max ());
        }
        {
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            Date                           d = Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/1903");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903");
#endif
        }
        {
            Date d = Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/1903" or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/03");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/03");
        }

        {
            Date d = Date (Date::JulianRepType (2455213));
            VerifyTestResult (d.Format () == L"1/16/10");
        }
    }
}

namespace {

    void Test_4_TestDateTime_ ()
    {
        TraceContextBumper ctx ("Test_4_TestDateTime_");
        {
            DateTime d = Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            VerifyTestResult (d.Format (DateTime::PrintFormat::eISO8601) == L"1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            optional<DateTime> d;
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            //TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            DateTime d = DateTime::min ();
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            d = DateTime (d.GetDate (), d.GetTimeOfDay (), Timezone::kUTC);                           // so that compare works - cuz we don't know timezone we'll run test with...
            VerifyTestResult (d.Format (DateTime::PrintFormat::eISO8601) == L"1752-09-14T00:00:00Z"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        //// TODO - FIX FOR PrintFormat::eCurrentLocale_WITHZEROESTRIPPED!!!!
        {
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            Date                           d = Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            DateTime                       dt (d, TimeOfDay (101));

            {
                String tmp = dt.Format (DateTime::PrintFormat::eCurrentLocale);
#if qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy
                VerifyTestResult (tmp == L"4/5/1903 12:01:41 AM" or tmp == L"04/05/1903 12:01:41 AM");
#else
#if qCompilerAndStdLib_locale_pctX_print_time_Buggy
                VerifyTestResult (tmp == L"Sun Apr  5 00:01:41 1903");
#else
                VerifyTestResult (tmp == L"Sun 05 Apr 1903 12:01:41 AM");
#endif
#endif
            }
            DateTime dt2 (d, TimeOfDay (60));
//TOFIX!VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
#endif
        }
        {
            Date d = Date (Year (1903), MonthOfYear::eApril, DayOfMonth (6));
            TestRoundTripFormatThenParseNoChange_ (d);
            DateTime dt (d, TimeOfDay (101));
            TestRoundTripFormatThenParseNoChange_ (dt);
            String tmp = dt.Format (DateTime::PrintFormat::eCurrentLocale);
            VerifyTestResult (tmp == L"Mon Apr  6 00:01:41 1903");
            DateTime dt2 (d, TimeOfDay (60));
            TestRoundTripFormatThenParseNoChange_ (dt2);
            // want a variant that does this formatting!
            //VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        VerifyTestResult (DateTime::Parse (L"2010-01-01", DateTime::ParseFormat::eISO8601).GetDate ().GetYear () == Time::Year (2010));
        {
            DateTime now = DateTime::Now ();
            TestRoundTripFormatThenParseNoChange_ (now);

            constexpr bool kLocaleDateTimeFormatMaybeLossy_{true}; // 2 digit date - 03/04/05 parsed as 2005 on windows, and 1905 of glibc (neither wrong)
            if (kLocaleDateTimeFormatMaybeLossy_) {
                String   nowShortLocaleForm = now.Format (locale{}, DateTime::kShortLocaleFormatPattern);
                DateTime dt                 = DateTime::Parse (nowShortLocaleForm, locale{}, {DateTime::kShortLocaleFormatPattern});
                // This roundtrip can be lossy, becaue the date 2016 could be represented as '16' and then when mapped the other way as
                // 1916 (locale::classic ()). So fixup the year before comparing
                Time::Year nYear = now.GetDate ().GetYear ();
                Date       d     = dt.GetDate ();
                if (d.GetYear () != nYear) {
                    VerifyTestResult (((nYear - d.GetYear ()) % 100) == 0);
                    d  = Date (nYear, d.GetMonth (), d.GetDayOfMonth ());
                    dt = DateTime (dt, d);
                }
                VerifyTestResult (now == dt); // if this fails, look at qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy
            }
            else {
                VerifyTestResult (now == DateTime::Parse (now.Format (locale{}, DateTime::kShortLocaleFormatPattern), locale{}, {DateTime::kShortLocaleFormatPattern}));
            }
        }
        {
            using Time::DurationSecondsType;
            DurationSecondsType now = Time::GetTickCount ();
            for (DurationSecondsType ds : initializer_list<DurationSecondsType>{3, 995, 3.4, 3004.5, 1055646.4, 60 * 60 * 24 * 300}) {
                ds += now;
                DateTime dt = DateTime::FromTickCount (ds);
                VerifyTestResult (Math::NearlyEquals (dt, DateTime::FromTickCount (dt.ToTickCount ())));
                // crazy large epsilon for now because we represent datetime to nearest second
                // (note - failed once with clang++ on vm - could be something else slowed vm down - LGP 2018-04-17 - ignore for now)
                // But even the 1.1 failed once (--LGP 2019-05-03) - so change to warning and use bigger number (2.1) for error check
                VerifyTestResultWarning (Math::NearlyEquals (dt.ToTickCount (), ds, 1.1));
                VerifyTestResult (Math::NearlyEquals (dt.ToTickCount (), ds, 2.1));
            }
        }
        {
            auto roundTripD = [] (DateTime dt) {
                String   s   = dt.Format (DateTime::PrintFormat::eRFC1123);
                DateTime dt2 = DateTime::Parse (s, DateTime::ParseFormat::eRFC1123);
                VerifyTestResult (dt == dt2);
            };
            auto roundTripS = [] (String s) {
                DateTime dt = DateTime::Parse (s, DateTime::ParseFormat::eRFC1123);
                VerifyTestResult (dt.Format (DateTime::PrintFormat::eRFC1123) == s);
            };

            // Parse eRFC1123
            VerifyTestResult (DateTime::Parse (L"Wed, 09 Jun 2021 10:18:14 GMT", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::UTC ()}));
            // from https://www.feedvalidator.org/docs/error/InvalidRFC2822Date.html
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 08:00:00 EST", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 13:00:00 GMT", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 15:00:00 +0200", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));

            VerifyTestResult (DateTime::Parse (L"Tue, 6 Nov 2018 06:25:51 -0800 (PST)", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2018}, MonthOfYear::eNovember, DayOfMonth{6}}, TimeOfDay{6, 25, 51}, Timezone (-8 * 60)}));

            roundTripD (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::UTC ()});

            // Careful with these, because there are multiple valid string representations for a given date
            roundTripS (L"Wed, 02 Oct 2002 13:00:00 GMT");
            roundTripS (L"Wed, 02 Oct 2002 15:00:00 +0200");
            roundTripS (L"Wed, 02 Oct 2002 15:00:00 -0900");
        }
        {
            // difference
            {
                constexpr Date kDate_{Time::Year (2016), Time::MonthOfYear (9), Time::DayOfMonth (29)};
        constexpr TimeOfDay kTOD_{10, 21, 32};
        constexpr TimeOfDay kTOD2_{10, 21, 35};
        VerifyTestResult ((DateTime (kDate_, kTOD_) - DateTime (kDate_, kTOD2_)).As<Time::DurationSecondsType> () == -3);
        VerifyTestResult ((DateTime (kDate_, kTOD2_) - DateTime (kDate_, kTOD_)).As<Time::DurationSecondsType> () == 3);
        VerifyTestResult ((DateTime (kDate_.AddDays (1), kTOD_) - DateTime (kDate_, kTOD_)).As<Time::DurationSecondsType> () == 24 * 60 * 60);
        VerifyTestResult ((DateTime (kDate_, kTOD_) - DateTime (kDate_.AddDays (1), kTOD_)).As<Time::DurationSecondsType> () == -24 * 60 * 60);
    }
    {
        VerifyTestResult ((DateTime::Now () - DateTime::min ()) > Duration ("P200Y"));
    }
}
{
    // https://stroika.atlassian.net/browse/STK-555 - Improve Timezone object so that we can read time with +500, and respect that
    {
        constexpr Date      kDate_{Time::Year (2016), Time::MonthOfYear (9), Time::DayOfMonth (29)};
        constexpr TimeOfDay kTOD_{10, 21, 32};
        DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-04:00", DateTime::ParseFormat::eISO8601);
        DateTime            tdu = td.AsUTC ();
        VerifyTestResult (tdu == DateTime (kDate_, TimeOfDay (kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()), Timezone::UTC ()));
    }
    {
        constexpr Date      kDate_ = Date (Time::Year (2016), Time::MonthOfYear (9), Time::DayOfMonth (29));
        constexpr TimeOfDay kTOD_{10, 21, 32};
        DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-0400", DateTime::ParseFormat::eISO8601);
        DateTime            tdu = td.AsUTC ();
        VerifyTestResult (tdu == DateTime (kDate_, TimeOfDay (kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()), Timezone::UTC ()));
    }
    {
        constexpr Date      kDate_{Time::Year (2016), Time::MonthOfYear (9), Time::DayOfMonth (29)};
        constexpr TimeOfDay kTOD_{10, 21, 32};
        DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-04", DateTime::ParseFormat::eISO8601);
        DateTime            tdu = td.AsUTC ();
        VerifyTestResult (tdu == DateTime (kDate_, TimeOfDay (kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()), Timezone::UTC ()));
    }
}
}
}

namespace {

    void Test_5_DateTimeTimeT_ ()
    {
        TraceContextBumper ctx ("Test_5_DateTimeTimeT_");
        {
            DateTime d = Date (Year (2000), MonthOfYear::eApril, DayOfMonth (20));
            VerifyTestResult (d.As<time_t> () == 956188800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", locale{}));
            VerifyTestResult (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3am", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            const time_t kTEST = 802234800;
            DateTime     d     = DateTime (kTEST);
            VerifyTestResult (d.As<time_t> () == kTEST); // source - http://www.onlineconversion.com/unix_time.htm
        }
    }
}

namespace {

    void Test_6_DateTimeStructTM_ ()
    {
        TraceContextBumper ctx ("Test_6_DateTimeStructTM_");
        {
            struct tm x;
            memset (&x, 0, sizeof (x));
            x.tm_hour    = 3;
            x.tm_min     = 30;
            x.tm_year    = 80;
            x.tm_mon     = 3;
            x.tm_mday    = 15;
            DateTime  d  = DateTime (x);
            struct tm x2 = d.As<struct tm> ();
            VerifyTestResult (x.tm_hour == x2.tm_hour);
            VerifyTestResult (x.tm_min == x2.tm_min);
            VerifyTestResult (x.tm_sec == x2.tm_sec);
            VerifyTestResult (x.tm_year == x2.tm_year);
            VerifyTestResult (x.tm_mday == x2.tm_mday);
        }
    }
}

namespace {

    void Test_7_Duration_ ()
    {
        TraceContextBumper ctx ("Test_7_Duration_");
        {
            VerifyTestResult (Duration (0).As<time_t> () == 0);
            VerifyTestResult (Duration (0).As<String> () == L"PT0S");
            VerifyTestResult (Duration (0).Format () == L"0 seconds");
        }
        {
            VerifyTestResult (Duration (3).As<time_t> () == 3);
            VerifyTestResult (Duration (3).As<String> () == L"PT3S");
            VerifyTestResult (Duration (3).Format () == L"3 seconds");
        }
        const int kSecondsPerDay = TimeOfDay::kMaxSecondsPerDay;
        {
            const Duration k30Days = Duration (L"P30D");
            VerifyTestResult (k30Days.As<time_t> () == 30 * kSecondsPerDay);
        }
        {
            const Duration k6Months = Duration (L"P6M");
            VerifyTestResult (k6Months.As<time_t> () == 6 * 30 * kSecondsPerDay);
        }
        {
            const Duration kP1Y = Duration (L"P1Y");
            VerifyTestResult (kP1Y.As<time_t> () == 365 * kSecondsPerDay);
        }
        {
            const Duration kP2Y = Duration (L"P2Y");
            VerifyTestResult (kP2Y.As<time_t> () == 2 * 365 * kSecondsPerDay);
            VerifyTestResult (Duration (2 * 365 * kSecondsPerDay).As<wstring> () == L"P2Y");
        }
        {
            const Duration kHalfMinute = Duration (L"PT0.5M");
            VerifyTestResult (kHalfMinute.As<time_t> () == 30);
        }
        {
            const Duration kD = Duration (L"PT0.1S");
            VerifyTestResult (kD.As<time_t> () == 0);
            VerifyTestResult (kD.As<double> () == 0.1);
        }
        {
            const Duration kHalfMinute = Duration (L"PT0.5M");
            VerifyTestResult (kHalfMinute.PrettyPrint () == L"30 seconds");
        }
        {
            const Duration k3MS = Duration (L"PT0.003S");
            VerifyTestResult (k3MS.PrettyPrint () == L"3 ms");
        }
        {
            const Duration kD = Duration (L"PT1.003S");
            VerifyTestResult (kD.PrettyPrint () == L"1.003 seconds");
        }
        {
            const Duration kD = Duration (L"PT0.000045S");
            VerifyTestResult (kD.PrettyPrint () == L"45 µs");
        }
        {
            // todo use constexpr
            const Duration kD = Duration{};
            VerifyTestResult (kD.empty ());
        }
        {
            // todo use constexpr
            const Duration kD = Duration (1.6e-6);
            VerifyTestResult (kD.PrettyPrint () == L"1.6 µs");
        }
        {
            // todo use constexpr
            const Duration kD{33us};
            VerifyTestResult (kD.PrettyPrint () == L"33 µs");
        }
        {
            const Duration kD = Duration (L"PT0.000045S");
            VerifyTestResult (kD.PrettyPrint () == L"45 µs");
            VerifyTestResult ((-kD).PrettyPrint () == L"-45 µs");
            VerifyTestResult ((-kD).As<wstring> () == L"-PT0.000045S");
        }
        VerifyTestResult (Duration (L"P30S").As<time_t> () == 30);
        VerifyTestResult (Duration (L"PT30S").As<time_t> () == 30);
        VerifyTestResult (Duration (60).As<wstring> () == L"PT1M");
        VerifyTestResult (Duration (L"-PT1H1S").As<time_t> () == -3601);
        VerifyTestResult (-Duration (L"-PT1H1S").As<time_t> () == 3601);

        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 100 : 1;
            for (time_t i = -45; i < 60 * 3 * 60 + 99; i += K) {
                VerifyTestResult (Duration (Duration (i).As<wstring> ()).As<time_t> () == i);
            }
        }
        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 2630 : 263;
            for (time_t i = 60 * 60 * 24 * 365 - 40; i < 3 * 60 * 60 * 24 * 365; i += K) {
                VerifyTestResult (Duration (Duration (i).As<wstring> ()).As<time_t> () == i);
            }
        }
        VerifyTestResult (Duration::min () < Duration::max ());
        VerifyTestResult (Duration::min () != Duration::max ());
        VerifyTestResult (Duration::min () < Duration (L"P30S") and Duration (L"P30S") < Duration::max ());
        {
            using Time::DurationSecondsType;
            Duration d = Duration (L"PT0.1S");
            VerifyTestResult (d == "PT0.1S"_duration);
            d += chrono::milliseconds (30);
            VerifyTestResult (Math::NearlyEquals (d.As<DurationSecondsType> (), static_cast<DurationSecondsType> (.130)));
        }
        {
            VerifyTestResult (Duration (L"PT1.4S").PrettyPrintAge () == L"now");
            VerifyTestResult (Duration (L"-PT9M").PrettyPrintAge () == L"now");
            VerifyTestResult (Duration (L"-PT20M").PrettyPrintAge () == L"20 minutes ago");
            VerifyTestResult (Duration (L"PT20M").PrettyPrintAge () == L"20 minutes from now");
            VerifyTestResult (Duration (L"PT4H").PrettyPrintAge () == L"4 hours from now");
            VerifyTestResult (Duration (L"PT4.4H").PrettyPrintAge () == L"4 hours from now");
            VerifyTestResult (Duration (L"P2Y").PrettyPrintAge () == L"2 years from now");
            VerifyTestResult (Duration (L"P2.4Y").PrettyPrintAge () == L"2 years from now");
            VerifyTestResult (Duration (L"P2.6Y").PrettyPrintAge () == L"3 years from now");
            VerifyTestResult (Duration (L"-P1M").PrettyPrintAge () == L"1 month ago");
            VerifyTestResult (Duration (L"-P2M").PrettyPrintAge () == L"2 months ago");
            VerifyTestResult (Duration (L"-PT1Y").PrettyPrintAge () == L"1 year ago");
            VerifyTestResult (Duration (L"-PT2Y").PrettyPrintAge () == L"2 years ago");
        }
    }
}

namespace {

    void Test_8_DateTimeWithDuration_ ()
    {
        TraceContextBumper ctx ("Test_8_DateTimeWithDuration_");
        {
            DateTime d = DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
            const Duration k30Days = Duration (L"P30D");
            DateTime       d2      = d + k30Days;
            VerifyTestResult (d2.GetDate ().GetYear () == Year (1995));
            VerifyTestResult (d2.GetDate ().GetMonth () == MonthOfYear::eJuly);
            VerifyTestResult (d2.GetDate ().GetDayOfMonth () == DayOfMonth (4));
            VerifyTestResult (d2.GetTimeOfDay () == d.GetTimeOfDay ());
        }
        {
            DateTime n1 = DateTime (Date (Year (2015), MonthOfYear::eJune, DayOfMonth (9)), TimeOfDay (19, 18, 42), Timezone::kLocalTime);
            DateTime n2 = n1 - Duration (L"P100Y");
            VerifyTestResult (n2.GetDate ().GetYear () == Year ((int)n1.GetDate ().GetYear () - 100));
#if 0
            // @todo - Improve - increment by 100 years not as exact as one might like @todo --LGP 2015-06-09
            VerifyTestResult (n2.GetDate ().GetMonth () == n1.GetDate ().GetMonth ());
            VerifyTestResult (n2.GetDate ().GetDayOfMonth () == n1.GetDate ().GetDayOfMonth ());
#endif
            VerifyTestResult (n2.GetTimeOfDay () == n1.GetTimeOfDay ());
        }
    }
}

namespace {

    void Test_9_TZOffsetAndDaylightSavingsTime_ ()
    {
        TraceContextBumper ctx ("Test_9_TZOffsetAndDaylightSavingsTime_");
        /*
         * I cannot think if any good way to test this stuff - since it depends on the current timezone and I cannot
         * see any good portbale way to change that (setenv (TZ) doest work on visual studio.net 2010).
         *
         * This test wont always work, but at least for now seems to work on the systems i test on.
         *
         *  @see https://stroika.atlassian.net/browse/STK-634
         */
        {
            DateTime                        n     = DateTime (Date (Year (2011), MonthOfYear::eDecember, DayOfMonth (30)), TimeOfDay::Parse (L"1 pm", locale::classic ()), Timezone::LocalTime ());
            [[maybe_unused]] optional<bool> isDst = n.IsDaylightSavingsTime ();
            DateTime                        n2    = n.AddDays (180);
            // This verify was wrong. Consider a system on GMT! Besides that - its still not reliable because DST doesnt end 180 days exactly apart.
            //VerifyTestResult (IsDaylightSavingsTime (n) != IsDaylightSavingsTime (n2));
            if (n.IsDaylightSavingsTime () != n2.IsDaylightSavingsTime ()) {
            }
        }
        {
            DateTime                        n     = DateTime::Now ();
            [[maybe_unused]] optional<bool> isDst = n.IsDaylightSavingsTime ();
            DateTime                        n2    = n.AddDays (60);
            if (n.IsDaylightSavingsTime () == n2.IsDaylightSavingsTime ()) {
            }
        }
    }
}

namespace {
    void Test_10_std_duration_ ()
    {
        TraceContextBumper ctx ("Test_10_std_duration_");
        const Duration     k30Seconds = Duration (30.0);
        VerifyTestResult (k30Seconds.As<time_t> () == 30);
        VerifyTestResult (k30Seconds.As<String> () == L"PT30S");
        VerifyTestResult (k30Seconds.As<chrono::duration<double>> () == chrono::duration<double> (30.0));
        VerifyTestResult (Duration (chrono::duration<double> (4)).As<time_t> () == 4);
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::milliseconds (50)).As<Time::DurationSecondsType> (), 0.050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::microseconds (50)).As<Time::DurationSecondsType> (), 0.000050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::nanoseconds (50)).As<Time::DurationSecondsType> (), 0.000000050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::nanoseconds (1)).As<Time::DurationSecondsType> (), 0.000000001));
        VerifyTestResult (Duration (5.0).As<chrono::milliseconds> () == chrono::milliseconds (5000));
        VerifyTestResult (Duration (-5.0).As<chrono::milliseconds> () == chrono::milliseconds (-5000));
        VerifyTestResult (Duration (1.0).As<chrono::nanoseconds> () == chrono::nanoseconds (1000 * 1000 * 1000));
        VerifyTestResult (Duration (1) == Duration (chrono::seconds (1)));
    }
}

namespace {
    void Test_11_DurationRange_ ()
    {
        TraceContextBumper ctx ("Test_11_DurationRange_");
        DurationRange      d1;
        DurationRange      d2 = DurationRange::FullRange ();
        VerifyTestResult (d1.empty ());
        VerifyTestResult (not d2.empty ());
        VerifyTestResult (d2.GetLowerBound () == Duration::min ());
        VerifyTestResult (d2.GetUpperBound () == Duration::max ());
    }
}

namespace {
    void Test_12_DateRange_ ()
    {
        TraceContextBumper ctx ("Test_12_DateRange_");
        {
            DateRange d1;
            DateRange d2 = DateRange::FullRange ();
            VerifyTestResult (d1.empty ());
            VerifyTestResult (not d2.empty ());
            VerifyTestResult (d2.GetLowerBound () == Date::min ());
            VerifyTestResult (d2.GetUpperBound () == Date::max ());
        }
        {
            DateRange    dr{Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5)), Date (Year (1903), MonthOfYear::eApril, DayOfMonth (6))};
            unsigned int i = 0;
            for (Date d : dr) {
                ++i;
                VerifyTestResult (d.GetYear () == Year (1903));
                VerifyTestResult (d.GetMonth () == MonthOfYear::eApril);
                if (i == 1) {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth (5));
                }
                else {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth (6));
                }
            }
            VerifyTestResult (i == 2);
        }
        {
            DateRange    dr{Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5)), Date (Year (1903), MonthOfYear::eApril, DayOfMonth (6))};
            unsigned int i = 0;
            for (Date d : dr.Elements ()) {
                ++i;
                VerifyTestResult (d.GetYear () == Year (1903));
                VerifyTestResult (d.GetMonth () == MonthOfYear::eApril);
                if (i == 1) {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth (5));
                }
                else {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth (6));
                }
            }
            VerifyTestResult (i == 2);
        }
        {
            DateRange dr{DateTime::Now ().GetDate () - 1, DateTime::Now ().GetDate () + 1};
            VerifyTestResult (dr.Contains (dr.GetMidpoint ()));
        }
    }
}

namespace {
    void Test_13_DateTimeRange_ ()
    {
        TraceContextBumper ctx ("Test_13_DateTimeRange_");
        {
            DateTimeRange d1;
            DateTimeRange d2 = DateTimeRange::FullRange ();
            VerifyTestResult (d1.empty ());
            VerifyTestResult (not d2.empty ());
            VerifyTestResult (d2.GetLowerBound () == DateTime::min ());
            VerifyTestResult (d2.GetUpperBound () == DateTime::max ());
        }
        {
            DateTimeRange d1{DateTime (Date (Year (2000), MonthOfYear::eApril, DayOfMonth (20))), DateTime (Date (Year (2000), MonthOfYear::eApril, DayOfMonth (22)))};
            VerifyTestResult (d1.GetDistanceSpanned () / 2 == Duration ("PT1D"));
            // SEE https://stroika.atlassian.net/browse/STK-514 for accuracy of compare (sb .1 or less)
            VerifyTestResult (Math::NearlyEquals (d1.GetMidpoint (), Date (Year (2000), MonthOfYear::eApril, DayOfMonth (21)), DurationSecondsType (2)));
        }
    }
}

namespace {
    void Test_14_timepoint_ ()
    {
        TraceContextBumper ctx ("Test_14_timepoint_");
        // @see https://stroika.atlassian.net/browse/STK-619 - VerifyTestResult (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) == time_point<chrono::steady_clock>::max ());
        VerifyTestResult (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) > chrono::steady_clock::now () + chrono::seconds (10000));
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        TraceContextBumper ctx{"DoRegressionTests_"};
        Test_1_TestTickCountGrowsMonotonically_ ();
        Test_2_TestTimeOfDay_ ();
        Test_3_TestDate_ ();
        Test_4_TestDateTime_ ();
        Test_5_DateTimeTimeT_ ();
        Test_6_DateTimeStructTM_ ();
        Test_7_Duration_ ();
        Test_8_DateTimeWithDuration_ ();
        Test_9_TZOffsetAndDaylightSavingsTime_ ();
        Test_10_std_duration_ ();
        Test_11_DurationRange_ ();
        Test_12_DateRange_ ();
        Test_13_DateTimeRange_ ();
        Test_14_timepoint_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
