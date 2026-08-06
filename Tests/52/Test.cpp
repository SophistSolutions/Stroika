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
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/Containers/Mapping.h"
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
    // --orderby-probe: opt-in design probe, not part of the regular regression run (see Test_OrderBy_)
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
         *  Tests based on http://stroika-bugs.sophists.com/browse/STK-781
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
     *  OrderBy () implementation-strategy comparison, for both Sequence<T> and Iterable<T> - see the
     *  OrderBy entry in TODO.md.
     *
     *  Two things live here:
     *      o   Permanent entries in the regular run (Sequence<int>::OrderBy () and
     *          Iterable<int>::OrderBy () each vs std::stable_sort), so a future pessimization of
     *          either gets noticed. They are separate entries because they are not the same
     *          operation - see the comment on Real_IterableOrderBy_ below.
     *      o   A multi-variant design probe, run ONLY with --orderby-probe, for choosing between the
     *          candidate implementations. Off by default because it is slow and answers a design
     *          question rather than guarding a regression.
     *
     *  IMPORTANT for fairness: every variant below performs a COMPLETE OrderBy () - including whatever
     *  copy that strategy cannot avoid. OrderBy () is const and returns a new Sequence, so even the
     *  "sort in place" strategies must first COW-clone the buffer they then sort. Timing only the sort
     *  would flatter them with a copy the real implementation still has to pay for.
     */
    namespace Test_OrderBy_ {

        constexpr size_t       kN_    = 1000;
        constexpr unsigned int kSeed_ = 20260805;

        volatile size_t sSink_ = 0;

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
                sSink_ = sSink_ + c.size () + Magnitude_ (c[0]);
            }
        }
        template <typename T>
        void Consume_ (const Iterable<T>& c)
        {
            if (auto f = c.First ()) {
                sSink_ = sSink_ + c.size () + Magnitude_ (*f);
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

        // ---- A: exactly what OrderBy () does today - copy out through Stroika's Iterator<T>, then sort
        template <typename T, typename SEQUENCE_T>
        void A_Today_ (const SEQUENCE_T& seq)
        {
            vector<T> tmp{seq.begin (), Iterator<T>{seq.end ()}};
            stable_sort (tmp.begin (), tmp.end (), less<T>{});
            Consume_ (Containers::Concrete::Sequence_stdvector<T>{move (tmp)});
        }

        // ---- B: same, but pre-size the vector and fill via a plain forward walk
        template <typename T, typename SEQUENCE_T>
        void B_Reserve_ (const SEQUENCE_T& seq)
        {
            vector<T> tmp;
            tmp.reserve (seq.size ());
            for (const auto& e : seq) {
                tmp.push_back (e);
            }
            stable_sort (tmp.begin (), tmp.end (), less<T>{});
            Consume_ (Containers::Concrete::Sequence_stdvector<T>{move (tmp)});
        }

        // ---- C: the "virtual exposes mutable storage (span<T>), stable_sort stays in the outer
        //      template" strategy - comparer inlined. The vector copy models the COW clone that a const
        //      OrderBy () must do before it may touch the rep's buffer.
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
         *      o   reserve () LOSES here. size () is a virtual call (O(n) for a generic rep), and
         *          push_back pays a capacity check per element, while the range CTOR's copy loop does
         *          not. Two attempts at a special-cased As<vector<T>> both came out worse than doing
         *          nothing: Apply () + std::function was 1.33x, and reserve () + assign () was 1.55x
         *          (that one pays for the length THREE times - size (), distance (), then copies).
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

        // ---- the REAL Sequence<T>::OrderBy (), called as a user would call it
        template <typename T, typename SEQUENCE_T>
        void Real_SequenceOrderBy_ (const SEQUENCE_T& seq)
        {
            Consume_ (seq.OrderBy ());
        }

        /*
         *  ---- the REAL Iterable<T>::OrderBy (). Worth measuring separately from Sequence's because the
         *  two are NOT the same operation today, despite Sequence<T>::OrderBy () merely hiding this one:
         *      o   Iterable<T>::OrderBy () takes an Execution::SequencePolicy defaulting to ePar, so by
         *          default it sorts in PARALLEL.
         *      o   Sequence<T>::OrderBy () drops that parameter entirely and is always sequential.
         *  So the Sequence-vs-Iterable score below is the practical cost/benefit of that divergence -
         *  see item 1 of the OrderBy entry in TODO.md. Both policies are measured, since the parallel
         *  default is only a win once N is large enough to pay for the thread hand-off.
         */
        template <typename T>
        void Real_IterableOrderBy_ (const Iterable<T>& it, Execution::SequencePolicy seq)
        {
            Consume_ (it.OrderBy (less<T>{}, seq));
        }
        // Calls OrderBy () with NO policy argument on purpose - the permanent regression entry must
        // guard whatever the DEFAULT path actually is, so it keeps tracking reality if the default
        // is changed again (it already moved from ePar to eSeq once).
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
            const Iterable<int>                                kIterInts_{SourceInts_ ()};

            GetOutStream_ () << "=== Sequence<>::OrderBy () DESIGN PROBE (--orderby-probe) ===" << endl;
            GetOutStream_ () << "score < 1 means the second (compareWith) strategy is faster" << endl << endl;

            (void)Tester (
                "OrderBy probe int: A_today vs B_reserve", [&] () { A_Today_<int> (kSeqInts_); }, "A_today",
                [&] () { B_Reserve_<int> (kSeqInts_); }, "B_reserve", kRunCount_, kNoWarn_);
            (void)Tester (
                "OrderBy probe int: A_today vs C_inplace_inlined", [&] () { A_Today_<int> (kSeqInts_); }, "A_today",
                [&] () { C_InPlaceInlined_<int> (SourceInts_ ()); }, "C_inplace_inlined", kRunCount_, kNoWarn_);
            (void)Tester (
                "OrderBy probe int: C_inplace_inlined vs D_inplace_erased  <== THE DECISION",
                [&] () { C_InPlaceInlined_<int> (SourceInts_ ()); }, "C_inplace_inlined",
                [&] () { D_InPlaceErased_<int> (SourceInts_ ()); }, "D_inplace_erased", kRunCount_, kNoWarn_);

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

            // Sequence<T>::OrderBy () hides Iterable<T>::OrderBy () and has no SequencePolicy at all, so
            // it is always sequential; Iterable's now defaults to eSeq too but can be asked for ePar.
            (void)Tester (
                "OrderBy divergence: Sequence<int>::OrderBy () vs Iterable<int>::OrderBy () [both default]",
                [&] () { Real_SequenceOrderBy_<int> (kSeqInts_); }, "Sequence::OrderBy (no policy possible)",
                [&] () { Real_IterableOrderByDefault_<int> (kIterInts_); }, "Iterable::OrderBy (default, now eSeq)", kRunCount_, kNoWarn_);
            (void)Tester (
                "OrderBy divergence: Iterable<int>::OrderBy () eSeq vs ePar",
                [&] () { Real_IterableOrderBy_<int> (kIterInts_, Execution::SequencePolicy::eSeq); }, "Iterable::OrderBy (eSeq)",
                [&] () { Real_IterableOrderBy_<int> (kIterInts_, Execution::SequencePolicy::ePar); }, "Iterable::OrderBy (ePar)", kRunCount_, kNoWarn_);

            (void)Tester (
                "OrderBy probe String: A_today vs C_inplace_inlined", [&] () { A_Today_<String> (kSeqStrs_); }, "A_today",
                [&] () { C_InPlaceInlined_<String> (SourceStrings_ ()); }, "C_inplace_inlined", kRunCount_ / 10, kNoWarn_);
            (void)Tester (
                "OrderBy probe String: C_inplace_inlined vs D_inplace_erased  <== THE DECISION",
                [&] () { C_InPlaceInlined_<String> (SourceStrings_ ()); }, "C_inplace_inlined",
                [&] () { D_InPlaceErased_<String> (SourceStrings_ ()); }, "D_inplace_erased", kRunCount_ / 10, kNoWarn_);
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

        Set<String> failedTests;

#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            Stroika::Frameworks::Test::WarnTestIssue ("qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy and "
                                                      "Debug::kBuiltWithUndefinedBehaviorSanitizer so skipping test");
            return;
        }
