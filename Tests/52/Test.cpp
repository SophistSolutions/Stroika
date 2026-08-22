/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::PERFORMANCE

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <sstream>

#include "Stroika/Foundation/Common/StroikaVersion.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/StroikaVersion.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/MultiSet.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/INI/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/INI/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Writer.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CPUAffinity.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Math/Statistics.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include "Stroika/Foundation/Traversal/Generator.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Math;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Time;

using namespace Stroika::Frameworks;

using std::byte;

namespace {
    constexpr char kDefaultPerfOutFile_[] = "PerformanceDump.txt";
    bool           sShowOutput_           = false;
    // --orderby-probe: opt-in design probe, not part of the regular regression run (see Test_IterableAlgorithms_)
    bool sRunOrderByProbe_ = false;
}

namespace {
    string pctFaster2String_ (double pct)
    {
        if (pct < 0) {
            return Format ("{.2f}% slower"_f, -pct).AsNarrowSDKString (eIgnoreErrors);
        }
        else {
            return Format ("{.2f}% faster"_f, pct).AsNarrowSDKString (eIgnoreErrors);
        }
    }
}

namespace {
#if qStroika_Foundation_Debug_AssertionsChecked
    double sTimeMultiplier_ = (Debug::IsRunningUnderValgrind () or Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) ? .001 : 1.0;
#else
    double sTimeMultiplier_ = (Debug::IsRunningUnderValgrind () or Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) ? .002 : 1.0;
#endif
}

#if qStroika_HasComponent_googletest
/*
 *  TODO:
 *
 */

// Turn this on rarely to calibrate so # runs a good test
//#define   qPrintOutIfBaselineOffFromOneSecond (!qStroika_Foundation_Debug_AssertionsChecked && defined (_MSC_VER) && defined (WIN32) && !defined (_WIN64))

// My performance expectation numbers WERE calibrated for MSVC (2k13.net) and 32-bit code - until Stroika v3. Now
// still using visual studio.net as warning baseline, but using 64-bit tests.
//
// Don't print when they differ on other platforms.
// This is only intended to alert me when something changes GROSSLY.
//
//  WHY THE WARNINGS ARE WINDOWS-ONLY, and why extending them to Linux/macOS was looked at and dropped
//  (2026-08-21). This costs something real - the four Linux CI jobs run the whole perf suite and throw
//  the verdict away - so it is a deliberate choice, not an oversight:
//
//      o   Every warnIfPerfScore below is a RATIO against the platform's own standard library, so the
//          same number means different things on different platforms. Measured: running this suite on
//          g++-14 (ubuntu-24.04, release) and checking each score against its threshold, 27 of 62
//          entries would fire, the worst at 12.1x. That is not drift or noise - the worst offenders are
//          all entries whose BASELINE is a std::vector operation ("add many at once", "Build
//          Sequence<int> from vector<int>"), because libstdc++'s bulk vector operations are far faster
//          relative to Stroika than MSVC's are.
//
//      o   So one shared "slack multiplier" for non-MSVC cannot work: enough slack to quiet the worst
//          entry (12x) makes the other 61 gates meaningless.
//
//      o   Per-compiler-family thresholds - kT_ (msvc, gcc, clang) picked at compile time - would be
//          simple and free at runtime, but one number per family still has to cover every version of
//          that family we test (g++-12 through g++-15, and both old and new clang), and how far apart
//          those versions actually sit here was never measured - only g++-14 was. So it would triple
//          this table to 186 numbers, two thirds of them guesses, and a guessed threshold that fires
//          is worse than no gate: it teaches you to skim past the warnings, which is how a real
//          regression gets missed.
//
//      o   What is actually lost is narrow: a regression that is BOTH gross AND invisible on Windows.
//          Windows is the baseline platform precisely because it already catches gross changes.
//
//  Note the cross-platform view is NOT missing, it just lives outside the test:
//  Tests/Scripts/AnalyzePerformanceTrends.py compares against real per-platform history in
//  Tests/HistoricalPerformanceRegressionTestResults, which does carry Linux and macOS (for 3.0,
//  ~19 Ubuntu2204 dumps and ~14 Ubuntu2404, alongside the Windows ones). So the data for a
//  platform-agnostic rule already exists - warn when a score moves more than N% from the last release
//  ON THIS PLATFORM, rather than against a constant compiled into the test. That is the shape worth
//  building if this ever matters enough; it needs the test to read that history at runtime, which is a
//  feature, not a flag flip.
namespace {
#if defined(_MSC_VER)
    constexpr bool kPrintOutIfFailsToMeetPerformanceExpectations_ =
        not qStroika_Foundation_Debug_AssertionsChecked and qStroika_Foundation_Memory_PreferBlockAllocation and
        not qStroika_Foundation_Debug_DefaultTracingOn and sizeof (void*) == sizeof (int64_t);
#else
    constexpr bool kPrintOutIfFailsToMeetPerformanceExpectations_ = false;
#endif
}

// Use this so when running #if qStroika_Foundation_Debug_AssertionsChecked case - we don't waste a ton of time with this test
#define qDebugCaseRuncountRatio (.01)

namespace {
    ostream& GetOutStream_ ()
    {
        static shared_ptr<ostream> out2File;
        if (not sShowOutput_ and out2File == nullptr) {
            out2File.reset (new ofstream{kDefaultPerfOutFile_});
        }
        ostream& outTo = (sShowOutput_ ? cout : *out2File);
        return outTo;
    }
}

namespace {
    void DEFAULT_TEST_PRINTER (const String& testName, const String& baselineTName, const String& compareWithTName,
                               double warnIfPerformanceScoreHigherThan, Duration baselineTime, Duration compareWithTime)
    {
        ostream& outTo = GetOutStream_ ();
        outTo << "Test " << testName << " (" << baselineTName << " vs " << compareWithTName << ")" << endl;
        double         performanceScore = (baselineTime == 0s) ? 1000000 : compareWithTime.count () / baselineTime.count ();
        constexpr char kOneTab_[]       = "\t";
        {
            FloatConversion::ToStringOptions fo = FloatConversion::ToStringOptions{FloatConversion::SignificantFigures{2}};
            outTo << kOneTab_ << "PERFORMANCE_SCORE" << kOneTab_ << FloatConversion::ToString (performanceScore, fo) << endl;
        }
        outTo << kOneTab_ << "DETAILS:         " << kOneTab_;
        outTo << "[baseline test " << baselineTime.count () << " secs, and comparison " << compareWithTime.count ()
              << " sec, and warnIfPerfScore > " << warnIfPerformanceScoreHigherThan << ", and perfScore=" << performanceScore << "]" << endl;
        outTo << kOneTab_ << "                 " << kOneTab_;
        if (performanceScore < 1) {
            outTo << compareWithTName << " is FASTER" << endl;
        }
        else if (performanceScore > 1) {
            outTo << compareWithTName << " is ***SLOWER***" << endl;
        }
        if constexpr (kPrintOutIfFailsToMeetPerformanceExpectations_) {
            if (performanceScore > warnIfPerformanceScoreHigherThan) {
                outTo << kOneTab_ << "                 " << kOneTab_;
                outTo << "{{{WARNING - expected performance score less than " << warnIfPerformanceScoreHigherThan << " and got "
                      << performanceScore << "}}}" << endl;
            }
        }
        outTo << endl;
    }

    DurationSeconds RunTest_ (function<void ()> t, unsigned int runCount)
    {
        runCount = Math::AtLeast<unsigned int> (runCount, 1);
        const size_t                              kNParts2Divide_{10};
        Memory::StackBuffer<DurationSeconds::rep> times{kNParts2Divide_};
        unsigned int                              actualRanCount{};
        for (size_t i = 0; i < kNParts2Divide_; ++i) {
            TimePointSeconds start = Time::GetTickCount ();
            for (unsigned int ii = 0; ii < Math::AtLeast<unsigned int> (runCount / kNParts2Divide_, 1); ++ii) {
                if (actualRanCount >= runCount) {
                    break;
                }
                t ();
                actualRanCount++;
            }
            times[i] = (Time::GetTickCount () - start).count ();
        }
        DurationSeconds::rep m = Math::Median (times.begin (), times.end ());
        return DurationSeconds{m * kNParts2Divide_}; // this should provide a more stable estimate than the total time
    }

    // return true if test failed (slower than expected)
    bool Tester (String testName, function<void ()> baselineT, String baselineTName, function<void ()> compareWithT,
                 String compareWithTName, unsigned int runCount, double warnIfPerformanceScoreHigherThan,
                 function<void (String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, Duration baselineTime, Duration compareWithTime)>
                     printResults = DEFAULT_TEST_PRINTER)
    {
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Tester", "testName={}, runCount={}"_f, testName, runCount)};
#if qStroika_Foundation_Debug_AssertionsChecked
        runCount = static_cast<unsigned int> (runCount * qDebugCaseRuncountRatio);
#endif
        Duration baselineTime    = RunTest_ (baselineT, runCount);
        Duration compareWithTime = RunTest_ (compareWithT, runCount);
#if qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals<Duration::rep> (baselineTime.count (), 1, .15)) {
            cerr << "SUGGESTION: Baseline Time: " << baselineTime.count () << " and runCount = " << runCount
                 << " so try using runCount = " << int (runCount / baselineTime.count ()) << endl;
        }
#endif
        printResults (testName, baselineTName, compareWithTName, warnIfPerformanceScoreHigherThan, baselineTime, compareWithTime);
        if constexpr (kPrintOutIfFailsToMeetPerformanceExpectations_) {
            double ratio = compareWithTime.count () / baselineTime.count ();
            return ratio > warnIfPerformanceScoreHigherThan;
        }
        else {
            return false;
        }
    }
    bool Tester (String testName, Duration baselineTime, function<void ()> compareWithT, String compareWithTName, unsigned int runCount,
                 double warnIfPerformanceScoreHigherThan,
                 function<void (String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, Duration baselineTime, Duration compareWithTime)>
                     printResults = DEFAULT_TEST_PRINTER)
    {
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("Tester", "testName={}, runCount={}"_f, testName, runCount)};
#if qStroika_Foundation_Debug_AssertionsChecked
        runCount = Math::AtLeast<unsigned int> (static_cast<unsigned int> (runCount * qDebugCaseRuncountRatio), 1);
#endif
        baselineTime *= runCount;
        Duration compareWithTime = RunTest_ (compareWithT, runCount);
#if qPrintOutIfBaselineOffFromOneSecond
        if (not NearlyEquals<Duration::rep> (baselineTime.count (), 1, .15)) {
            cerr << "SUGGESTION: Baseline Time: " << baselineTime.count () << " and runCount = " << runCount
                 << " so try using runCount = " << int (runCount / baselineTime.count ()) << endl;
        }
#endif
        printResults (testName, "{} seconds"_f(baselineTime.count ()), compareWithTName, warnIfPerformanceScoreHigherThan, baselineTime, compareWithTime);
        if constexpr (kPrintOutIfFailsToMeetPerformanceExpectations_) {
            double ratio = compareWithTime.count () / baselineTime.count ();
            return ratio > warnIfPerformanceScoreHigherThan;
        }
        else {
            return false;
        }
    }

    void Tester (String testName, function<void ()> compareWithT, String compareWithTName, unsigned int runCount,
                 double warnIfPerformanceScoreHigherThan, Set<String>* failedTestAccumulator,
                 function<void (String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, Duration baselineTime, Duration compareWithTime)>
                     printResults = DEFAULT_TEST_PRINTER)
    {
        Duration baselineTime = 1 / double (runCount);
        if (Tester (testName, baselineTime, compareWithT, compareWithTName, static_cast<unsigned int> (sTimeMultiplier_ * runCount),
                    warnIfPerformanceScoreHigherThan, printResults)) {
            failedTestAccumulator->Add (testName);
        }
    }
    void Tester (String testName, function<void ()> baselineT, String baselineTName, function<void ()> compareWithT,
                 String compareWithTName, unsigned int runCount, double warnIfPerformanceScoreHigherThan, Set<String>* failedTestAccumulator,
                 function<void (String testName, String baselineTName, String compareWithTName, double warnIfPerformanceScoreHigherThan, Duration baselineTime, Duration compareWithTime)>
                     printResults = DEFAULT_TEST_PRINTER)
    {
        if (Tester (testName, baselineT, baselineTName, compareWithT, compareWithTName,
                    static_cast<unsigned int> (sTimeMultiplier_ * runCount), warnIfPerformanceScoreHigherThan, printResults)) {
            failedTestAccumulator->Add (testName);
        }
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_StructWithStringsFillingAndCopying ()
    {
        struct S {
            WIDESTRING_IMPL fS1;
            WIDESTRING_IMPL fS2;
            WIDESTRING_IMPL fS3;
            WIDESTRING_IMPL fS4;
            S () = default;
            S (const WIDESTRING_IMPL& w1, const WIDESTRING_IMPL& w2, const WIDESTRING_IMPL& w3, const WIDESTRING_IMPL& w4)
                : fS1{w1}
                , fS2{w2}
                , fS3{w3}
                , fS4{w4}
            {
            }
        };
        S s1;
        S s2{L"hi mom", L"124 south vanbergan highway", L"Los Angeles 201243", L"834-313-2144"};
        s1 = s2;
        vector<S> v;
        for (size_t i = 1; i < 10; ++i) {
            v.push_back (s2);
        }
        sort (v.begin (), v.end (), [] (S a, S b) { return b.fS1 < a.fS1; });
#if qStroika_HasComponent_googletest
        EXPECT_TRUE (v[0].fS1 == v[1].fS1);
#endif
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_StructWithStringsFillingAndCopying2 ()
    {
        struct S {
            WIDESTRING_IMPL fS1;
            WIDESTRING_IMPL fS2;
            WIDESTRING_IMPL fS3;
            WIDESTRING_IMPL fS4;
            WIDESTRING_IMPL fS5;
            WIDESTRING_IMPL fS6;
            S () = default;
            S (const WIDESTRING_IMPL& w1, const WIDESTRING_IMPL& w2, const WIDESTRING_IMPL& w3, const WIDESTRING_IMPL& w4)
                : fS1{w1}
                , fS2{w2}
                , fS3{w3}
                , fS4{w4}
            {
            }
        };
        S s1;
        S s2{L"hi mom", L"124 south vanbergan highway", L"Los Angeles 201243", L"834-313-2144"};
        s1 = s2;
        vector<S> v;
        v.reserve (10);
        for (size_t i = 1; i < 10; ++i) {
            v.push_back (s2);
        }
        sort (v.begin (), v.end (), [] (S a, S b) { return b.fS1 < a.fS1; });
#if qStroika_HasComponent_googletest
        EXPECT_TRUE (v[0].fS1 == v[1].fS1);
#endif
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends1_ ()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        WIDESTRING_IMPL       w;
        for (int i = 0; i < 10; ++i) {
            w = w + KBase;
        }
#if qStroika_HasComponent_googletest
        EXPECT_EQ (w.length (), KBase.length () * 10u);
#endif
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends2_ ()
    {
        const wchar_t   KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w = w + KBase;
        }
#if qStroika_HasComponent_googletest
        EXPECT_EQ (w.length (), wcslen (KBase) * 10u);
#endif
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends3_ ()
    {
        const wchar_t   KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 100; ++i) {
            w = w + KBase;
        }
#if qStroika_HasComponent_googletest
        EXPECT_EQ (w.length (), wcslen (KBase) * 100u);
#endif
    }
}

namespace {
    namespace {
        template <typename WIDESTRING_IMPL>
        void Test_SimpleStringConCat1_T1_ (const WIDESTRING_IMPL& src)
        {
            WIDESTRING_IMPL tmp = src + src;
            tmp                 = tmp + src;
            tmp                 = src + tmp;
            EXPECT_EQ (tmp.length (), src.length () * 4u);
        }
    }
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringConCat1_ ()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        Test_SimpleStringConCat1_T1_ (KBase);
    }
}

namespace {
    namespace {
        template <typename WIDESTRING_IMPL>
        void Test_StringSubStr_T1_ (const WIDESTRING_IMPL& src)
        {
            WIDESTRING_IMPL tmp = src.substr (5, 20);
            EXPECT_TRUE (tmp.length () == 20);
            EXPECT_TRUE (src.substr (5, 20).length () == 20);
            EXPECT_TRUE (src.substr (5, 20).substr (3, 3).length () == 3);
        }
    }
    template <typename WIDESTRING_IMPL>
    void Test_StringSubStr_ ()
    {
        static const WIDESTRING_IMPL KBase = L"01234567890123456789012345678901234567890123456789";
        Test_StringSubStr_T1_ (KBase);
    }
}

namespace {
    namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_ {
        mutex Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex;
        int   Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int = 1;
        void  Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK (function<void (int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I don't care about the (much rarer) write case where we really need to modify
            lock_guard<mutex> critSec (Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_mutex);
            doInsideLock (&Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK_int);
        }
        shared_ptr<int> Test_MutexVersusSharedPtrCopy_sharedPtrCase = shared_ptr<int> (new int (1));
        void            Test_MutexVersusSharedPtrCopy_SharedPtrCopy (function<void (int*)> doInsideLock)
        {
            // This is to String class locking. We want to know if copying the shared_ptr rep is faster,
            // or just using a mutex
            //
            // I don't care about the (much rarer) write case where we really need to modify
            shared_ptr<int> tmp = Test_MutexVersusSharedPtrCopy_sharedPtrCase;
            doInsideLock (tmp.get ());
        }

        int  s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT;
        void Test_MutexVersusSharedPtrCopy_COUNTEST (int* i)
        {
            s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT += *i;
        }
    }

    void Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK ()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        EXPECT_TRUE (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000); // so nothing optimized away
    }
    void Test_MutexVersusSharedPtrCopy_shared_ptr_copy ()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_SharedPtrCopy (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        EXPECT_TRUE (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000); // so nothing optimized away
    }
}

namespace {
    namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_ {
        mutex s_Mutex_;
        int   sCnt2Add_ = 1;
        void  Test_MutexVersusSpinLock_MUTEXT_LOCK (function<void (int*)> doInsideLock)
        {
            lock_guard<mutex> critSec (s_Mutex_);
            doInsideLock (&sCnt2Add_);
        }
        SpinLock s_SpinLock_;
        void     Test_MutexVersusSpinLock_SPINLOCK_LOCK (function<void (int*)> doInsideLock)
        {
            lock_guard<SpinLock> critSec (s_SpinLock_);
            doInsideLock (&sCnt2Add_);
        }
        int  sRunningCnt_;
        void Test_MutexVersusSpinLock_COUNTEST (int* i)
        {
            sRunningCnt_ += *i;
        }
    }

    void Test_MutexVersusSpinLock_MUTEXT_LOCK ()
    {
        using namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_;
        sRunningCnt_ = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSpinLock_MUTEXT_LOCK (Test_MutexVersusSpinLock_COUNTEST);
        }
        EXPECT_TRUE (sRunningCnt_ == 1000); // so nothing optimized away
    }
    void Test_MutexVersusSpinLock_SPIN_LOCK ()
    {
        using namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_;
        sRunningCnt_ = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSpinLock_SPINLOCK_LOCK (Test_MutexVersusSpinLock_COUNTEST);
        }
        EXPECT_TRUE (sRunningCnt_ == 1000); // so nothing optimized away
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_OperatorINSERT_ostream_ ()
    {
        using namespace std;
        static WIDESTRING_IMPL kT1 = L"abc";
        static WIDESTRING_IMPL kT2 = L"123";
        static WIDESTRING_IMPL kT3 = L"abc123abc123";
        wstringstream          out;
        for (int i = 0; i < 1000; ++i) {
            out << kT1 << kT2 << kT3;
        }
        EXPECT_TRUE (out.str ().length () == 18 * 1000);
    }
}

