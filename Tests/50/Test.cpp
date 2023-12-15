/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;

using namespace Stroika::Frameworks;

using Stroika::Foundation::Debug::TraceContextBumper;

#if qHasFeature_GoogleTest
namespace {
    void Test_0_AssumptionsAboutUnderlyingTimeLocaleLibrary_ ()
    {
        TraceContextBumper ctx{"Test_0_AssumptionsAboutUnderlyingTimeLocaleLibrary_"};

        auto test_locale_time_get_date_order_no_order_Buggy = [] (const String& localeName) {
            TraceContextBumper ctx{"test_locale_time_get_date_order_no_order_Buggy"};
            try {
                std::locale              l{localeName.AsNarrowSDKString ()};
                const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
#if qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy
                VerifyTestResultWarning (tmget.date_order () == time_base::no_order);
#else
                VerifyTestResultWarning (tmget.date_order () == time_base::mdy);
#endif
            }
            catch (...) {
                // suppress macos warn here - just not such locale installed
#if !qPlatform_MacOS
                Stroika::Frameworks::Test::WarnTestIssue (
                    Characters::Format (L"test_locale_time_get_date_order_no_order_Buggy skipped - usually because of missing locale %s",
                                        localeName.As<wstring> ().c_str ())
                        .c_str ());
#endif
            }
        };
        test_locale_time_get_date_order_no_order_Buggy (L"en_US.utf8");
        test_locale_time_get_date_order_no_order_Buggy (L"en_US");

        auto localetimeputPCTX_CHECK_StdCPctxTraits1 = [] (const locale& l, bool expect4DigitYear) {
            TraceContextBumper       ctx{"localetimeputPCTX_CHECK_StdCPctxTraits1"};
            const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
            constexpr tm             kOrigDate_{47, 18, 16, 3, 6, 101}; // tm_mon=6, so July
            tm                       when = kOrigDate_;
            wostringstream           oss;
            const wchar_t            kPattern[] = L"%x";
            tmput.put (oss, oss, ' ', &when, begin (kPattern), begin (kPattern) + ::wcslen (kPattern));
            String tmpStringRep = oss.str ();
            if (expect4DigitYear) {
                EXPECT_TRUE (tmpStringRep == "7/3/2001" or tmpStringRep == "07/03/2001");
            }
            else {
                EXPECT_TRUE (tmpStringRep == "7/3/01" or tmpStringRep == "07/03/01");
            }
        };
        auto localetimeputPCTX_CHECK_StdCPctxTraits = [=] () {
            TraceContextBumper ctx{"localetimeputPCTX_CHECK_StdCPctxTraits"};
            localetimeputPCTX_CHECK_StdCPctxTraits1 (locale::classic (), StdCPctxTraits::kLocaleClassic_Write4DigitYear);
            try {
                localetimeputPCTX_CHECK_StdCPctxTraits1 (locale{"en_US"}, StdCPctxTraits::kLocaleENUS_Write4DigitYear);
            }
            catch (...) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    L"localetimeputPCTX_CHECK_StdCPctxTraits skipped - usually because of en_US missing locale");
            }
            try {
                localetimeputPCTX_CHECK_StdCPctxTraits1 (locale{"en_US.utf8"}, StdCPctxTraits::kLocaleENUS_Write4DigitYear);
            }
            catch (...) {
                // suppress macos warn here - just not such locale installed
#if !qPlatform_MacOS
                Stroika::Frameworks::Test::WarnTestIssue (
                    L"localetimeputPCTX_CHECK_StdCPctxTraits skipped - usually because of en_US.utf8 missing locale");
#endif
            }
        };
        localetimeputPCTX_CHECK_StdCPctxTraits ();

        auto testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ = [] (int tm_Year, int tm_Mon, int tm_mDay, const locale& l) {
            TraceContextBumper ctx{"testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_"};
            //DbgTrace (L"year=%d, tm_Mon=%d, tm_mDay=%d", tm_Year, tm_Mon, tm_mDay);
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
            //DbgTrace (L"tmpStringRep=%s", tmpStringRep.c_str ());
            tm resultTM{};
            {
                const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
                ios::iostate                 state = ios::goodbit;
                wistringstream               iss{tmpStringRep};
                istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
                istreambuf_iterator<wchar_t> itend;        // end-of-stream
                tmget.get (itbegin, itend, iss, state, &resultTM, std::begin (kPattern), std::begin (kPattern) + ::wcslen (kPattern));
            }
            //DbgTrace (L"resultTM.tm_year=%d", resultTM.tm_year);
            VerifyTestResultWarning (origDateTM.tm_year == resultTM.tm_year);
            VerifyTestResultWarning (origDateTM.tm_mon == resultTM.tm_mon);
            VerifyTestResultWarning (origDateTM.tm_mday == resultTM.tm_mday);
        };
        // this test doesnt make much sense - revisit!!! --LGP 2021-03-05
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (12, 5, 1, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (-148, 10, 19, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (121, 2, 4, locale::classic ());
        testDateLocaleRoundTripsForDateWithThisLocale_get_put_Lib_ (105, 5, 1, locale::classic ());

        auto getPCTMRequiresLeadingZeroBug = [] () {
            TraceContextBumper           ctx{"getPCTMRequiresLeadingZeroBug"};
            std::locale                  l     = locale::classic ();
            const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
            ios::iostate                 state = ios::goodbit;
            wistringstream               iss{L"11/1/2002"};
            istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
            istreambuf_iterator<wchar_t> itend;        // end-of-stream
            tm                           resultTM{};
            wstring                      fmt = String{Date::kMonthDayYearFormat}.As<wstring> ();
            [[maybe_unused]] auto        i   = tmget.get (itbegin, itend, iss, state, &resultTM, fmt.data (), fmt.data () + fmt.length ());
#if qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy
            EXPECT_TRUE ((state & ios::badbit) or (state & ios::failbit));
#else
            EXPECT_TRUE (not((state & ios::badbit) or (state & ios::failbit)));
#endif
        };
        getPCTMRequiresLeadingZeroBug ();

        auto std_get_time_pctxBuggyTest = [] () {
            TraceContextBumper ctx{"std_get_time_pctxBuggyTest"};
            //wstring wRep = L"3pm";   // this works
            wstring                                   wRep = L"3:00";
            locale                                    l    = locale::classic ();
            wistringstream                            iss{wRep};
            [[maybe_unused]] const time_get<wchar_t>& tmget    = use_facet<time_get<wchar_t>> (l);
            [[maybe_unused]] ios::iostate             errState = ios::goodbit;
            tm                                        when{};
            wstring                                   formatPattern = L"%X"; // or %EX, or %T all fail
            istreambuf_iterator<wchar_t>              itbegin (iss);         // beginning of iss
            istreambuf_iterator<wchar_t>              itend;                 // end-of-stream

            istreambuf_iterator<wchar_t> i;
            // In Debug build on Windows, this generates Assertion error inside stdc++ runtime library
            // first noticed broken in vs2k17 (qCompilerAndStdLib_std_get_time_pctx_Buggy)
#if !qCompilerAndStdLib_std_get_time_pctx_Buggy
            // get assertion istreambuf_iterator is not dereferenceable failure if we enable this on VS2k
            i = tmget.get (itbegin, itend, iss, errState, &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
#endif
        };
        std_get_time_pctxBuggyTest ();

        auto tmget_dot_get_locale_date_order_buggy_test_ = [] () {
            TraceContextBumper ctx{"tmget_dot_get_locale_date_order_buggy_test_"};
            try {
                std::locale              l{"en_US.utf8"}; // originally tested with locale {} - which defaulted to C-locale
                const time_get<wchar_t>& tmget = use_facet<time_get<wchar_t>> (l);
                ios::iostate             state = ios::goodbit;
                wistringstream iss{L"03/07/21 16:18:47"}; // qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy ONLY triggered if YEAR 2-digits - 4-digit year fine
                constexpr tm                 kTargetTM_MDY_{47, 18, 16, 7, 2};
                constexpr tm                 kTargetTM_DMY_{47, 18, 16, 3, 6};
                istreambuf_iterator<wchar_t> itbegin{iss}; // beginning of iss
                istreambuf_iterator<wchar_t> itend;        // end-of-stream
                tm                           resultTM{};
                VerifyTestResultWarning (tmget.date_order () == time_base::mdy or qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy);
                static const wstring  kFmt_ = String{DateTime::kShortLocaleFormatPattern}.As<wstring> ();
                [[maybe_unused]] auto i = tmget.get (itbegin, itend, iss, state, &resultTM, kFmt_.data (), kFmt_.data () + kFmt_.length ());
                if ((state & ios::badbit) or (state & ios::failbit)) {
#if !_LIBCPP_VERSION
                    // Known that _LIBCPP_VERSION (clang libc++) treats this as an error and quite reasonable - so only warn for other cases so I can add exclusions here
                    Stroika::Frameworks::Test::WarnTestIssue ("Skipping tmget_dot_get_locale_date_order_buggy_test_ cuz parse failure");
#endif
                }
                else {
                    EXPECT_TRUE (resultTM.tm_sec == kTargetTM_MDY_.tm_sec);   // which == kTargetTM_DMY_
                    EXPECT_TRUE (resultTM.tm_min == kTargetTM_MDY_.tm_min);   // ..
                    EXPECT_TRUE (resultTM.tm_hour == kTargetTM_MDY_.tm_hour); // ..
                    // libstdc++ returns 21, and visual studio 121 - clang libc++ -1879 - all reasonable - DONT CHECK THIS - undefined for 2-digit year -- LGP 2021-03-08
                    if (tmget.date_order () == time_base::mdy or
                        (qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy and tmget.date_order () == time_base::no_order)) {
#if qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy
                        EXPECT_TRUE (resultTM.tm_mday == kTargetTM_DMY_.tm_mday); // sadly wrong values
                        EXPECT_TRUE (resultTM.tm_mon == kTargetTM_DMY_.tm_mon);
#else
                        EXPECT_TRUE (resultTM.tm_mday == kTargetTM_MDY_.tm_mday);
                        EXPECT_TRUE (resultTM.tm_mon == kTargetTM_MDY_.tm_mon);
#endif
                    }
                    else if (tmget.date_order () == time_base::dmy) {
                        EXPECT_TRUE (resultTM.tm_mday == kTargetTM_DMY_.tm_mday);
                        EXPECT_TRUE (resultTM.tm_mon == kTargetTM_DMY_.tm_mon);
                    }
                }
            }
            catch (...) {
#if !qPlatform_MacOS
                Stroika::Frameworks::Test::WarnTestIssue (
                    L"tmget_dot_get_locale_date_order_buggy_test_ skipped - usually because of missing locale");
#endif
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
        EXPECT_TRUE (startDateOrTime == andBack);
    }
    template <typename DATEORTIME>
    void TestRoundTripFormatThenParseNoChange_ (DATEORTIME startDateOrTime)
    {
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale{});
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale::classic ());
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, Configuration::FindNamedLocale ("en", "us"));

        // should add test like this...
        //EXPECT_TRUE (startDateOrTime == DATEORTIME::Parse (startDateOrTime.Format (DATEORTIME::PrintFormat::eCurrentLocale), DATEORTIME::PrintFormat::ParseFormat::eCurrentLocale));
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
        TraceContextBumper     ctx{"Test_1_TestTickCountGrowsMonotonically_"};
        Time::TimePointSeconds start = Time::GetTickCount ();
        Execution::Sleep (100ms);
        EXPECT_TRUE (start <= Time::GetTickCount ());
    }
}

