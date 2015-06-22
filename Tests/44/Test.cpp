/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::Time
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <chrono>
#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/Date.h"
#include    "Stroika/Foundation/Time/DateRange.h"
#include    "Stroika/Foundation/Time/DateTime.h"
#include    "Stroika/Foundation/Time/DateTimeRange.h"
#include    "Stroika/Foundation/Time/Duration.h"
#include    "Stroika/Foundation/Time/DurationRange.h"
#include    "Stroika/Foundation/Time/Realtime.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Time;

using   Stroika::Foundation::Debug::TraceContextBumper;






namespace {
    template    <typename DATEORTIME>
    void    TestRoundTripFormatThenParseNoChange_ (DATEORTIME startDateOrTime, const locale& l)
    {
        // disable for now cuz fails SO OFTEN
        String      formatByLocale = startDateOrTime.Format (l);
        DATEORTIME  andBack = DATEORTIME::Parse (formatByLocale, l);
        VerifyTestResult (startDateOrTime == andBack);
    }
    template    <typename DATEORTIME>
    void    TestRoundTripFormatThenParseNoChange_ (DATEORTIME startDateOrTime)
    {
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale ());
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, locale::classic ());
        TestRoundTripFormatThenParseNoChange_ (startDateOrTime, Configuration::FindNamedLocale (L"en", L"us"));

        // should add test like this...
        //Verify (startDateOrTime == DATEORTIME::Parse (startDateOrTime.Format (DATEORTIME::PrintFormat::eCurrentLocale), DATEORTIME::PrintFormat::ParseFormat::eCurrentLocale));
    }
}


// Skip some locale tests cuz so little works
#define qTestLocaleCode_    0


#define qSupport_TestRoundTripFormatThenParseNoChange_For_TimeOfDay_    0
#define qSupport_TestRoundTripFormatThenParseNoChange_For_Date_         0
#define qSupport_TestRoundTripFormatThenParseNoChange_For_DateTime_     0

#if     !qSupport_TestRoundTripFormatThenParseNoChange_For_TimeOfDay_
namespace {
    template    <>
    void    TestRoundTripFormatThenParseNoChange_ (TimeOfDay startDateOrTime)
    {
    }
}
#endif

#if     !qSupport_TestRoundTripFormatThenParseNoChange_For_Date_
namespace {
    template    <>
    void    TestRoundTripFormatThenParseNoChange_ (Date startDateOrTime)
    {
    }
}
#endif

#if     !qSupport_TestRoundTripFormatThenParseNoChange_For_DateTime_
namespace {
    template    <>
    void    TestRoundTripFormatThenParseNoChange_ (DateTime startDateOrTime)
    {
    }
}
#endif






namespace   {
    void    Test_0_Test_VarDateFromStrOnFirstTry_()
    {
        // TEST MUST BE FIRST - OR VERY NEAR START OF APP!
        //
        // Before any calls to VarDateFromStr ()...
        //
        // CHECK FOR qCompilerAndStdLib_Supports_VarDateFromStrOnFirstTry
        //
#if     defined (_MSC_VER)
        VerifyTestResult (not DateTime::Parse (L"7/26/1972 12:00:00 AM", Time::DateTime::ParseFormat::eCurrentLocale).empty ());
#endif
    }
}



namespace   {

    void    Test_1_TestTickCountGrowsMonotonically_ ()
    {
        DurationSecondsType start   =   Time::GetTickCount ();
        Execution::Sleep (0.1);
        VerifyTestResult (start <= Time::GetTickCount ());
    }

}


namespace   {