namespace {
    template <typename STREAMISH_STRINGBUILDERIMPL, typename STRING_EXTRACTOR>
    void Test_StreamBuilderStringBuildingWithExtract_ (STRING_EXTRACTOR extractor)
    {
        STREAMISH_STRINGBUILDERIMPL out;
        for (int i = 0; i < 20; ++i) {
            out << L"0123456789";
            out << L" ";
            out << L"01234567890123456789";
        }
        EXPECT_TRUE (extractor (out).length () == 31 * 20);
    }
}

namespace {
    namespace Private_ {
        template <typename CONTAINER>
        void Test_SequenceVectorAdditionsAndCopies_RecCall_ (CONTAINER c, int recCalls)
        {
            if (recCalls > 0) {
                Test_SequenceVectorAdditionsAndCopies_RecCall_ (c, recCalls - 1);
            }
            EXPECT_TRUE (c.size () == 500);
        }
    }
    template <typename CONTAINER, typename ELEMENTTYPE = typename CONTAINER::value_type>
    void Test_SequenceVectorAdditionsAndCopies_ ()
    {
        ELEMENTTYPE addEachTime = ELEMENTTYPE{};
        CONTAINER   c;
        for (int i = 0; i < 500; ++i) {
            c.push_back (addEachTime);
        }
        Private_::Test_SequenceVectorAdditionsAndCopies_RecCall_ (c, 20);
    }
}

namespace {
    /*
     ********************************************************************************
     *  Stroika containers versus their std counterparts, ONE OPERATION PER ENTRY
     ********************************************************************************
     *
     *  These cover what the older "... basics" entries (Test_SequenceVectorAdditionsAndCopies_ and
     *  Test_CollectionVectorAdditionsAndCopies_) were reaching for. Those append 500 elements one at a time
     *  AND then make 21 by-value copies, and report one number for both. For Stroika those pull in OPPOSITE
     *  directions - it loses on per-element appends (a virtual _IRep::Insert () per element) and wins hugely
     *  on copies (copy-on-write makes them a refcount bump instead of a deep copy) - so the score is a small
     *  residual of two large opposing effects and cannot attribute a change to either. It read 1.09-1.44 for
     *  int while the append cost alone was ~5x and the copy advantage was order-of-magnitude. Those also
     *  append ELEMENTTYPE{} - the EMPTY string in the string case - so their "expensive element" case is not
     *  expensive, and they run EXPECT_TRUE () inside the timed recursion.
     *
     *  So: one operation per entry, the same operation on both sides, the container built outside the timed
     *  region, and a non-degenerate element.
     *
     *  The "... basics" entries are deliberately still here rather than deleted, for now. Two reasons: they
     *  carry ~30 releases of archive history where these start empty, so running both for a few releases
     *  gives the overlap needed to relate the two - which is exactly what is missing whenever a name gets
     *  re-pointed at a new workload (see the "+=wchar_t[] 100x" note below). And they are not strictly
     *  redundant even now: the blended Collection entries add string{} or all-identical strings, where these
     *  add distinct ones, so the degenerate all-equal-keys case is absent here rather than reproduced.
     *  Revisit once there is a release or two of overlap to look at.
     *
     *  What each is expected to show, so a surprise is recognizable as one:
     *      o   ADD ONE AT A TIME - Stroika slower, and inherently so. Each push_back () is one virtual
     *          dispatch plus a copy-on-write check; there is nothing to batch. This is the standing cost of
     *          the envelope/rep design, worth SEEING rather than chasing.
     *      o   ADD MANY AT ONCE - near parity since c384915a32 handed contiguous sources to the rep as a
     *          single span. This is the entry that would notice that fast path breaking.
     *      o   COPY - Stroika much faster, and MORE so for String than for int: copying vector<T> costs N
     *          per-element copies (for Stroika's String, itself copy-on-write, N atomic increments) where
     *          Sequence<T> costs exactly one, while for int vector only has to memcpy.
     *      o   COPY THEN WRITE - the honest other half. Copy-on-write only defers; writing to the copy pays
     *          the deep copy after all, plus the bookkeeping. Without this entry the suite would only tell
     *          the flattering half of the story.
     *
     *  As measured 3.0d24, Windows x86_64 release, 500 elements, pinned, RANGE over repeated runs (each
     *  threshold is 1.25x the worst run seen, so it has ~25% headroom over the worst of these):
     *
     *                                          add 1-at-a-time   add many at once   copy      copy then write
     *      Sequence<int>                          2.00-3.37 (!)      1.73-1.91      0.19-0.29     2.68-3.10
     *      Sequence<String>                       2.35-2.58          0.92-1.11    0.0019-0.0023   1.42-1.82
     *      Sequence_Array<int>                    1.44-1.79          1.31-1.51
     *      Sequence_stdvector<int>                2.46-2.70          1.53-1.59
     *      Sequence_DoublyLinkedList<String>      2.02-2.09          3.47-4.12                    2.21-2.60
     *      Collection<int>                       13.28-13.90      328-374 (!!)
     *      Collection<String>                     9.15-9.70         17.46-19.41  0.0021-0.0024    2.44-2.98
     *      Collection_LinkedList<String>          1.42-1.51          2.75-2.79
     *      Collection_stdforward_list<String>     1.46-1.61          3.11-3.20
     *
     *  Reading it:
     *      o   COPY is where copy-on-write pays: 0.0021 means ~480x, since copying vector<String> costs 500
     *          refcount bumps where Sequence/Collection<String> costs exactly one. For int, where vector only
     *          has to memcpy, the same win is 5x. COPY THEN WRITE is the other half of that trade and reads
     *          ABOVE 1.0 - the deferred deep copy plus the rep bookkeeping costs more than an eager copy did.
     *      o   ADD MANY AT ONCE reads WORSE than add-one-at-a-time for every node-based backend. That is
     *          mostly the BASELINE getting cheaper (vector::insert of a range is ~2x a push_back loop), not
     *          Stroika getting slower: at equal runCount the absolute Stroika times are within ~1% of each
     *          other, because AddAll () on those containers just loops. Only Sequence<T> batches
     *          (c384915a32), which is why Sequence_Array/stdvector<int> sit near 1.5 and everything else does
     *          not. Collection<int> at ~350x is the extreme case and the best argument for doing that work.
     *      o   A SHORT COMPARISON WINDOW NEEDS MARGIN EVEN WHEN REPEATED RUNS LOOK STABLE. Sequence<int>:
     *          copy measures ~4ms and read 0.187-0.213 over five consecutive runs, so it was set to 0.27 -
     *          and the sixth run produced 0.288. Five agreeing samples are not evidence that a 4ms
     *          measurement is stable; they are five draws from a distribution with a tail. The three
     *          copy-on-write entries therefore carry margin set from the measurement WINDOW (~1-4ms), not
     *          from the observed spread. They lose little by it: the failure they exist to catch is
     *          copy-on-write breaking, which lands near 1.0, so anything below ~0.5 still catches it.
     *      o   (!) Sequence<int>: add one at a time is the LOOSEST gate here (4.9 against a ~2.7 typical) and
     *          catches only an ~80% regression. Its baseline is 500 vector<int> push_back () calls - nearly
     *          pure allocator behaviour with no element-copy cost to steady it - and it ranged 0.0395-0.0717s
     *          while the comparison side held 0.126-0.147s. Tightening it means finding a less
     *          allocator-dominated baseline, not just lowering the number. The add-many and copy entries
     *          cover the same code path far more precisely; prefer them when reading a regression.
     */
    namespace ContainerVsStd_ {
        constexpr size_t kEltCount_ = 500;

        volatile size_t sSink_ = 0; // keep the work from being optimized away

        const vector<int>& SourceInts_ ()
        {
            static const vector<int> kData_ = [] () {
                vector<int> r;
                r.reserve (kEltCount_);
                for (size_t i = 0; i < kEltCount_; ++i) {
                    r.push_back (static_cast<int> (i));
                }
                return r;
            }();
            return kData_;
        }
        const vector<String>& SourceStrings_ ()
        {
            // deliberately not String{} - the old test appended empty strings, so it measured nothing about
            // the cost of a real element. Long enough to be a normal heap-backed string rather than degenerate.
            static const vector<String> kData_ = [] () {
                vector<String> r;
                r.reserve (kEltCount_);
                for (size_t i = 0; i < kEltCount_; ++i) {
                    r.push_back ("element {} of a string long enough not to be degenerate"_f(i));
                }
                return r;
            }();
            return kData_;
        }
        template <typename T>
        const vector<T>& Source_ ();
        template <>
        const vector<int>& Source_<int> ()
        {
            return SourceInts_ ();
        }
        template <>
        const vector<String>& Source_<String> ()
        {
            return SourceStrings_ ();
        }

        /*
         *  The container families spell one-at-a-time and all-at-once differently - Sequence<T> uses
         *  push_back ()/AppendAll (), Collection<T> uses Add ()/AddAll (), std uses push_back ()/insert () -
         *  so pick the spelling here rather than writing the whole matrix out three times. It stays ONE
         *  operation either way, which is the property these entries depend on.
         */
        template <typename CONTAINER>
        void AddOne_ (CONTAINER& c, const typename CONTAINER::value_type& e)
        {
            if constexpr (requires { c.push_back (e); }) {
                c.push_back (e);
            }
            else {
                c.Add (e);
            }
        }
        template <typename CONTAINER>
        void AddAll_ (CONTAINER& c, const vector<typename CONTAINER::value_type>& src)
        {
            if constexpr (requires { c.AppendAll (src); }) {
                c.AppendAll (src);
            }
            else if constexpr (requires { c.AddAll (src); }) {
                c.AddAll (src);
            }
            else {
                c.insert (c.end (), src.begin (), src.end ());
            }
        }

        // built ONCE, outside any timed region, so the copy entries measure only the copy
        template <typename CONTAINER>
        const CONTAINER& Prebuilt_ ()
        {
            static const CONTAINER kC_ = [] () {
                CONTAINER c;
                for (const auto& i : Source_<typename CONTAINER::value_type> ()) {
                    AddOne_ (c, i);
                }
                return c;
            }();
            return kC_;
        }

        template <typename CONTAINER>
        void AddOneAtATime_ ()
        {
            CONTAINER c;
            for (const auto& i : Source_<typename CONTAINER::value_type> ()) {
                AddOne_ (c, i);
            }
            sSink_ = sSink_ + c.size ();
        }
        template <typename CONTAINER>
        void AddManyAtOnce_ ()
        {
            // the same operation on both sides: add the whole range to an empty container
            CONTAINER c;
            AddAll_ (c, Source_<typename CONTAINER::value_type> ());
            sSink_ = sSink_ + c.size ();
        }
        // A NON-CONTIGUOUS source, so AddAll () takes its per-element branch - which is exactly what
        // the code did before the span-taking _IRep::Add () landed. Pairing this against
        // AddManyAtOnce_ (vector source) on the SAME binary measures the batching win itself.
        template <typename T>
        const list<T>& SourceAsList_ ()
        {
            static const list<T> kData_ = [] () {
                const vector<T>& v = Source_<T> ();
                return list<T>{v.begin (), v.end ()};
            }();
            return kData_;
        }
        template <typename CONTAINER>
        void AddManyAtOnce_FromNonContiguous_ ()
        {
            CONTAINER   c;
            const auto& src = SourceAsList_<typename CONTAINER::value_type> ();
            if constexpr (requires { c.AppendAll (src.begin (), src.end ()); }) {
                c.AppendAll (src.begin (), src.end ());
            }
            else if constexpr (requires { c.AddAll (src.begin (), src.end ()); }) {
                c.AddAll (src.begin (), src.end ());
            }
            else {
                c.insert (c.end (), src.begin (), src.end ());
            }
            sSink_ = sSink_ + c.size ();
        }
        // 500 values NOT in the container, so Contains () has to scan/descend all the way and miss
        const vector<int>& AbsentInts_ ()
        {
            static const vector<int> kData_ = [] () {
                vector<int> r;
                r.reserve (kEltCount_);
                for (size_t i = 0; i < kEltCount_; ++i) {
                    r.push_back (static_cast<int> (i + kEltCount_ * 10)); // disjoint from SourceInts_
                }
                return r;
            }();
            return kData_;
        }
        const vector<String>& AbsentStrings_ ()
        {
            static const vector<String> kData_ = [] () {
                vector<String> r;
                r.reserve (kEltCount_);
                for (size_t i = 0; i < kEltCount_; ++i) {
                    r.push_back ("ABSENT {} of a string long enough not to be degenerate"_f(i));
                }
                return r;
            }();
            return kData_;
        }
        template <typename T>
        const vector<T>& Absent_ ();
        template <>
        const vector<int>& Absent_<int> ()
        {
            return AbsentInts_ ();
        }
        template <>
        const vector<String>& Absent_<String> ()
        {
            return AbsentStrings_ ();
        }
        template <typename CONTAINER>
        void ContainsEachPresent_ ()
        {
            const CONTAINER& c = Prebuilt_<CONTAINER> ();
            size_t           n = 0;
            for (const auto& i : Source_<typename CONTAINER::value_type> ()) {
                if (c.Contains (i)) {
                    ++n;
                }
            }
            sSink_ = sSink_ + n;
        }
        template <typename CONTAINER>
        void ContainsEachAbsent_ ()
        {
            const CONTAINER& c = Prebuilt_<CONTAINER> ();
            size_t           n = 0;
            for (const auto& i : Absent_<typename CONTAINER::value_type> ()) {
                if (c.Contains (i)) {
                    ++n;
                }
            }
            sSink_ = sSink_ + n;
        }
        template <typename CONTAINER>
        void CopyOnly_ ()
        {
            CONTAINER tmp = Prebuilt_<CONTAINER> ();
            sSink_        = sSink_ + tmp.size ();
        }
        template <typename CONTAINER>
        void CopyThenWrite_ ()
        {
            CONTAINER tmp = Prebuilt_<CONTAINER> ();
            AddOne_ (tmp, typename CONTAINER::value_type{}); // forces copy-on-write to actually copy
            sSink_ = sSink_ + tmp.size ();
        }
    }
}

#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
namespace {
    /*
     *  append_range () against the std spelling of the same operation - std::vector::append_range (), C++23.
     *  A like-for-like pairing: both take an existing container and append the same N elements from the same
     *  contiguous source, so the score is purely the cost of getting those elements in.
     *
     *  Deliberately a PROBE (kNoWarn_) and not a gate, though it compares against the standard library
     *  rather than another Stroika path. Measured twice on the same machine it scored 55.9 and 29.7: the
     *  Stroika side moved 2% between runs while the BASELINE moved 91%, because vector::append_range () of
     *  1000 ints is ~90-180ns and most of that is the two allocations per iteration, not the memmove - and
     *  allocator timing swings with heap state. A threshold loose enough to tolerate 30-56 could only catch
     *  a >2.3x regression, which is a dead gate by the standards of AnalyzePerformanceThresholds.py. The
     *  ratio is still far too large to be explained by that noise, so it serves fine as a before/after.
     *
     *  This entry is what proved the AppendAll () batching landed. Before it, append_range () forwarded to
     *  an AppendAll () that made one VIRTUAL _IRep::Insert () call per element (a span of size 1 each), and
     *  this scored 30-56x. With contiguous sources handed to the rep as a single span, it scores ~1.5x - the
     *  residue being one dispatch plus the copy-on-write check against a bare memmove.
     *
     *  \note Guarded on __cpp_lib_containers_ranges: vector::append_range () is C++23, and Stroika only
     *        requires C++20, so on an older toolchain this entry simply does not exist.
     */
    const vector<int>& AppendRange_SourceInts_ ()
    {
        static const vector<int> kData_ = [] () {
            vector<int> r;
            r.reserve (1000);
            for (int i = 0; i < 1000; ++i) {
                r.push_back (i);
            }
            return r;
        }();
        return kData_;
    }
    void Test_AppendRange_StdVector_ ()
    {
        vector<int> v{0};
        v.append_range (AppendRange_SourceInts_ ());
        EXPECT_TRUE (v.size () == 1001); // so nothing gets optimized away
    }
    void Test_AppendRange_Sequence_ ()
    {
        Sequence<int> s{0};
        s.append_range (AppendRange_SourceInts_ ());
        EXPECT_TRUE (s.size () == 1001);
    }
}
#endif

namespace {
    namespace Private_ {
        template <typename CONTAINER>
        void Test_CollectionVectorAdditionsAndCopies_RecCall_ (CONTAINER c, int recCalls)
        {
            if (recCalls > 0) {
                Test_CollectionVectorAdditionsAndCopies_RecCall_ (c, recCalls - 1);
            }
            EXPECT_TRUE (c.size () == 500);
        }
    }
    template <typename CONTAINER, typename ELEMENTTYPE = typename CONTAINER::value_type>
    void Test_CollectionVectorAdditionsAndCopies_ (function<void (CONTAINER* c)> f2Add)
    {
        CONTAINER c;
        for (int i = 0; i < 500; ++i) {
            f2Add (&c);
        }
        Private_::Test_CollectionVectorAdditionsAndCopies_RecCall_ (c, 20);
    }
}

namespace {
    namespace Private_ {
        template <typename CONTAINER>
        void CopyContainerByValue (CONTAINER c, int nMoreCalls)
        {
            auto checkContains = [] ([[maybe_unused]] const CONTAINER& c) {
                using ELEMENTTYPE = typename CONTAINER::value_type;
                for (int i = 0; i < 500; ++i) {
                    if constexpr (same_as<set<ELEMENTTYPE>, CONTAINER>) {
                        EXPECT_TRUE (c.find (i) != c.end ());
                    }
                    else {
                        EXPECT_TRUE (c.Contains (i));
                    }
                }
                if constexpr (same_as<set<ELEMENTTYPE>, CONTAINER>) {
                    EXPECT_TRUE (c.find (501) == c.end ());
                }
                else {
                    EXPECT_TRUE (not c.Contains (501));
                }
            };
            if (nMoreCalls == 0) {
                checkContains (c);
            }
            else {
                CopyContainerByValue (c, nMoreCalls - 1);
            }
        }
    }
    template <typename CONTAINER, typename ELEMENTTYPE = typename CONTAINER::value_type>
    void Test_SetvsSet_ ()
    {
        CONTAINER c;
        for (int i = 0; i < 500; ++i) {
            if constexpr (same_as<set<ELEMENTTYPE>, CONTAINER>) {
                c.insert (i);
            }
            else {
                c.Add (i);
            }
        }
        Private_::CopyContainerByValue (c, 5);
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_String_Format_ ()
    {
        EXPECT_EQ (Format ("a, {}, {}"_f, L"xxx", 33), L"a, xxx, 33");
        EXPECT_EQ (Format ("0x{:x}"_f, 0x20), "0x20");
    }
    template <>
    void Test_String_Format_<wstring> ()
    {
        {
            wchar_t buf[1024];
            EXPECT_TRUE (swprintf (buf, std::size (buf), L"a, %ls, %d", L"xxx", 33) == 10);
            EXPECT_TRUE (wstring (buf) == L"a, xxx, 33");
        }
        {
            wchar_t buf[1024];
            EXPECT_TRUE (swprintf (buf, std::size (buf), L"0x%x", 0x20) == 4);
            EXPECT_TRUE (wstring (buf) == L"0x20");
        }
    }
}

namespace {
    namespace Test_BLOB_Versus_Vector_Byte_DETAILS {
        static constexpr byte kCArr_4k_[4 * 1024] = {
            0x1_b,
            0x2_b,
            0x3_b,
        };

