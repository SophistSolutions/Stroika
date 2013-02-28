/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//	TEST	Foundation::Time
#include    "Stroika/Foundation/StroikaPreComp.h"

#if     qCompilerAndStdLib_Supports_stdchrono
#include    <chrono>
#endif

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/Sleep.h"
#include    "Stroika/Foundation/Time/Date.h"
#include    "Stroika/Foundation/Time/DateTime.h"
#include    "Stroika/Foundation/Time/Duration.h"
#include    "Stroika/Foundation/Time/Realtime.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Time;

using   Stroika::Foundation::Debug::TraceContextBumper;



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
            VerifyTestResult (t.Format ().empty ());
            VerifyTestResult (not t2.Format ().empty ());
            VerifyTestResult (t2.GetHours () == 0);
            VerifyTestResult (t2.GetMinutes () == 0);
            VerifyTestResult (t2.GetSeconds () == 2);
        }
        {
            TimeOfDay   t2 (5 * 60 * 60 + 3 * 60 + 49);
            VerifyTestResult (t2.GetHours () == 5);
            VerifyTestResult (t2.GetMinutes () == 3);
            VerifyTestResult (t2.GetSeconds () == 49);
        }
        {
            TimeOfDay   t2 (25 * 60 * 60);
            VerifyTestResult (t2.GetHours () == 23);
            VerifyTestResult (t2.GetMinutes () == 59);
            VerifyTestResult (t2.GetSeconds () == 59);
            VerifyTestResult (t2 == TimeOfDay::kMax);
        }
        {
            VerifyTestResult (TimeOfDay::Parse (L"3pm", locale::classic ()).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00", locale::classic ()).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00", locale::classic ()).GetAsSecondsCount () == 16 * 60 * 60);
        }
        {
            // Not sure these should ALWAYS work in any locale. Probably not. But any locale I'd test in??? Maybe... Good for starters anyhow...
            //      -- LGP 2011-10-08
            VerifyTestResult (TimeOfDay::Parse (L"3pm", TimeOfDay::PrintFormat::eCurrentLocale).GetAsSecondsCount () == 15 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3am", TimeOfDay::PrintFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"3:00", TimeOfDay::PrintFormat::eCurrentLocale).GetAsSecondsCount () == 3 * 60 * 60);
            VerifyTestResult (TimeOfDay::Parse (L"16:00", TimeOfDay::PrintFormat::eCurrentLocale).GetAsSecondsCount () == 16 * 60 * 60);
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
        }

    }

}


namespace   {
    void    VERIFY_ROUNDTRIP_XML_ (const Date& d)
    {
        VerifyTestResult (Date::Parse (d.Format (Date::PrintFormat::eXML), Date::PrintFormat::eXML) == d);
    }

    void    Test_3_TestDate_ ()
    {
        {
            Date    d (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-04");
            VERIFY_ROUNDTRIP_XML_ (d);
            d = d.AddDays (4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-08");
            d = d.AddDays (-4);
            VERIFY_ROUNDTRIP_XML_ (d);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1903-04-04");
        }
        {
            Date    d   =   Date::Parse (L"09/14/1752", locale::classic ());
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d == Date::kMin);
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
        }
        {
            Date    d;
            VerifyTestResult (d.empty ());
            VerifyTestResult (d < DateTime::GetToday ());
            VerifyTestResult (DateTime::GetToday () > d);
        }
        {
            Date    d   =   Date::kMin;
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d < DateTime::Now ().GetDate ());
            VerifyTestResult (not (DateTime::Now ().GetDate () < d));
            VerifyTestResult (d.Format (Date::PrintFormat::eXML) == L"1752-09-14"); // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
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
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::PrintFormat::eJavascript) == Date (Year (2001), Time::MonthOfYear::eNovember, DayOfMonth (1)));
            VerifyTestResult (Date::Parse (L"11/1/2001", Date::PrintFormat::eJavascript).Format (Date::PrintFormat::eJavascript) == L"11/01/2001");
        }
        {
            VerifyTestResult (Date::kMin < Date::kMax);
            VerifyTestResult (Date::kMin <= Date::kMax);
            VerifyTestResult (not (Date::kMin > Date::kMax));
            VerifyTestResult (not (Date::kMin >= Date::kMax));
        }
    }

}


namespace   {

    void    Test_4_TestDateTime_ ()
    {
        {
            DateTime    d   =   Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4));
            VerifyTestResult (d.Format (DateTime::PrintFormat::eXML) == L"1903-04-04");
        }
        {
            DateTime    d;
            VerifyTestResult (d.empty ());
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
        }
        {
            DateTime    d   =   DateTime::kMin;
            VerifyTestResult (not d.empty ());
            VerifyTestResult (d < DateTime::Now ());
            VerifyTestResult (DateTime::Now () > d);
            d = DateTime (d.GetDate (), d.GetTimeOfDay (), DateTime::Timezone::eUTC);   // so that compare works - cuz we dont know timezone we'll run test with...
            VerifyTestResult (d.Format (DateTime::PrintFormat::eXML) == L"1752-09-14T00:00:00Z");   // xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
        }
#if     qPlatform_Windows
        {
            const   LCID    kUS_ENGLISH_LOCALE  =   MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
            wstring testCase    =   L"2010-01-01";
            //TODO: FIX SO THIS WORKS... (or come up with better test)
            //VerifyTestResult (DateTime::Parse (testCase, kUS_ENGLISH_LOCALE) == DateTime::Parse (testCase, locale::classic ()));
        }
#endif
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
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", TimeOfDay::PrintFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802278000);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3am", TimeOfDay::PrintFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800);    // source - http://www.onlineconversion.com/unix_time.htm
        }
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::PrintFormat::eCurrentLocale));
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
    }

}



namespace   {

    void    Test_8_DateTimeWithDuration_ ()
    {
        {
            DateTime    d   =   DateTime (Date (Year (1995), MonthOfYear::eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::PrintFormat::eCurrentLocale));
            VerifyTestResult (d.As<time_t> () == 802234800);    // source - http://www.onlineconversion.com/unix_time.htm
            const   Duration    k30Days     =   Duration (L"P30D");
            DateTime    d2  =   d + k30Days;
            VerifyTestResult (d2.GetDate ().GetYear () == Year (1995));
            VerifyTestResult (d2.GetDate ().GetMonth () == MonthOfYear::eJuly);
            VerifyTestResult (d2.GetDate ().GetDayOfMonth () == DayOfMonth (4));
            VerifyTestResult (d2.GetTimeOfDay () == d.GetTimeOfDay ());
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
            VerifyTestResult (IsDaylightSavingsTime (n) != IsDaylightSavingsTime (n2));
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
#if     qCompilerAndStdLib_Supports_stdchrono
        const   Duration    k30Seconds      =   Duration (30.0);
        VerifyTestResult (k30Seconds.As<time_t> () == 30);
        VerifyTestResult (k30Seconds.As<std::chrono::duration<double>> () == std::chrono::duration<double> (30.0));
        VerifyTestResult (Duration (std::chrono::duration<double> (4)).As<time_t> () == 4);
#endif
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
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
    }
}




#if qOnlyOneMain
extern  int TestDateAndTime ()
#else
int main (int argc, const char* argv[])
#endif
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