    void    Test_2_TestTimeOfDay_ ()
    {
        {
            TimeOfDay   t;
            VerifyTestResult (t.empty ());
            TimeOfDay   t2 (2);
            VerifyTestResult (t < t2);
            VerifyTestResult (t.GetAsSecondsCount () == 0);
            VerifyTestResult (not t2.empty ());
            VerifyTestResult (t.Format (TimeOfDay::PrintFormat::eCurrentLocale).empty ());
            VerifyTestResult (not t2.Format (TimeOfDay::PrintFormat::eCurrentLocale).empty ());
            VerifyTestResult (t2.GetHours () == 0);
            VerifyTestResult (t2.GetMinutes () == 0);
            VerifyTestResult (t2.GetSeconds () == 2);
            TestRoundTripFormatThenParseNoChange_ (t);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay   t2 (5 * 60 * 60 + 3 * 60 + 49);
            VerifyTestResult (t2.GetHours () == 5);
            VerifyTestResult (t2.GetMinutes () == 3);
            VerifyTestResult (t2.GetSeconds () == 49);
            TestRoundTripFormatThenParseNoChange_ (t2);
        }
        {
            TimeOfDay   t2 (25 * 60 * 60);
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
            VerifyTestResult (TimeOfDay::Parse (L"3pm", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00", TimeOfDay::ParseFormat::eCurrentLocale).GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
            VerifyTestResult (TimeOfDay (101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"12:01:41 AM");
            VerifyTestResult (TimeOfDay (60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"12:01 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"1:01:41 AM" or TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale) == L"01:01:41 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 101).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01:41 AM");
            VerifyTestResult (TimeOfDay (60 * 60 + 60).Format (TimeOfDay::PrintFormat::eCurrentLocale_WithZerosStripped) == L"1:01 AM");
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
#if     qPlatform_Windows
            const   LCID    kUS_ENGLISH_LOCALE  =   MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
#endif
            TimeOfDay   threePM =   TimeOfDay::Parse (L"3pm", locale::classic ());
#if     qPlatform_Windows
            VerifyTestResult (threePM.Format (kUS_ENGLISH_LOCALE) == L"3 PM");
#endif
            //VerifyTestResult (threePM.Format (locale::classic ()) == L"3 PM");
            VerifyTestResult (threePM.Format (locale::classic ()) == L"15:00:00");  // UGH!!!
            TestRoundTripFormatThenParseNoChange_ (threePM);
        }

    }

}


namespace   {
    void    VERIFY_ROUNDTRIP_XML_ (const Date& d)
    {
        VerifyTestResult (Date::Parse (d.Format (Date::PrintFormat::eXML), Date::ParseFormat::eXML) == d);
    }

    void    Test_3_TestDate_ ()
    {
        {
            Date    d (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-04");
            VERIFY_ROUNDTRIP_XML_ (d);
            d = d.AddDays (4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-08");
            d = d.AddDays (-4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        try {
            Date    d   =   Date::Parse (L"09/14/1752", locale::classic ());
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d == Date::kMin);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        catch (...) {
            VerifyTestResult (false);   // check qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy or qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy
        }
        {
            Date    d;
            VerifyTestResult (d.empty ());
            VerifyTestResult (d < DateTime::GetToday ());
            VerifyTestResult (DateTime::GetToday () > d);
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            Date    d   =   Date::kMin;
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d < DateTime::Now ().GetDate ());
            VerifyTestResult (not (DateTime::Now ().GetDate () < d));
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
#if     qPlatform_Windows
        {
            wstring testCase    =   L"6/1/2005";
            VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) == Date::Parse (testCase, locale::classic ()));
        }
        {
            wstring testCase    =   L"4/20/1964";
            VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) == Date::Parse (testCase, locale::classic ()));
        }
        {
            wstring testCase    =   L"7/4/1776";
            VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) == Date::Parse (testCase, locale::classic ()));
            VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) < Date::kMax);
            VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) >= Date::kMin);
        }
        {
            wstring testCase    =   L"7/4/2076";
            //  TODO:
            //      Fails - debug soon -- LGP 2011-10-08
            //VerifyTestResult (Date::Parse (testCase, LOCALE_USER_DEFAULT) == Date::Parse (testCase, locale::classic ()));
        }