        template <typename BLOBISH_IMPL>
        size_t T1_SIZER_ (BLOBISH_IMPL b)
        {
            return b.size ();
        }
        template <typename BLOBISH_IMPL>
        void T1_ ()
        {
            BLOBISH_IMPL bn;
            for (int i = 0; i < 100; ++i) {
                BLOBISH_IMPL bl = BLOBISH_IMPL (begin (kCArr_4k_), end (kCArr_4k_));
                BLOBISH_IMPL b2 = bl;
                BLOBISH_IMPL b3 = bl;
                BLOBISH_IMPL b4 = bl;
                bn              = b4;
            }
            EXPECT_TRUE (T1_SIZER_ (bn) == sizeof (kCArr_4k_));
        }
    }
    template <typename BLOBISH_IMPL>
    void Test_BLOB_Versus_Vector_Byte ()
    {
        Test_BLOB_Versus_Vector_Byte_DETAILS::T1_<BLOBISH_IMPL> ();
    }
}

namespace {
    namespace Test_BLOB_Versus_Vector_Byte_2_DETAILS {
        static constexpr byte kCArr_32b_[32] = {
            0x1_b,
            0x2_b,
            0x3_b,
        };
        static constexpr byte kCArr_4k_[4 * 1024] = {
            0x1_b,
            0x2_b,
            0x3_b,
        };

        template <typename BLOBISH_IMPL>
        size_t T1_SIZER_ (BLOBISH_IMPL b)
        {
            return b.size ();
        }
        template <typename BLOBISH_IMPL>
        void T1_ ()
        {
            BLOBISH_IMPL bn;
            for (int i = 0; i < 100; ++i) {
                BLOBISH_IMPL bl = BLOBISH_IMPL{begin (kCArr_4k_), end (kCArr_4k_)};
                BLOBISH_IMPL b2 = bl;
                BLOBISH_IMPL b3 = bl;
                BLOBISH_IMPL b4 = bl;
                bn              = b4;
            }
            EXPECT_TRUE (T1_SIZER_ (bn) == sizeof (kCArr_4k_));
        }
        template <typename BLOBISH_IMPL>
        void T2_ ()
        {
            BLOBISH_IMPL bn; // run test with smaller blobs more times, on theorey you construct more small ones than big ones?
            for (int i = 0; i < 5 * 100; ++i) {
                BLOBISH_IMPL bl = BLOBISH_IMPL{begin (kCArr_32b_), end (kCArr_32b_)};
                BLOBISH_IMPL b2 = bl;
                BLOBISH_IMPL b3 = bl;
                BLOBISH_IMPL b4 = bl;
                bn              = b4;
            }
            EXPECT_TRUE (T1_SIZER_ (bn) == sizeof (kCArr_32b_));
        }
    }
    template <typename BLOBISH_IMPL>
    void Test_BLOB_Versus_Vector_Byte_2 ()
    {
        Test_BLOB_Versus_Vector_Byte_2_DETAILS::T1_<BLOBISH_IMPL> ();
        Test_BLOB_Versus_Vector_Byte_2_DETAILS::T2_<BLOBISH_IMPL> ();
    }
}

namespace {
    namespace Test_JSONReadWriteFile_ {
        constexpr uint8_t kSAMPLE_FILE_[] = "{\
    \"Aux-Data\" : {\
        \"C3\" : \"-0\",\
        \"EngineId\" : \"B1E56F82-B217-40D3-A24D-FAC491EDCDE8\",\
        \"Gas-Cell\" : \"Short\",\
        \"Sample-Pressure\" : \"-129.277\",\
        \"Sample-Temperature\" : \"-74.51\",\
        \"iC4\" : \"-0\",\
        \"nC4\" : \"-0\",\
        \"nC5\" : \"-0\"\
    },\
    \"Background-ID\" : 5378,\
    \"Raw-Spectrum\" : {\
        \"1000\" : 102.207499,\
        \"1001\" : 104.437091,\
        \"1002\" : 105.038416,\
        \"1003\" : 106.717942,\
        \"1004\" : 108.897728,\
        \"1005\" : 110.894962,\
        \"1006\" : 109.450638,\
        \"1007\" : 109.475062,\
        \"1008\" : 112.364229,\
        \"1009\" : 114.742569,\
        \"1010\" : 114.876544,\
        \"1011\" : 115.329764,\
        \"1012\" : 114.287301,\
        \"1013\" : 116.257922,\
        \"1014\" : 116.513513,\
        \"1015\" : 118.098858,\
        \"1016\" : 120.959124,\
        \"1017\" : 122.762808,\
        \"1018\" : 120.115923,\
        \"1019\" : 123.527138,\
        \"1020\" : 123.097837,\
        \"1021\" : 123.135073,\
        \"1022\" : 127.237821,\
        \"1023\" : 126.812848,\
        \"1024\" : 129.457969,\
        \"1025\" : 128.345917,\
        \"1026\" : 129.984425,\
        \"1027\" : 131.375393,\
        \"1028\" : 131.884103,\
        \"1029\" : 132.656497,\
        \"1030\" : 134.59663,\
        \"1031\" : 135.856095,\
        \"1032\" : 135.952807,\
        \"1033\" : 137.859343,\
        \"1034\" : 136.869097,\
        \"1035\" : 138.835342,\
        \"1036\" : 140.525628,\
        \"1037\" : 141.462064,\
        \"1038\" : 143.509938,\
        \"1039\" : 143.025406,\
        \"1040\" : 145.696478,\
        \"1041\" : 145.621319,\
        \"1042\" : 144.466126,\
        \"1043\" : 147.638448,\
        \"1044\" : 146.277531,\
        \"1045\" : 147.775448,\
        \"1046\" : 151.020184,\
        \"1047\" : 150.296378,\
        \"1048\" : 150.245013,\
        \"1049\" : 154.725126,\
        \"1050\" : 152.592499,\
        \"1051\" : 154.033013,\
        \"1052\" : 157.257001,\
        \"1053\" : 155.615766,\
        \"1054\" : 157.941679,\
        \"1055\" : 158.318632,\
        \"1056\" : 159.615589,\
        \"1057\" : 160.554203,\
        \"1058\" : 163.494878,\
        \"1059\" : 161.874425,\
        \"1060\" : 164.481251,\
        \"1061\" : 166.688488,\
        \"1062\" : 167.057323,\
        \"1063\" : 168.264615,\
        \"1064\" : 166.01023,\
        \"1065\" : 168.396505,\
        \"1066\" : 168.089616,\
        \"1067\" : 170.023101,\
        \"1068\" : 173.431235,\
        \"1069\" : 173.246185,\
        \"1070\" : 173.722823,\
        \"1071\" : 173.153608,\
        \"1072\" : 176.137788,\
        \"1073\" : 175.371997,\
        \"1074\" : 176.688347,\
        \"1075\" : 180.784501,\
        \"1076\" : 180.070223,\
        \"1077\" : 182.610873,\
        \"1078\" : 181.150272,\
        \"1079\" : 183.36719,\
        \"1080\" : 184.092786,\
        \"1081\" : 186.711791,\
        \"1082\" : 187.572159,\
        \"1083\" : 188.785103,\
        \"1084\" : 189.263375,\
        \"1085\" : 190.892796,\
        \"1086\" : 190.323923,\
        \"1087\" : 189.079699,\
        \"1088\" : 190.530891,\
        \"1089\" : 194.173038,\
        \"1090\" : 193.672212,\
        \"1091\" : 194.384409,\
        \"1092\" : 197.342573,\
        \"1093\" : 197.415818,\
        \"1094\" : 199.179823,\
        \"1095\" : 200.596746,\
        \"1096\" : 201.53603,\
        \"1097\" : 199.419327,\
        \"1098\" : 200.564464,\
        \"1099\" : 201.269345,\
        \"1100\" : 202.64605,\
        \"900\" : 5.276987,\
        \"901\" : 6.840619,\
        \"902\" : 6.500289,\
        \"903\" : 8.03882,\
        \"904\" : 6.711202,\
        \"905\" : 10.280579,\
        \"906\" : 10.475101,\
        \"907\" : 10.606406,\
        \"908\" : 13.54551,\
        \"909\" : 13.841327,\
        \"910\" : 12.198958,\
        \"911\" : 16.643659,\
        \"912\" : 14.578187,\
        \"913\" : 18.954581,\
        \"914\" : 18.36068,\
        \"915\" : 19.383819,\
        \"916\" : 21.717242,\
        \"917\" : 19.646697,\
        \"918\" : 23.250176,\
        \"919\" : 22.704226,\
        \"920\" : 25.335496,\
        \"921\" : 25.824652,\
        \"922\" : 26.055423,\
        \"923\" : 28.658592,\
        \"924\" : 28.517954,\
        \"925\" : 30.196885,\
        \"926\" : 31.030664,\
        \"927\" : 30.098038,\
        \"928\" : 33.244351,\
        \"929\" : 34.620035,\
        \"930\" : 35.68152,\
        \"931\" : 36.90124,\
        \"932\" : 36.283642,\
        \"933\" : 38.395281,\
        \"934\" : 39.033175,\
        \"935\" : 40.367104,\
        \"936\" : 39.395377,\
        \"937\" : 39.044223,\
        \"938\" : 43.853149,\
        \"939\" : 43.431587,\
        \"940\" : 42.951665,\
        \"941\" : 44.675716,\
        \"942\" : 46.983485,\
        \"943\" : 45.722461,\
        \"944\" : 47.532444,\
        \"945\" : 49.952878,\
        \"946\" : 48.673128,\
        \"947\" : 51.676115,\
        \"948\" : 50.569155,\
        \"949\" : 54.984444,\
        \"950\" : 54.494472,\
        \"951\" : 53.869777,\
        \"952\" : 54.25914,\
        \"953\" : 58.971926,\
        \"954\" : 57.559519,\
        \"955\" : 57.468957,\
        \"956\" : 60.300616,\
        \"957\" : 59.822135,\
        \"958\" : 61.488314,\
        \"959\" : 62.261685,\
        \"960\" : 64.511706,\
        \"961\" : 63.959589,\
        \"962\" : 65.517899,\
        \"963\" : 66.882009,\
        \"964\" : 66.425605,\
        \"965\" : 68.261815,\
        \"966\" : 70.418805,\
        \"967\" : 69.337126,\
        \"968\" : 72.908709,\
        \"969\" : 73.304041,\
        \"970\" : 73.119387,\
        \"971\" : 76.79671,\
        \"972\" : 74.605076,\
        \"973\" : 75.799573,\
        \"974\" : 77.236541,\
        \"975\" : 79.427965,\
        \"976\" : 78.456946,\
        \"977\" : 82.153217,\
        \"978\" : 81.065032,\
        \"979\" : 83.16378,\
        \"980\" : 85.913195,\
        \"981\" : 86.969779,\
        \"982\" : 86.159601,\
        \"983\" : 87.806876,\
        \"984\" : 88.487533,\
        \"985\" : 90.95273,\
        \"986\" : 88.678744,\
        \"987\" : 92.527448,\
        \"988\" : 93.661814,\
        \"989\" : 93.114072,\
        \"990\" : 92.373317,\
        \"991\" : 96.029655,\
        \"992\" : 96.849889,\
        \"993\" : 97.373866,\
        \"994\" : 98.85073,\
        \"995\" : 100.856283,\
        \"996\" : 101.807204,\
        \"997\" : 102.519829,\
        \"998\" : 100.426089,\
        \"999\" : 104.86934\
    },\
    \"Reference-ID\" : 5379,\
    \"Scan-End\" : \"2015-01-14T15:31:08Z\",\
    \"Scan-ID\" : 5856,\
    \"Scan-Kind\" : \"Sample\",\
    \"Scan-Label\" : \"\",\
    \"Scan-Start\" : \"2015-01-14T15:31:07Z\"\
}";
        enum class ScanKindType {
            Background,
            Reference,
            Sample,
            Stroika_Define_Enum_Bounds (Background, Sample)
        };
        constexpr Common::EnumNames<ScanKindType> ScanKindType_NAMES{{{
            {ScanKindType::Background, L"Background"},
            {ScanKindType::Reference, L"Reference"},
            {ScanKindType::Sample, L"Sample"},
        }}};
        using ScanIDType = int;
        struct SpectrumType : Mapping<double, double> {
            struct CompareNumbersEqual_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
                bool operator() (double lhs, double rhs) const
                {
                    return Math::NearlyEquals (lhs, rhs, .001); //return lhs == rhs; due to convert to / from json we lose precision
                }
            };
            struct CompareNumbersLess_ : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eStrictInOrder> {
                bool operator() (double lhs, double rhs) const
                {
                    // special case handle numbers that are close
                    if (CompareNumbersEqual_{}(lhs, rhs)) {
                        return false;
                    }
                    return less<double>{}(lhs, rhs);
                }
            };
            SpectrumType ()
                // : Mapping<double, double>{CompareNumbersEqual_{}} //works but slower
                : Mapping<double, double>{Containers::Concrete::SortedMapping_stdmap<double, double>{CompareNumbersLess_{}}}
            {
            }
            bool operator== (const SpectrumType& rhs) const
            {
                return EqualsComparer<CompareNumbersEqual_>{}(*this, rhs); // must compare values with 'nearlyequals'
            }
        };
        using PersistenceScanAuxDataType = Mapping<String, String>;
        struct ScanDetails_ {
            ScanIDType                 fScanID{};
            optional<DateTime>         fScanStart;
            optional<DateTime>         fScanEnd;
            ScanKindType               fScanKind{};
            String                     fScanLabel{};
            SpectrumType               fRawSpectrum{};
            PersistenceScanAuxDataType fAuxData{};
            optional<ScanIDType>       fUseBackground{};
            optional<ScanIDType>       fUseReference{};
        };

        DataExchange::ObjectVariantMapper GetPersistenceDetailsMapper_ ()
        {
            using namespace DataExchange;
            ObjectVariantMapper mapper;
            mapper.AddCommonType<ScanIDType> ();
            mapper.AddCommonType<optional<ScanIDType>> ();
            mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ScanKindType> (ScanKindType_NAMES));
            mapper.AddCommonType<SpectrumType> ();
            mapper.AddCommonType<PersistenceScanAuxDataType> ();
            mapper.AddClass<ScanDetails_> ({
                {"Scan-ID"sv, &ScanDetails_::fScanID},
                {"Scan-Start"sv, &ScanDetails_::fScanStart},
                {"Scan-End"sv, &ScanDetails_::fScanEnd},
                {"Scan-Kind"sv, &ScanDetails_::fScanKind},
                {"Scan-Label"sv, &ScanDetails_::fScanLabel},
                {"Raw-Spectrum"sv, &ScanDetails_::fRawSpectrum},
                {"Aux-Data"sv, &ScanDetails_::fAuxData},
                {"Background-ID"sv, &ScanDetails_::fUseBackground},
                {"Reference-ID"sv, &ScanDetails_::fUseReference},
            });
            return mapper;
        }
        ScanDetails_ doRead_ (const InputStream::Ptr<byte> in)
        {
            using namespace DataExchange;
            VariantValue                     o{Variant::JSON::Reader{}.Read (in)};
            static const ObjectVariantMapper kMapper_ = GetPersistenceDetailsMapper_ ();
            return kMapper_.ToObject<ScanDetails_> (o);
        }
        Memory::BLOB doWrite_ (const ScanDetails_& scan)
        {
            using namespace DataExchange;
            Streams::MemoryStream::Ptr<byte> out      = Streams::MemoryStream::New<byte> ();
            static const ObjectVariantMapper kMapper_ = GetPersistenceDetailsMapper_ ();
            Variant::JSON::Writer{}.Write (kMapper_.FromObject (scan), out);
            return out.As<Memory::BLOB> ();
        }
        void DoRunPerfTest ()
        {
            ScanDetails_ sd = doRead_ (Streams::ExternallyOwnedSpanInputStream::New<byte> (span{kSAMPLE_FILE_}));
            EXPECT_TRUE (sd.fAuxData.ContainsKey ("Sample-Pressure"));
            EXPECT_TRUE (sd.fScanID == 5856);
            Memory::BLOB b = doWrite_ (sd);
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
            ScanDetails_ sd2 = doRead_ (Streams::ExternallyOwnedSpanInputStream::New<byte> (span<const byte>{b}));
#else
            ScanDetails_ sd2 = doRead_ (Streams::ExternallyOwnedSpanInputStream::New<byte> (span{b}));
#endif
            EXPECT_EQ (sd2.fScanID, sd.fScanID);
            EXPECT_EQ (sd2.fAuxData, sd.fAuxData);
            EXPECT_EQ (sd2.fRawSpectrum, sd.fRawSpectrum); // @todo - FIX - this test should pass!
        }
    }
}

namespace {
    namespace Test_Optional_ {
        namespace Private_ {
            template <typename T>
            void T1_ ()
            {
                for (int i = 0; i < 1000; ++i) {
                    optional<T>                  x;
                    [[maybe_unused]] optional<T> y = x;
                }
            }
            template <typename T>
            void T2_ ()
            {
                for (int i = 0; i < 1000; ++i) {
                    optional<T>                  x = T{};
                    [[maybe_unused]] optional<T> y = x;
                }
            }
            template <typename T>
            void TAll_ ()
            {
                T1_<T> ();
                T2_<T> ();
            }
        }
        void DoRunPerfTest ()
        {
            using namespace Private_;
            TAll_<int> ();
            TAll_<string> ();
            TAll_<wstring> ();
            TAll_<Characters::String> ();
        }
    }
}

#if qStroika_Foundation_Common_Platform_Windows && 0
namespace {
    namespace Test_WString2UTF8_ {
        static const codecvt_utf8<wchar_t> kConverter_; // safe to keep static because only read-only const methods used
        constexpr wchar_t                  kS1_[] =
            L"asdbf asdkfja sdflkja ls;dkfja s;ldkfj aslkd;fj alksdfj alskdfj aslk;df;j as;lkdfj aslk;dfj asl;dkfj asdf";
        constexpr wchar_t kS2_[] = L"z\u00df\u6c34\U0001d10b";
        void              Test_WString2UTF8_codecvt_utf8 (const wchar_t* s, const wchar_t* e)
        {
            const wchar_t*                                 sc = s;
            const wchar_t*                                 ec = e;
            string                                         tmp ((e - s) * kConverter_.max_length (), '\0');
            const wchar_t*                                 from_next;
            char*                                          to_next;
            mbstate_t                                      mb{};
            [[maybe_unused]] codecvt_utf8<wchar_t>::result r = kConverter_.out (mb, sc, ec, from_next, &tmp[0], &tmp[tmp.size ()], to_next);
            tmp.resize (to_next - &tmp[0]);
        }
    }
    void Test_WString2UTF8_codecvt_utf8 ()
    {
        using namespace Test_WString2UTF8_;
        Test_WString2UTF8_codecvt_utf8 (begin (kS1_), end (kS1_));
        Test_WString2UTF8_codecvt_utf8 (begin (kS2_), end (kS2_));
    }
}
#endif

// NOTE - to include this test case - run the script DownloadAltJSON.sh (in folder with this test case)
// and it will populate this directory and run this test.
#if __has_include("AltJSONImpls2BenchMark/nlohmann/json.hpp")
#include "AltJSONImpls2BenchMark/nlohmann/json.hpp"
#endif

#if __has_include("boost/json.hpp")
#include <boost/json.hpp>
#endif
namespace {
    namespace JSONTests_ {
        /*
         *  Tests based on https://github.com/SophistSolutions/Stroika/issues/913 (STK-781)
         *  and complaints on reddit (2022-12-13) when I posted there without any benchmark test results.
         *
         *  Test lifted/comparable to 
         *      https://www.thousandeyes.com/blog/efficiency-comparison-c-json-libraries#:~:text=Based%20on%20the%20results%20of,for%20both%20parsing%20and%20serializing.
         *  Got exact code (test logic) and test cases from:
         *      https://github.com/salessandri/json-bechmarks
         */
#if __has_include("AltJSONImpls2BenchMark/nlohmann/json.hpp")
        void DoStroikaJSONParse_nlohmann_json (const string& p, unsigned int nTimes)
        {
            using json = nlohmann::json;
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                auto ex1 = json::parse (p);
            }
        }
#endif
#if __has_include("boost/json.hpp")
        void DoStroikaJSONParse_boost_json (const string& p, unsigned int nTimes)
        {
            using namespace boost::json;
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                std::error_code ec;
                auto            ex1 = parse (p, ec);
            }
        }
        void DoStroikaJSONParse_boost_json2Stk (const string& p, unsigned int nTimes)
        {
            using namespace boost::json;
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                std::error_code ec;
                auto            ex1 = DataExchange::VariantValue{parse (p, ec)};
            }
        }