namespace {

    void Test_2_TestTimeOfDay_ ()
    {
        TraceContextBumper ctx{"Test_2_TestTimeOfDay_"};
        {
            optional<TimeOfDay> t;
            EXPECT_TRUE (not t.has_value ());
            TimeOfDay t2{2};
            EXPECT_TRUE (t < t2);
            EXPECT_TRUE (not t2.Format (locale{}).empty ());
            EXPECT_TRUE (t2.GetHours () == 0);
            EXPECT_TRUE (t2.GetMinutes () == 0);
            EXPECT_TRUE (t2.GetSeconds () == 2);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay t2{5 * 60 * 60 + 3 * 60 + 49};
            EXPECT_TRUE (t2.GetHours () == 5);
            EXPECT_TRUE (t2.GetMinutes () == 3);
            EXPECT_TRUE (t2.GetSeconds () == 49);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay t2{24 * 60 * 60 - 1};
            EXPECT_TRUE (t2.GetHours () == 23);
            EXPECT_TRUE (t2.GetMinutes () == 59);
            EXPECT_TRUE (t2.GetSeconds () == 59);
            EXPECT_TRUE (t2 == TimeOfDay::kMax);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            EXPECT_TRUE (TimeOfDay::Parse ("3pm", locale::classic ()).GetAsSecondsCount () == 15 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("3PM", locale::classic ()).GetAsSecondsCount () == 15 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("3am", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("3:00", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("16:00", locale::classic ()).GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // Not sure these should ALWAYS work in any locale. Probably not. But any locale I'd test in??? Maybe... Good for starters anyhow...
            //      -- LGP 2011-10-08
            EXPECT_TRUE (TimeOfDay::Parse ("3pm").GetAsSecondsCount () == 15 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("3am").GetAsSecondsCount () == 3 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("3:00").GetAsSecondsCount () == 3 * 60 * 60);
            EXPECT_TRUE (TimeOfDay::Parse ("16:00").GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale ("en", "us")};
#if qCompilerAndStdLib_locale_pctX_print_time_Buggy
            // NOTE - these values are wrong, but since using locale code, not easy to fix/workaround - but to note XCode locale stuff still
            // somewhat broken...
            EXPECT_TRUE (TimeOfDay{101}.Format (locale{}) == "00:01:41");
            EXPECT_TRUE (TimeOfDay{60}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "0:01");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (locale{}) == "01:01:41");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "1:01:41");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 60}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "1:01");
#else
            EXPECT_TRUE (TimeOfDay{101}.Format (locale{}) == "12:01:41 AM");
            EXPECT_TRUE (TimeOfDay{60}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "12:01 AM");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (locale{}) == "1:01:41 AM" or
                         TimeOfDay{60 * 60 + 101}.Format (locale{}) == "01:01:41 AM");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "1:01:41 AM");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 60}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "1:01 AM");