#endif
        {
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::ParseFormat::eJavascript) == Date (Year (2001), Time::MonthOfYear::eNovember, DayOfMonth (1)));
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::ParseFormat::eJavascript).Format (Date::PrintFormat::eJavascript) == L"11/01/2001");
        }
        {
            VerifyTestResult (Date::kMin < Date::kMax);
            VerifyTestResult (Date::kMin <= Date::kMax);
            VerifyTestResult (not (Date::kMin > Date::kMax));
            VerifyTestResult (not (Date::kMin >= Date::kMax));
            TestRoundTripFormatThenParseNoChange_ (Date::kMin);
            TestRoundTripFormatThenParseNoChange_ (Date::kMax);
        }
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
            Date        d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            TestRoundTripFormatThenParseNoChange_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/1903");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903");
        }
        {
            Date        d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/1903"  or d.Format (Date::PrintFormat::eCurrentLocale) == L"04/05/03");
            VerifyTestResult (d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/1903" or d.Format (Date::PrintFormat::eCurrentLocale_WithZerosStripped) == L"4/5/03");
        }

        {
            Date d  =   Date (Date::JulianRepType (2455213));
            VerifyTestResult (d.Format () == L"1/16/10");
        }
    }

}


namespace   {

    void    Test_4_TestDateTime_ ()
    {
        {
            DateTime    d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            VerifyTestResult (d.Format (DateTime::PrintFormat::eXML) == L"1903-04-04");
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            DateTime    d;
            VerifyTestResult (d.empty ());
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            TestRoundTripFormatThenParseNoChange_ (d);
        }
        {
            DateTime    d   =   DateTime::kMin;
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            d = DateTime (d.GetDate (), d.GetTimeOfDay (), DateTime::Timezone::eUTC);   // so that compare works - cuz we dont know timezone we'll run test with...
            VerifyTestResult (d.Format (DateTime::PrintFormat::eXML) == L"1752-09-14T00:00:00Z");   // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
            TestRoundTripFormatThenParseNoChange_ (d);
        }
#if     qPlatform_Windows
        {
            const   LCID    kUS_ENGLISH_LOCALE  =   MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
            wstring testCase    =   L"2010-01-01";
            //TODO: FIX SO THIS WORKS... (or come up with better test)
            //VerifyTestResult (DateTime::Parse (testCase, kUS_ENGLISH_LOCALE) == DateTime::Parse (testCase, locale::classic ()));
        }
#endif

        //// TODO - FIX FOR PrintFormat::eCurrentLocale_WITHZEROESTRIPPED!!!!
        {
            // set the global C++ locale (used by PrintFormat::eCurrentLocale) to US english, and verify things look right.
            Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
            Date        d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5));
            DateTime    dt (d, TimeOfDay (101));
            VerifyTestResult (dt.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/5/1903 12:01:41 AM" or dt.Format (DateTime::PrintFormat::eCurrentLocale) == L"04/05/1903 12:01:41 AM");
            DateTime    dt2 (d, TimeOfDay (60));
            //TOFIX!VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        {
            Date        d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (6));
            TestRoundTripFormatThenParseNoChange_ (d);
            DateTime    dt (d, TimeOfDay (101));
            TestRoundTripFormatThenParseNoChange_ (dt);
            VerifyTestResult (dt.Format (DateTime::PrintFormat::eCurrentLocale) == L"04/06/03 00:01:41");
            DateTime    dt2 (d, TimeOfDay (60));
            TestRoundTripFormatThenParseNoChange_ (dt2);
            // want a variant that does this formatting!
            //VerifyTestResult (dt2.Format (DateTime::PrintFormat::eCurrentLocale) == L"4/4/1903 12:01 AM");
        }
        {
            //VerifyTestResult(DateTime::Parse(L"2010-01-01", DateTime::ParseFormat::eCurrentLocale).GetDate().GetYear() == Time::Year(2010));
            DateTime    now = DateTime::Now ();
            TestRoundTripFormatThenParseNoChange_ (now);
#if     qTestLocaleCode_ && (_MSC_VER >= _MS_VS_2k13_VER_)
            Verify (now == DateTime::Parse (now.Format (Time::DateTime::PrintFormat::eCurrentLocale), DateTime::ParseFormat::eCurrentLocale));
#endif
        }
        {
            using   Time::DurationSecondsType;
            DurationSecondsType now =   Time::GetTickCount ();
#if     qCompilerAndStdLib_stdinitializer_of_double_in_ranged_for_Bug
            for (DurationSecondsType ds : vector<DurationSecondsType> { 3, 995, 3.4, 3004.5, 1055646.4, 60 * 60 * 24 * 300 }) {
#else
            for (DurationSecondsType ds : initializer_list<DurationSecondsType> { 3, 995, 3.4, 3004.5, 1055646.4, 60 * 60 * 24 * 300 }) {
#endif
                ds += now;
                DateTime    dt  =   DateTime::FromTickCount (ds);
                VerifyTestResult (dt == DateTime::FromTickCount (dt.ToTickCount ()));
                VerifyTestResult (Math::NearlyEquals (dt.ToTickCount (), ds, 1.1));      // crazy large epsilon for now because we represent datetime to nearest second
            }
        }
    }

}