#endif
        void DoStroikaJSONParse_forcedNative_ (const string& p, unsigned int nTimes)
        {
            using namespace DataExchange;
            using namespace Streams;
            Variant::JSON::Reader reader{Variant::JSON::ReaderOptions{.fPreferredAlgorithm = Variant::JSON::ReaderOptions::eStroikaNative}};
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                VariantValue output{reader.Read (ExternallyOwnedSpanInputStream::New<byte> (span{p}))};
            }
        }
        void DoStroikaJSONParse_ (const string& p, unsigned int nTimes)
        {
            using namespace DataExchange;
            using namespace Streams;
            Variant::JSON::Reader reader{Variant::JSON::ReaderOptions{}};
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                VariantValue output{reader.Read (ExternallyOwnedSpanInputStream::New<byte> (span{p}))};
            }
        }
        void DoJSONParse_ (const filesystem::path& p, unsigned int nTimes,
                           const function<void (const string&, unsigned int)>& function2Test, const string& testName)
        {
            GetOutStream_ () << testName << ": " << p << endl;
            if (not filesystem::exists (p)) {
                GetOutStream_ () << "\t\t***SKIPPED CUZ TEST FILE MISSING: " << p << endl;
                return;
            }
            std::string data2ParseAsString = [&p] () {
                if constexpr (true) {
                    // write the same thing in a simpler stroika-based style
                    return IO::FileSystem::FileInputStream::New (p).ReadAll ().As<string> ();
                }
                else {
                    // ape the behavior of https://github.com/salessandri/json-bechmarks/blob/master/jsonspirit-map-serializer-testrunner.cpp
                    string   to_parse;
                    ifstream inputfile;
                    inputfile.exceptions (ifstream::badbit | ifstream::failbit);
                    inputfile.open (p);
                    inputfile.seekg (0, std::ios::end);
                    to_parse.reserve (inputfile.tellg ());
                    inputfile.seekg (0, std::ios::beg);
                    to_parse.assign ((std::istreambuf_iterator<char>{inputfile}), std::istreambuf_iterator<char>{});
                    return to_parse;
                }
            }();
            Time::TimePointSeconds start = Time::GetTickCount ();
            function2Test (data2ParseAsString, nTimes);
            Time::DurationSeconds took = Time::GetTickCount () - start;
            GetOutStream_ () << "\t"
                             << "DETAILS"
                             << "\t\t\t" << took.count () << " seconds" << endl;
            GetOutStream_ () << endl;
        }
        void Run ()
        {
            using filesystem::path;
            unsigned int nTimes = max (1u, static_cast<unsigned int> (sTimeMultiplier_));

            using TEST_FUN_TYPE           = function<void (const string&, unsigned int)>;
            static const auto kTestCases_ = vector<tuple<TEST_FUN_TYPE, string>>{
                {make_tuple (DoStroikaJSONParse_forcedNative_, "stroika-json-native-parser"), make_tuple (DoStroikaJSONParse_, "stroika-json-parser")
#if __has_include("AltJSONImpls2BenchMark/nlohmann/json.hpp")
                                                                                                  ,
                 make_tuple (DoStroikaJSONParse_nlohmann_json, "nlohmann_json-parser")
#endif
#if __has_include("boost/json.hpp")
                     ,
                 make_tuple (DoStroikaJSONParse_boost_json, "boost_json-parser")
#endif
#if __has_include("boost/json.hpp")
                     ,
                 make_tuple (DoStroikaJSONParse_boost_json2Stk, "boost_json-vv-parser")
#endif
                }};
            path jsonTestRoot = path{"."} / "51" / "JSONTestData";
            // hack a bit to find jsonTestRoot, since sometimes run from different places; no need to do good/formal job here
            // since this is for a rarely used test suite
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{"."} / "Tests" / "51" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / "Tests" / "51" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / ".." / "Tests" / "51" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / ".." / ".." / "51" / "JSONTestData";
            }
            for (auto testCase : kTestCases_) {
                DoJSONParse_ (jsonTestRoot / "small-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                if constexpr (not qStroika_Foundation_Debug_AssertionsChecked) {
                    // don't bother testing these except in release builds - too slow
                    DoJSONParse_ (jsonTestRoot / "medium-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                    DoJSONParse_ (jsonTestRoot / "large-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                }
            }
        }

    }
}

namespace {
    /*
     *  Implementation-strategy comparisons for Iterable<T>/Sequence<T> algorithms - see the OrderBy
     *  and _PeekContiguousStorage entries in TODO.md. Two subjects share these fixtures:
     *      o   OrderBy () - which sorting strategy the reps should use.
     *      o   Materializing an Iterable into a vector<T> (As<vector<T>> ()) - which feeds OrderBy (),
     *          Top () and Repeat (), so it matters well beyond As<> itself.
     *
     *  Two KINDS of entry live here:
     *      o   Permanent entries in the regular run, so a future pessimization gets noticed. Where two
     *          exist for one operation they exercise different reps - see the comment on
     *          Real_SequenceOrderBy_ below.
     *      o   A multi-variant design probe, run ONLY with --orderby-probe, for choosing between
     *          candidate implementations. Off by default because it is slow and answers a design
     *          question rather than guarding a regression.
     *
     *  IMPORTANT for fairness: every OrderBy variant below performs a COMPLETE OrderBy () - including
     *  whatever copy that strategy cannot avoid. OrderBy () is const and returns a new Sequence, so even
     *  the "sort in place" strategies must first COW-clone the buffer they then sort. Timing only the
     *  sort would flatter them with a copy the real implementation still has to pay for.
     */
    namespace Test_IterableAlgorithms_ {

        constexpr size_t       kN_    = 1000;
        constexpr unsigned int kSeed_ = 20260805;

        /*
         *  Optimizer barrier. Every variant below funnels a value derived from its result into this
         *  volatile, because a store to a volatile is an observable side effect the compiler may not
         *  remove - which in turn keeps it from deleting the work that produced the value. Without a
         *  sink, a benchmark whose result is never read is dead code, and an optimizing Release build
         *  is entitled to delete the whole thing and report a time near zero.
         *
         *  Accumulated into (rather than assigned) so that no single iteration's value can be dropped
         *  as overwritten-before-read.
         */
        volatile size_t sOptimizerSink_ = 0;

        // Reduce one element to a size_t so Consume_ can fold any element type into sOptimizerSink_ - overloaded
        // per T because there is no common spelling for "some cheap value that depends on this element".
        size_t Magnitude_ (int v)
        {
            return static_cast<size_t> (v);
        }
        size_t Magnitude_ (const String& v)
        {
            return v.size ();
        }
        /*
         *  Read a real (order-dependent) element value, so the sort cannot be optimized away.
         *
         *  Only the FIRST element - deliberately. It is O(1) for both an indexable container and for
         *  Iterable<T>::First (), so the barrier costs the same in every variant. Reading the last
         *  element too would be O(1) via Sequence's operator[] but O(n) via Iterable, which would
         *  quietly bias any Sequence-vs-Iterable comparison.
         */
        template <typename CONTAINER_T>
        void Consume_ (const CONTAINER_T& c)
        {
            if (not c.empty ()) {
                sOptimizerSink_ = sOptimizerSink_ + c.size () + Magnitude_ (c[0]);
            }
        }
        template <typename T>
        void Consume_ (const Iterable<T>& c)
        {
            if (auto f = c.First ()) {
                sOptimizerSink_ = sOptimizerSink_ + c.size () + Magnitude_ (*f);
            }
        }

        const vector<int>& SourceInts_ ()
        {
            static const vector<int> kData_ = [] () {
                mt19937                       gen{kSeed_};
                uniform_int_distribution<int> dist{0, numeric_limits<int>::max ()};
                vector<int>                   r;
                r.reserve (kN_);
                for (size_t i = 0; i < kN_; ++i) {
                    r.push_back (dist (gen));
                }
                return r;
            }();
            return kData_;
        }
        const vector<String>& SourceStrings_ ()
        {
            static const vector<String> kData_ = [] () {
                vector<String> r;
                r.reserve (kN_);
                for (int i : SourceInts_ ()) {
                    r.push_back ("{}"_f(i));
                }
                return r;
            }();
            return kData_;
        }

        /*
         *  THE TYPE-ERASURE TAX. These two are identical except that D's comparer must cross a vtable
         *  and so is wrapped in a std::function<>, which isolates exactly that cost.
         *
         *  This pair started life as part of an OrderBy () design probe (should _IRep expose mutable
         *  storage, or take the comparer?) and has outlived that question - the in-place OrderBy () idea
         *  is decided and closed, see the DESIGN NOTE on Sequence<T>::_IRep. What survives is the
         *  general fact these measure: passing a comparer through a std::function<> costs ~3.4x for int
         *  and ~1.0x for String, ie type erasure is only affordable when the operation itself is
         *  expensive relative to an indirect call. That is why Min ()/Max ()/Sum () bypass Reduce ()
         *  rather than accelerate it, and it should govern any future erased-callback design.
         *
         *  The vector copy in both models the COW clone a const method must do before it may touch a
         *  rep's buffer; it is common to both sides, so it cancels out of the ratio.
         */
        template <typename T>
        void C_InPlaceInlined_ (const vector<T>& src)
        {
            vector<T> storage = src;
            stable_sort (storage.begin (), storage.end (), less<T>{});
            Consume_ (storage);
        }

        // ---- D: the "virtual takes the comparer" strategy - identical to C except the comparer must be
        //      type-erased to cross the vtable, so this isolates exactly that cost.
        template <typename T>
        void D_InPlaceErased_ (const vector<T>& src)
        {
            static const function<bool (Common::ArgByValueType<T>, Common::ArgByValueType<T>)> kErased_ = less<T>{};
            vector<T>                                                                          storage  = src;
            stable_sort (storage.begin (), storage.end (), kErased_);
            Consume_ (storage);
        }

        // plain-std baseline for the permanent regression entry
        template <typename T>
        void Baseline_StdStableSort_ (const vector<T>& src)
        {
            vector<T> storage = src;
            stable_sort (storage.begin (), storage.end (), less<T>{});
            Consume_ (storage);
        }

        /*
         *  ---- COPY-STRATEGY comparison, isolated from any sort.
         *
         *  OrderBy ()/Top ()/Repeat () all begin by materializing the Iterable into a vector<T>, so how
         *  that copy is done matters to all three. Copy_AsVector_ tracks whatever
         *  Iterable<T>::As<vector<T>> () currently does, so this keeps measuring the real thing if that
         *  implementation changes again.
         *
         *  MEASURED (Release/MSVC, Iterable<int>, N=1000; ratios vs the iterator-pair baseline, all
         *  within one run):
         *      iterator-pair  vector{begin,end}            1.00x   <- distance () + copy, 2 walks
         *      assign         assign (begin, end)          ~0.97x  <- same 2 walks, within noise of above
         *      As<vector<T>>  (currently the generic path) ~1.00x  <- ie the same thing
         *      reserve+walk   reserve (size ()) + push_back ~1.17x  <- consistently SLOWEST
         *  Conclusions that fell out of this:
         *      o   reserve () LOSES here. push_back pays a capacity check per element where the range
         *          CTOR's copy loop does not, and the size () call was itself O(n) for the rep measured
         *          (a generator over a type-erased function<>, whose length genuinely cannot be known
         *          without running it). Do NOT read that as "size () is O(n) on any non-array rep" - the
         *          linked-list reps are O(n) today only by choice, and say so themselves (see the note on
         *          Sequence_DoublyLinkedList's size ()). Two attempts at a special-cased As<vector<T>>
         *          both came out worse than doing nothing: Apply () + std::function was 1.33x, and
         *          reserve () + assign () was 1.55x (paying for the length THREE times - size (),
         *          distance (), then the copy).
         *      o   So the special case was disabled; the generic range CTOR is the thing to beat.
         *
         *  These MUST be compared within a single run - cross-run comparison is swamped by machine
         *  variance (measured: the plain std baseline moved 43% between two runs on this box, and this
         *  same iterator-pair baseline has ranged 0.335-0.436s across runs).
         */
        template <typename T>
        void Copy_IteratorPair_ (const Iterable<T>& it)
        {
            vector<T> tmp{it.begin (), Iterator<T>{it.end ()}};
            Consume_ (tmp);
        }
        template <typename T>
        void Copy_ReserveWalk_ (const Iterable<T>& it)
        {
            vector<T> tmp;
            tmp.reserve (it.size ());
            for (const auto& e : it) {
                tmp.push_back (e);
            }
            Consume_ (tmp);
        }
        /*
         *  Takes the CONCRETE container type, not const Iterable<T>&, on purpose - the static type decides
         *  which As () is called, and that has bitten once already: Sequence<T> used to declare its own
         *  As () hiding Iterable<T>::As (), so a version of this measured through an Iterable<T>& scored
         *  1.1 while the same call written the way a user writes it scored 113.
         */
        template <typename T, typename CONCRETE_T>
        void Copy_AsVector_Concrete_ (const CONCRETE_T& it)
        {
            vector<T> tmp = it.template As<vector<T>> ();
            Consume_ (tmp);
        }
        /*
         *  As<CONTAINER> () for a NON-vector target. Pairing this against the same call on a source
         *  without contiguous storage isolates the fast path: same operation, same target type, the only
         *  difference being whether PeekContiguousStorage () has anything to offer.
         */
        template <typename CONTAINER_T, typename SOURCE_T>
        void Copy_As_ (const SOURCE_T& it)
        {
            CONTAINER_T tmp = it.template As<CONTAINER_T> ();
            if (not tmp.empty ()) {
                sOptimizerSink_ = sOptimizerSink_ + tmp.size () + Magnitude_ (*tmp.begin ());
            }
        }
        template <typename T>
        void Copy_AsVector_ (const Iterable<T>& it)
        {
            vector<T> tmp = it.template As<vector<T>> ();
            Consume_ (tmp);
        }
        // vector::assign with a FORWARD iterator (Iterator<T> satisfies forward_iterator) makes the
        // library call distance () first, so this traverses the whole range twice - once to count,
        // once to copy - paying Stroika's virtual advance on every step of both walks.
        template <typename T>
        void Copy_Assign_ (const Iterable<T>& it)
        {
            vector<T> tmp;
            tmp.assign (it.begin (), Iterator<T>{it.end ()});
            Consume_ (tmp);
        }
        /*
         *  SequentialEquals () - the first real consumer built on PeekContiguousStorage (). Both sides
         *  compare the SAME two contiguous containers; only the mechanism differs, so the score is the
         *  fast path's own contribution. SeqEquals_Iterating_ is a hand copy of what the general path does
         *  (advance two Stroika iterators in lockstep), so it stays a valid 'before' even after the real
         *  implementation stops doing that. Equal sequences on purpose - that is the worst case, since a
         *  mismatch short-circuits.
         */
        template <typename CONTAINER_T>
        void SeqEquals_Fast_ (const CONTAINER_T& a, const CONTAINER_T& b)
        {
            sOptimizerSink_ = sOptimizerSink_ + (a.SequentialEquals (b) ? 1 : 0);
        }
        template <typename T, typename CONTAINER_T>
        void SeqEquals_Iterating_ (const CONTAINER_T& a, const CONTAINER_T& b)
        {
            auto ai = a.begin ();
            auto bi = b.begin ();
            auto ae = Iterator<T>{a.end ()};
            auto be = Iterator<T>{b.end ()};
            bool eq = true;
            for (; ai != ae and bi != be; ++ai, ++bi) {
                if (not(*ai == *bi)) {
                    eq = false;
                    break;
                }
            }
            sOptimizerSink_ = sOptimizerSink_ + ((eq and ai == ae and bi == be) ? 1 : 0);
        }
        /*
         *  Contains () / IndexOf () - the second real consumer built on PeekContiguousStorage ().
         *  The _Old_ helpers are hand copies of what each did BEFORE the fast path: Contains () asked
         *  Find () for an iterator and threw it away, and IndexOf () asked Find () to carry a counting
         *  side effect in its predicate. Both therefore paid type erasure into std::function, per-element
         *  virtual iteration, and an Iterator<T> construction. Keeping the old shape here rather than
         *  calling the real method means these stay valid 'before' baselines afterwards.
         *
         *  Searching for an ABSENT value on purpose - that is the worst case and the only one that
         *  measures the whole scan; a match short-circuits both sides at wherever it happens to sit.
         */
        template <typename T, typename CONTAINER_T>
        void Contains_Old_ (const CONTAINER_T& it, ArgByValueType<T> v)
        {
            sOptimizerSink_ = sOptimizerSink_ + (static_cast<bool> (it.Find ([&v] (T i) -> bool { return equal_to<T>{}(i, v); })) ? 1 : 0);
        }
        template <typename T, typename CONTAINER_T>
        void Contains_Fast_ (const CONTAINER_T& it, ArgByValueType<T> v)
        {
            sOptimizerSink_ = sOptimizerSink_ + (it.Contains (v) ? 1 : 0);
        }
        template <typename T, typename CONTAINER_T>
        void IndexOf_Old_ (const CONTAINER_T& it, ArgByValueType<T> v)
        {
            size_t n = 0;
            auto r = it.Find ([&n, &v] (ArgByValueType<T> ii) { return equal_to<T>{}(ii, v) ? true : (n++, false); }) ? optional<size_t>{n}
                                                                                                                      : optional<size_t>{};
            sOptimizerSink_ = sOptimizerSink_ + r.value_or (0);
        }
        template <typename T, typename CONTAINER_T>
        void IndexOf_Fast_ (const CONTAINER_T& it, ArgByValueType<T> v)
        {
            sOptimizerSink_ = sOptimizerSink_ + it.IndexOf (v).value_or (0);
        }
        /*
         *  Min () / Sum (). The _Old_ helpers call Reduce () with the very lambda the old
         *  implementations passed it, so they stay valid 'before' baselines. Reduce () takes its
         *  operation as a std::function<>, so these pay an indirect call PER ELEMENT on top of the
         *  virtual iteration - which is why the fast paths bypass Reduce () rather than accelerate it.
         *
         *  Sum () is measured for int only: over String it concatenates 1000 strings and the timing
         *  becomes dominated by reallocation rather than by iteration, which measures nothing useful.
         */
        template <typename T, typename CONTAINER_T>
        void Min_Old_ (const CONTAINER_T& it)
        {
            auto r          = it.template Reduce<T> ([] (T lhs, T rhs) -> T { return min (lhs, rhs); });
            sOptimizerSink_ = sOptimizerSink_ + (r ? Magnitude_ (*r) : 0);
        }
        template <typename CONTAINER_T>
        void Min_Fast_ (const CONTAINER_T& it)
        {
            auto r          = it.Min ();
            sOptimizerSink_ = sOptimizerSink_ + (r ? Magnitude_ (*r) : 0);
        }
        template <typename T, typename CONTAINER_T>
        void Sum_Old_ (const CONTAINER_T& it)
        {
            auto r          = it.template Reduce<T> ([] (T lhs, T rhs) { return lhs + rhs; });
            sOptimizerSink_ = sOptimizerSink_ + (r ? Magnitude_ (*r) : 0);
        }
        template <typename T, typename CONTAINER_T>
        void Sum_Fast_ (const CONTAINER_T& it)
        {
            auto r          = it.template Sum<T> ();
            sOptimizerSink_ = sOptimizerSink_ + (r ? Magnitude_ (*r) : 0);
        }
        /*
         *  The shape of a NON-COPYING consumer - Contains ()/Find ()/IndexOf ()/Min ()/Max ()/Sum ()/
         *  SequentialEquals () all touch every element but copy none of them. Walk_Iterators_ is how they
         *  read the container today; Walk_Contiguous_ is what they would do given a span. Neither calls
         *  As<> - this isolates per-element ITERATION cost with no copy diluting it.
         */
        template <typename SOURCE_T>
        void Walk_Iterators_ (const SOURCE_T& it)
        {
            size_t acc = 0;
            for (const auto& e : it) {
                acc += Magnitude_ (e);
            }
            sOptimizerSink_ = sOptimizerSink_ + acc;
        }
        template <typename T>
        void Walk_Contiguous_ (const vector<T>& src)
        {
            size_t acc = 0;
            for (const auto& e : src) {
                acc += Magnitude_ (e);
            }
            sOptimizerSink_ = sOptimizerSink_ + acc;
        }
        /*
         *  Baseline for the permanent As<vector<T>> () entry below: what this same data costs to copy
         *  with no Stroika machinery at all. That makes the entry's score 'what per-element virtual
         *  iteration costs over a bulk copy' - ie the gap a contiguous-storage hook on _IRep would
         *  close, which is why it is worth guarding permanently rather than only in the probe.
         *
         *  The extra sink of tmp.data () guards against a specific way this baseline could go wrong:
         *  Consume_ observes only size () and element [0], so an optimizer that proves tmp is a copy of
         *  src could rewrite both reads to hit src and delete the copy entirely, leaving this timing
         *  nothing. Escaping the buffer ADDRESS stops that. Verified on MSVC/Release that it makes no
         *  measurable difference today (ie no elision is happening) - it is insurance, not a fix.
         */
        template <typename T>
        void Baseline_VectorCopy_ (const vector<T>& src)
        {
            vector<T> tmp   = src;
            sOptimizerSink_ = sOptimizerSink_ + reinterpret_cast<size_t> (tmp.data ());
            Consume_ (tmp);
        }

