/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/DateTimeRange.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;

using Stroika::Foundation::Debug::TraceContextBumper;

namespace {
    void Test_0_AssumptionsAboutUnderlyingTimeLocaleLibrary_ ()
    {
        TraceContextBumper ctx{"Test_0_AssumptionsAboutUnderlyingTimeLocaleLibrary_"};
        auto               testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ = [] (int tm_Year, int tm_Mon, int tm_mDay, const locale& l) {
            DbgTrace (L"year=%d, tm_Mon=%d, tm_mDay=%d", tm_Year, tm_Mon, tm_mDay);
            ::tm origDateTM{};
            origDateTM.tm_year = tm_Year;
            origDateTM.tm_mon  = tm_Mon;
            origDateTM.tm_mday = tm_mDay;

            //const wchar_t kPattern[] = L"%x";
            const wchar_t kPattern[] = L"%Y-%m-%d";

            wstring tmpStringRep;
            {
                const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
                tm                       when  = origDateTM;
                wostringstream           oss;
                tmput.put (oss, oss, ' ', &when, begin (kPattern), begin (kPattern) + ::wcslen (kPattern));
                tmpStringRep = oss.str ();
            }
            DbgTrace (L"tmpStringRep=%s", tmpStringRep.c_str ());
            tm resultTM{};
            {
                const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
                ios::iostate                 state = ios::goodbit;
                wistringstream               iss (tmpStringRep);
                istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
                istreambuf_iterator<wchar_t> itend;         // end-of-stream
                tmget.get (itbegin, itend, iss, state, &resultTM, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
            }
            DbgTrace (L"resultTM.tm_year=%d", resultTM.tm_year);
            Assert (origDateTM.tm_year == resultTM.tm_year);
            Assert (origDateTM.tm_mon == resultTM.tm_mon);
            Assert (origDateTM.tm_mday == resultTM.tm_mday);
        };
        // this test doesnt make much sense - revisit!!! --LGP 2021-03-05
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (12, 5, 1, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (-148, 10, 19, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (121, 2, 4, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (105, 5, 1, locale::classic ());

        auto getPCTMRequiresLeadingZeroBug = [] () {
            std::locale                  l     = locale::classic ();
            const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
            ios::iostate                 state = ios::goodbit;
            wistringstream               iss (L"11/1/2002");
            istreambuf_iterator<wchar_t> itbegin (iss); // beginning of iss
            istreambuf_iterator<wchar_t> itend;         // end-of-stream
            tm                           resultTM{};
            [[maybe_unused]] auto        i = tmget.get (itbegin, itend, iss, state, &resultTM, Date::kMonthDayYearFormat.data (), Date::kMonthDayYearFormat.data () + Date::kMonthDayYearFormat.length ());
#if qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy
            VerifyTestResult ((state & ios::badbit) or (state & ios::failbit));
#else
            VerifyTestResult (not((state & ios::badbit) or (state & ios::failbit)));
#endif
        };
        getPCTMRequiresLeadingZeroBug ();

        auto std_get_time_pctxBuggyTest = [] () {
            //wstring wRep = L"3pm";   // this works
            wstring                      wRep = L"3:00";
            locale                       l    = locale::classic ();
            wistringstream               iss (wRep);
            const time_get<wchar_t>&     tmget    = use_facet<time_get<wchar_t>> (l);
            ios::iostate                 errState = ios::goodbit;
            tm                           when{};
            wstring                      formatPattern = L"%X"; // or %EX, or %T all fail
            istreambuf_iterator<wchar_t> itbegin (iss);         // beginning of iss
            istreambuf_iterator<wchar_t> itend;                 // end-of-stream

            istreambuf_iterator<wchar_t> i;
            // In Debug build on Windows, this generates Assertion error inside stdc++ runtime library
            // first noticed broken in vs2k17 (qCompilerAndStdLib_std_get_time_pctx_Buggy)
            i = tmget.get (itbegin, itend, iss, errState, &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
        };
        // get assertion istreambuf_iterator is not dereferenceable failure if we enable this on VS2k
#if !qCompilerAndStdLib_std_get_time_pctx_Buggy
        std_get_time_pctxBuggyTest ();
#endif

        auto tmget_dot_get_locale_date_order_buggy_test_ = [] () {
            try {
                std::locale                  l{"en_US.utf8"}; // originally tested with locale {} - which defaulted to C-locale
                const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
                ios::iostate                 state = ios::goodbit;
                wistringstream               iss{L"03/07/21 16:18:47"};
                const tm                     kTargetTM_MDY_{47, 18, 15, 7, 2, 21};
                const tm                     kTargetTM_DMY_{47, 18, 15, 3, 6, 21};
                istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
                istreambuf_iterator<wchar_t> itend;        // end-of-stream
                tm                           resultTM{};
                // GCC reports no_order (not technically a bug, but wierd) - but appears to parse the dates properly anyhow
                // Visual Studio gets the date_order() correct but still parsed in wrong order (qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy)
                VerifyTestResultWarning (tmget.date_order () == time_base::mdy or tmget.date_order () == time_base::no_order);
                [[maybe_unused]] auto i = tmget.get (itbegin, itend, iss, state, &resultTM, DateTime::kShortLocaleFormatPattern.data (), DateTime::kShortLocaleFormatPattern.data () + DateTime::kShortLocaleFormatPattern.length ());
                VerifyTestResult (not((state & ios::badbit) or (state & ios::failbit)));
                VerifyTestResult (resultTM.tm_sec == kTargetTM_MDY_.tm_sec);
                VerifyTestResult (resultTM.tm_min == kTargetTM_MDY_.tm_min);
                VerifyTestResult (resultTM.tm_mday == kTargetTM_MDY_.tm_mday);
                VerifyTestResult (resultTM.tm_year == kTargetTM_MDY_.tm_year);
                if (tmget.date_order () == time_base::mdy or tmget.date_order () == time_base::no_order) {
#if qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy
                    VerifyTestResult (resultTM.tm_mday == kTargetTM_MDY_.tm_mday);
                    VerifyTestResult (resultTM.tm_mon == kTargetTM_MDY_.tm_mon);
#else
                    VerifyTestResult (resultTM.tm_mday == kTargetTM_MDY_.tm_mday);
                    VerifyTestResult (resultTM.tm_mon == kTargetTM_MDY_.tm_mon);
#endif
                }
                else if (tmget.date_order () == time_base::dmy) {
#if qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy
                    VerifyTestResult (resultTM.tm_mday == kTargetTM_DMY_.tm_mday);
                    VerifyTestResult (resultTM.tm_mon == kTargetTM_DMY_.tm_mon);
#else
                    VerifyTestResult (resultTM.tm_mday == kTargetTM_DMY_.tm_mday);
                    VerifyTestResult (resultTM.tm_mon == kTargetTM_DMY_.tm_mon);
#endif
                }
            }
            catch (...) {
                Stroika::TestHarness::WarnTestIssue (L"tmget_dot_get_locale_date_order_buggy_test_ skipped - usually because of missing locale");
            }
        };

        tmget_dot_get_locale_date_order_buggy_test_ ();
    }
}

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
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, Configuration::FindNamedLocale (L"en", L"us"));

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
        TraceContextBumper  ctx{"Test_1_TestTickCountGrowsMonotonically_"};
        DurationSecondsType start = Time::GetTickCount ();
        Execution::Sleep (100ms);
        VerifyTestResult (start <= Time::GetTickCount ());
    }
}

namespace {

    void Test_2_TestTimeOfDay_ ()
    {
        TraceContextBumper ctx{"Test_2_TestTimeOfDay_"};
        {
            optional<TimeOfDay> t;
            VerifyTestResult (not t.has_value ());
            TimeOfDay t2{2};
            VerifyTestResult (t < t2);
            VerifyTestResult (not t2.Format (TimeOfDay::PrintFormat::eCurrentLocale).empty ());
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
            VerifyTestResult (t2 == TimeOfDay::kMax);
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
            VerifyTestResult (TimeOfDay::Parse (L"3pm").GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am").GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00").GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00").GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
#if qCompilerAndStdLib_locale_pctX_print_time_Buggy
            // NOTE - these values are wrong, but since using locale code, not easy to fix/workaround - but to note XCode locale stuff still
            // somewhat broken...
            VerifyTestResult (TimeOfDay{101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:41");
            VerifyTestResult (TimeOfDay{60}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"0:01");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41");
            VerifyTestResult (TimeOfDay{60 * 60 + 60}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01");
#else
            VerifyTestResult (TimeOfDay{101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"12:01:41 AM");
            VerifyTestResult (TimeOfDay{60}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"12:01 AM");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"1:01:41 AM" or TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41 AM");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41 AM");
            VerifyTestResult (TimeOfDay{60 * 60 + 60}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01 AM");
#endif
        }
        {
            VerifyTestResult (TimeOfDay{101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:41");
            VerifyTestResult (TimeOfDay{60}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"00:01:00");
            VerifyTestResult (TimeOfDay{60}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"0:01");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41");
            VerifyTestResult (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41");
            VerifyTestResult (TimeOfDay{60 * 60 + 60}.Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:00");
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
        VerifyTestResult (Date::Parse (d.Format (Date::kISO8601Format), Date::kISO8601Format) == d);
    }

    void Test_3_TestDate_ ()
    {
        TraceContextBumper ctx{"Test_3_TestDate_"};
        {
            Date d (Year{1903}, MonthOfYear::eApril, DayOfMonth{4});
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (Date::kISO8601Format) == L"1903-04-04");
            VERIFY_ROUNDTRIP_XML_ (d);
            d = d.AddDays (4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::kISO8601Format) == L"1903-04-08");
            d = d.AddDays (-4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::kISO8601Format) == L"1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        try {
            Date d = Date::Parse (L"09/14/1752", Date::kMonthDayYearFormat);
            VerifyTestResult (d == Date::kMin);
            VerifyTestResult (d.Format (Date::kISO8601Format) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
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
            Date d = Date::kMin;
            VerifyTestResult (d < DateTime::Now ().GetDate ());
            VerifyTestResult (not(DateTime::Now ().GetDate () < d));
            VerifyTestResult (d.Format (Date::kISO8601Format) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            VerifyTestResult (Date::Parse (L"11/3/2001", Date::kMonthDayYearFormat) == Date (Year{2001}, Time::MonthOfYear::eNovember, DayOfMonth{3}));
            VerifyTestResult (Date::Parse (L"11/3/2001", Date::kMonthDayYearFormat).Format (Date::kMonthDayYearFormat) == L"11/03/2001");
        }
        {
            VerifyTestResult (Date::kMin < Date::kMax);
            VerifyTestResult (Date::kMin <= Date::kMax);
            VerifyTestResult (not(Date::kMin > Date::kMax));
            VerifyTestResult (not(Date::kMin >= Date::kMax));
            TestRoundTripFormatThenParseNoChange_ (Date::kMin);
            TestRoundTripFormatThenParseNoChange_ (Date::kMax);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            Date                           d = Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{5}};
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (locale{}) == L"4/5/1903" or d.Format (locale{}) == L"04/05/1903");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903");
        }
        {
            Date d = Date (Year{1903}, MonthOfYear::eApril, DayOfMonth{5});
            VerifyTestResult (d.Format (locale{}) == L"4/5/1903" or d.Format (locale{}) == L"04/05/1903" or d.Format (locale{}) == L"04/05/03");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/03");
        }
        {
            Date d = Date{Date::JulianRepType{2455213}};
            VerifyTestResult (d.Format () == L"1/16/10");
        }
    }
}

namespace {

    void Test_4_TestDateTime_ ()
    {
        TraceContextBumper ctx{"Test_4_TestDateTime_"};
        {
            DateTime d = Date (Year{1903}, MonthOfYear::eApril, DayOfMonth{4});
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
            DbgTrace (L"DateTime::Now()=%s", Characters::ToString (DateTime::Now ()).c_str ());
            DbgTrace (L"DateTime::Now().AsUTC ()=%s", Characters::ToString (DateTime::Now ().AsUTC ()).c_str ());
            DbgTrace (L"DateTime::Now().AsLocalTime ()=%s", Characters::ToString (DateTime::Now ().AsLocalTime ()).c_str ());
            DbgTrace (L"Timezone::kLocalTime.GetBiasFromUTC (fDate_, TimeOfDay{0})=%d", Timezone::kLocalTime.GetBiasFromUTC (DateTime::Now ().GetDate (), TimeOfDay{0}));
            {
                DateTime regTest{time_t (1598992961)};
                VerifyTestResult (regTest.GetTimezone () == Timezone::kUTC);
                VerifyTestResult ((regTest.GetDate () == Date{Year{2020}, MonthOfYear::eSeptember, DayOfMonth{1}}));
                VerifyTestResult ((regTest.GetTimeOfDay () == TimeOfDay{20, 42, 41}));
                if (Timezone::kLocalTime.GetBiasFromUTC (regTest.GetDate (), *regTest.GetTimeOfDay ()) == -4 * 60 * 60) {
                    DbgTrace ("Eastern US timezone");
                    VerifyTestResult ((regTest.AsLocalTime () == DateTime{Date{Year{2020}, MonthOfYear::eSeptember, DayOfMonth{1}}, TimeOfDay{20 - 4, 42, 41}, Timezone::kLocalTime}));
                }
                else {
                    DbgTrace ("other timezone: offset=%d", Timezone::kLocalTime.GetBiasFromUTC (regTest.GetDate (), *regTest.GetTimeOfDay ()));
                }
            }
        }
        {
            DateTime d = DateTime::kMin;
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            d = DateTime{d.GetDate (), d.GetTimeOfDay (), Timezone::kUTC};                            // so that compare works - cuz we don't know timezone we'll run test with...
            VerifyTestResult (d.Format (DateTime::PrintFormat::eISO8601) == L"1752-09-14T00:00:00Z"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        //// TODO - FIX FOR PrintFormat::eCurrentLocale_WITHZEROESTRIPPED!!!!
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            Date                           d = Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{5}};
            DateTime                       dt{d, TimeOfDay{101}};

            {
                String tmp = dt.Format (locale{});
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
            DateTime dt2{d, TimeOfDay{60}};
            //TOFIX!VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        {
            Date d = Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{6}};
            TestRoundTripFormatThenParseNoChange_ (d);
            DateTime dt{d, TimeOfDay (101)};
            TestRoundTripFormatThenParseNoChange_ (dt);
            String tmp = dt.Format (locale{});
            VerifyTestResult (tmp == L"Mon Apr  6 00:01:41 1903");
            DateTime dt2{d, TimeOfDay{60}};
            TestRoundTripFormatThenParseNoChange_ (dt2);
            // want a variant that does this formatting!
            //VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        VerifyTestResult (DateTime::Parse (L"2010-01-01", DateTime::ParseFormat::eISO8601).GetDate ().GetYear () == Time::Year{2010});
        {
            DateTime now = DateTime::Now ();
            TestRoundTripFormatThenParseNoChange_ (now);

            constexpr bool kLocaleDateTimeFormatMaybeLossy_{true}; // 2 digit date - 03/04/05 parsed as 2005 on windows, and 1905 of glibc (neither wrong)
            if (kLocaleDateTimeFormatMaybeLossy_) {
                String   nowShortLocaleForm = now.Format (locale{}, DateTime::kShortLocaleFormatPattern);
                DateTime dt                 = DateTime::Parse (nowShortLocaleForm, DateTime::kShortLocaleFormatPattern);
                // This roundtrip can be lossy, becaue the date 2016 could be represented as '16' and then when mapped the other way as
                // 1916 (locale::classic ()). So fixup the year before comparing
                Time::Year nYear = now.GetDate ().GetYear ();
                Date       d     = dt.GetDate ();
                if (d.GetYear () != nYear) {
                    VerifyTestResult (((nYear - d.GetYear ()) % 100) == 0);
                    d  = Date{nYear, d.GetMonth (), d.GetDayOfMonth ()};
                    dt = DateTime{dt, d};
                }
                VerifyTestResult (now == dt); // if this fails, look at qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy
            }
            else {
                VerifyTestResult (now == DateTime::Parse (now.Format (DateTime::kShortLocaleFormatPattern), DateTime::kShortLocaleFormatPattern));
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
            VerifyTestResult (DateTime::Parse (L"Wed, 09 Jun 2021 10:18:14 GMT", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC}));
            // from https://www.feedvalidator.org/docs/error/InvalidRFC2822Date.html
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 08:00:00 EST", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 13:00:00 GMT", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            VerifyTestResult (DateTime::Parse (L"Wed, 02 Oct 2002 15:00:00 +0200", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2002}, MonthOfYear::eOctober, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));

            VerifyTestResult (DateTime::Parse (L"Tue, 6 Nov 2018 06:25:51 -0800 (PST)", DateTime::ParseFormat::eRFC1123) == (DateTime{Date{Time::Year{2018}, MonthOfYear::eNovember, DayOfMonth{6}}, TimeOfDay{6, 25, 51}, Timezone (-8 * 60)}));

            roundTripD (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC});

            // Careful with these, because there are multiple valid string representations for a given date
            roundTripS (L"Wed, 02 Oct 2002 13:00:00 GMT");
            roundTripS (L"Wed, 02 Oct 2002 15:00:00 +0200");
            roundTripS (L"Wed, 02 Oct 2002 15:00:00 -0900");
        }
        // clang-format off
        {
            // difference
            {
                constexpr Date kDate_{Time::Year {2016}, Time::MonthOfYear{9}, Time::DayOfMonth{29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                constexpr TimeOfDay kTOD2_{10, 21, 35};
                VerifyTestResult ((DateTime {kDate_, kTOD_} - DateTime {kDate_, kTOD2_}).As<Time::DurationSecondsType> () == -3);
                VerifyTestResult ((DateTime {kDate_, kTOD2_} - DateTime {kDate_, kTOD_}).As<Time::DurationSecondsType> () == 3);
                VerifyTestResult ((DateTime {kDate_.AddDays (1), kTOD_} - DateTime {kDate_, kTOD_}).As<Time::DurationSecondsType> () == 24 * 60 * 60);
                VerifyTestResult ((DateTime {kDate_, kTOD_} - DateTime {kDate_.AddDays (1), kTOD_}).As<Time::DurationSecondsType> () == -24 * 60 * 60);
            }
            {
                VerifyTestResult ((DateTime::Now () - DateTime::kMin) > "P200Y"_duration);
            }
        }
        {
            // https://stroika.atlassian.net/browse/STK-555 - Improve Timezone object so that we can read time with +500, and respect that
            {
                constexpr Date      kDate_{Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth{29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-04:00", DateTime::ParseFormat::eISO8601);
                DateTime            tdu = td.AsUTC ();
                VerifyTestResult ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
            {
                constexpr Date      kDate_ = Date {Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth{29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-0400", DateTime::ParseFormat::eISO8601);
                DateTime            tdu = td.AsUTC ();
                VerifyTestResult ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
            {
                constexpr Date      kDate_{Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth {29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse (L"2016-09-29T10:21:32-04", DateTime::ParseFormat::eISO8601);
                DateTime            tdu = td.AsUTC ();
                VerifyTestResult ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
        }
    }
}
// clang-format on

namespace {

    void Test_5_DateTimeTimeT_ ()
    {
        TraceContextBumper ctx{"Test_5_DateTimeTimeT_"};
        {
            DateTime d = Date{Year{2000}, MonthOfYear::eApril, DayOfMonth{20}};
            VerifyTestResult (d.As<time_t> () == 956188800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, MonthOfYear::eJune, DayOfMonth{4}}, TimeOfDay::Parse (L"3pm", locale{})};
            VerifyTestResult (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, MonthOfYear::eJune, DayOfMonth{4}}, TimeOfDay::Parse (L"3pm")};
            VerifyTestResult (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, MonthOfYear::eJune, DayOfMonth{4}}, TimeOfDay::Parse (L"3am")};
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, MonthOfYear::eJune, DayOfMonth{4}}, TimeOfDay::Parse (L"3:00")};
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            const time_t kTEST = 802234800;
            DateTime     d     = DateTime{kTEST};
            VerifyTestResult (d.As<time_t> () == kTEST); // source - http://www.onlineconversion.com/unix_time.htm
        }
    }
}

namespace {

    void Test_6_DateTimeStructTM_ ()
    {
        TraceContextBumper ctx{"Test_6_DateTimeStructTM_"};
        {
            struct tm x;
            memset (&x, 0, sizeof (x));
            x.tm_hour    = 3;
            x.tm_min     = 30;
            x.tm_year    = 80;
            x.tm_mon     = 3;
            x.tm_mday    = 15;
            DateTime  d  = DateTime{x};
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
        TraceContextBumper ctx{"Test_7_Duration_"};
        {
            VerifyTestResult (Duration{0}.As<time_t> () == 0);
            VerifyTestResult (Duration{0}.As<String> () == L"PT0S");
            VerifyTestResult (Duration{0}.Format () == L"0 seconds");
        }
        {
            VerifyTestResult (Duration{3}.As<time_t> () == 3);
            VerifyTestResult (Duration{3}.As<String> () == L"PT3S");
            VerifyTestResult (Duration{3}.Format () == L"3 seconds");
        }
        const int kSecondsPerDay = TimeOfDay::kMaxSecondsPerDay;
        {
            const Duration k30Days = Duration{L"P30D"};
            VerifyTestResult (k30Days.As<time_t> () == 30 * kSecondsPerDay);
        }
        {
            const Duration k6Months = Duration{L"P6M"};
            VerifyTestResult (k6Months.As<time_t> () == 6 * 30 * kSecondsPerDay);
        }
        {
            const Duration kP1Y = Duration{L"P1Y"};
            VerifyTestResult (kP1Y.As<time_t> () == 365 * kSecondsPerDay);
        }
        {
            const Duration kP2Y = Duration{L"P2Y"};
            VerifyTestResult (kP2Y.As<time_t> () == 2 * 365 * kSecondsPerDay);
            VerifyTestResult (Duration (2 * 365 * kSecondsPerDay).As<wstring> () == L"P2Y");
        }
        {
            const Duration kHalfMinute = Duration{L"PT0.5M"};
            VerifyTestResult (kHalfMinute.As<time_t> () == 30);
        }
        {
            const Duration kD = Duration{L"PT0.1S"};
            VerifyTestResult (kD.As<time_t> () == 0);
            VerifyTestResult (kD.As<double> () == 0.1);
        }
        {
            const Duration kHalfMinute = Duration{L"PT0.5M"};
            VerifyTestResult (kHalfMinute.PrettyPrint () == L"30 seconds");
        }
        {
            const Duration k3MS = Duration{L"PT0.003S"};
            VerifyTestResult (k3MS.PrettyPrint () == L"3 ms");
        }
        {
            const Duration kD = Duration{L"PT1.003S"};
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
            const Duration kD = Duration{1.6e-6};
            VerifyTestResult (kD.PrettyPrint () == L"1.6 µs");
        }
        {
            // todo use constexpr
            const Duration kD{33us};
            VerifyTestResult (kD.PrettyPrint () == L"33 µs");
        }
        {
            const Duration kD = Duration{L"PT0.000045S"};
            VerifyTestResult (kD.PrettyPrint () == L"45 µs");
            VerifyTestResult ((-kD).PrettyPrint () == L"-45 µs");
            VerifyTestResult ((-kD).As<wstring> () == L"-PT0.000045S");
        }
        VerifyTestResult (Duration{L"P30S"}.As<time_t> () == 30);
        VerifyTestResult (Duration{L"PT30S"}.As<time_t> () == 30);
        VerifyTestResult (Duration{60}.As<wstring> () == L"PT1M");
        VerifyTestResult (Duration{L"-PT1H1S"}.As<time_t> () == -3601);
        VerifyTestResult (-Duration{L"-PT1H1S"}.As<time_t> () == 3601);

        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 100 : 1;
            for (time_t i = -45; i < 60 * 3 * 60 + 99; i += K) {
                VerifyTestResult (Duration{Duration{i}.As<wstring> ()}.As<time_t> () == i);
            }
        }
        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 2630 : 263;
            for (time_t i = 60 * 60 * 24 * 365 - 40; i < 3 * 60 * 60 * 24 * 365; i += K) {
                VerifyTestResult (Duration{Duration (i).As<wstring> ()}.As<time_t> () == i);
            }
        }
        VerifyTestResult (Duration::min () < Duration::max ());
        VerifyTestResult (Duration::min () != Duration::max ());
        VerifyTestResult (Duration::min () < Duration{L"P30S"} and Duration{L"P30S"} < Duration::max ());
        {
            using Time::DurationSecondsType;
            Duration d = Duration{L"PT0.1S"};
            VerifyTestResult (d == "PT0.1S"_duration);
            d += chrono::milliseconds{30};
            VerifyTestResult (Math::NearlyEquals (d.As<DurationSecondsType> (), static_cast<DurationSecondsType> (.130)));
        }
        {
            VerifyTestResult (Duration{L"PT1.4S"}.PrettyPrintAge () == L"now");
            VerifyTestResult (Duration{L"-PT9M"}.PrettyPrintAge () == L"now");
            VerifyTestResult (Duration{L"-PT20M"}.PrettyPrintAge () == L"20 minutes ago");
            VerifyTestResult (Duration{L"PT20M"}.PrettyPrintAge () == L"20 minutes from now");
            VerifyTestResult (Duration{L"PT4H"}.PrettyPrintAge () == L"4 hours from now");
            VerifyTestResult (Duration{L"PT4.4H"}.PrettyPrintAge () == L"4 hours from now");
            VerifyTestResult (Duration{L"P2Y"}.PrettyPrintAge () == L"2 years from now");
            VerifyTestResult (Duration{L"P2.4Y"}.PrettyPrintAge () == L"2 years from now");
            VerifyTestResult (Duration{L"P2.6Y"}.PrettyPrintAge () == L"3 years from now");
            VerifyTestResult (Duration{L"-P1M"}.PrettyPrintAge () == L"1 month ago");
            VerifyTestResult (Duration{L"-P2M"}.PrettyPrintAge () == L"2 months ago");
            VerifyTestResult (Duration{L"-PT1Y"}.PrettyPrintAge () == L"1 year ago");
            VerifyTestResult (Duration{L"-PT2Y"}.PrettyPrintAge () == L"2 years ago");
        }
    }
}

namespace {

    void Test_8_DateTimeWithDuration_ ()
    {
        TraceContextBumper ctx{"Test_8_DateTimeWithDuration_"};
        {
            DateTime d = DateTime{Date{Year{1995}, MonthOfYear::eJune, DayOfMonth{4}}, TimeOfDay::Parse (L"3:00")};
            VerifyTestResult (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
            const Duration k30Days = Duration{L"P30D"};
            DateTime       d2      = d + k30Days;
            VerifyTestResult (d2.GetDate ().GetYear () == Year{1995});
            VerifyTestResult (d2.GetDate ().GetMonth () == MonthOfYear::eJuly);
            VerifyTestResult (d2.GetDate ().GetDayOfMonth () == DayOfMonth{4});
            VerifyTestResult (d2.GetTimeOfDay () == d.GetTimeOfDay ());
        }
        {
            DateTime n1 = DateTime{Date{Year{2015}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{19, 18, 42}, Timezone::kLocalTime};
            DateTime n2 = n1 - Duration{L"P100Y"};
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
        TraceContextBumper ctx{"Test_9_TZOffsetAndDaylightSavingsTime_"};
        /*
         * I cannot think if any good way to test this stuff - since it depends on the current timezone and I cannot
         * see any good portbale way to change that (setenv (TZ) doest work on visual studio.net 2010).
         *
         * This test wont always work, but at least for now seems to work on the systems i test on.
         *
         *  @see https://stroika.atlassian.net/browse/STK-634
         */
        {
            DateTime                        n     = DateTime{Date{Year{2011}, MonthOfYear::eDecember, DayOfMonth{30}}, TimeOfDay::Parse (L"1 pm", locale::classic ()), Timezone::kLocalTime};
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
        TraceContextBumper ctx{"Test_10_std_duration_"};
        const Duration     k30Seconds = Duration{30.0};
        VerifyTestResult (k30Seconds.As<time_t> () == 30);
        VerifyTestResult (k30Seconds.As<String> () == L"PT30S");
        VerifyTestResult (k30Seconds.As<chrono::duration<double>> () == chrono::duration<double>{30.0});
        VerifyTestResult (Duration{chrono::duration<double> (4)}.As<time_t> () == 4);
        VerifyTestResult (Math::NearlyEquals (Duration{chrono::milliseconds{50}}.As<Time::DurationSecondsType> (), 0.050));
        VerifyTestResult (Math::NearlyEquals (Duration{chrono::microseconds{50}}.As<Time::DurationSecondsType> (), 0.000050));
        VerifyTestResult (Math::NearlyEquals (Duration{chrono::nanoseconds{50}}.As<Time::DurationSecondsType> (), 0.000000050));
        VerifyTestResult (Math::NearlyEquals (Duration{chrono::nanoseconds{1}}.As<Time::DurationSecondsType> (), 0.000000001));
        VerifyTestResult (Duration{5.0}.As<chrono::milliseconds> () == chrono::milliseconds{5000});
        VerifyTestResult (Duration{-5.0}.As<chrono::milliseconds> () == chrono::milliseconds{-5000});
        VerifyTestResult (Duration{1.0}.As<chrono::nanoseconds> () == chrono::nanoseconds{1000 * 1000 * 1000});
        VerifyTestResult (Duration{1} == Duration{chrono::seconds{1}});
    }
}

namespace {
    void Test_11_DurationRange_ ()
    {
        TraceContextBumper ctx{"Test_11_DurationRange_"};
        using Traversal::Range;
        Range<Duration> d1;
        Range<Duration> d2 = Range<Duration>::FullRange ();
        VerifyTestResult (d1.empty ());
        VerifyTestResult (not d2.empty ());
        VerifyTestResult (d2.GetLowerBound () == Duration::min ());
        VerifyTestResult (d2.GetUpperBound () == Duration::max ());
    }
}

namespace {
    void Test_12_DateRange_ ()
    {
        TraceContextBumper ctx{"Test_12_DateRange_"};
        using Traversal::DiscreteRange;
        {
            DiscreteRange<Date> d1;
            DiscreteRange<Date> d2 = DiscreteRange<Date>::FullRange ();
            VerifyTestResult (d1.empty ());
            VerifyTestResult (not d2.empty ());
            VerifyTestResult (d2.GetLowerBound () == Date::kMin);
            VerifyTestResult (d2.GetUpperBound () == Date::kMax);
        }
        {
            DiscreteRange<Date> dr{Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{5}}, Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{6}}};
            unsigned int        i = 0;
            for (Date d : dr) {
                ++i;
                VerifyTestResult (d.GetYear () == Year{1903});
                VerifyTestResult (d.GetMonth () == MonthOfYear::eApril);
                if (i == 1) {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth{5});
                }
                else {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth{6});
                }
            }
            VerifyTestResult (i == 2);
        }
        {
            DiscreteRange<Date> dr{Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{5}}, Date{Year{1903}, MonthOfYear::eApril, DayOfMonth{6}}};
            unsigned int        i = 0;
            for (Date d : dr.Elements ()) {
                ++i;
                VerifyTestResult (d.GetYear () == Year{1903});
                VerifyTestResult (d.GetMonth () == MonthOfYear::eApril);
                if (i == 1) {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth{5});
                }
                else {
                    VerifyTestResult (d.GetDayOfMonth () == DayOfMonth{6});
                }
            }
            VerifyTestResult (i == 2);
        }
        {
#if qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy
            Date                d1 = DateTime::Now ().GetDate () - 1;
            Date                d2 = DateTime::Now ().GetDate () + 1;
            String              t1 = Characters::ToString (d1);
            DiscreteRange<Date> dr{d1, d2};
            Stroika::Foundation::Debug::Emitter::Get ().EmitTraceMessage (L"dr=%d", Characters::ToString (dr).c_str ());
            Stroika::Foundation::Debug::Emitter::Get ().EmitTraceMessage (L"drContains=%d", dr.Contains (dr.GetMidpoint ()));
#else
            DiscreteRange<Date> dr{DateTime::Now ().GetDate () - 1, DateTime::Now ().GetDate () + 1};
#endif
            VerifyTestResult (dr.Contains (dr.GetMidpoint ()));
        }
    }
}

namespace {
    void Test_13_DateTimeRange_ ()
    {
        TraceContextBumper ctx ("Test_13_DateTimeRange_");
        using Traversal::Range;
        {
            Range<DateTime> d1;
            Range<DateTime> d2 = Range<DateTime>::FullRange ();
            VerifyTestResult (d1.empty ());
            VerifyTestResult (not d2.empty ());
            VerifyTestResult (d2.GetLowerBound () == DateTime::kMin);
            VerifyTestResult (d2.GetUpperBound () == DateTime::kMax);
        }
        {
            Range<DateTime> d1{DateTime{Date{Year{2000}, MonthOfYear::eApril, DayOfMonth{20}}}, DateTime{Date{Year{2000}, MonthOfYear::eApril, DayOfMonth{22}}}};
            VerifyTestResult (d1.GetDistanceSpanned () / 2 == Duration{"PT1D"});
            // SEE https://stroika.atlassian.net/browse/STK-514 for accuracy of compare (sb .1 or less)
            VerifyTestResult (Math::NearlyEquals (d1.GetMidpoint (), Date{Year{2000}, MonthOfYear::eApril, DayOfMonth{21}}, DurationSecondsType{2}));
        }
    }
}

namespace {
    void Test_14_timepoint_ ()
    {
        TraceContextBumper ctx{"Test_14_timepoint_"};
        // @see https://stroika.atlassian.net/browse/STK-619 - VerifyTestResult (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) == time_point<chrono::steady_clock>::max ());
        VerifyTestResult (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinite) > chrono::steady_clock::now () + chrono::seconds (10000));
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        TraceContextBumper ctx{"DoRegressionTests_"};
        Test_0_AssumptionsAboutUnderlyingTimeLocaleLibrary_ ();
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