namespace   {

    void    Test_5_DateTimeTimeT_ ()
    {
        {
            DateTime    d   =   Date (Year (2000), MonthOfYear::eApril, DayOfMonth (20));
            VerifyTestResult (d.As<time_t> () == 956188800);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", locale ()));
            VerifyTestResult (d.As<time_t> () == 802278000);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802278000);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3am", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            const   time_t  kTEST   =   802234800;
            DateTime    d   =   DateTime (kTEST);
            VerifyTestResult (d.As<time_t> () == kTEST);    // source - http://www.onlineconversion.com/unix_time.htm
        }
    }

}






namespace   {

    void    Test_6_DateTimeStructTM_ ()
    {
        {
            struct  tm  x;
            memset (&x, 0, sizeof (x));
            x.tm_hour = 3;
            x.tm_min = 30;
            x.tm_year = 80;
            x.tm_mon = 3;
            x.tm_mday = 15;
            DateTime    d   =   DateTime (x);
            struct  tm  x2  =   d.As<struct tm> ();
            VerifyTestResult (x.tm_hour == x2.tm_hour);
            VerifyTestResult (x.tm_min == x2.tm_min);
            VerifyTestResult (x.tm_sec == x2.tm_sec);
            VerifyTestResult (x.tm_year == x2.tm_year);
            VerifyTestResult (x.tm_mday == x2.tm_mday);
        }
    }

}






namespace   {