        /*
         *  ---- BUILDING a Stroika container from N elements, which is the reverse direction from the
         *  As<vector<T>> () entries above and is NOT covered by them.
         *
         *  Every Sequence<T> constructor that takes a range or an iterable - and operator+= - routes through
         *  AppendAll (), which hoists the copy-on-write accessor out of the loop (good) and then makes one
         *  VIRTUAL _IRep::Insert () call per element, each wrapping span{&elt, 1}:
         *
         *      for (auto i = start; i != end; ++i) {
         *          const T& tmp = *i;
         *          accessor._GetWriteableRep ().Insert (_IRep::_kSentinelLastItemIndex, span{&tmp, 1u});
         *      }
         *
         *  So an N-element Sequence costs N virtual dispatches and N ReserveAtLeast () checks where one of
         *  each would do - _IRep::Insert () already takes a span<const T>, so the interface to batch through
         *  exists and no backend would have to change.
         *
         *  Those entries drove the fix and now guard it. Before batching: int 60-70x, String 5.0-5.9x,
         *  envelope-vs-concrete 1.25-1.37x. After: 1.30x, 1.00x, 0.99x - so an N-element Sequence<int> build
         *  went 0.171s -> 0.0042s (~40x), and Sequence<String> now costs exactly what copying a
         *  vector<String> costs, since both are just N string copies once the dispatch is gone.
         *
         *  Both element types are measured on purpose. The per-element overhead is fixed, so it should be a
         *  large share of the total for int and a small one for String - the same shape as every other
         *  erasure cost in this file - and that predicts where batching will and will not show up.
         */
        template <typename T>
        void Construct_Sequence_FromVector_ (const vector<T>& src)
        {
            Sequence<T> tmp{src};
            Consume_ (tmp);
        }
        template <typename T>
        void AppendAll_Sequence_FromVector_ (const vector<T>& src)
        {
            Sequence<T> tmp;
            tmp.AppendAll (src);
            Consume_ (tmp);
        }
        /*
         *  ---- THE TWO GAPS LEFT OVER from the AppendAll () batching, measured BEFORE closing them so
         *  there is a real before-number to quote. A ratio produced only after a change proves nothing.
         *
         *  (1) A STROIKA SOURCE. AppendAll ()'s fast path requires a contiguous_iterator, and Iterator<T>
         *      is not one - so 'dest.AppendAll (someSequence)' still pays one virtual _IRep::Insert () per
         *      element even when BOTH sides are array-backed and the source could hand over its whole
         *      buffer in one call. _IRep::PeekContiguousStorage () exists for precisely this and is what
         *      As<vector<T>> () already uses; AppendAll () simply never asks. Measured against a vector
         *      source - which DOES take the span path - on the same binary, so the score is exactly the
         *      penalty for the source being a Stroika container and nothing else.
         *
         *  (2) PREPEND. InsertAll () loops 'Insert (insertAt++, *ii)', so prepending m elements onto a
         *      container already holding n shifts those n elements m separate times: O (m*n), where one
         *      memmove plus one bulk copy would do.
         *
         *      The target MUST be non-empty to see this, which is why these take a populated Sequence
         *      rather than starting from {}. Into an empty target the advancing index makes every Insert ()
         *      land at the end, so it degenerates to an append and shifts nothing - the naive probe measures
         *      zero penalty and looks like there is no bug.
         *
         *  Both take the already-batched shape as their BASELINE and the un-batched shape as the
         *  comparison, so the score reads as "what this costs extra today" and stays meaningful as a
         *  regression gate afterwards (a threshold near 1 then says "still batched").
         */
        template <typename T>
        void AppendAll_Sequence_FromStroika_ (const Sequence<T>& src)
        {
            Sequence<T> tmp;
            tmp.AppendAll (src);
            Consume_ (tmp);
        }
        // The COW copy is O (1) and identical on both sides of the prepend pair, so it cancels out of the
        // ratio; it is here only to get a non-empty target whose buffer the insert has to shift.
        template <typename T>
        void AppendAll_Sequence_OntoNonEmpty_ (const Sequence<T>& target, const vector<T>& src)
        {
            Sequence<T> tmp{target};
            tmp.AppendAll (src);
            Consume_ (tmp);
        }
        template <typename T>
        void PrependAll_Sequence_OntoNonEmpty_ (const Sequence<T>& target, const vector<T>& src)
        {
            Sequence<T> tmp{target};
            tmp.PrependAll (src);
            Consume_ (tmp);
        }
        /*
         *  ---- A NON-CONTIGUOUS SOURCE (std::list here, but equally a generator or a lazy Where ()
         *  pipeline) can take NEITHER fast path: not the contiguous_iterator one, because a list iterator
         *  is not contiguous, and not PeekContiguousStorage (), because a list has no such storage. So it
         *  still pays one virtual _IRep::Insert () per element, and for InsertAll () it is still O (m*n) as
         *  well, since each element inserts separately at an advancing index.
         *
         *  Each pair below feeds the SAME data from a list and from a vector, so the score isolates exactly
         *  one thing: the cost of the source not being contiguous. That is the number that would justify (or
         *  kill) chunking a non-contiguous source through a stack buffer, which is not free - it copies
         *  source->buffer->container, ie TWO copies per element, so it should win big for int and could
         *  easily LOSE for String.
         */
        template <typename T>
        const list<T>& SourceAsList_ (const vector<T>& from)
        {
            static const list<T> kData_{from.begin (), from.end ()};
            return kData_;
        }
        template <typename T>
        void AppendAll_Sequence_FromList_ (const list<T>& src)
        {
            Sequence<T> tmp;
            tmp.AppendAll (src);
            Consume_ (tmp);
        }
        template <typename T>
        void InsertAll_Sequence_FromList_OntoNonEmpty_ (const Sequence<T>& target, const list<T>& src)
        {
            Sequence<T> tmp{target};
            tmp.InsertAll (2, src);
            Consume_ (tmp);
        }
        template <typename T>
        void InsertAll_Sequence_FromVector_OntoNonEmpty_ (const Sequence<T>& target, const vector<T>& src)
        {
            Sequence<T> tmp{target};
            tmp.InsertAll (2, src);
            Consume_ (tmp);
        }
        /*
         *  ---- THE SOURCE SIDE, for the containers whose _IRep::Add () still takes ONE item (Set, MultiSet,
         *  KeyedCollection - Collection's takes a span already). Filling one from a Stroika container costs
         *  TWO virtual calls per element: one to advance Iterator<T>, one to Add (). Filling it from a vector
         *  costs only the second, because a vector iterator is a raw pointer.
         *
         *  So this ratio is purely the SOURCE-side dispatch, with the destination doing identical work on
         *  both sides - which is what makes it the right measurement for whether asking the source for
         *  PeekContiguousStorage () is worth it WITHOUT any change to those rep interfaces.
         */
        template <typename CONTAINER, typename SRC>
        void AddAllInto_ (const SRC& src)
        {
            CONTAINER c;
            c.AddAll (src);
            sOptimizerSink_ = sOptimizerSink_ + c.size ();
        }
        /*
         *  The same source-side question for the other two single-item reps. Neither is a copy of Set:
         *
         *      o   MultiSet<T>::AddAll (ITERABLE) is templated on IIterableOfTo<TRAITS::CountedValueType>,
         *          so a Sequence<int> source is NOT an Iterable<CountedValue<int>> - a single derived_from
         *          check against the counted type would silently never fire for the common case. It also
         *          calls the PUBLIC Add () per element, which takes a fresh accessor each time - a cost a
         *          vector source pays too, and which hoisting the accessor would fix for both.
         *      o   KeyedCollection<T,KEY>::AddAll () returns a count, so a span walk must accumulate it.
         */
        template <typename SRC>
        void AddAll_MultiSet_ (const SRC& src)
        {
            MultiSet<int> c;
            c.AddAll (src);
            sOptimizerSink_ = sOptimizerSink_ + c.size ();
        }
        /*
         *  NB KeyedCollection<int,int> does NOT compile: with T and KEY_TYPE the same type,
         *  Contains (ArgByValueType<value_type>) and Contains (ArgByValueType<MAPPED_VALUE_TYPE>) collapse
         *  to one signature (likewise Remove/RemoveIf). Hence pair<int,int> keyed on .first - T and KEY
         *  stay distinct, and no new struct is needed.
         */
        using KCElt_ = pair<int, int>;
        const vector<KCElt_>& SourceKCElts_ ()
        {
            static const vector<KCElt_> kData_ = [] () {
                vector<KCElt_> r;
                r.reserve (SourceInts_ ().size ());
                for (int i : SourceInts_ ()) {
                    r.push_back (KCElt_{i, i});
                }
                return r;
            }();
            return kData_;
        }
        template <typename SRC>
        void AddAll_KeyedCollection_ (const SRC& src)
        {
            // the extractor is on both sides of the ratio, so its cost cancels
            KeyedCollection<KCElt_, int> c{[] (KCElt_ e) { return e.first; }};
            c.AddAll (src);
            sOptimizerSink_ = sOptimizerSink_ + c.size ();
        }
        /*
         *  The same build against the CONCRETE backend, to separate two costs that would otherwise be
         *  confounded: Sequence<T> also pays a factory call to pick a backend, where Sequence_Array<T> names
         *  one. If the concrete entry is much cheaper than the envelope entry, the factory is worth looking
         *  at separately from the per-element dispatch.
         */
        template <typename T>
        void Construct_SequenceArray_FromVector_ (const vector<T>& src)
        {
            Containers::Concrete::Sequence_Array<T> tmp{src};
            Consume_ (tmp);
        }

        /*
         *  ---- the REAL OrderBy (), called as a user would call it. Sequence<T>::OrderBy () and the
         *  Iterable<T>::OrderBy () it hides offer the same two overloads (comparer alone, or comparer +
         *  Execution::SequencePolicy) and differ only in return type, so the Sequence-vs-Iterable score below
         *  is the cost of the two reps rather than of an API divergence. eSeq-vs-ePar is measured for each,
         *  since parallel only wins once N is large enough to pay for the thread hand-off and the crossover
         *  is unmeasured - see the OrderBy entry in TODO.md.
         */
        template <typename T, typename SEQUENCE_T>
        void Real_SequenceOrderBy_ (const SEQUENCE_T& seq, Execution::SequencePolicy policy)
        {
            Consume_ (seq.OrderBy (less<T>{}, policy));
        }
        template <typename T>
        void Real_IterableOrderBy_ (const Iterable<T>& it, Execution::SequencePolicy policy)
        {
            Consume_ (it.OrderBy (less<T>{}, policy));
        }
        // The ...Default_ variants call OrderBy () with NO policy argument on purpose - the permanent
        // regression entries must guard whatever the DEFAULT path actually is, so they keep tracking
        // reality if the default is changed again (Iterable's already moved from ePar to eSeq once).
        template <typename T, typename SEQUENCE_T>
        void Real_SequenceOrderByDefault_ (const SEQUENCE_T& seq)
        {
            Consume_ (seq.OrderBy ());
        }
        template <typename T>
        void Real_IterableOrderByDefault_ (const Iterable<T>& it)
        {
            Consume_ (it.OrderBy ());
        }

