/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::PERFORMANCE
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <fstream>
#include    <mutex>
#include    <sstream>

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Execution/CommandLine.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/Realtime.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include    "Stroika/Foundation/Traversal/Generator.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Math;
using   namespace   Stroika::Foundation::Time;


// Turn this on rarely to calibrate so # runs a good test
#define   qPrintOutIfBaselineOffFromOneSecond (!qDebug && defined (_MSC_VER))


// My performance expectation numbers are calibrated for MSVC (2k13.net)
// Dont print when they differ on other platforms.
// This is only intended to alert me when something changes GROSSLY.
#define   qPrintOutIfFailsToMeetPerformanceExpectations (!qDebug && defined (_MSC_VER))


// Use this so when running #if qDebug case - we dont waste a ton of time with this test
#define   qDebugCaseRuncountRatio (.1)





namespace {
    const char kDefaultPerfOutFile_[]   =   "PERF-OUT.txt";
    bool    sShowOutput_    =   false;
}


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

        static  shared_ptr<ostream> out2File;
        if (not sShowOutput_ and out2File.get () == nullptr)
        {
            out2File.reset (new ofstream (kDefaultPerfOutFile_));
        }
        ostream&    outTo = (sShowOutput_ ? cout : *out2File);
        outTo << "Test " << testName.AsNarrowSDKString () << " (" << baselineTName.AsNarrowSDKString () << " vs " << compareWithTName.AsNarrowSDKString ()  << ")" << endl;
        DurationSecondsType totalTime = baselineTime + compareWithTime;
        double ratio = compareWithTime / baselineTime;
        double  changePct   =   (1 - ratio) * 100.0;
        if (changePct >= 0)
        {
            outTo << "    " << compareWithTName.AsNarrowSDKString () << " is " << changePct << "% faster";
        }
        else {
            outTo << "    " << compareWithTName.AsNarrowSDKString () << " is " << (-changePct) << "% slower";
        }
        outTo << " [baseline test " << baselineTime << " seconds, and comparison " << compareWithTime << " seconds]" << endl;
#if     qPrintOutIfFailsToMeetPerformanceExpectations
        if (changePct < expectedPercentFaster)
        {
            outTo << "    {{{WARNING - expected at least " << expectedPercentFaster << "% faster}}}" << endl;
        }
#endif

    }
                   )
    {
#if     qDebug
        runCount = static_cast<unsigned int> (runCount * qDebugCaseRuncountRatio);
#endif
        DurationSecondsType time1 = RunTest_ (baselineT, runCount);
        DurationSecondsType time2 = RunTest_ (compareWithT, runCount);
#if     qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals<DurationSecondsType> (time1, 1, .1)) {
            cerr << "SUGGESTION: Baseline Time: " << time1 << " and runCount = " << runCount << " so try using runCount = " << int (runCount / time1) << endl;
        }
#endif
        printResults (testName, baselineTName, compareWithTName, expectedPercentFaster, time1, time2);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_StructWithStringsFillingAndCopying()
    {
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
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
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wreorder\"");  // clang appears confused
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
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == KBase.length () * 10);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends2_()
    {
        const wchar_t KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen(KBase) * 10);
    }

}





namespace {

    template <typename WIDESTRING_IMPL>
    void    Test_SimpleStringAppends3_()
    {
        const wchar_t KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 100; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen(KBase) * 100);
    }

}






namespace {

    namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_ {
        mutex   Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex;
        int     Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int = 1;
        void    Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK(function<void(int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            lock_guard<mutex> critSec (Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex);
            doInsideLock (&Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int);
        }
        shared_ptr<int> Test_MutexVersusSharedPtrCopy_sharedPtrCase = shared_ptr<int> (new int (1));
        void    Test_MutexVersusSharedPtrCopy_SharedPtrCopy(function<void(int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I dont care about the (much rarer) write case where we really need to modify
            shared_ptr<int> tmp = Test_MutexVersusSharedPtrCopy_sharedPtrCase;
            doInsideLock (tmp.get ());
        }

        int s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT;
        void    Test_MutexVersusSharedPtrCopy_COUNTEST (int* i)
        {
            s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT += *i;
        }

    }

    void    Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000);   // so nothing optimized away
    }
    void    Test_MutexVersusSharedPtrCopy_shared_ptr_copy()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_SharedPtrCopy (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000);   // so nothing optimized away
    }

}








namespace {
    template <typename WIDESTRING_IMPL>
    void    Test_OperatorINSERT_ostream_ ()
    {
        using namespace std;
        WIDESTRING_IMPL kT1 =   L"abc";
        WIDESTRING_IMPL kT2 =   L"123";
        WIDESTRING_IMPL kT3 =   L"abc123abc123";
        wstringstream   out;
        for (int i = 0; i < 1000; ++i) {
            out << kT1 << kT2 << kT3;
        }
        VerifyTestResult (out.str ().length () == 18 * 1000);
    }
}








namespace   {
    void    RunPerformanceTests_ ()
    {
        Tester (
            L"Test of simple locking strategies",
            Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK, L"mutex",
            Test_MutexVersusSharedPtrCopy_shared_ptr_copy, L"shared_ptr<> copy",
            15239,
            -1000000    // disable warning for this
        );
        Tester (
            L"Simple Struct With Strings Filling And Copying",
            Test_StructWithStringsFillingAndCopying<wstring>, L"wstring",
            Test_StructWithStringsFillingAndCopying<String>, L"Charactes::String",
            40000,
            6
        );
        Tester (
            L"Simple String append test (+='string object') 10x",
            Test_SimpleStringAppends1_<wstring>, L"wstring",
            Test_SimpleStringAppends1_<String>, L"Charactes::String",
            1172017,
            -1900
        );
        Tester (
            L"Simple String append test (+=wchar_t[]) 10x",
            Test_SimpleStringAppends2_<wstring>, L"wstring",
            Test_SimpleStringAppends2_<String>, L"Charactes::String",
            1312506,
            -2500
        );
        Tester (
            L"Simple String append test (+=wchar_t[]) 100x",
            Test_SimpleStringAppends3_<wstring>, L"wstring",
            Test_SimpleStringAppends3_<String>, L"Charactes::String",
            272170,
            -5000
        );
        Tester (
            L"wstringstream << test",
            Test_OperatorINSERT_ostream_<wstring>, L"wstring",
            Test_OperatorINSERT_ostream_<String>, L"Charactes::String",
            5438 ,
            -10
        );
    }
}








int     main (int argc, const char* argv[])
{
    // NOTE: run with --show or look for output in PERF-OUT.txt
    Sequence<String>  cmdLine   =   Execution::ParseCommandLine (argc, argv);
    sShowOutput_ = Execution::MatchesCommandLineArgument (cmdLine, L"show");
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (RunPerformanceTests_);
    return EXIT_SUCCESS;
}