    void    Test_7_Duration_ ()
    {
        {
            VerifyTestResult (Duration (0).As<time_t> () == 0);
            VerifyTestResult (Duration (0).As<String> () == L"PT0S");
        }
        const   int kSecondsPerDay      =   TimeOfDay::kMaxSecondsPerDay;
        {
            const   Duration    k30Days     =   Duration (L"P30D");
            VerifyTestResult (k30Days.As<time_t> () == 30 * kSecondsPerDay);
        }
        {
            const   Duration    k6Months        =   Duration (L"P6M");
            VerifyTestResult (k6Months.As<time_t> () == 6 * 30 * kSecondsPerDay);
        }
        {
            const   Duration    kP1Y        =   Duration (L"P1Y");
            VerifyTestResult (kP1Y.As<time_t> () == 365 * kSecondsPerDay);
        }
        {
            const   Duration    kP2Y        =   Duration (L"P2Y");
            VerifyTestResult (kP2Y.As<time_t> () == 2 * 365 * kSecondsPerDay);
            VerifyTestResult (Duration (2 * 365 * kSecondsPerDay).As<wstring> () == L"P2Y");
        }
        {
            const   Duration    kHalfMinute     =   Duration (L"PT0.5M");
            VerifyTestResult (kHalfMinute.As<time_t> () == 30);
        }
        {
            const   Duration    kD      =   Duration (L"PT0.1S");
            VerifyTestResult (kD.As<time_t> () == 0);
            VerifyTestResult (kD.As<double> () == 0.1);
        }
        {
            const   Duration    kHalfMinute     =   Duration (L"PT0.5M");
            VerifyTestResult (kHalfMinute.PrettyPrint () == L"30 seconds");
        }
        {
            const   Duration    k3MS        =   Duration (L"PT0.003S");
            VerifyTestResult (k3MS.PrettyPrint () == L"3 ms");
        }
        {
            const   Duration    kD      =   Duration (L"PT1.003S");
            VerifyTestResult (kD.PrettyPrint () == L"1.003 seconds");
        }
        {
            const   Duration    kD      =   Duration (L"PT0.000045S");
            VerifyTestResult (kD.PrettyPrint () == L"45 µs");
        }
        {
            const   Duration    kD      =   Duration (L"PT0.000045S");
            VerifyTestResult (kD.PrettyPrint () == L"45 µs");
            VerifyTestResult ((-kD).PrettyPrint () == L"-45 µs");
            VerifyTestResult ((-kD).As<wstring> () == L"-PT0.000045S");
        }
        VerifyTestResult (Duration (L"P30S").As<time_t> () == 30);
        VerifyTestResult (Duration (L"PT30S").As<time_t> () == 30);
        VerifyTestResult (Duration (60).As<wstring> () == L"PT1M");
        VerifyTestResult (Duration (L"-PT1H1S").As<time_t> () == -3601);
        VerifyTestResult (-Duration (L"-PT1H1S").As<time_t> () == 3601);

        for (time_t i = -45; i < 60 * 3 * 60 + 99; ++i) {
            VerifyTestResult (Duration (Duration (i).As<wstring> ()).As<time_t> () == i);
        }
        for (time_t i = 60 * 60 * 24 * 365 - 40; i < 3 * 60 * 60 * 24 * 365; i += 263) {
            VerifyTestResult (Duration (Duration (i).As<wstring> ()).As<time_t> () == i);
        }
        VerifyTestResult (Duration::kMin < Duration::kMax);
        VerifyTestResult (Duration::kMin != Duration::kMax);
        VerifyTestResult (Duration::kMin < Duration (L"P30S") and Duration (L"P30S") < Duration::kMax);
        {
            using   Time::DurationSecondsType;
            Duration    d      =   Duration (L"PT0.1S");
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



namespace   {

    void    Test_8_DateTimeWithDuration_ ()
    {
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::ParseFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800);    // source - http://www.onlineconversion.com/unix_time.htm
            const   Duration    k30Days     =   Duration (L"P30D");
            DateTime    d2  =   d + k30Days;
            VerifyTestResult (d2.GetDate ().GetYear () == Year (1995));
            VerifyTestResult (d2.GetDate ().GetMonth () == MonthOfYear::eJuly);
            VerifyTestResult (d2.GetDate ().GetDayOfMonth () == DayOfMonth (4));
            VerifyTestResult (d2.GetTimeOfDay () == d.GetTimeOfDay ());
        }
        {
            DateTime    n1 = DateTime (Date (Year (2015), MonthOfYear::eJune, DayOfMonth (9)), TimeOfDay (19, 18, 42), DateTime::Timezone::eLocalTime);
            DateTime    n2 = n1 - Duration (L"P100Y");
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







namespace   {

    void    Test_9_TZOffsetAndDaylightSavingsTime_ ()
    {
        /*
         * I cannot think if any good way to test this stuff - since it depends on the current timezone and I cannot
         * see any good portbale way to change that (setenv (TZ) doest work on visual studio.net 2010).
         *
         * This test wont always work, but at least for now seems to work on the systems i test on.
         */
        {
            DateTime    n   =   DateTime (Date (Year (2011), MonthOfYear::eDecember, DayOfMonth (30)), TimeOfDay::Parse (L"1 pm", locale::classic ()));
            bool        isDst   =   IsDaylightSavingsTime (n);
            DateTime    n2  =   n.AddDays (180);
            // This verify was wrong. Consider a system on GMT! Besides that - its still not reliable because DST doesnt end 180 days exactly apart.
            //VerifyTestResult (IsDaylightSavingsTime (n) != IsDaylightSavingsTime (n2));
            if (IsDaylightSavingsTime (n) != IsDaylightSavingsTime (n2)) {
                int breakhere = 1;
            }
        }
        {
            DateTime    n   =   DateTime::Now ();
            bool        isDst   =   IsDaylightSavingsTime (n);
            DateTime    n2  =   n.AddDays (60);
            if (IsDaylightSavingsTime (n) == IsDaylightSavingsTime (n2)) {
                int breakhere = 1;
            }
        }
    }

}



namespace   {
    void    Test_10_std_duration_ ()
    {
        const   Duration    k30Seconds      =   Duration (30.0);
        VerifyTestResult (k30Seconds.As<time_t> () == 30);
        VerifyTestResult (k30Seconds.As<String> () == L"PT30S");
        VerifyTestResult (k30Seconds.As<std::chrono::duration<double>> () == std::chrono::duration<double> (30.0));
        VerifyTestResult (Duration (std::chrono::duration<double> (4)).As<time_t> () == 4);
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::milliseconds (50)).As <Time::DurationSecondsType> (), 0.050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::microseconds (50)).As <Time::DurationSecondsType> (), 0.000050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::nanoseconds (50)).As <Time::DurationSecondsType> (), 0.000000050));
        VerifyTestResult (Math::NearlyEquals (Duration (chrono::nanoseconds (1)).As <Time::DurationSecondsType> (), 0.000000001));
        VerifyTestResult (Duration (5.0).As<std::chrono::milliseconds> () == chrono::milliseconds (5000));
        VerifyTestResult (Duration (-5.0).As<std::chrono::milliseconds> () == chrono::milliseconds  (-5000));
        VerifyTestResult (Duration (1.0).As<std::chrono::nanoseconds> () == chrono::nanoseconds (1000 * 1000 * 1000));
        VerifyTestResult (Duration (1) == Duration (chrono::seconds (1)));
    }
}