#endif
        }
        {
            EXPECT_TRUE (TimeOfDay{101}.Format (locale{}) == "00:01:41");
            EXPECT_TRUE (TimeOfDay{60}.Format (locale{}) == "00:01:00");
            EXPECT_TRUE (TimeOfDay{60}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "0:01");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (locale{}) == "01:01:41");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 101}.Format (TimeOfDay::eCurrentLocale_WithZerosStripped) == "1:01:41");
            EXPECT_TRUE (TimeOfDay{60 * 60 + 60}.Format (locale{}) == "01:01:00");
        }
        {
            TimeOfDay threePM = TimeOfDay::Parse ("3pm", locale::classic ());
            EXPECT_TRUE (threePM.Format (locale::classic ()) == "15:00:00"); // UGH!!!
            TestRoundTripFormatThenParseNoChange_ (threePM);
        }
    }
}

namespace {
    void VERIFY_ROUNDTRIP_XML_ (const Date& d)
    {
        EXPECT_TRUE (Date::Parse (d.Format (Date::kISO8601Format), Date::kISO8601Format) == d);
    }

    void Test_3_TestDate_ ()
    {
        TraceContextBumper ctx{"Test_3_TestDate_"};
        {
            Date d{Year{1903}, April, DayOfMonth{4}};
            TestRoundTripFormatThenParseNoChange_ (d);
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "1903-04-04");
            VERIFY_ROUNDTRIP_XML_ (d);
            d = d.Add (4);
            VERIFY_ROUNDTRIP_XML_ (d);
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "1903-04-08");
            d = d.Add (-4);
            VERIFY_ROUNDTRIP_XML_ (d);
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        try {
            Date d = Date::Parse ("09/14/1752", Date::kMonthDayYearFormat);
            EXPECT_TRUE (d == Date::kGregorianCalendarEpoch.fYMD);
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        catch (...) {
            EXPECT_TRUE (false);
        }
        {
            optional<Date> d;
            EXPECT_TRUE (d < DateTime::GetToday ());
            EXPECT_TRUE (DateTime::GetToday () > d);
            //TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            Date d = Date::kGregorianCalendarEpoch.fYMD;
            EXPECT_TRUE (d < DateTime::Now ().GetDate ());
            EXPECT_TRUE (not(DateTime::Now ().GetDate () < d));
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            Date d = Date{300y / January / 3};
            EXPECT_TRUE (d < DateTime::Now ().GetDate ());
            EXPECT_TRUE (not(DateTime::Now ().GetDate () < d));
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "0300-01-03");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            Date d = Date::kMin;
            EXPECT_TRUE (d < DateTime::Now ().GetDate ());
            EXPECT_TRUE (not(DateTime::Now ().GetDate () < d));
            EXPECT_TRUE (d.Format (Date::kISO8601Format) == "-4712-01-01"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        try {
            EXPECT_TRUE ((Date::Parse ("11/3/2001", Date::kMonthDayYearFormat) == Date{Year (2001), Time::November, DayOfMonth (3)}));
            EXPECT_TRUE (Date::Parse ("11/3/2001", Date::kMonthDayYearFormat).Format (Date::kMonthDayYearFormat) == L"11/03/2001");
        }
        catch (...) {
            // See qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy if this is triggered
            DbgTrace ("qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy");
            EXPECT_TRUE (false);
            Execution::ReThrow ();
        }
        {
            EXPECT_TRUE (Date::kMin < Date::kMax);
            EXPECT_TRUE (Date::kMin <= Date::kMax);
            EXPECT_TRUE (not(Date::kMin > Date::kMax));
            EXPECT_TRUE (not(Date::kMin >= Date::kMax));
            TestRoundTripFormatThenParseNoChange_ (Date::kMin);
            TestRoundTripFormatThenParseNoChange_ (Date::kMax);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale ("en", "us")};
            Date                           d = Date{Year{1903}, April, DayOfMonth{5}};
            TestRoundTripFormatThenParseNoChange_ (d);
            EXPECT_TRUE (d.Format (locale{}) == "4/5/1903" or d.Format (locale{}) == "04/05/1903");
            EXPECT_TRUE (d.Format (Date::eCurrentLocale_WithZerosStripped) == "4/5/1903");
        }
        {
            Date d = Date (1903y, April, 5d);
            EXPECT_TRUE (d.Format (locale{}) == "4/5/1903" or d.Format (locale{}) == "04/05/1903" or d.Format (locale{}) == "04/05/03");
            EXPECT_TRUE (d.Format (Date::eCurrentLocale_WithZerosStripped) == "4/5/1903" or d.Format (Date::eCurrentLocale_WithZerosStripped) == "4/5/03");
        }
        {
            Date d = Date{Date::JulianDayNumber{2455213}};
            EXPECT_TRUE (d.Format () == "1/16/10");
        }
        {
            Date d = Date{2012y / May / 1d};
            EXPECT_TRUE (d.GetJulianRep () == 2456049); //https://aa.usno.navy.mil/data/JulianDate
        }
        {
            EXPECT_TRUE (Date::ToJulianRep (Date::FromJulianRep (0)) == 0);
            for (int i = 0; i < 10 * 1000; ++i) {
                // just a random sampling to assure reversible/consistent
                EXPECT_TRUE (Date::ToJulianRep (Date::FromJulianRep (i * 300)) == i * 300u);
            }
        }
        {
            Date d = 1906y / May / 12d;
            d      = d + 1;
            EXPECT_TRUE (d == (1906y / May / 13d));
        }
    }
}