        /*
         *  The design probe. Each Tester () call reports compareWith/baseline, so a score < 1 means the
         *  'compareWith' strategy is the faster one. The C-vs-D score IS the type-erasure tax, and is
         *  the number that decides whether _IRep::OrderBy () may take the comparer.
         */
        void RunProbe_ ()
        {
            constexpr unsigned int                             kRunCount_ = 30000;
            constexpr double                                   kNoWarn_   = 1000.0; // a probe has no regression threshold
            const Containers::Concrete::Sequence_Array<int>    kSeqInts_{SourceInts_ ()};
            const Containers::Concrete::Sequence_Array<String> kSeqStrs_{SourceStrings_ ()};
            // Distinct objects, equal contents - so SequentialEquals () must actually traverse, and cannot
            // short-circuit on identity or on a shared rep.
            const Containers::Concrete::Sequence_Array<int>    kSeqIntsCopy_{SourceInts_ ()};
            const Containers::Concrete::Sequence_Array<String> kSeqStrsCopy_{SourceStrings_ ()};
            const Iterable<int>                                kIterInts_{SourceInts_ ()};

            GetOutStream_ () << "=== Sequence<>::OrderBy () DESIGN PROBE (--orderby-probe) ===" << endl;
            GetOutStream_ () << "score < 1 means the second (compareWith) strategy is faster" << endl << endl;

            (void)Tester (
                "Type-erasure tax int: inlined comparer vs std::function comparer", [&] () { C_InPlaceInlined_<int> (SourceInts_ ()); },
                "comparer inlined", [&] () { D_InPlaceErased_<int> (SourceInts_ ()); }, "comparer via std::function", kRunCount_, kNoWarn_);

            /*
             *  COPY STRATEGY - which way should Iterable<T>::As<vector<T>> () materialize? This feeds
             *  OrderBy ()/Top ()/Repeat (), so it matters beyond As<> itself. All four are measured in
             *  ONE run on purpose: comparing across runs is swamped by machine variance (the plain std
             *  baseline was observed moving 43% between two runs on this box).
             */
            (void)Tester (
                "Copy strategy int: iterator-pair vs reserve+walk", [&] () { Copy_IteratorPair_<int> (kIterInts_); },
                "vector{begin,end} (2 walks: distance+copy)", [&] () { Copy_ReserveWalk_<int> (kIterInts_); }, "reserve+range-for (1 walk)",
                kRunCount_, kNoWarn_);
            (void)Tester (
                "Copy strategy int: reserve+walk vs As<vector<T>>  <== IS A SPECIAL CASE WORTH IT?",
                [&] () { Copy_ReserveWalk_<int> (kIterInts_); }, "reserve+range-for", [&] () { Copy_AsVector_<int> (kIterInts_); },
                "As<vector<T>> (as implemented)", kRunCount_, kNoWarn_);
            (void)Tester (
                "Copy strategy int: As<vector<T>> vs assign(begin,end)", [&] () { Copy_AsVector_<int> (kIterInts_); },
                "As<vector<T>> (as implemented)", [&] () { Copy_Assign_<int> (kIterInts_); }, "assign (2 walks)", kRunCount_, kNoWarn_);

            /*
             *  Does the As<> fast path help NON-vector targets? Both sides make the identical call on the
             *  identical data; only the source rep differs.
             *
             *  CAVEAT - this does NOT isolate the fast path. The slow side is a generic Iterable<int>,
             *  which is a generator rep, and that is slower than Sequence_Array for reasons that have
             *  nothing to do with contiguous storage - the "OrderBy divergence" entry below measures the
             *  same rep gap at ~1.5 with no fast path involved. So read these as "what a user gains by
             *  holding a contiguous container", not as the fast path's own contribution. The clean
             *  measurement of the fast path itself is the As<vector<int>> permanent entry, which sits at
             *  ~1.0 against a raw memcpy.
             *
             *  Even so the ordering is informative: a std::list allocates a node per element, so the
             *  target's own cost swamps any source-iteration saving, while a Stroika target does not.
             */
            (void)Tester (
                "As<> fast path, non-vector target: list<int> from Sequence_Array vs from generic Iterable",
                [&] () { Copy_As_<list<int>> (kSeqInts_); }, "As<list<int>> (contiguous source)",
                [&] () { Copy_As_<list<int>> (kIterInts_); }, "As<list<int>> (no contiguous storage)", kRunCount_ / 10, kNoWarn_);
            (void)Tester (
                "As<> fast path, Stroika target: Sequence<int> from Sequence_Array vs from generic Iterable",
                [&] () { Copy_As_<Sequence<int>> (kSeqInts_); }, "As<Sequence<int>> (contiguous source)",
                [&] () { Copy_As_<Sequence<int>> (kIterInts_); }, "As<Sequence<int>> (no contiguous storage)", kRunCount_ / 10, kNoWarn_);

            /*
             *  DOES PeekContiguousStorage () STILL PAY FOR A NON-TRIVIAL T?
             *
             *  Everything the hook has been justified with so far was measured on int, where the fast path
             *  replaces per-element virtual iteration with a memcpy AND the per-element copy is free. For a
             *  T whose copy costs something, the fast path still removes the ITERATION but not the COPY, so
             *  its share of the total has to fall. How far it falls decides how much the remaining
             *  consumers (SequentialEquals (), Contains ()/Find ()/IndexOf (), Min ()/Max ()/Sum ()) are
             *  worth building.
             *
             *  The first two entries pair TODAY's As<vector<T>> () against what As<vector<T>> () DID before
             *  the fast path existed - the range CTOR over Stroika's iterators - on the same contiguous
             *  source. Only the copy mechanism differs, so the score is the fast path's own contribution,
             *  and the int and String numbers are comparable because they are taken in one run.
             *  score << 1 = big win for that T; score ~1 = the hook bought nothing.
             *
             *  NB Stroika's String is refcounted, so copying one is an atomic increment, not a deep copy -
             *  cheap, but not free the way an int's memcpy is. So this is the interesting MIDDLE case, not
             *  the worst case. A T with a genuinely expensive copy would score closer to 1 still.
             */
            (void)Tester (
                "Non-trivial T: the fast path's own contribution, int (old As<> vs new As<>)",
                [&] () { Copy_IteratorPair_<int> (kSeqInts_); }, "range CTOR over Stroika iterators (the old As<>)",
                [&] () { Copy_AsVector_Concrete_<int> (kSeqInts_); }, "As<vector<int>> (fast path)", kRunCount_, kNoWarn_);
            (void)Tester (
                "Non-trivial T: the fast path's own contribution, String  <== THE DECISION",
                [&] () { Copy_IteratorPair_<String> (kSeqStrs_); }, "range CTOR over Stroika iterators (the old As<>)",
                [&] () { Copy_AsVector_Concrete_<String> (kSeqStrs_); }, "As<vector<String>> (fast path)", kRunCount_ / 10, kNoWarn_);
            // And how close to the floor String now sits - the String analogue of the permanent
            // As<vector<int>> entry, whose score against a raw copy is ~1.0.
            (void)Tester (
                "Non-trivial T: As<vector<String>> () vs plain vector<String> copy",
                [&] () { Baseline_VectorCopy_<String> (SourceStrings_ ()); }, "vector<String> copy CTOR",
                [&] () { Copy_AsVector_Concrete_<String> (kSeqStrs_); }, "Sequence_Array<String>::As<vector<String>> ()", kRunCount_ / 10, kNoWarn_);

            /*
             *  ---- The OTHER direction: BUILDING a Sequence<T> from N elements, which nothing above measures.
             *
             *  AppendAll () makes one virtual _IRep::Insert () call per element (see the note on
             *  Construct_Sequence_FromVector_), so these are the before-numbers for batching that through the
             *  span<const T> overload _IRep::Insert () already has. Three questions, one entry each:
             *
             *      1. how much does the per-element dispatch cost, for a cheap T
             *      2. does it still matter when T is expensive to copy (predicted: much less)
             *      3. is the envelope's factory lookup a separate cost from the dispatch, or noise
             */
            (void)Tester (
                "Build Sequence<int> from vector<int> vs vector<int> copy CTOR", [&] () { Baseline_VectorCopy_<int> (SourceInts_ ()); },
                "vector<int> copy CTOR", [&] () { Construct_Sequence_FromVector_<int> (SourceInts_ ()); }, "Sequence<int>{vector<int>}",
                kRunCount_, kNoWarn_);
            (void)Tester (
                "Build Sequence<String> from vector<String> vs vector<String> copy CTOR",
                [&] () { Baseline_VectorCopy_<String> (SourceStrings_ ()); }, "vector<String> copy CTOR",
                [&] () { Construct_Sequence_FromVector_<String> (SourceStrings_ ()); }, "Sequence<String>{vector<String>}", kRunCount_ / 10, kNoWarn_);
            (void)Tester (
                "Build: Sequence<int> envelope vs Sequence_Array<int> concrete (isolates the factory)",
                [&] () { Construct_SequenceArray_FromVector_<int> (SourceInts_ ()); }, "Sequence_Array<int>{vector<int>}",
                [&] () { Construct_Sequence_FromVector_<int> (SourceInts_ ()); }, "Sequence<int>{vector<int>}", kRunCount_, kNoWarn_);
            // AppendAll () onto an already-built Sequence, in case the CTOR path differs from the append path
            (void)Tester (
                "Build Sequence<int> via AppendAll () vs via CTOR (same rep calls either way?)",
                [&] () { Construct_Sequence_FromVector_<int> (SourceInts_ ()); }, "Sequence<int>{vector<int>}",
                [&] () { AppendAll_Sequence_FromVector_<int> (SourceInts_ ()); }, "Sequence<int>{}.AppendAll (vector<int>)", kRunCount_, kNoWarn_);

            /*
             *  ...and what a NON-COPYING consumer would gain, which is the number that actually decides
             *  whether SequentialEquals ()/Contains ()/Find ()/IndexOf ()/Min ()/Max ()/Sum () are worth
             *  building on the hook. Those touch every element but copy none, so the fast path's saving is
             *  not diluted by a per-element copy the way As<vector<T>> ()'s is. If the String score here
             *  lands well below the score As<vector<String>> () gets above, then the COPY - not the
             *  iteration - is what caps that number, and these consumers are worth more than it suggests.
             */
            (void)Tester (
                "Non-copying consumer shape, int: Stroika iteration vs contiguous walk", [&] () { Walk_Iterators_ (kSeqInts_); },
                "walk via Iterator<int>", [&] () { Walk_Contiguous_<int> (SourceInts_ ()); }, "walk contiguous storage", kRunCount_, kNoWarn_);
            (void)Tester (
                "Non-copying consumer shape, String: Stroika iteration vs contiguous walk", [&] () { Walk_Iterators_ (kSeqStrs_); },
                "walk via Iterator<String>", [&] () { Walk_Contiguous_<String> (SourceStrings_ ()); }, "walk contiguous storage",
                kRunCount_ / 10, kNoWarn_);

            /*
             *  SequentialEquals () - the prediction the "non-copying consumer shape" entries above make,
             *  now measured on the real thing. int should beat the 46x that shape scored, because dropping
             *  the predicate lets the library collapse equal spans to a memcmp; String should land near
             *  its 13x, since it still pays a String compare per element either way.
             */
            (void)Tester (
                "SequentialEquals int: lockstep Stroika iterators vs contiguous fast path",
                [&] () { SeqEquals_Iterating_<int> (kSeqInts_, kSeqIntsCopy_); }, "lockstep Iterator<int>",
                [&] () { SeqEquals_Fast_ (kSeqInts_, kSeqIntsCopy_); }, "SequentialEquals (fast path)", kRunCount_, kNoWarn_);
            (void)Tester (
                "SequentialEquals String: lockstep Stroika iterators vs contiguous fast path",
                [&] () { SeqEquals_Iterating_<String> (kSeqStrs_, kSeqStrsCopy_); }, "lockstep Iterator<String>",
                [&] () { SeqEquals_Fast_ (kSeqStrs_, kSeqStrsCopy_); }, "SequentialEquals (fast path)", kRunCount_ / 10, kNoWarn_);

            /*
             *  Contains () / IndexOf (). Predicted from THE MODEL (see TODO.md): the saving is the fixed
             *  per-element iteration (~6ns int, ~12ns String), and what remains is the comparison itself.
             *  int equality is ~free, so this should look like SequentialEquals's int case; String
             *  equality dominates, so expect ~2x there.
             */
            constexpr int kAbsentInt_ = -1; // SourceInts_ () is drawn from [0, INT_MAX], so never present
            const String  kAbsentStr_ = "no-such-element"sv;
            (void)Tester (
                "Contains int: old Find ()-based vs contiguous fast path", [&] () { Contains_Old_<int> (kSeqInts_, kAbsentInt_); },
                "Contains via Find () (the old way)", [&] () { Contains_Fast_<int> (kSeqInts_, kAbsentInt_); }, "Contains (fast path)",
                kRunCount_, kNoWarn_);
            (void)Tester (
                "Contains String: old Find ()-based vs contiguous fast path", [&] () { Contains_Old_<String> (kSeqStrs_, kAbsentStr_); },
                "Contains via Find () (the old way)", [&] () { Contains_Fast_<String> (kSeqStrs_, kAbsentStr_); }, "Contains (fast path)",
                kRunCount_ / 10, kNoWarn_);
            (void)Tester (
                "IndexOf int: old Find ()-with-counter vs contiguous fast path", [&] () { IndexOf_Old_<int> (kSeqInts_, kAbsentInt_); },
                "IndexOf via Find () (the old way)", [&] () { IndexOf_Fast_<int> (kSeqInts_, kAbsentInt_); }, "IndexOf (fast path)",
                kRunCount_, kNoWarn_);
            (void)Tester (
                "IndexOf String: old Find ()-with-counter vs contiguous fast path",
                [&] () { IndexOf_Old_<String> (kSeqStrs_, kAbsentStr_); }, "IndexOf via Find () (the old way)",
                [&] () { IndexOf_Fast_<String> (kSeqStrs_, kAbsentStr_); }, "IndexOf (fast path)", kRunCount_ / 10, kNoWarn_);

            /*
             *  Min () / Sum (). These should beat the other consumers by MORE than the model predicts,
             *  because the old path paid the std::function indirect call per element on top of the
             *  virtual iteration - two costs removed rather than one.
             */
            (void)Tester (
                "Min int: old Reduce ()-based vs contiguous fast path", [&] () { Min_Old_<int> (kSeqInts_); },
                "Min via Reduce () (the old way)", [&] () { Min_Fast_ (kSeqInts_); }, "Min (fast path)", kRunCount_, kNoWarn_);
            (void)Tester (
                "Min String: old Reduce ()-based vs contiguous fast path", [&] () { Min_Old_<String> (kSeqStrs_); },
                "Min via Reduce () (the old way)", [&] () { Min_Fast_ (kSeqStrs_); }, "Min (fast path)", kRunCount_ / 10, kNoWarn_);
            (void)Tester (
                "Sum int: old Reduce ()-based vs contiguous fast path", [&] () { Sum_Old_<int> (kSeqInts_); },
                "Sum via Reduce () (the old way)", [&] () { Sum_Fast_<int> (kSeqInts_); }, "Sum (fast path)", kRunCount_, kNoWarn_);

            // Rep cost only - the two OrderBy ()s offer the same overloads, and the no-policy one currently
            // picks eSeq in both.
            (void)Tester (
                "OrderBy divergence: Sequence<int>::OrderBy () vs Iterable<int>::OrderBy () [both default]",
                [&] () { Real_SequenceOrderByDefault_<int> (kSeqInts_); }, "Sequence::OrderBy (default)",
                [&] () { Real_IterableOrderByDefault_<int> (kIterInts_); }, "Iterable::OrderBy (default)", kRunCount_, kNoWarn_);
            // The two entries that say whether eSeq is the right default. Measured separately for each
            // because they materialize their vector<T> differently, so the sort is a different share of
            // the total - and it shows: ePar costs 2.08x on Sequence but 1.81x on Iterable, whose larger
            // copy dilutes it. Both still say eSeq at N=1000; the crossover where ePar wins is unmeasured.
            (void)Tester (
                "OrderBy policy: Sequence<int>::OrderBy () eSeq vs ePar",
                [&] () { Real_SequenceOrderBy_<int> (kSeqInts_, Execution::SequencePolicy::eSeq); }, "Sequence::OrderBy (eSeq)",
                [&] () { Real_SequenceOrderBy_<int> (kSeqInts_, Execution::SequencePolicy::ePar); }, "Sequence::OrderBy (ePar)", kRunCount_, kNoWarn_);
            (void)Tester (
                "OrderBy policy: Iterable<int>::OrderBy () eSeq vs ePar",
                [&] () { Real_IterableOrderBy_<int> (kIterInts_, Execution::SequencePolicy::eSeq); }, "Iterable::OrderBy (eSeq)",
                [&] () { Real_IterableOrderBy_<int> (kIterInts_, Execution::SequencePolicy::ePar); }, "Iterable::OrderBy (ePar)", kRunCount_, kNoWarn_);

            // String as well as int, because the tax is only affordable where the operation itself is
            // expensive relative to an indirect call - and String is exactly that case (~1.0x, ie free).
            (void)Tester (
                "Type-erasure tax String: inlined comparer vs std::function comparer",
                [&] () { C_InPlaceInlined_<String> (SourceStrings_ ()); }, "comparer inlined",
                [&] () { D_InPlaceErased_<String> (SourceStrings_ ()); }, "comparer via std::function", kRunCount_ / 10, kNoWarn_);
        }
    }
}

namespace {
    void RunPerformanceTests_ ()
    {
#if 0
        // for profiling, handy to JUST do one test I'm working on for the profile - stick in here
        {
            using namespace JSONTests_;
            using filesystem::path;
            path jsonTestRoot = path{"."} / "52" / "JSONTestData";
            // hack a bit to find jsonTestRoot, since sometimes run from different places; no need to do good/formal job here
            // since this is for a rarely used test suite
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{"."} / "Tests" / "52" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / "Tests" / "52" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / path{".."} / "Tests" / "52" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / path{".."} / path{".."} / "52" / "JSONTestData";
            }

            // WEIRD/CONFUSING - TIMES APPEAR TO DEPEND (ALOT ON WINDOWS) on order of calls!!! SO CAREFUL COMPARING
            DoJSONParse_ (jsonTestRoot / "large-dict.json", 5, DoStroikaJSONParse_, "stroika-default-json");
            DoJSONParse_ (jsonTestRoot / "large-dict.json", 5, DoStroikaJSONParse_forcedNative_, "stroika-native-json");
            DoJSONParse_ (jsonTestRoot / "large-dict.json", 5, DoStroikaJSONParse_nlohmann_json, "nlohmann");
            DoJSONParse_ (jsonTestRoot / "large-dict.json", 5, DoStroikaJSONParse_boost_json, "boost_json");
            DoJSONParse_ (jsonTestRoot / "large-dict.json", 5, DoStroikaJSONParse_boost_json2Stk, "stroika-via-boost-json");
            return;
        }
#endif

        DateTime startedAt = DateTime::Now ();
        GetOutStream_ () << "Performance score 1.0 means both sides equal (ratio), and tests setup so lower is generally better" << endl
                         << endl;
        GetOutStream_ () << "[[[Started testing at: " << startedAt.Format () << "]]]" << endl << endl;
        if (not Math::NearlyEquals (sTimeMultiplier_, 1.0)) {
            GetOutStream_ () << "Using TIME MULTIPLIER: " << sTimeMultiplier_ << endl << endl;
        }

        /*
         *  Pin to a single logical CPU core, so that a run measures the code rather than the scheduler
         *  moving it between cores partway through. Measured on Windows x86_64, this takes the run-to-run
         *  spread from 8.3% median (39% worst) down to 2.6% (17% worst) - about 3x tighter - while leaving
         *  the scores themselves alone (median pinned/unpinned ratio 0.992x over 34 tests).
         *
         *  Done HERE and not only in Build/Scripts/RunPerformanceRegressionTests, because that script is
         *  not the only way these get run: 'make run-tests' runs this test as an ordinary gtest, and the
         *  exe gets run by hand. Pinning in the harness covers every path. It is also idempotent - if the
         *  script already pinned us, our permitted set is that one core and this re-picks it.
         *
         *  SKIPPED for --orderby-probe, which deliberately measures eSeq against ePar: on a single core
         *  ePar is pathological and that comparison would be meaningless.
         *
         *  Reported into the dump on purpose. A dump that does not record how it was measured cannot
         *  safely be compared against one measured differently, and the archive already has that problem.
         */
        if (sRunOrderByProbe_) {
            GetOutStream_ () << "NOT pinning to a CPU core: --orderby-probe measures parallel policies" << endl << endl;
        }
        else if (optional<unsigned int> pinnedTo = Execution::PinToOneLogicalCPUCoreQuietly ()) {
            GetOutStream_ () << "Pinned to logical CPU core: " << *pinnedTo << endl << endl;
        }
        else {
            GetOutStream_ () << "NOT pinned to a CPU core (not available here) - expect more run-to-run variance" << endl << endl;
        }

        Set<String> failedTests;

        /*
         *  ABOUT THE warnIfPerformanceScoreHigherThan ARGUMENT (the last double in each Tester () call).
         *
         *  These are calibrated for ONE configuration - the only one that can warn. See
         *  kPrintOutIfFailsToMeetPerformanceExpectations_ above: it requires _MSC_VER, a release build, block
         *  allocation, no default tracing, and sizeof (void*) == 8. So a warning is possible on Windows
         *  x86_64 release ONLY, and the wide spread of these scores across platforms (about 4x, MacOS m1 and
         *  32-bit being the far ends) is irrelevant to picking these numbers. Calibrate against the
         *  Windows-x86_64 dumps in Tests/HistoricalPerformanceRegressionTestResults/ and nothing else -
         *  mixing the other platforms in produces values 2-4x too loose, which is silently useless.
         *
         *  Rederive with:
         *
         *      Tests/Scripts/AnalyzePerformanceThresholds.py [PerformanceDump.txt]
         *
         *  which flags the two failure modes: DEAD (threshold so far above the real score that only a
         *  multi-x regression trips it - 7 were, before 3.0d24) and HAIR-TRIGGER (threshold at or below the
         *  typical value, so it warns constantly and means nothing - about 20 were).
         *
         *  AND CALIBRATE FROM THE LOWEST MULTIPLIER YOU CARE ABOUT, not from the archive's -x 15 dumps.
         *  The score is multiplier-invariant for the FOUR-argument Tester () - both sides are measured, so the
         *  multiplier cancels - but NOT for the two-argument form, where the baseline is synthetic (1/runCount
         *  then scaled, i.e. exactly the multiplier in seconds) while the comparison is real work. If that work
         *  does not scale linearly, the ratio moves: Test_JSONReadWriteFile runs 0.62ms/iteration at -x 1 but
         *  0.40ms at -x 15 as warmup and file caching amortize, so it scores 0.396 versus 0.255. Calibrating
         *  those two from -x 15 alone (as the 3.0d24 pass first did) produced a threshold that fired on plain
         *  'make run-tests', which passes no -x at all.
         *
         *  The rule used for the 3.0d24 pass: worst per-release MEDIAN on Windows x86_64 over the releases
         *  since the workloads were reworked, times 1.20. Per-release median rather than worst single
         *  observation, because one outlier run would otherwise set the bound (String Characters::Format ()
         *  ranges 2.88-3.54 per release but has a lone 5.99, which would have set 6.89 instead of 4.25).
         *  The 1.20 is about 3x the 7% per-release run-to-run noise measured on this platform, so ordinary
         *  jitter cannot trip it.
         *
         *  THAT MARGIN CAN SHRINK, once there is pinned history to calibrate from.
         *  Build/Scripts/RunPerformanceRegressionTests now pins the run to one CPU, and measured back to
         *  back on this box that takes the run-to-run spread from 8.3% median (39% worst) down to 2.9%
         *  median (17% worst) - about 3x tighter - while moving the scores themselves not at all (median
         *  pinned/unpinned 0.992x, and both pinned runs warn on nothing with the values below). The
         *  numbers below are still derived from UNPINNED dumps, since that is all the archive holds. Once
         *  a few pinned releases exist, rederiving with a ~1.10 margin would make these detect roughly
         *  twice as small a regression as they can today.
         *
         *  AND NOTE: a threshold is only meaningful while the workload behind it is unchanged. Every one of
         *  these went stale when the tests were reworked around 3.0d11-d16 - the +=wchar_t[] 100x baseline
         *  went 5.8s -> 93.9s, which moved its score 66 -> 7 while leaving the threshold at 78, i.e. dead
         *  for four releases. Do not calibrate across that boundary, and if you change what a test measures,
         *  rederive its threshold in the same commit.
         *
         *  Tests too new to calibrate (fewer than 4 releases of data) are deliberately left alone.
         */

#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            Stroika::Frameworks::Test::WarnTestIssue ("qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy and "
                                                      "Debug::kBuiltWithUndefinedBehaviorSanitizer so skipping test");
            return;
        }