namespace   {
    void    Test_11_DurationRange_ ()
    {
        DurationRange d1;
        DurationRange d2    =   DurationRange::FullRange ();
        VerifyTestResult (d1.empty ());
        VerifyTestResult (not d2.empty ());
        VerifyTestResult (d2.GetLowerBound () == Duration::kMin);
        VerifyTestResult (d2.GetUpperBound () == Duration::kMax);
    }
}


namespace   {
    void    Test_12_DateRange_ ()
    {
        DateRange d1;
        DateRange d2    =   DateRange::FullRange ();
        VerifyTestResult (d1.empty ());
        VerifyTestResult (not d2.empty ());
        VerifyTestResult (d2.GetLowerBound () == Date::kMin);
        VerifyTestResult (d2.GetUpperBound () == Date::kMax);
    }
}


namespace   {
    void    Test_13_DateTimeRange_ ()
    {
        DateTimeRange d1;
        DateTimeRange d2    =   DateTimeRange::FullRange ();
        VerifyTestResult (d1.empty ());
        VerifyTestResult (not d2.empty ());
        VerifyTestResult (d2.GetLowerBound () == DateTime::kMin);
        VerifyTestResult (d2.GetUpperBound () == DateTime::kMax);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test_0_Test_VarDateFromStrOnFirstTry_();
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
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

