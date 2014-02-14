/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::PERFORMANCE
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Debug/Assertions.h"

#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/Realtime.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include    "Stroika/Foundation/Traversal/Generator.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Math;
using   namespace   Stroika::Foundation::Time;


// Turn this on rarely to calibrate so # runs a good test
#define   qPrintOutIfBaselineOffFromOneSecond (!qDebug)






namespace {

    DurationSecondsType RunTest_(function<void()> t, unsigned int runCount)
    {
        DurationSecondsType start = Time::GetTickCount ();
        for (unsigned int i = 0; i < runCount; ++i) {
            t();
        }
        return Time::GetTickCount () - start;
    }

    void    Tester (String testName,
                    function<void()> baselineT, String baselineTName,
                    function<void()> compareWithT, String compareWithTName,
                    unsigned int runCount,
                    double expectedPercentFaster,
    function<void(String testName, String baselineTName, String compareWithTName, double expectedPercentFaster, DurationSecondsType baselineTime, DurationSecondsType compareWithTime)> printResults = [] (String testName, String baselineTName, String compareWithTName, double expectedPercentFaster, DurationSecondsType baselineTime, DurationSecondsType compareWithTime) -> void {
        cout << "Test " << testName.AsNarrowSDKString () << " (" << baselineTName.AsNarrowSDKString () << " vs " << compareWithTName.AsNarrowSDKString ()  << ")" << endl;
        DurationSecondsType totalTime = baselineTime + compareWithTime;
        double ratio = compareWithTime / baselineTime;
        double  changePct   =   (1 - ratio) * 100.0;
        if (changePct >= 0)
        {
            cout << "    " << compareWithTName.AsNarrowSDKString () << " is " << changePct << "% faster";
        }
        else {
            cout << "    " << compareWithTName.AsNarrowSDKString () << " is " << (-changePct) << "% slower";
        }
        if (changePct < expectedPercentFaster)
        {
            cout << " {{{WARNING - expected at least " << expectedPercentFaster << " faster}}}";
        }
        cout << " [baseline test " << baselineTime << " seconds, and comparison " << compareWithTime << " seconds]" << endl;

    }
                   )
    {
        DurationSecondsType time1 = RunTest_ (baselineT, runCount);
        DurationSecondsType time2 = RunTest_ (compareWithT, runCount);
#if qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals (time1, 1, .1)) {
            cout << "SUGGESTION: Baseline Time: " << time1 << " and runCount = " << runCount << " so try using runCount = " << int (runCount / time1) << endl;
        }
#endif
        printResults (testName, baselineTName, compareWithTName, expectedPercentFaster, time1, time2);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_StructWithStringsFillingAndCopying()
    {
        struct  S {
            WIDESTRING_IMPL fS1;
            WIDESTRING_IMPL fS2;
            WIDESTRING_IMPL fS3;
            WIDESTRING_IMPL fS4;
            S() {}
            S (const WIDESTRING_IMPL& w1, const WIDESTRING_IMPL& w2, const WIDESTRING_IMPL& w3, const WIDESTRING_IMPL& w4)
                : fS1(w1)
                , fS2(w2)
                , fS3(w3)
                , fS4(w4)
            {
            }
        };
        S   s1;
        S   s2 (L"hi mom", L"124 south vanbergan highway", L"Los Angeles 201243", L"834-313-2144");
        s1 = s2;
        vector<S>   v;
        for (size_t i = 1; i < 10; ++i) {
            v.push_back (s2);
        }
        sort (v.begin (), v.end (), [](S a, S b) {
            return b.fS1 < a.fS1;
        });
        VerifyTestResult (v[0].fS1 == v[1].fS1);
    }

}









namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends1_()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        WIDESTRING_IMPL w = KBase;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == KBase.length () * 11);
    }

}






namespace   {
    void    RunPerformanceTests_ ()
    {
        Tester (L"Simple Struct With Strings Filling And Copying",
                Test_StructWithStringsFillingAndCopying<wstring>, L"wstring",
                Test_StructWithStringsFillingAndCopying<String>, L"Charactes::String",
                40000,
                6
               );
        Tester (L"Simple String append test (+=STROBJ) 10x",
                Test_SimpleStringAppends1_<wstring>, L"wstring",
                Test_SimpleStringAppends1_<String>, L"Charactes::String",
                1172017,
                -1800
               );
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (RunPerformanceTests_);
    return EXIT_SUCCESS;
}