#endif

        Tester ("Test of simple locking strategies (mutex v shared_ptr copy)", Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK, "mutex",
                Test_MutexVersusSharedPtrCopy_shared_ptr_copy, "shared_ptr<> copy", 24500, 1.7, &failedTests);
        Tester ("Test of simple locking strategies (mutex v SpinLock)", Test_MutexVersusSpinLock_MUTEXT_LOCK, "mutex",
                Test_MutexVersusSpinLock_SPIN_LOCK, "SpinLock", 24500, 0.83, &failedTests);
        Tester ("Simple Struct With Strings Filling And Copying", Test_StructWithStringsFillingAndCopying<wstring>, "wstring",
                Test_StructWithStringsFillingAndCopying<String>, "Characters::String", 65000, 0.6, &failedTests);
        Tester ("Simple Struct With Strings Filling And Copying2", Test_StructWithStringsFillingAndCopying2<wstring>, "wstring",
                Test_StructWithStringsFillingAndCopying2<String>, "Characters::String", 66000, 0.76, &failedTests);
        Tester ("Simple String append test (+='string object') 10x", Test_SimpleStringAppends1_<wstring>, "wstring",
                Test_SimpleStringAppends1_<String>, "Characters::String", 1350000, 2.08, &failedTests);
        Tester ("Simple String append test (+=wchar_t[]) 10x", Test_SimpleStringAppends2_<wstring>, "wstring",
                Test_SimpleStringAppends2_<String>, "Characters::String", 1500000, 5.86, &failedTests);
        Tester ("Simple String append test (+=wchar_t[]) 100x", Test_SimpleStringAppends3_<wstring>, "wstring",
                Test_SimpleStringAppends3_<String>, "Characters::String", 360000, 10, &failedTests);
        Tester ("String a + b", Test_SimpleStringConCat1_<wstring>, "wstring", Test_SimpleStringConCat1_<String>, "String", 2200000, 2.05, &failedTests);
        Tester ("wstringstream << test", Test_OperatorINSERT_ostream_<wstring>, "wstring", Test_OperatorINSERT_ostream_<String>,
                "Characters::String", 6000, 1.58, &failedTests);
        Tester ("String::substr()", Test_StringSubStr_<wstring>, "wstring", Test_StringSubStr_<String>, "Characters::String", 2700000, 2.47, &failedTests);
        struct MemStreamOfChars_ : public MemoryStream::Ptr<Characters::Character> {
            MemStreamOfChars_ ()
                : Ptr{MemoryStream::New<Characters::Character> ()}
            {
            }
        };
        Tester (
            "wstringstream versus BasicTextOutputStream",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, "wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<MemStreamOfChars_> ([] (const MemStreamOfChars_& w) { return w.As<String> (); });
            },
            "MemoryStream<Characters::Character>", 210000, 1.57, &failedTests);
        Tester (
            "wstringstream versus StringBuilder",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, "wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<StringBuilder<>> ([] (const StringBuilder<>& w) { return w.As<String> (); });
            },
            "StringBuilder", 220000, 0.53, &failedTests);
        Tester ("Sequence<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, "vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<int>>, "Sequence<int>", 125000, 1.44, &failedTests);
        /*
         *  Sequence<T> vs vector<T>, one operation per entry - see the note on namespace ContainerVsStd_.
         *  NEW NAMES on purpose: the archive keys its 30-release history on the test name, so re-pointing an
         *  existing name at a different workload silently invalidates it. That is exactly what happened to
         *  "Simple String append test (+=wchar_t[]) 100x" around 3.0d11-d16, whose baseline went 5.8s -> 93.9s
         *  and score 66 -> 7 with nothing saying so.
         *
         *  ONE ELEMENT TYPE PER BACKEND, deliberately. The int/String pair on the DEFAULT backend already
         *  characterizes what the element type costs, so repeating both types for every concrete backend
         *  would add entries without adding signal. int for the array-backed ones (where the question is
         *  realloc and memcpy) and String for the node-based ones (where it is one allocation per element).
         *
         *  EVERY container gets "add many at once", INCLUDING the ones whose AddAll () currently just loops.
         *  That is the point of those entries rather than an argument against them: c384915a32 batched
         *  Sequence<T>::AppendAll () only, and the same optimization is outstanding for Collection/Set/MultiSet
         *  (see TODO.md). A gate can only ever notice a regression; what motivates the work, and what will
         *  later prove it landed, is a BEFORE number. Any of these reading far above its add-one-at-a-time
         *  sibling is naming a container whose rep does not take a span yet.
         *
         *  Caveat on those, since it affects how far the thresholds can be trusted: Tester () uses one
         *  runCount for both sides, and add-many on a vector<int> is a memcpy (~66ns for 500 elements), so a
         *  runCount that keeps the Stroika side to a fraction of a second leaves the BASELINE only a few ms.
         *  Short baselines are noisier, so those entries carry more margin than the rest.
         */
        Tester ("Sequence<int> vs vector<int>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddOneAtATime_<Sequence<int>>, "Sequence<int>", 40000, 4.9, &failedTests);
        Tester ("Sequence<String> vs vector<String>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<String>>, "vector<String>",
                ContainerVsStd_::AddOneAtATime_<Sequence<String>>, "Sequence<String>", 20000, 3.7, &failedTests);
        Tester ("Sequence<int> vs vector<int>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddManyAtOnce_<Sequence<int>>, "Sequence<int>", 200000, 2.8, &failedTests);
        Tester ("Sequence<String> vs vector<String>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<String>>, "vector<String>",
                ContainerVsStd_::AddManyAtOnce_<Sequence<String>>, "Sequence<String>", 20000, 1.5, &failedTests);
        Tester ("Sequence<int> vs vector<int>: copy (COW)", ContainerVsStd_::CopyOnly_<vector<int>>, "vector<int>",
                ContainerVsStd_::CopyOnly_<Sequence<int>>, "Sequence<int>", 400000, 0.47, &failedTests);
        Tester ("Sequence<String> vs vector<String>: copy (COW)", ContainerVsStd_::CopyOnly_<vector<String>>, "vector<String>",
                ContainerVsStd_::CopyOnly_<Sequence<String>>, "Sequence<String>", 100000, 0.005, &failedTests);
        Tester ("Sequence<String> vs vector<String>: copy then write (COW pays up)", ContainerVsStd_::CopyThenWrite_<vector<String>>,
                "vector<String>", ContainerVsStd_::CopyThenWrite_<Sequence<String>>, "Sequence<String>", 20000, 2.4, &failedTests);
        Tester ("Sequence<int> vs vector<int>: copy then write (COW pays up)", ContainerVsStd_::CopyThenWrite_<vector<int>>, "vector<int>",
                ContainerVsStd_::CopyThenWrite_<Sequence<int>>, "Sequence<int>", 300000, 4.5, &failedTests);
        /*
         *  The concrete backends, same operations. What the DEFAULT Sequence<T> entries above cannot show is
         *  that the four operations do not rank the backends the same way: an array-backed rep appends with
         *  amortized realloc and breaks copy-on-write with one memcpy, where a node-based one pays one
         *  allocation per element for BOTH. The old blended "..._basics" entries averaged those two effects
         *  together, which is why Sequence_DoublyLinkedList<int> needed a threshold of 11 and nobody could
         *  say which half of the workload it came from.
         */
        Tester ("Sequence_Array<int> vs vector<int>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Sequence_Array<int>>, "Sequence_Array<int>", 40000, 2.7, &failedTests);
        Tester ("Sequence_Array<int> vs vector<int>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Sequence_Array<int>>, "Sequence_Array<int>", 200000, 2.3, &failedTests);
        Tester ("Sequence_stdvector<int> vs vector<int>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Sequence_stdvector<int>>, "Sequence_stdvector<int>", 40000, 3.6, &failedTests);
        Tester ("Sequence_stdvector<int> vs vector<int>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Sequence_stdvector<int>>, "Sequence_stdvector<int>", 200000, 2.5, &failedTests);
        Tester ("Sequence_DoublyLinkedList<String> vs vector<String>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Sequence_DoublyLinkedList<String>>,
                "Sequence_DoublyLinkedList<String>", 10000, 3.0, &failedTests);
        Tester ("Sequence_DoublyLinkedList<String> vs vector<String>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Sequence_DoublyLinkedList<String>>,
                "Sequence_DoublyLinkedList<String>", 10000, 6.1, &failedTests);
        Tester ("Sequence_DoublyLinkedList<String> vs vector<String>: copy then write (COW pays up)", ContainerVsStd_::CopyThenWrite_<vector<String>>,
                "vector<String>", ContainerVsStd_::CopyThenWrite_<Containers::Concrete::Sequence_DoublyLinkedList<String>>,
                "Sequence_DoublyLinkedList<String>", 10000, 3.4, &failedTests);
        /*
         *  Collection<T>, the same operations. Note this is NOT the same comparison as Sequence<T> versus
         *  vector<T> even though the baseline is still vector<T>: Collection<T> promises no order, and its
         *  default factory picks SortedCollection_stdmultiset<T> for any totally_ordered T (see
         *  Collection_Factory.inl), so "add one at a time" here buys a sort nobody asked for. That is the
         *  real cost being reported, and it is a FACTORY choice rather than anything about Collection<T> -
         *  which is why Collection_LinkedList<String> and Collection_stdforward_list<String> are here too.
         *
         *  The elements are DISTINCT strings, so the degenerate all-identical-keys case that the old "with
         *  rnd strings" entry existed to dodge cannot arise. One fewer entry, by construction rather than by
         *  a second workload.
         */
        Tester ("Collection<int> vs vector<int>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddOneAtATime_<Collection<int>>, "Collection<int>", 10000, 20, &failedTests);
        Tester ("Collection<int> vs vector<int>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddManyAtOnce_<Collection<int>>, "Collection<int>", 20000, 725, &failedTests);
        Tester ("Collection<String> vs vector<String>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddOneAtATime_<Collection<String>>, "Collection<String>", 5000, 13, &failedTests);
        Tester ("Collection<String> vs vector<String>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<String>>, "vector<String>",
                ContainerVsStd_::AddManyAtOnce_<Collection<String>>, "Collection<String>", 5000, 35, &failedTests);
        Tester ("Collection<String> vs vector<String>: copy (COW)", ContainerVsStd_::CopyOnly_<vector<String>>, "vector<String>",
                ContainerVsStd_::CopyOnly_<Collection<String>>, "Collection<String>", 100000, 0.005, &failedTests);
        Tester ("Collection<String> vs vector<String>: copy then write (COW pays up)", ContainerVsStd_::CopyThenWrite_<vector<String>>,
                "vector<String>", ContainerVsStd_::CopyThenWrite_<Collection<String>>, "Collection<String>", 10000, 3.9, &failedTests);
        Tester ("Collection_LinkedList<String> vs vector<String>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Collection_LinkedList<String>>,
                "Collection_LinkedList<String>", 10000, 2.3, &failedTests);
        Tester ("Collection_stdforward_list<String> vs vector<String>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Collection_stdforward_list<String>>,
                "Collection_stdforward_list<String>", 10000, 2.3, &failedTests);
        Tester ("Collection_LinkedList<String> vs vector<String>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_LinkedList<String>>,
                "Collection_LinkedList<String>", 10000, 4.6, &failedTests);
        Tester ("Collection_stdforward_list<String> vs vector<String>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<String>>,
                "vector<String>", ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_stdforward_list<String>>,
                "Collection_stdforward_list<String>", 10000, 4.7, &failedTests);
        /*
         *  Collection_Array<int> - the rep where the span-taking _IRep::Add () has a real bulk path
         *  (Array::Insert (at, span) reserves once, then one Memory::Insert), as opposed to the
         *  sorted reps above where per-element tree/skiplist insertion dominates regardless.
         */
        /*
         *  THE LOOKUP SIDE of the factory's sorted-by-default choice (see TODO.md). Baseline is
         *  Collection_Array (linear scan over contiguous memory), comparison is Collection<int>/<String>
         *  - i.e. SortedCollection_stdmultiset, which overrides _IRep::Find_equal_to () so Contains ()
         *  really does descend the tree.
         *
         *  Score BELOW 1.0 means the sorted default is earning its ~8x/200x add penalty; ABOVE 1.0
         *  means at this size the array wins on lookups too, and the default is simply wrong.
         */
        (void)Tester ("Contains () each PRESENT, Collection_Array<int> vs sorted default",
                      ContainerVsStd_::ContainsEachPresent_<Containers::Concrete::Collection_Array<int>>, "Collection_Array<int>",
                      ContainerVsStd_::ContainsEachPresent_<Collection<int>>, "Collection<int> (sorted)", 2000,
                      1000.0 /* probe - answers a design question, not a gate */);
        (void)Tester ("Contains () each ABSENT, Collection_Array<int> vs sorted default",
                      ContainerVsStd_::ContainsEachAbsent_<Containers::Concrete::Collection_Array<int>>, "Collection_Array<int>",
                      ContainerVsStd_::ContainsEachAbsent_<Collection<int>>, "Collection<int> (sorted)", 2000,
                      1000.0 /* probe - answers a design question, not a gate */);
        (void)Tester ("Contains () each PRESENT, Collection_Array<String> vs sorted default",
                      ContainerVsStd_::ContainsEachPresent_<Containers::Concrete::Collection_Array<String>>, "Collection_Array<String>",
                      ContainerVsStd_::ContainsEachPresent_<Collection<String>>, "Collection<String> (sorted)", 300,
                      1000.0 /* probe - answers a design question, not a gate */);
        (void)Tester ("Contains () each ABSENT, Collection_Array<String> vs sorted default",
                      ContainerVsStd_::ContainsEachAbsent_<Containers::Concrete::Collection_Array<String>>, "Collection_Array<String>",
                      ContainerVsStd_::ContainsEachAbsent_<Collection<String>>, "Collection<String> (sorted)", 300,
                      1000.0 /* probe - answers a design question, not a gate */);
        /*
         *  And the OTHER factory branch: for non-ordered T it picks Collection_LinkedList when asked to
         *  optimize for updates. That predates Collection_Array having a bulk insert, so it may now be
         *  backwards - matched element type here so the two are actually comparable.
         */
        (void)Tester ("add many at once, Collection_Array<String> vs Collection_LinkedList<String>",
                      ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_Array<String>>, "Collection_Array<String>",
                      ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_LinkedList<String>>, "Collection_LinkedList<String>",
                      10000, 1000.0 /* probe - answers a design question, not a gate */);
        (void)Tester ("add one at a time, Collection_Array<String> vs Collection_LinkedList<String>",
                      ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Collection_Array<String>>, "Collection_Array<String>",
                      ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Collection_LinkedList<String>>, "Collection_LinkedList<String>",
                      10000, 1000.0 /* probe - answers a design question, not a gate */);
        Tester ("Collection_Array<int> vs vector<int>: add one at a time", ContainerVsStd_::AddOneAtATime_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddOneAtATime_<Containers::Concrete::Collection_Array<int>>, "Collection_Array<int>", 40000, 3.2, &failedTests);
        /*
         *  What the span-taking _IRep::Add () actually bought. Baseline is a std::list source, which
         *  takes AddAll ()'s per-element branch - the pre-change code path exactly - and comparison is
         *  the same call from a vector, which takes the span branch. A score WELL BELOW 1.0 is the
         *  batching win; 1.0 would mean it bought nothing.
         *
         *  A probe (kNoWarn_), not a gate: both sides move together with allocator state, and the
         *  number wanted here is a ratio to read, not a regression line to defend.
         */
        (void)Tester ("Collection_Array<int>: AddAll from vector vs from list (batching win)",
                      ContainerVsStd_::AddManyAtOnce_FromNonContiguous_<Containers::Concrete::Collection_Array<int>>,
                      "from list (per-element)", ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_Array<int>>,
                      "from vector (span)", 100000, 1000.0 /* probe - no regression threshold */);
        (void)Tester ("Collection<int>: AddAll from vector vs from list (batching win)",
                      ContainerVsStd_::AddManyAtOnce_FromNonContiguous_<Collection<int>>, "from list (per-element)",
                      ContainerVsStd_::AddManyAtOnce_<Collection<int>>, "from vector (span)", 20000, 1000.0 /* probe - no regression threshold */);
        Tester ("Collection_Array<int> vs vector<int>: add many at once", ContainerVsStd_::AddManyAtOnce_<vector<int>>, "vector<int>",
                ContainerVsStd_::AddManyAtOnce_<Containers::Concrete::Collection_Array<int>>, "Collection_Array<int>", 200000, 3.8, &failedTests);
        /*
         *  GUARDS THE AppendAll () BATCHING. Building a Sequence<int> from a contiguous source hands the whole
         *  range to _IRep::Insert () as one span; if that fast path ever stops firing - a concept tweak, a
         *  refactor that loses contiguity, a source type that no longer satisfies it - this drops straight
         *  back to one virtual dispatch per element and the score goes from ~1.7 to ~60, so even a loose
         *  threshold catches the thing this exists to catch. 3.3 is 1.30x the worst of 10 runs: the entry
         *  reads 1.65-1.88 seven times out of eight and then produces 2.48, and 2.3 (set from too few runs)
         *  duly fired on one of those excursions.
         *
         *  Unlike the --orderby-probe entries this runs every time, which is the point: a probe that has to be
         *  asked for cannot notice a regression. \note the probe there shares this entry's NAME - harmless,
         *  since the two never appear in one dump, but do not assume the name identifies one Tester call.
         */
        Tester (
            "Build Sequence<int> from vector<int> vs vector<int> copy CTOR",
            [] () { Test_IterableAlgorithms_::Baseline_VectorCopy_<int> (Test_IterableAlgorithms_::SourceInts_ ()); }, "vector<int> copy CTOR",
            [] () { Test_IterableAlgorithms_::Construct_Sequence_FromVector_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
            "Sequence<int>{vector<int>}", 1000000, 3.3, &failedTests);
        /*
         *  The two remaining batching gaps - see the long comment on AppendAll_Sequence_FromStroika_ ().
         *  PROBES (no threshold) for now: these exist to capture the BEFORE numbers. Once the fix lands and
         *  the scores settle, the useful ones become gates with a threshold near 1.0, which then reads as
         *  "a contiguous source is still taking the bulk path".
         *
         *  Run counts here are first guesses at ~1s per side (kN_ = 1000 elements per call, and the prepend
         *  pair is O (m*n), so it needs far fewer iterations than the append pair) - recalibrate from the
         *  reported times rather than trusting these.
         */
        {
            static const Sequence<int>                              kSrcSeqInts_{Test_IterableAlgorithms_::SourceInts_ ()};
            static const Sequence<Test_IterableAlgorithms_::KCElt_> kSrcSeqKCElts_{Test_IterableAlgorithms_::SourceKCElts_ ()};
            static const Sequence<String>                           kSrcSeqStrs_{Test_IterableAlgorithms_::SourceStrings_ ()};
            (void)Tester (
                "Sequence<int>::AppendAll (): Stroika source vs vector source",
                [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromVector_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
                "from vector<int> (span path)", [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromStroika_<int> (kSrcSeqInts_); },
                "from Sequence<int> (per-element)", 50000, 1000.0 /* probe - no threshold until the fix lands */);
            (void)Tester (
                "Sequence<String>::AppendAll (): Stroika source vs vector source",
                [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromVector_<String> (Test_IterableAlgorithms_::SourceStrings_ ()); },
                "from vector<String> (span path)",
                [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromStroika_<String> (kSrcSeqStrs_); },
                "from Sequence<String> (per-element)", 20000, 1000.0 /* probe - no threshold until the fix lands */);
            (void)Tester (
                "Sequence<int>: PrependAll () vs AppendAll (), onto a NON-EMPTY target",
                [] () {
                    Test_IterableAlgorithms_::AppendAll_Sequence_OntoNonEmpty_<int> (kSrcSeqInts_, Test_IterableAlgorithms_::SourceInts_ ());
                },
                "AppendAll (batched)",
                [] () {
                    Test_IterableAlgorithms_::PrependAll_Sequence_OntoNonEmpty_<int> (kSrcSeqInts_, Test_IterableAlgorithms_::SourceInts_ ());
                },
                "PrependAll (O (m*n))", 2000, 1000.0 /* probe - no threshold until the fix lands */);
            (void)Tester (
                "Sequence<String>: PrependAll () vs AppendAll (), onto a NON-EMPTY target",
                [] () {
                    Test_IterableAlgorithms_::AppendAll_Sequence_OntoNonEmpty_<String> (kSrcSeqStrs_, Test_IterableAlgorithms_::SourceStrings_ ());
                },
                "AppendAll (batched)",
                [] () {
                    Test_IterableAlgorithms_::PrependAll_Sequence_OntoNonEmpty_<String> (kSrcSeqStrs_, Test_IterableAlgorithms_::SourceStrings_ ());
                },
                "PrependAll (O (m*n))", 200, 1000.0 /* probe - no threshold until the fix lands */);
            /*
             *  The NON-CONTIGUOUS source, and the source-side dispatch for the still-single-item reps.
             *  See the comments on SourceAsList_ () and AddAllInto_ (). All probes: these exist to decide
             *  whether the remaining two AppendAll () batching items are worth building at all.
             */
            (void)Tester (
                "Sequence<int>::AppendAll (): vector source vs LIST source",
                [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromVector_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
                "from vector<int> (span path)",
                [] () {
                    Test_IterableAlgorithms_::AppendAll_Sequence_FromList_<int> (
                        Test_IterableAlgorithms_::SourceAsList_<int> (Test_IterableAlgorithms_::SourceInts_ ()));
                },
                "from list<int> (per-element)", 50000, 1000.0 /* probe */);
            (void)Tester (
                "Sequence<String>::AppendAll (): vector source vs LIST source",
                [] () { Test_IterableAlgorithms_::AppendAll_Sequence_FromVector_<String> (Test_IterableAlgorithms_::SourceStrings_ ()); },
                "from vector<String> (span path)",
                [] () {
                    Test_IterableAlgorithms_::AppendAll_Sequence_FromList_<String> (
                        Test_IterableAlgorithms_::SourceAsList_<String> (Test_IterableAlgorithms_::SourceStrings_ ()));
                },
                "from list<String> (per-element)", 20000, 1000.0 /* probe */);
            (void)Tester (
                "Sequence<int>::InsertAll (middle): vector source vs LIST source",
                [] () {
                    Test_IterableAlgorithms_::InsertAll_Sequence_FromVector_OntoNonEmpty_<int> (kSrcSeqInts_, Test_IterableAlgorithms_::SourceInts_ ());
                },
                "from vector<int> (one span insert)",
                [] () {
                    Test_IterableAlgorithms_::InsertAll_Sequence_FromList_OntoNonEmpty_<int> (
                        kSrcSeqInts_, Test_IterableAlgorithms_::SourceAsList_<int> (Test_IterableAlgorithms_::SourceInts_ ()));
                },
                "from list<int> (still O (m*n))", 2000, 1000.0 /* probe */);
            (void)Tester (
                "Set<int>::AddAll (): vector source vs STROIKA source (source-side dispatch)",
                [] () { Test_IterableAlgorithms_::AddAllInto_<Set<int>> (Test_IterableAlgorithms_::SourceInts_ ()); }, "from vector<int>",
                [] () { Test_IterableAlgorithms_::AddAllInto_<Set<int>> (kSrcSeqInts_); }, "from Sequence<int>", 5000, 1000.0 /* probe */);
            (void)Tester (
                "MultiSet<int>::AddAll (): vector source vs STROIKA source (source-side dispatch)",
                [] () { Test_IterableAlgorithms_::AddAll_MultiSet_ (Test_IterableAlgorithms_::SourceInts_ ()); }, "from vector<int>",
                [] () { Test_IterableAlgorithms_::AddAll_MultiSet_ (kSrcSeqInts_); }, "from Sequence<int>", 5000, 1000.0 /* probe */);
            (void)Tester (
                "KeyedCollection<int,int>::AddAll (): vector source vs STROIKA source (source-side dispatch)",
                [] () { Test_IterableAlgorithms_::AddAll_KeyedCollection_ (Test_IterableAlgorithms_::SourceKCElts_ ()); },
                "from vector<pair<int,int>>", [] () { Test_IterableAlgorithms_::AddAll_KeyedCollection_ (kSrcSeqKCElts_); },
                "from Sequence<pair<int,int>>", 5000, 1000.0 /* probe */);
        }
#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
        // Sequence<int>::append_range () against std::vector<int>::append_range () - the same operation, so
        // the score is the cost of Stroika's per-element _IRep::Insert () dispatch. See the batching @todo in
        // Sequence.h; this entry is what will show that work landing.
        Tester ("Sequence<int>::append_range () vs vector<int>::append_range ()", Test_AppendRange_StdVector_, "vector<int>::append_range ()",
                Test_AppendRange_Sequence_, "Sequence<int>::append_range ()", 500000, 1000.0 /* probe, not a gate - see note */, &failedTests);
#endif
        Tester ("Sequence<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, "vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<string>>, "Sequence<string>", 9900, 0.31, &failedTests);
        Tester ("Sequence_DoublyLinkedList<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, "vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<int>>,
                "Sequence_DoublyLinkedList<int>", 120000, 11, &failedTests);
        Tester ("Sequence_Array<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, "vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_Array<int>>, "Sequence_Array<int>", 120000, 1.42, &failedTests);
        Tester ("Sequence_stdvector<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, "vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_stdvector<int>>, "Sequence_stdvector<int>", 120000,
                1.97, &failedTests);
        Tester ("Sequence_DoublyLinkedList<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, "vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<string>>,
                "Sequence_DoublyLinkedList<string>", 9900, 0.84, &failedTests);
        Tester (
            "Collection<int> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<int>> ([] (vector<int>* c) { c->push_back (2); }); }, "vector<int>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<int>> ([] (Collection<int>* c) { c->Add (2); }); },
            "Collection<int>", 113000, 12, &failedTests);
        Tester (
            "Collection<string> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, "vector<string>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<string>> ([] (Collection<string>* c) { c->Add (string{}); }); },
            "Collection<string>", 9600, 1.38, &failedTests);
        {
            // In Stroika 2.1b15, we changed the default Collection factory to use SortedCollection_stdmultiset, so include
            // Collection_stdforward_list for comparison (maybe orig was something else but this works).
            //
            // That default is NOT a "small pessimization" on the add path, which is what this file measures: see the
            // "Collection_Array<int> ... add" entries above - one-at-a-time costs ~8x more through the sorted multiset,
            // and add-many ~200x more, because each element pays a tree insertion. It buys O(log n) Contains ()/Remove ()
            // instead, which NOTHING here measures - so do not read these numbers as a verdict on the factory choice.
            using Containers::Concrete::Collection_LinkedList;
            using Containers::Concrete::Collection_stdforward_list;
            using Containers::Concrete::SortedCollection_stdmultiset;
            Tester (
                "Collection_LinkedList<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, "vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_LinkedList<string>> (
                        [] (Collection_LinkedList<string>* c) { c->Add (string{}); });
                },
                "Collection_LinkedList<string>", 9600, 0.86, &failedTests);
            Tester (
                "Collection_stdforward_list<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, "vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_stdforward_list<string>> (
                        [] (Collection_stdforward_list<string>* c) { c->Add (string{}); });
                },
                "Collection_stdforward_list<string>", 9600, 0.92, &failedTests);
            Tester (
                "SortedCollection_stdmultiset<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, "vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<SortedCollection_stdmultiset<string>> (
                        [] (SortedCollection_stdmultiset<string>* c) { c->Add (string{}); });
                },
                "SortedCollection_stdmultiset<string>", 9600, 1.38, &failedTests);
        }
        {
            using Containers::Concrete::SortedCollection_stdmultiset;
            // In Stroika 2.1b15, we changed the default Collection factory to use SortedCollection_stdmultiset. This is probably a good choice,
            // but is a small pessimization when we have ALL IDENTICAL strings (so all conflicts). Just avoid that
            static const vector<string> kRandomStrings_ = [] () {
                vector<string> r;
                r.reserve (100);
                for (int i = 0; i < 100; ++i) {
                    char buf[1024];
                    snprintf (buf, std::size (buf), "hello %d", i);
                    r.push_back (buf);
                }
                return r;
            }();
            // this would do much better if we cared about mem usage, or did lookups, remove, etc...
            Tester (
                "SortedCollection_stdmultiset<string> basics with rnd strings",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<vector<string>> (
                        [] (vector<string>* c) { c->push_back (kRandomStrings_[rand () % kRandomStrings_.size ()]); });
                },
                "vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<SortedCollection_stdmultiset<string>> (
                        [] (SortedCollection_stdmultiset<string>* c) { c->Add (kRandomStrings_[rand () % kRandomStrings_.size ()]); });
                },
                "SortedCollection_stdmultiset<string>", 9600, 1.8, &failedTests);
        }
        Tester ("std::set<int> vs Set<int>", Test_SetvsSet_<set<int>>, "set<int>", Test_SetvsSet_<Set<int>>, "Set<int>", 13000, 0.31, &failedTests);
        Tester ("String Characters::Format ()", Test_String_Format_<wstring>, "sprintf", Test_String_Format_<String>,
                "String Characters::Format", 2100000, 4.25, &failedTests);
        Tester ("BLOB versus vector<byte>", Test_BLOB_Versus_Vector_Byte<vector<byte>>, "vector<byte>",
                Test_BLOB_Versus_Vector_Byte<Memory::BLOB>, "BLOB", 13000, 1.26, &failedTests);
        Tester ("BLOB versus vector<byte> ver#2", Test_BLOB_Versus_Vector_Byte_2<vector<byte>>, "vector<byte>",
                Test_BLOB_Versus_Vector_Byte_2<Memory::BLOB>, "BLOB", 5000, 1.17, &failedTests);
        Tester ("Test_JSONReadWriteFile", Test_JSONReadWriteFile_::DoRunPerfTest, "Test_JSONReadWriteFile",
                Debug::IsRunningUnderValgrind () ? 2 : 640, 0.48, &failedTests);
        Tester ("Test_Optional_", Test_Optional_::DoRunPerfTest, "Test_Optional_", 4875, 0.46, &failedTests);
        {
            // Guards against OrderBy () being pessimized. Baseline is what the same sort costs with plain
            // std machinery, so the score is 'what Stroika's OrderBy () adds over stable_sort'.
            // Threshold: measured 0.94-1.02 across runs - ie Sequence<T>::OrderBy () now costs about what a
            // raw std::stable_sort does, and sometimes less (the baseline copies too, and Sequence_stdvector
            // adopts its vector by move). It scored 2.23-2.63 until _IRep::PeekContiguousStorage () removed
            // the copy, so 1.5 both leaves flap room and catches a fall back to that regime.
            static const Containers::Concrete::Sequence_Array<int> kSeqInts_{Test_IterableAlgorithms_::SourceInts_ ()};
            static const Iterable<int>                             kIterInts_{Test_IterableAlgorithms_::SourceInts_ ()};
            // A DISTINCT object with equal contents, so SequentialEquals () below must traverse the whole
            // sequence rather than short-circuit on identity or on a shared rep.
            static const Containers::Concrete::Sequence_Array<int> kSeqIntsCopy_{Test_IterableAlgorithms_::SourceInts_ ()};
            Tester (
                "Sequence<int>::OrderBy () vs std::stable_sort",
                [] () { Test_IterableAlgorithms_::Baseline_StdStableSort_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
                "std::stable_sort (vector<int>)", [] () { Test_IterableAlgorithms_::Real_SequenceOrderByDefault_<int> (kSeqInts_); },
                "Sequence<int>::OrderBy ()", 140000, 1.5, &failedTests);
            // Separate entry because these are different reps, not because they are different operations
            // (the APIs match now). Both deliberately exercise the DEFAULT path rather than naming a
            // policy, so they keep guarding whatever the default actually is - Iterable's already moved
            // ePar -> eSeq once, which silently turned an earlier version of this entry into a guard on a
            // non-default path. The explicit eSeq-vs-ePar measurements live in --orderby-probe instead.
            //
            // Threshold: measured 3.98 here. Set to 5.0 rather than something snug, because this score
            // has ranged 3.98-5.17 across runs on one machine (it is sensitive to load, and moved with
            // the As<vector<T>> implementation). 4.0 gave 0.6% headroom and flapped. Per this file's
            // convention this is a gross-change alarm, not a tight bound.
            // It reads high next to Sequence's 2.5 partly because these are different containers -
            // Sequence_Array<int> vs a generic Iterable<int> rep - not purely an OrderBy () difference.
            Tester (
                "Iterable<int>::OrderBy () vs std::stable_sort",
                [] () { Test_IterableAlgorithms_::Baseline_StdStableSort_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
                "std::stable_sort (vector<int>)", [] () { Test_IterableAlgorithms_::Real_IterableOrderByDefault_<int> (kIterInts_); },
                "Iterable<int>::OrderBy () [default policy]", 140000, 5.0, &failedTests);
            /*
             *  Guards As<vector<T>> () over a CONTIGUOUS backend - the case a contiguous-storage hook on
             *  _IRep would accelerate (see TODO.md), and the one no entry covered before: the probe's
             *  "Copy strategy" entries all run against a generic Iterable<int> rep, which has no
             *  contiguous storage to expose, so they cannot see such a change at all.
             *
             *  Baseline is a plain vector<int> copy of the same data, so the score is what Stroika's
             *  As<vector<T>> () costs over a bulk copy - ie ~1.0 means it IS a bulk copy.
             *
             *  This entry earns its keep: it scored ~170 before _IRep::PeekContiguousStorage () existed
             *  (memcpy moves an int in ~0.06ns; iterating one through Iterable<T>'s virtuals costs ~11ns),
             *  and ~113 after, while Sequence<T>::As () still hid Iterable<T>::As () and so never reached
             *  the fast path. Both regressions are silent and neither is visible in any other entry.
             *
             *  runCount deliberately breaks this file's baseline-should-take-about-1-second convention.
             *  It was chosen when the two sides differed by >100x, where no single count suited both; kept
             *  as-is so the pre/post numbers above stay comparable.
             *
             *  THRESHOLD - set from what this entry must DETECT, not from what it happens to MEASURE.
             *
             *  It was 1.5, picked from 4 standalone Release runs that read 0.97-1.07, and it flapped: the
             *  same entry reads 1.69-1.77 inside 'make run-tests', ie in the run that actually gates. That
             *  is not machine load, it is measurement CONTEXT - Test52 runs after 30+ other test binaries,
             *  so heap and cache state are nothing like a fresh process, and both sides here total only
             *  ~15-25ms, which is small enough for that to move the ratio ~2x on its own. Bare metal,
             *  docker, and github-actions runners each shift it again, and by different amounts.
             *
             *  Re-tuning to any ONE of those distributions just relocates the flap to the others, so do
             *  not do that. Instead note the gap between signal and noise: the regressions this exists to
             *  catch are ~113 and ~170, while every legitimate reading anywhere is under ~2. There are two
             *  orders of magnitude of empty space in between, so the threshold can sit almost anywhere in
             *  it. 10 is roughly log-centred: ~6x above the highest legitimate reading seen (1.77), ~11x
             *  below the regression it must catch (113). A context quirk cannot plausibly cover 6x when
             *  the observed spread is 2x; a fall off the fast path cannot plausibly hide under 11x.
             *
             *  Tightening this back toward 1.0 buys NOTHING - there is no known regression mode that lands
             *  between 2 and 113 - and costs a flapping gate, which is worse than no gate because it
             *  teaches people to ignore the suite.
             */
            Tester (
                "Sequence_Array<int>::As<vector<int>> () vs plain vector copy",
                [] () { Test_IterableAlgorithms_::Baseline_VectorCopy_<int> (Test_IterableAlgorithms_::SourceInts_ ()); },
                "vector<int> copy CTOR", [] () { Test_IterableAlgorithms_::Copy_AsVector_Concrete_<int> (kSeqInts_); },
                "Sequence_Array<int>::As<vector<int>> ()", 200000, 10.0, &failedTests);
            /*
             *  Guards SequentialEquals () over two CONTIGUOUS backends. The baseline advances two Stroika
             *  iterators in lockstep - literally what the general path does, and what this cost before
             *  _IRep::PeekContiguousStorage () - so the score is the fast path's own contribution:
             *  13.6ns/element -> 0.13ns/element, ie the comparer is dropped where it is the default one
             *  and the whole thing becomes a memcmp.
             *
             *  What this catches, and nothing else does: the fast path SILENTLY not engaging. It is gated
             *  on both operands offering a span, so a backend that stops overriding PeekContiguousStorage (),
             *  a change that makes the constructible_from guard stop matching, or a subclass that hides
             *  SequentialEquals () (which is exactly what Sequence<T> once did to As ()) all fall back to
             *  iteration with no other symptom.
             *
             *  Threshold: measured ~0.0099. 0.1 is a 10x margin, and a fall back to iteration scores ~1.0,
             *  ie 10x the OTHER side of the threshold - so this cannot flap the way the two 1.5-threshold
             *  entries above can. That is the general point: an entry whose two sides differ by orders of
             *  magnitude makes a robust gate; one whose sides are nearly equal does not (see TODO.md).
             *  It does not catch losing JUST the memcmp while keeping the span walk - that would score
             *  ~0.05 and pass. Guarding the big, silent regression robustly beats guarding both flakily.
             */
            Tester (
                "Sequence_Array<int>::SequentialEquals () vs lockstep Stroika iterators",
                [] () { Test_IterableAlgorithms_::SeqEquals_Iterating_<int> (kSeqInts_, kSeqIntsCopy_); }, "lockstep Iterator<int>",
                [] () { Test_IterableAlgorithms_::SeqEquals_Fast_ (kSeqInts_, kSeqIntsCopy_); }, "Sequence_Array<int>::SequentialEquals ()",
                75000, 0.1, &failedTests);
        }
        JSONTests_::Run ();

        GetOutStream_ () << "[[[Tests took: " << (DateTime::Now () - startedAt).PrettyPrint () << "]]]" << endl << endl;

        // extra tests
        {
            if (sRunOrderByProbe_) {
                Test_IterableAlgorithms_::RunProbe_ ();
            }
        }

        if (not failedTests.empty ()) {
            StringBuilder listAsMsg;
            failedTests.Apply ([&listAsMsg] (String i) {
                if (not listAsMsg.empty ()) {
                    listAsMsg += ", "sv;
                }
                listAsMsg += i;
            });
            if (sShowOutput_) {
                Stroika::Frameworks::Test::WarnTestIssue (
                    ("At least one test did not meet expected time constraint (see above): " + listAsMsg.As<String> ()));
            }
            else {
                Stroika::Frameworks::Test::WarnTestIssue ((Format ("At least one test ({}) did not meet expected time constraint (see {})"_f,
                                                                   listAsMsg.As<String> (), String{kDefaultPerfOutFile_})));
            }
        }
    }
}

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Stroika_Performance_Tests, all)
    {
        RunPerformanceTests_ ();
    }
}
#endif

namespace {
    // just temp hack to test one thing or another - which is performance related
    namespace TemporaryTest_ {
        void DoTest_ ()
        {
        }
    }
}
#endif

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    const CommandLine::Option kShowO_{.fLongName = "show"sv, .fHelpOptionText = "Show output of performance tests (defaults to only summary is shown)"sv};
    const CommandLine::Option kTimeMultiplierO_{
        .fSingleCharName = 'x', .fLongName = "x"sv, .fSupportsArgument = true, .fHelpOptionText = "Factor applied to number of iterations of each test"sv};
    const CommandLine::Option kOrderByProbeO_{
        .fLongName       = "orderby-probe"sv,
        .fHelpOptionText = "Also run the OrderBy () implementation-strategy probe (slow; a design aid, not a regression test)"sv};

    const CommandLine::Option kGTestBriefO_{.fLongName = "gtest_brief"sv};
    const CommandLine::Option kGTestO_{.fLongName = "gtest"sv};

    const Sequence<CommandLine::Option> kAllOptions_{
        Execution::StandardCommandLineOptions::kHelp, kShowO_, kTimeMultiplierO_, kOrderByProbeO_, kGTestBriefO_, kGTestO_};

    // NOTE: run with --show or look for output in PERF-OUT.txt
    CommandLine cmdLine{argc, argv};
    try {
        cmdLine.Validate (kAllOptions_);
        sShowOutput_      = cmdLine.Has (kShowO_);
        sRunOrderByProbe_ = cmdLine.Has (kOrderByProbeO_);
        if (auto o = cmdLine.GetArgument (kTimeMultiplierO_)) {
            sTimeMultiplier_ = FloatConversion::ToFloat<double> (*o);
        }
    }
    catch (const InvalidCommandLineArgument&) {
        cerr << Characters::ToString (current_exception ()) << endl;
        cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        auto exc = current_exception ();
        cerr << "Usage: " << Characters::ToString (exc) << endl;
        exit (EXIT_FAILURE);
    }

#if qStroika_HasComponent_googletest
    TemporaryTest_::DoTest_ ();

    if (sShowOutput_) {
        RunPerformanceTests_ ();
        return EXIT_SUCCESS;
    }
#endif

    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