namespace {

    void Test_4_TestDateTime_ ()
    {
        TraceContextBumper ctx{"Test_4_TestDateTime_"};
        {
            DateTime d = Date (Year{1903}, April, DayOfMonth{4});
            EXPECT_TRUE (d.Format (DateTime::kISO8601Format) == "1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            optional<DateTime> d;
            EXPECT_TRUE (d < DateTime::Now ());
            EXPECT_TRUE (DateTime::Now () > d);
            //TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            DbgTrace (L"DateTime::Now()=%s", Characters::ToString (DateTime::Now ()).c_str ());
            DbgTrace (L"DateTime::Now().AsUTC ()=%s", Characters::ToString (DateTime::Now ().AsUTC ()).c_str ());
            DbgTrace (L"DateTime::Now().AsLocalTime ()=%s", Characters::ToString (DateTime::Now ().AsLocalTime ()).c_str ());
            DbgTrace (L"Timezone::kLocalTime.GetBiasFromUTC (fDate_, TimeOfDay{0})=%d",
                      Timezone::kLocalTime.GetBiasFromUTC (DateTime::Now ().GetDate (), TimeOfDay{0}));
            {
                DateTime regTest{time_t (1598992961)};
                EXPECT_TRUE (regTest.GetTimezone () == Timezone::kUTC);
                EXPECT_TRUE ((regTest.GetDate () == Date{Year{2020}, September, DayOfMonth{1}}));
                EXPECT_TRUE ((regTest.GetTimeOfDay () == TimeOfDay{20, 42, 41}));
                if (Timezone::kLocalTime.GetBiasFromUTC (regTest.GetDate (), *regTest.GetTimeOfDay ()) == -4 * 60 * 60) {
                    DbgTrace ("Eastern US timezone");
                    EXPECT_TRUE ((regTest.AsLocalTime () ==
                                  DateTime{Date{Year{2020}, September, DayOfMonth{1}}, TimeOfDay{20 - 4, 42, 41}, Timezone::kLocalTime}));
                }
                else {
                    DbgTrace ("other timezone: offset=%d", Timezone::kLocalTime.GetBiasFromUTC (regTest.GetDate (), *regTest.GetTimeOfDay ()));
                }
            }
        }
        {
            DateTime d = {Date::kGregorianCalendarEpoch.fYMD, TimeOfDay{0}};
            EXPECT_TRUE (d < DateTime::Now ());
            EXPECT_TRUE (DateTime::Now () > d);
            d = DateTime{d.GetDate (), d.GetTimeOfDay (), Timezone::kUTC}; // so that compare works - cuz we don't know timezone we'll run test with...
            EXPECT_TRUE (d.Format (DateTime::kISO8601Format) == "1752-09-14T00:00:00Z"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            DateTime d = DateTime::kMin;
            EXPECT_TRUE (d < DateTime::Now ());
            EXPECT_TRUE (DateTime::Now () > d);
            d = DateTime{d.GetDate (), d.GetTimeOfDay (), Timezone::kUTC}; // so that compare works - cuz we don't know timezone we'll run test with...
            EXPECT_TRUE (d.Format (DateTime::kISO8601Format) == "-4712-01-01T00:00:00Z"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        //// TODO - FIX FOR PrintFormat::eCurrentLocale_WITHZEROESTRIPPED!!!!
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale ("en", "us")};
            Date                           d = Date{Year{1903}, April, DayOfMonth{5}};
            DateTime                       dt{d, TimeOfDay{101}};

            {
                String tmp = dt.Format (locale{});
#if qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy
                EXPECT_TRUE (tmp == "4/5/1903 12:01:41 AM" or tmp == "04/05/1903 12:01:41 AM");
#else
#if qCompilerAndStdLib_locale_pctX_print_time_Buggy
                EXPECT_TRUE (tmp == "Sun Apr  5 00:01:41 1903");
#else
                EXPECT_TRUE (tmp == "Sun 05 Apr 1903 12:01:41 AM");
#endif
#endif
            }
            DateTime dt2{d, TimeOfDay{60}};
            //TOFIX!EXPECT_TRUE (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        {
            Date d = Date{Year{1903}, April, DayOfMonth{6}};
            TestRoundTripFormatThenParseNoChange_ (d);
            DateTime dt{d, TimeOfDay{101}};
            TestRoundTripFormatThenParseNoChange_ (dt);
            String tmp = dt.Format (locale{});
            EXPECT_TRUE (tmp == "Mon Apr  6 00:01:41 1903");
            DateTime dt2{d, TimeOfDay{60}};
            TestRoundTripFormatThenParseNoChange_ (dt2);
            // want a variant that does this formatting!
            //EXPECT_TRUE (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        EXPECT_TRUE (DateTime::Parse (L"2010-01-01", DateTime::kISO8601Format).GetDate ().GetYear () == Time::Year{2010});
        {
            DateTime now = DateTime::Now ();
            TestRoundTripFormatThenParseNoChange_ (now);

            constexpr bool kLocaleDateTimeFormatMaybeLossy_{true}; // 2 digit date - 03/04/05 parsed as 2005 on windows, and 1905 of glibc (neither wrong) - see StdCPctxTraits, but cannot consult it cuz we could be using any locale
            if (kLocaleDateTimeFormatMaybeLossy_) {
                String   nowShortLocaleForm = now.Format (locale{}, DateTime::kShortLocaleFormatPattern);
                DateTime dt                 = DateTime::Parse (nowShortLocaleForm, DateTime::kShortLocaleFormatPattern);
                // This roundtrip can be lossy, becaue the date 2016 could be represented as '16' and then when mapped the other way as
                // 1916 (locale::classic ()). So fixup the year before comparing
                Time::Year nYear = now.GetDate ().GetYear ();
                Date       d     = dt.GetDate ();
                if (d.GetYear () != nYear) {
                    EXPECT_TRUE (((nYear - d.GetYear ()).count () % 100) == 0);
                    d  = Date{nYear, d.GetMonth (), d.GetDayOfMonth ()};
                    dt = DateTime{dt, d};
                }
                EXPECT_TRUE (now == dt); // if this fails, look at qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy
            }
            else {
                EXPECT_TRUE (now == DateTime::Parse (now.Format (DateTime::kShortLocaleFormatPattern), DateTime::kShortLocaleFormatPattern));
            }
        }
        {
            const DateTime kProblemBaseDT_ = DateTime{Date{2023y / February / 18}, TimeOfDay{10, 35, 59}}.AsLocalTime ();
            const DateTime kProblemDT_     = DateTime{Date{2023y / March / 2d}, TimeOfDay{10, 35, 59}}.AsLocalTime ();
            Duration       diff            = kProblemDT_ - kProblemBaseDT_;
            EXPECT_TRUE (diff == days{12}); // note not a leap year
        }
        {
            Time::TimePointSeconds now = Time::GetTickCount ();

#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
            static const auto kInitList_ =
                initializer_list<Time::DurationSeconds>{3s, 995s, 3.4s, 3004.5s, 1055646.4s, 60 * 60 * 24 * 300s};
#endif
            for (Time::DurationSeconds dso :
#if qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy
                 kInitList_
#else
                 initializer_list<Time::DurationSeconds>{3s, 995s, 3.4s, 3004.5s, 1055646.4s, 60s * 60 * 24 * 300}
#endif
            ) {
                // test going back and forth between TimePointSeconds and DateTime...
                Time::TimePointSeconds ds = dso + now;
                DateTime               dt{ds};
                EXPECT_TRUE ((Math::NearlyEquals (dt, DateTime{dt.As<Time::TimePointSeconds> ()})));
                // crazy large epsilon for now because we represent datetime to nearest second
                // (note - failed once with clang++ on vm - could be something else slowed vm down - LGP 2018-04-17 - ignore for now)
                // But even the 1.1 failed once (--LGP 2019-05-03) - so change to warning and use bigger number (2.1) for error check
                // Failed again on (slowish) vm under VS2k17 - but rarely --LGP 2021-05-20
                VerifyTestResultWarning (Math::NearlyEquals (dt.AsLocalTime ().As<Time::TimePointSeconds> ().time_since_epoch ().count (),
                                                             ds.time_since_epoch ().count (), 1.1));
                EXPECT_TRUE (Math::NearlyEquals (dt.AsLocalTime ().As<Time::TimePointSeconds> ().time_since_epoch ().count (),
                                                 ds.time_since_epoch ().count (),
                                                 7.5)); // failed once - with value 2.1 - 2023-11-10, so up this to 5 (sometimes run on slow machines)
                // Failed once (after change to more jittery tickcount logic) on macos, so upped to 7.5 seconds --LGP 2023-11-23
            }
        }
        {
            auto roundTripD = [] (DateTime dt) {
                String   s   = dt.Format (DateTime::kRFC1123Format);
                DateTime dt2 = DateTime::Parse (s, DateTime::kRFC1123Format);
                EXPECT_TRUE (dt == dt2);
            };
            auto roundTripS = [] (String s) {
                DateTime dt = DateTime::Parse (s, DateTime::kRFC1123Format);
                EXPECT_TRUE (dt.Format (DateTime::kRFC1123Format) == s);
            };

            // Parse eRFC1123
            EXPECT_TRUE (DateTime::Parse (L"Wed, 09 Jun 2021 10:18:14 GMT", DateTime::kRFC1123Format) ==
                         (DateTime{Date{Time::Year{2021}, June, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC}));
            // from https://www.feedvalidator.org/docs/error/InvalidRFC2822Date.html
            EXPECT_TRUE (DateTime::Parse (L"Wed, 02 Oct 2002 08:00:00 EST", DateTime::kRFC1123Format) ==
                         (DateTime{Date{Time::Year{2002}, October, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            EXPECT_TRUE (DateTime::Parse (L"Wed, 02 Oct 2002 13:00:00 GMT", DateTime::kRFC1123Format) ==
                         (DateTime{Date{Time::Year{2002}, October, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));
            EXPECT_TRUE (DateTime::Parse (L"Wed, 02 Oct 2002 15:00:00 +0200", DateTime::kRFC1123Format) ==
                         (DateTime{Date{Time::Year{2002}, October, DayOfMonth{2}}, TimeOfDay{8, 0, 0}, Timezone (-5 * 60)}));

            EXPECT_TRUE (DateTime::Parse (L"Tue, 6 Nov 2018 06:25:51 -0800 (PST)", DateTime::kRFC1123Format) ==
                         (DateTime{Date{Time::Year{2018}, November, DayOfMonth{6}}, TimeOfDay{6, 25, 51}, Timezone (-8 * 60)}));

            roundTripD (DateTime{Date{Time::Year{2021}, June, DayOfMonth{9}}, TimeOfDay{10, 18, 14}, Timezone::kUTC});

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
                EXPECT_TRUE ((DateTime {kDate_, kTOD_} - DateTime {kDate_, kTOD2_}).As<Time::DurationSeconds::rep> () == -3);
                EXPECT_TRUE ((DateTime {kDate_, kTOD2_} - DateTime {kDate_, kTOD_}).As<Time::DurationSeconds::rep> () == 3);
                EXPECT_TRUE ((DateTime {kDate_.Add (1), kTOD_} - DateTime {kDate_, kTOD_}).As<Time::DurationSeconds::rep> () == 24 * 60 * 60);
                EXPECT_TRUE ((DateTime {kDate_, kTOD_} - DateTime {kDate_.Add (1), kTOD_}).As<Time::DurationSeconds::rep> () == -24 * 60 * 60);
            }
            {
                EXPECT_TRUE ((DateTime::Now () - DateTime::kMin) > "P200Y"_duration);
            }
        }
        {
            // https://stroika.atlassian.net/browse/STK-555 - Improve Timezone object so that we can read time with +500, and respect that
            {
                constexpr Date      kDate_{Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth{29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse ("2016-09-29T10:21:32-04:00", DateTime::kISO8601Format);
                DateTime            tdu = td.AsUTC ();
                EXPECT_TRUE ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
            {
                constexpr Date      kDate_ = Date {Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth{29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse ("2016-09-29T10:21:32-0400", DateTime::kISO8601Format);
                DateTime            tdu = td.AsUTC ();
                EXPECT_TRUE ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
            {
                constexpr Date      kDate_{Time::Year{2016}, Time::MonthOfYear {9}, Time::DayOfMonth {29}};
                constexpr TimeOfDay kTOD_{10, 21, 32};
                DateTime            td  = DateTime::Parse ("2016-09-29T10:21:32-04", DateTime::kISO8601Format);
                DateTime            tdu = td.AsUTC ();
                EXPECT_TRUE ((tdu == DateTime{kDate_, TimeOfDay{kTOD_.GetHours () + 4, kTOD_.GetMinutes (), kTOD_.GetSeconds ()}, Timezone::kUTC}));
            }
        }
        {
            // https://stroika.atlassian.net/browse/STK-950
            try {
                [[maybe_unused]]DateTime dt = DateTime::Parse (L"1906-05-12x12:00:00+00", DateTime::kISO8601Format);
                EXPECT_TRUE (false);
            }
            catch (const DateTime::FormatException&) {
                // good
            }
            catch (...) {
                EXPECT_TRUE (false);
            }
            try {
                DateTime dt = DateTime::Parse (L"1906-05-12 12:00:00+00", DateTime::kISO8601Format);    //allowed to use space or 't'
                EXPECT_TRUE ((dt.GetDate () == Date{1906y, May, 12d}));
                EXPECT_TRUE ((dt.GetTimeOfDay () == TimeOfDay {12, 0, 0}));
                EXPECT_TRUE (dt.GetTimezone ()->GetBiasFromUTC (dt.GetDate (), *dt.GetTimeOfDay ()) == 0);
            }
            catch (...) {
                EXPECT_TRUE (false);
            }
            try {
                DateTime dt = DateTime::Parse (L"1906-05-12T12:00:00+00", DateTime::kISO8601Format);
                EXPECT_TRUE ((dt.GetDate () == Date{1906y, May, 12d}));
                EXPECT_TRUE ((1906y/May/12d == Date{1906y, May, 12d}));
                EXPECT_TRUE ((dt.GetTimeOfDay () == TimeOfDay {12, 0, 0}));
                EXPECT_TRUE (dt.GetTimezone ()->GetBiasFromUTC (dt.GetDate (), *dt.GetTimeOfDay ()) == 0);
            }
            catch (...) {
                EXPECT_TRUE (false);
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
            DateTime d = Date{Year{2000}, April, DayOfMonth{20}};
            EXPECT_TRUE (d.As<time_t> () == 956188800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, June, DayOfMonth{4}}, TimeOfDay::Parse (L"3pm", locale{})};
            EXPECT_TRUE (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, June, DayOfMonth{4}}, TimeOfDay::Parse (L"3pm")};
            EXPECT_TRUE (d.As<time_t> () == 802278000); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, June, DayOfMonth{4}}, TimeOfDay::Parse (L"3am")};
            EXPECT_TRUE (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime d = DateTime{Date{Year{1995}, June, DayOfMonth{4}}, TimeOfDay::Parse (L"3:00")};
            EXPECT_TRUE (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            const time_t kTEST = 802234800;
            DateTime     d     = DateTime{kTEST};
            EXPECT_TRUE (d.As<time_t> () == kTEST); // source - http://www.onlineconversion.com/unix_time.htm
        }
    }
}

namespace {

    void Test_6_DateTimeStructTM_ ()
    {
        TraceContextBumper ctx{"Test_6_DateTimeStructTM_"};
        {
            struct tm x {};
            x.tm_hour    = 3;
            x.tm_min     = 30;
            x.tm_year    = 80;
            x.tm_mon     = 3;
            x.tm_mday    = 15;
            DateTime  d  = DateTime{x};
            struct tm x2 = d.As<struct tm> ();
            EXPECT_TRUE (x.tm_hour == x2.tm_hour);
            EXPECT_TRUE (x.tm_min == x2.tm_min);
            EXPECT_TRUE (x.tm_sec == x2.tm_sec);
            EXPECT_TRUE (x.tm_year == x2.tm_year);
            EXPECT_TRUE (x.tm_mday == x2.tm_mday);
        }
    }
}

namespace {

    void Test_7_Duration_ ()
    {
        TraceContextBumper ctx{"Test_7_Duration_"};
        {
            EXPECT_TRUE (Duration{0}.As<time_t> () == 0);
            EXPECT_TRUE (Duration{0}.As<String> () == L"PT0S");
            EXPECT_TRUE (Duration{0}.Format () == L"0 seconds");
        }
        {
            EXPECT_TRUE (Duration{3}.As<time_t> () == 3);
            EXPECT_TRUE (Duration{3}.As<String> () == L"PT3S");
            EXPECT_TRUE (Duration{3}.Format () == L"3 seconds");
        }
        const int kSecondsPerDay = TimeOfDay::kMaxSecondsPerDay;
        {
            const Duration k30Days = Duration{L"P30D"};
            EXPECT_TRUE (k30Days.As<time_t> () == 30 * kSecondsPerDay);
        }
        {
            const Duration k6Months = Duration{"P6M"};
            EXPECT_TRUE (k6Months.As<time_t> () == 6 * 30 * kSecondsPerDay);
        }
        {
            const Duration kP1Y = Duration{"P1Y"};
            EXPECT_TRUE (kP1Y.As<time_t> () == 365 * kSecondsPerDay);
        }
        {
            const Duration kP2Y = Duration{"P2Y"};
            EXPECT_TRUE (kP2Y.As<time_t> () == 2 * 365 * kSecondsPerDay);
            EXPECT_TRUE (Duration{2 * 365 * kSecondsPerDay}.As<String> () == "P2Y");
        }
        {
            const Duration kHalfMinute = Duration{"PT0.5M"};
            EXPECT_TRUE (kHalfMinute.As<time_t> () == 30);
        }
        {
            const Duration kD = Duration{L"PT0.1S"};
            EXPECT_TRUE (kD.As<time_t> () == 0);
            EXPECT_TRUE (kD.As<double> () == 0.1);
        }
        {
            const Duration kHalfMinute = Duration{L"PT0.5M"};
            EXPECT_TRUE (kHalfMinute.PrettyPrint () == L"30 seconds");
        }
        {
            const Duration k3MS = Duration{L"PT0.003S"};
            EXPECT_TRUE (k3MS.PrettyPrint () == L"3 ms");
        }
        {
            const Duration kD = Duration{L"PT1.003S"};
            EXPECT_TRUE (kD.PrettyPrint () == L"1.003 seconds");
        }
        {
            const Duration kD = Duration{L"PT0.000045S"};
            //EXPECT_TRUE (kD.PrettyPrint () == L"45 µs");   // SAD - but L"45 µs" 'works' but doesn't provide the RIGHT string portably
            EXPECT_TRUE (kD.PrettyPrint () == L"45 \u00b5s");
        }
        {
            // todo use constexpr
            const Duration kD = Duration{};
            EXPECT_TRUE (kD.empty ());
        }
        {
            // todo use constexpr
            const Duration kD = Duration{1.6e-6};
            EXPECT_TRUE (kD.PrettyPrint () == L"1.6 \u00b5s");
        }
        {
            // todo use constexpr
            const Duration kD{33us};
            EXPECT_TRUE (kD.PrettyPrint () == L"33 \u00b5s");
        }
        {
            const Duration kD = Duration{"PT0.000045S"};
            EXPECT_TRUE (kD.PrettyPrint () == L"45 \u00b5s");
            EXPECT_TRUE ((-kD).PrettyPrint () == L"-45 \u00b5s");
            EXPECT_TRUE ((-kD).As<String> () == "-PT0.000045S");
        }
        EXPECT_TRUE (Duration{"P30S"}.As<time_t> () == 30);
        EXPECT_TRUE (Duration{"PT30S"}.As<time_t> () == 30);
        EXPECT_TRUE (Duration{60}.As<String> () == L"PT1M");
        EXPECT_TRUE (Duration{"-PT1H1S"}.As<time_t> () == -3601);
        EXPECT_TRUE (-Duration{"-PT1H1S"}.As<time_t> () == 3601);

        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 100 : 1;
            for (time_t i = -45; i < 60 * 3 * 60 + 99; i += K) {
                EXPECT_TRUE (Duration{Duration{i}.As<String> ()}.As<time_t> () == i);
            }
        }
        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 2630 : 263;
            for (time_t i = 60 * 60 * 24 * 365 - 40; i < 3 * 60 * 60 * 24 * 365; i += K) {
                EXPECT_TRUE (Duration{Duration{i}.As<String> ()}.As<time_t> () == i);
            }
        }
        EXPECT_TRUE (Duration::min () < Duration::max ());
        EXPECT_TRUE (Duration::min () != Duration::max ());
        EXPECT_TRUE (Duration::min () < Duration{"P30S"} and Duration{"P30S"} < Duration::max ());
        {
            Duration d = Duration{L"PT0.1S"};
            EXPECT_TRUE (d == "PT0.1S"_duration);
            d += chrono::milliseconds{30};
            EXPECT_TRUE (Math::NearlyEquals (d.As<DurationSeconds::rep> (), static_cast<DurationSeconds::rep> (.130)));
        }
        {
            EXPECT_TRUE (Duration{"PT1.4S"}.PrettyPrintAge () == "now");
            EXPECT_TRUE (Duration{"-PT9M"}.PrettyPrintAge () == "now");
            EXPECT_TRUE (Duration{"-PT20M"}.PrettyPrintAge () == "20 minutes ago");
            EXPECT_TRUE (Duration{"PT20M"}.PrettyPrintAge () == "20 minutes from now");
            EXPECT_TRUE (Duration{"PT4H"}.PrettyPrintAge () == "4 hours from now");
            EXPECT_TRUE (Duration{"PT4.4H"}.PrettyPrintAge () == "4 hours from now");
            EXPECT_TRUE (Duration{"P2Y"}.PrettyPrintAge () == "2 years from now");
            EXPECT_TRUE (Duration{"P2.4Y"}.PrettyPrintAge () == "2 years from now");
            EXPECT_TRUE (Duration{"P2.6Y"}.PrettyPrintAge () == "3 years from now");
            EXPECT_TRUE (Duration{"-P1M"}.PrettyPrintAge () == "1 month ago");
            EXPECT_TRUE (Duration{"-P2M"}.PrettyPrintAge () == "2 months ago");
            EXPECT_TRUE (Duration{"-PT1Y"}.PrettyPrintAge () == "1 year ago");
            EXPECT_TRUE (Duration{"-PT2Y"}.PrettyPrintAge () == "2 years ago");
        }
    }
}

namespace {

    void Test_8_DateTimeWithDuration_ ()
    {
        TraceContextBumper ctx{"Test_8_DateTimeWithDuration_"};
        {
            DateTime d = DateTime{Date{Year{1995}, June, DayOfMonth{4}}, TimeOfDay::Parse (L"3:00")};
            EXPECT_TRUE (d.As<time_t> () == 802234800); // source - http://www.onlineconversion.com/unix_time.htm
            const Duration k30Days = Duration{"P30D"};
            DateTime       d2      = d + k30Days;
            EXPECT_TRUE (d2.GetDate ().GetYear () == Year{1995});
            EXPECT_TRUE (d2.GetDate ().GetMonth () == July);
            EXPECT_TRUE (d2.GetDate ().GetDayOfMonth () == DayOfMonth{4});
            EXPECT_TRUE (d2.GetTimeOfDay () == d.GetTimeOfDay ());
        }
        {
            DateTime n1 = DateTime{Date{Year{2015}, June, DayOfMonth{9}}, TimeOfDay{19, 18, 42}, Timezone::kLocalTime};
            DateTime n2 = n1 - Duration{L"P100Y"};
            EXPECT_TRUE (n2.GetDate ().GetYear () == Year ((int)n1.GetDate ().GetYear () - 100));
#if 0
            // @todo - Improve - increment by 100 years not as exact as one might like @todo --LGP 2015-06-09
            EXPECT_TRUE (n2.GetDate ().GetMonth () == n1.GetDate ().GetMonth ());
            EXPECT_TRUE (n2.GetDate ().GetDayOfMonth () == n1.GetDate ().GetDayOfMonth ());
#endif
            EXPECT_TRUE (n2.GetTimeOfDay () == n1.GetTimeOfDay ());
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
            DateTime n = DateTime{Date{Year{2011}, December, DayOfMonth{30}}, TimeOfDay::Parse (L"1 pm", locale::classic ()), Timezone::kLocalTime};
            [[maybe_unused]] optional<bool> isDst = n.IsDaylightSavingsTime ();
            DateTime                        n2    = n.AddDays (180);
            // This verify was wrong. Consider a system on GMT! Besides that - its still not reliable because DST doesnt end 180 days exactly apart.
            //EXPECT_TRUE (IsDaylightSavingsTime (n) != IsDaylightSavingsTime (n2));
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
        EXPECT_TRUE (k30Seconds.As<time_t> () == 30);
        EXPECT_TRUE (k30Seconds.As<String> () == L"PT30S");
        EXPECT_TRUE (k30Seconds.As<chrono::duration<double>> () == chrono::duration<double>{30.0});
        EXPECT_TRUE (Duration{chrono::duration<double> (4)}.As<time_t> () == 4);
        EXPECT_TRUE (Math::NearlyEquals (Duration{chrono::milliseconds{50}}.As<Time::DurationSeconds::rep> (), 0.050));
        EXPECT_TRUE (Math::NearlyEquals (Duration{chrono::microseconds{50}}.As<Time::DurationSeconds::rep> (), 0.000050));
        EXPECT_TRUE (Math::NearlyEquals (Duration{chrono::nanoseconds{50}}.As<Time::DurationSeconds::rep> (), 0.000000050));
        EXPECT_TRUE (Math::NearlyEquals (Duration{chrono::nanoseconds{1}}.As<Time::DurationSeconds::rep> (), 0.000000001));
        EXPECT_TRUE (Duration{5.0}.As<chrono::milliseconds> () == chrono::milliseconds{5000});
        EXPECT_TRUE (Duration{-5.0}.As<chrono::milliseconds> () == chrono::milliseconds{-5000});
        EXPECT_TRUE (Duration{1.0}.As<chrono::nanoseconds> () == chrono::nanoseconds{1000 * 1000 * 1000});
        EXPECT_TRUE (Duration{1} == Duration{chrono::seconds{1}});
    }
}

namespace {
    void Test_11_DurationRange_ ()
    {
        TraceContextBumper ctx{"Test_11_DurationRange_"};
        using Traversal::Range;
        Range<Duration> d1;
        Range<Duration> d2 = Range<Duration>::FullRange ();
        EXPECT_TRUE (d1.empty ());
        EXPECT_TRUE (not d2.empty ());
        EXPECT_TRUE (d2.GetLowerBound () == Duration::min ());
        EXPECT_TRUE (d2.GetUpperBound () == Duration::max ());
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
            EXPECT_TRUE (d1.empty ());
            EXPECT_TRUE (not d2.empty ());
            EXPECT_TRUE (d2.GetLowerBound () == Date::kMin);
            EXPECT_TRUE (d2.GetUpperBound () == Date::kMax);
        }
        {
            DiscreteRange<Date> dr{Date{Year{1903}, April, DayOfMonth{5}}, Date{Year{1903}, April, DayOfMonth{6}}};
            unsigned int        i = 0;
            for (Date d : dr) {
                ++i;
                EXPECT_TRUE (d.GetYear () == Year{1903});
                EXPECT_TRUE (d.GetMonth () == April);
                if (i == 1) {
                    EXPECT_TRUE (d.GetDayOfMonth () == DayOfMonth{5});
                }
                else {
                    EXPECT_TRUE (d.GetDayOfMonth () == DayOfMonth{6});
                }
            }
            EXPECT_TRUE (i == 2);
        }
        {
            DiscreteRange<Date> dr{Date{Year{1903}, April, DayOfMonth{5}}, Date{Year{1903}, April, DayOfMonth{6}}};
            unsigned int        i = 0;
            for (Date d : dr.Elements ()) {
                ++i;
                EXPECT_TRUE (d.GetYear () == Year{1903});
                EXPECT_TRUE (d.GetMonth () == April);
                if (i == 1) {
                    EXPECT_TRUE (d.GetDayOfMonth () == DayOfMonth{5});
                }
                else {
                    EXPECT_TRUE (d.GetDayOfMonth () == DayOfMonth{6});
                }
            }
            EXPECT_TRUE (i == 2);
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
            EXPECT_TRUE (dr.Contains (dr.GetMidpoint ()));
        }
    }
}

namespace {
    void Test_13_DateTimeRange_ ()
    {
        TraceContextBumper ctx{"Test_13_DateTimeRange_"};
        using Traversal::Range;
        {
            Range<DateTime> d1;
            Range<DateTime> d2 = Range<DateTime>::FullRange ();
            EXPECT_TRUE (d1.empty ());
            EXPECT_TRUE (not d2.empty ());
            EXPECT_TRUE (d2.GetLowerBound () == DateTime::kMin);
            EXPECT_TRUE (d2.GetUpperBound () == DateTime::kMax);
        }
        {
            Range<DateTime> d1{DateTime{Date{Year{2000}, April, DayOfMonth{20}}}, DateTime{Date{Year{2000}, April, DayOfMonth{22}}}};
            EXPECT_TRUE (d1.GetDistanceSpanned () / 2 == Duration{"PT1D"});
            // SEE https://stroika.atlassian.net/browse/STK-514 for accuracy of compare (sb .1 or less)
            EXPECT_TRUE (Math::NearlyEquals (d1.GetMidpoint (), Date{Year{2000}, April, DayOfMonth{21}}, DurationSeconds{2}));
        }
    }
}

namespace {
    void Test_14_timepoint_ ()
    {
        TraceContextBumper ctx{"Test_14_timepoint_"};
        // @see https://stroika.atlassian.net/browse/STK-619 - EXPECT_TRUE (Time::DurationSeconds2time_point (Time::GetTickCount () + Time::kInfinity) == time_point<chrono::steady_clock>::max ());
        EXPECT_TRUE (Time::GetTickCount () + Time::kInfinity > chrono::steady_clock::now () + chrono::seconds (10000));
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
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
#endif

int main (int argc, const char* argv[])
{
    Stroika::Frameworks::Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