#endif

        Tester ("Test of simple locking strategies (mutex v shared_ptr copy)", Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK, L"mutex",
                Test_MutexVersusSharedPtrCopy_shared_ptr_copy, L"shared_ptr<> copy", 24500, .90, &failedTests);
        Tester ("Test of simple locking strategies (mutex v SpinLock)", Test_MutexVersusSpinLock_MUTEXT_LOCK, L"mutex",
                Test_MutexVersusSpinLock_SPIN_LOCK, L"SpinLock", 24500, .51, &failedTests);
        Tester ("Simple Struct With Strings Filling And Copying", Test_StructWithStringsFillingAndCopying<wstring>, L"wstring",
                Test_StructWithStringsFillingAndCopying<String>, L"Characters::String", 65000, 0.49, &failedTests);
        Tester ("Simple Struct With Strings Filling And Copying2", Test_StructWithStringsFillingAndCopying2<wstring>, L"wstring",
                Test_StructWithStringsFillingAndCopying2<String>, L"Characters::String", 66000, 0.45, &failedTests);
        Tester ("Simple String append test (+='string object') 10x", Test_SimpleStringAppends1_<wstring>, L"wstring",
                Test_SimpleStringAppends1_<String>, L"Characters::String", 1350000, 4.9, &failedTests);
        Tester ("Simple String append test (+=wchar_t[]) 10x", Test_SimpleStringAppends2_<wstring>, L"wstring",
                Test_SimpleStringAppends2_<String>, L"Characters::String", 1500000, 4.1, &failedTests);
        Tester ("Simple String append test (+=wchar_t[]) 100x", Test_SimpleStringAppends3_<wstring>, L"wstring",
                Test_SimpleStringAppends3_<String>, L"Characters::String", 360000, 78, &failedTests);
        Tester ("String a + b", Test_SimpleStringConCat1_<wstring>, L"wstring", Test_SimpleStringConCat1_<String>, L"String", 2200000, 2.1, &failedTests);
        Tester ("wstringstream << test", Test_OperatorINSERT_ostream_<wstring>, L"wstring", Test_OperatorINSERT_ostream_<String>,
                L"Characters::String", 6000, 1.4, &failedTests);
        Tester ("String::substr()", Test_StringSubStr_<wstring>, L"wstring", Test_StringSubStr_<String>, L"Characters::String", 2700000, 1.7, &failedTests);
        struct MemStreamOfChars_ : public MemoryStream::Ptr<Characters::Character> {
            MemStreamOfChars_ ()
                : Ptr{MemoryStream::New<Characters::Character> ()}
            {
            }
        };
        Tester (
            "wstringstream versus BasicTextOutputStream",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, L"wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<MemStreamOfChars_> ([] (const MemStreamOfChars_& w) { return w.As<String> (); });
            },
            L"MemoryStream<Characters::Character>", 210000, 1.2, &failedTests);
        Tester (
            "wstringstream versus StringBuilder",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, L"wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<StringBuilder<>> ([] (const StringBuilder<>& w) { return w.As<String> (); });
            },
            "StringBuilder", 220000, 0.48, &failedTests);
        Tester ("Sequence<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<int>>, "Sequence<int>", 125000, 0.75, &failedTests);
        Tester ("Sequence<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<string>>, "Sequence<string>", 9900, 0.33, &failedTests);
        Tester ("Sequence_DoublyLinkedList<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<int>>,
                "Sequence_DoublyLinkedList<int>", 120000, 5.1, &failedTests);
        Tester ("Sequence_Array<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_Array<int>>, L"Sequence_Array<int>", 120000, 0.7, &failedTests);
        Tester ("Sequence_stdvector<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_stdvector<int>>, L"Sequence_stdvector<int>", 120000,
                1.1, &failedTests);
        Tester ("Sequence_DoublyLinkedList<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<string>>,
                "Sequence_DoublyLinkedList<string>", 9900, 0.55, &failedTests);
        Tester (
            "Collection<int> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<int>> ([] (vector<int>* c) { c->push_back (2); }); }, L"vector<int>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<int>> ([] (Collection<int>* c) { c->Add (2); }); },
            L"Collection<int>", 113000, 4.9, &failedTests);
        Tester (
            "Collection<string> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<string>> ([] (Collection<string>* c) { c->Add (string{}); }); },
            "Collection<string>", 9600, 0.85, &failedTests);
        {
            // In Stroika 2.1b15, we changed the default Collection factory to use SortedCollection_stdmultiset. This is probably a good choice,
            // but is a small pessimization so include original Collection_stdforward_list for comparison (maybe orig was something else but this works).
            using Containers::Concrete::Collection_LinkedList;
            using Containers::Concrete::Collection_stdforward_list;
            using Containers::Concrete::SortedCollection_stdmultiset;
            Tester (
                "Collection_LinkedList<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_LinkedList<string>> (
                        [] (Collection_LinkedList<string>* c) { c->Add (string{}); });
                },
                "Collection_LinkedList<string>", 9600, 0.6, &failedTests);
            Tester (
                "Collection_stdforward_list<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_stdforward_list<string>> (
                        [] (Collection_stdforward_list<string>* c) { c->Add (string{}); });
                },
                "Collection_stdforward_list<string>", 9600, 0.6, &failedTests);
            Tester (
                "SortedCollection_stdmultiset<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<SortedCollection_stdmultiset<string>> (
                        [] (SortedCollection_stdmultiset<string>* c) { c->Add (string{}); });
                },
                "SortedCollection_stdmultiset<string>", 9600, 1.0, &failedTests);
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
                L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<SortedCollection_stdmultiset<string>> (
                        [] (SortedCollection_stdmultiset<string>* c) { c->Add (kRandomStrings_[rand () % kRandomStrings_.size ()]); });
                },
                "SortedCollection_stdmultiset<string>", 9600, 1.3, &failedTests);
        }
        Tester ("std::set<int> vs Set<int>", Test_SetvsSet_<set<int>>, "set<int>", Test_SetvsSet_<Set<int>>, "Set<int>", 13000, 0.21, &failedTests);
        Tester ("String Characters::Format ()", Test_String_Format_<wstring>, "sprintf", Test_String_Format_<String>,
                "String Characters::Format", 2100000, 1.8, &failedTests);
        Tester ("BLOB versus vector<byte>", Test_BLOB_Versus_Vector_Byte<vector<byte>>, L"vector<byte>",
                Test_BLOB_Versus_Vector_Byte<Memory::BLOB>, "BLOB", 13000, 1.0, &failedTests);
        Tester ("BLOB versus vector<byte> ver#2", Test_BLOB_Versus_Vector_Byte_2<vector<byte>>, L"vector<byte>",
                Test_BLOB_Versus_Vector_Byte_2<Memory::BLOB>, "BLOB", 5000, 0.85, &failedTests);
        Tester ("Test_JSONReadWriteFile", Test_JSONReadWriteFile_::DoRunPerfTest, "Test_JSONReadWriteFile",
                Debug::IsRunningUnderValgrind () ? 2 : 640, 0.5, &failedTests);
        Tester ("Test_Optional_", Test_Optional_::DoRunPerfTest, "Test_Optional_", 4875, 0.5, &failedTests);
        {
            // Guards against OrderBy () being pessimized. Baseline is what the same sort costs with plain
            // std machinery, so the score is 'what Stroika's OrderBy () adds over stable_sort'.
            // Threshold: measured 2.23-2.63 across runs, so 3.5 leaves ~33% headroom.
            static const Containers::Concrete::Sequence_Array<int> kSeqInts_{Test_OrderBy_::SourceInts_ ()};
            static const Iterable<int>                             kIterInts_{Test_OrderBy_::SourceInts_ ()};
            Tester (
                "Sequence<int>::OrderBy () vs std::stable_sort",
                [] () { Test_OrderBy_::Baseline_StdStableSort_<int> (Test_OrderBy_::SourceInts_ ()); }, "std::stable_sort (vector<int>)",
                [] () { Test_OrderBy_::Real_SequenceOrderBy_<int> (kSeqInts_); }, "Sequence<int>::OrderBy ()", 140000, 3.5, &failedTests);
            // Separate entry because Iterable's OrderBy () is NOT the same operation as Sequence's - it
            // takes an Execution::SequencePolicy, which Sequence<T>::OrderBy () (which hides it) does
            // not. Deliberately exercises the DEFAULT path rather than naming a policy, so this keeps
            // guarding whatever the default actually is - it already moved ePar -> eSeq once, which
            // silently turned an earlier version of this entry into a guard on a non-default path.
            // The explicit eSeq-vs-ePar measurement lives in --orderby-probe instead.
            //
            // Threshold: measured 3.98 here. Set to 5.0 rather than something snug, because this score
            // has ranged 3.98-5.17 across runs on one machine (it is sensitive to load, and moved with
            // the As<vector<T>> implementation). 4.0 gave 0.6% headroom and flapped. Per this file's
            // convention this is a gross-change alarm, not a tight bound.
            // It reads high next to Sequence's 2.5 partly because these are different containers -
            // Sequence_Array<int> vs a generic Iterable<int> rep - not purely an OrderBy () difference.
            Tester (
                "Iterable<int>::OrderBy () vs std::stable_sort",
                [] () { Test_OrderBy_::Baseline_StdStableSort_<int> (Test_OrderBy_::SourceInts_ ()); }, "std::stable_sort (vector<int>)",
                [] () { Test_OrderBy_::Real_IterableOrderByDefault_<int> (kIterInts_); }, "Iterable<int>::OrderBy () [default policy]",
                140000, 5.0, &failedTests);
        }
        JSONTests_::Run ();

        GetOutStream_ () << "[[[Tests took: " << (DateTime::Now () - startedAt).PrettyPrint () << "]]]" << endl << endl;

        // extra tests
        {
            if (sRunOrderByProbe_) {
                Test_OrderBy_::RunProbe_ ();
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
