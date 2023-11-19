/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

#include "Stroika/Foundation/Configuration/StroikaVersion.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/StroikaVersion.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdmultiset.h"
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
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Math/Statistics.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include "Stroika/Foundation/Traversal/Generator.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Math;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Time;

using std::byte;

/*
 *  TODO:
 *
 */

// Turn this on rarely to calibrate so # runs a good test
//#define   qPrintOutIfBaselineOffFromOneSecond (!qDebug && defined (_MSC_VER) && defined (WIN32) && !defined (_WIN64))

// My performance expectation numbers WERE calibrated for MSVC (2k13.net) and 32-bit code - until Stroika v3. Now
// still using visual studio.net as warning baseline, but using 64-bit tests.
//
// Don't print when they differ on other platforms.
// This is only intended to alert me when something changes GROSSLY.
namespace {
#if defined(_MSC_VER)
    constexpr bool kPrintOutIfFailsToMeetPerformanceExpectations_ =
        not qDebug and qAllowBlockAllocation and not qStroika_Foundation_Debug_Trace_DefaultTracingOn and sizeof (void*) == sizeof (int64_t);
#else
    constexpr bool kPrintOutIfFailsToMeetPerformanceExpectations_ = false;
#endif
}

// Use this so when running #if qDebug case - we don't waste a ton of time with this test
#define qDebugCaseRuncountRatio (.01)

namespace {
    string pctFaster2String_ (double pct)
    {
        if (pct < 0) {
            return Format (L"%.2f%% slower", -pct).AsNarrowSDKString ();
        }
        else {
            return Format (L"%.2f%% faster", pct).AsNarrowSDKString ();
        }
    }
}

namespace {
    constexpr char kDefaultPerfOutFile_[] = "PerformanceDump.txt";
    bool           sShowOutput_           = false;
}

namespace {
#if qDebug
    double sTimeMultiplier_ = (Debug::IsRunningUnderValgrind () or Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) ? .001 : 1.0;
#else
    double sTimeMultiplier_ = (Debug::IsRunningUnderValgrind () or Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) ? .002 : 1.0;
#endif
}

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
        outTo << "Test " << testName.AsNarrowSDKString () << " (" << baselineTName.AsNarrowSDKString () << " vs "
              << compareWithTName.AsNarrowSDKString () << ")" << endl;
        double         performanceScore = (baselineTime == 0s) ? 1000000 : compareWithTime.count () / baselineTime.count ();
        constexpr char kOneTab_[]       = "\t";
        {
            FloatConversion::ToStringOptions fo = FloatConversion::ToStringOptions{FloatConversion::eDontTrimZeros, FloatConversion::Precision{2}};
            outTo << kOneTab_ << "PERFORMANCE_SCORE" << kOneTab_ << FloatConversion::ToString (performanceScore, fo).AsNarrowSDKString () << endl;
        }
        outTo << kOneTab_ << "DETAILS:         " << kOneTab_;
        outTo << "[baseline test " << baselineTime.count () << " secs, and comparison " << compareWithTime.count ()
              << " sec, and warnIfPerfScore > " << warnIfPerformanceScoreHigherThan << ", and perfScore=" << performanceScore << "]" << endl;
        outTo << kOneTab_ << "                 " << kOneTab_;
        if (performanceScore < 1) {
            outTo << compareWithTName.AsNarrowSDKString () << " is FASTER" << endl;
        }
        else if (performanceScore > 1) {
            outTo << compareWithTName.AsNarrowSDKString () << " is ***SLOWER***" << endl;
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
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Tester", L"testName=%s, runCount=%d", testName.c_str (), runCount)};
#if qDebug
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
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Tester", L"testName=%s, runCount=%d", testName.c_str (), runCount)};
#if qDebug
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
        printResults (testName, Characters::Format (L"%f seconds", baselineTime.count ()), compareWithTName,
                      warnIfPerformanceScoreHigherThan, baselineTime, compareWithTime);
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
        VerifyTestResult (v[0].fS1 == v[1].fS1);
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
        VerifyTestResult (v[0].fS1 == v[1].fS1);
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends1_ ()
    {
        const WIDESTRING_IMPL KBase = L"1234568321";
        WIDESTRING_IMPL       w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == KBase.length () * 10);
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends2_ ()
    {
        const wchar_t   KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 10; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen (KBase) * 10);
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_SimpleStringAppends3_ ()
    {
        const wchar_t   KBase[] = L"1234568321";
        WIDESTRING_IMPL w;
        for (int i = 0; i < 100; ++i) {
            w += KBase;
        }
        VerifyTestResult (w.length () == wcslen (KBase) * 100);
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
            VerifyTestResult (tmp.length () == src.length () * 4);
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
            VerifyTestResult (tmp.length () == 20);
            VerifyTestResult (src.substr (5, 20).length () == 20);
            VerifyTestResult (src.substr (5, 20).substr (3, 3).length () == 3);
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
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000); // so nothing optimized away
    }
    void Test_MutexVersusSharedPtrCopy_shared_ptr_copy ()
    {
        using namespace Test_MutexVersusSharedPtrCopy_MUTEXT_PRIVATE_;
        s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSharedPtrCopy_SharedPtrCopy (Test_MutexVersusSharedPtrCopy_COUNTEST);
        }
        VerifyTestResult (s_Test_MutexVersusSharedPtrCopy_IGNROED_COUNT == 1000); // so nothing optimized away
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
        VerifyTestResult (sRunningCnt_ == 1000); // so nothing optimized away
    }
    void Test_MutexVersusSpinLock_SPIN_LOCK ()
    {
        using namespace Test_MutexVersusSpinLock_MUTEXT_PRIVATE_;
        sRunningCnt_ = 0;
        for (int i = 0; i < 1000; ++i) {
            Test_MutexVersusSpinLock_SPINLOCK_LOCK (Test_MutexVersusSpinLock_COUNTEST);
        }
        VerifyTestResult (sRunningCnt_ == 1000); // so nothing optimized away
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
        VerifyTestResult (out.str ().length () == 18 * 1000);
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
        VerifyTestResult (extractor (out).length () == 31 * 20);
    }
}

namespace {
    template <typename WIDESTRING_IMPL>
    void Test_String_cstr_call_ ()
    {
        static WIDESTRING_IMPL s1 = L"abcd 23234j aksdf alksdjf lkasf jklsdf asdf baewr";
        static WIDESTRING_IMPL s2 = L"o3424";
        static WIDESTRING_IMPL s3 = L"o3424";
        static WIDESTRING_IMPL s4 = L"o3424";
        static WIDESTRING_IMPL s5 = L"abcd 23234j aksdf alksdjf lkasf "
                                    L"jklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdfjklsdf asdf baewr";
        size_t s1len = s1.length ();
        size_t s2len = s2.length ();
        size_t s3len = s3.length ();
        size_t s4len = s4.length ();
        size_t s5len = s5.length ();
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-volatile\""); // warning: increment of object of volatile-qualified type 'volatile unsigned int' is deprecated [-Wdeprecated-volatile]
#endif
#if (defined(__GNUC__) && !defined(__clang__)) && (__GNUC__ >= 10)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wvolatile\""); // warning: '++' expression of 'volatile'-qualified type is deprecated
#endif
        for (volatile int i = 0; i < 200; ++i) {
            VerifyTestResult (s1len == ::wcslen (s1.c_str ()));
            VerifyTestResult (s2len == ::wcslen (s2.c_str ()));
            VerifyTestResult (s3len == ::wcslen (s3.c_str ()));
            VerifyTestResult (s4len == ::wcslen (s4.c_str ()));
            VerifyTestResult (s5len == ::wcslen (s5.c_str ()));
        }
#if (defined(__GNUC__) && !defined(__clang__)) && (__GNUC__ >= 10)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wvolatile\"");
#endif
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-volatile\"");
#endif
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
            VerifyTestResult (c.size () == 500);
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
            VerifyTestResult (c.size () == 500);
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
                    if constexpr (is_same_v<set<ELEMENTTYPE>, CONTAINER>) {
                        VerifyTestResult (c.find (i) != c.end ());
                    }
                    else {
                        VerifyTestResult (c.Contains (i));
                    }
                }
                if constexpr (is_same_v<set<ELEMENTTYPE>, CONTAINER>) {
                    VerifyTestResult (c.find (501) == c.end ());
                }
                else {
                    VerifyTestResult (not c.Contains (501));
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
            if constexpr (is_same_v<set<ELEMENTTYPE>, CONTAINER>) {
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
        VerifyTestResult (Format (L"a, %s, %d", L"xxx", 33) == L"a, xxx, 33");
        VerifyTestResult (Format (L"0x%x", 0x20) == L"0x20");
    }
    template <>
    void Test_String_Format_<wstring> ()
    {
        {
            wchar_t buf[1024];
            VerifyTestResult (swprintf (buf, NEltsOf (buf), L"a, %ls, %d", L"xxx", 33) == 10);
            VerifyTestResult (wstring (buf) == L"a, xxx, 33");
        }
        {
            wchar_t buf[1024];
            VerifyTestResult (swprintf (buf, NEltsOf (buf), L"0x%x", 0x20) == 4);
            VerifyTestResult (wstring (buf) == L"0x20");
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
            VerifyTestResult (T1_SIZER_ (bn) == sizeof (kCArr_4k_));
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
            VerifyTestResult (T1_SIZER_ (bn) == sizeof (kCArr_4k_));
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
            VerifyTestResult (T1_SIZER_ (bn) == sizeof (kCArr_32b_));
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
        constexpr Configuration::EnumNames<ScanKindType> ScanKindType_NAMES{Configuration::EnumNames<ScanKindType>::BasicArrayInitializer{{
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
                : Mapping<double, double>{Containers::Concrete::Mapping_stdmap<double, double>{CompareNumbersLess_{}}}
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
            mapper.AddClass<ScanDetails_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                {L"Scan-ID", StructFieldMetaInfo{&ScanDetails_::fScanID}},
                {L"Scan-Start", StructFieldMetaInfo{&ScanDetails_::fScanStart}},
                {L"Scan-End", StructFieldMetaInfo{&ScanDetails_::fScanEnd}},
                {L"Scan-Kind", StructFieldMetaInfo{&ScanDetails_::fScanKind}},
                {L"Scan-Label", StructFieldMetaInfo{&ScanDetails_::fScanLabel}},
                {L"Raw-Spectrum", StructFieldMetaInfo{&ScanDetails_::fRawSpectrum}},
                {L"Aux-Data", StructFieldMetaInfo{&ScanDetails_::fAuxData}},
                {L"Background-ID", StructFieldMetaInfo{&ScanDetails_::fUseBackground}},
                {L"Reference-ID", StructFieldMetaInfo{&ScanDetails_::fUseReference}},
            });
            return mapper;
        }
        ScanDetails_ doRead_ (const InputStream<byte>::Ptr in)
        {
            using namespace DataExchange;
            VariantValue                     o{Variant::JSON::Reader{}.Read (in)};
            static const ObjectVariantMapper kMapper_ = GetPersistenceDetailsMapper_ ();
            return kMapper_.ToObject<ScanDetails_> (o);
        }
        Memory::BLOB doWrite_ (const ScanDetails_& scan)
        {
            using namespace DataExchange;
            Streams::MemoryStream<byte>::Ptr out      = Streams::MemoryStream<byte>::New ();
            static const ObjectVariantMapper kMapper_ = GetPersistenceDetailsMapper_ ();
            Variant::JSON::Writer ().Write (kMapper_.FromObject (scan), out);
            return out.As<Memory::BLOB> ();
        }
        void DoRunPerfTest ()
        {
            ScanDetails_ sd = doRead_ (Streams::ExternallyOwnedMemoryInputStream<byte>::New (begin (kSAMPLE_FILE_), end (kSAMPLE_FILE_)));
            VerifyTestResult (sd.fAuxData.ContainsKey (L"Sample-Pressure"));
            VerifyTestResult (sd.fScanID == 5856);
            Memory::BLOB b   = doWrite_ (sd);
            ScanDetails_ sd2 = doRead_ (Streams::ExternallyOwnedMemoryInputStream<byte>::New (begin (b), end (b)));
            VerifyTestResult (sd2.fScanID == sd.fScanID);
            VerifyTestResult (sd2.fAuxData == sd.fAuxData);
            VerifyTestResult (sd2.fRawSpectrum == sd.fRawSpectrum); // @todo - FIX - this test should pass!
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

#if qPlatform_Windows
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
         *  Tests based on https://stroika.atlassian.net/browse/STK-781
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
                VariantValue output{reader.Read (ExternallyOwnedMemoryInputStream<byte>::New (begin (p), end (p)))};
            }
        }
        void DoStroikaJSONParse_ (const string& p, unsigned int nTimes)
        {
            using namespace DataExchange;
            using namespace Streams;
            Variant::JSON::Reader reader{Variant::JSON::ReaderOptions{}};
            for (unsigned int tryNum = 0; tryNum < nTimes; ++tryNum) {
                VariantValue output{reader.Read (ExternallyOwnedMemoryInputStream<byte>::New (begin (p), end (p)))};
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
                jsonTestRoot = path{".."} / ".." / "Tests" / "52" / "JSONTestData";
            }
            if (not filesystem::exists (jsonTestRoot)) {
                jsonTestRoot = path{".."} / ".." / ".." / "52" / "JSONTestData";
            }
            for (auto testCase : kTestCases_) {
                DoJSONParse_ (jsonTestRoot / "small-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                if constexpr (not qDebug) {
                    // don't bother testing these except in release builds - too slow
                    DoJSONParse_ (jsonTestRoot / "medium-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                    DoJSONParse_ (jsonTestRoot / "large-dict.json", nTimes, std::get<0> (testCase), std::get<1> (testCase));
                }
            }
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
        GetOutStream_ () << "[[[Started testing at: " << startedAt.Format ().AsNarrowSDKString () << "]]]" << endl << endl;
        if (not Math::NearlyEquals (sTimeMultiplier_, 1.0)) {
            GetOutStream_ () << "Using TIME MULTIPLIER: " << sTimeMultiplier_ << endl << endl;
        }

        Set<String> failedTests;

#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
        if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
            Stroika::TestHarness::WarnTestIssue (L"qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy and "
                                                 L"Debug::kBuiltWithUndefinedBehaviorSanitizer so skipping test");
            return;
        }
#endif

        Tester (L"Test of simple locking strategies (mutex v shared_ptr copy)", Test_MutexVersusSharedPtrCopy_MUTEXT_LOCK, L"mutex",
                Test_MutexVersusSharedPtrCopy_shared_ptr_copy, L"shared_ptr<> copy", 24500, .90, &failedTests);
        Tester (L"Test of simple locking strategies (mutex v SpinLock)", Test_MutexVersusSpinLock_MUTEXT_LOCK, L"mutex",
                Test_MutexVersusSpinLock_SPIN_LOCK, L"SpinLock", 24500, .51, &failedTests);
        Tester (L"Simple Struct With Strings Filling And Copying", Test_StructWithStringsFillingAndCopying<wstring>, L"wstring",
                Test_StructWithStringsFillingAndCopying<String>, L"Charactes::String", 65000, 0.45, &failedTests);
        Tester (L"Simple Struct With Strings Filling And Copying2", Test_StructWithStringsFillingAndCopying2<wstring>, L"wstring",
                Test_StructWithStringsFillingAndCopying2<String>, L"Charactes::String", 66000, 0.45, &failedTests);
        Tester (L"Simple String append test (+='string object') 10x", Test_SimpleStringAppends1_<wstring>, L"wstring",
                Test_SimpleStringAppends1_<String>, L"Charactes::String", 1350000, 4.8, &failedTests);
        Tester (L"Simple String append test (+=wchar_t[]) 10x", Test_SimpleStringAppends2_<wstring>, L"wstring",
                Test_SimpleStringAppends2_<String>, L"Charactes::String", 1500000, 4.1, &failedTests);
        Tester (L"Simple String append test (+=wchar_t[]) 100x", Test_SimpleStringAppends3_<wstring>, L"wstring",
                Test_SimpleStringAppends3_<String>, L"Charactes::String", 360000, 78, &failedTests);
        Tester (L"String a + b", Test_SimpleStringConCat1_<wstring>, L"wstring", Test_SimpleStringConCat1_<String>, L"String", 2200000, 2.1, &failedTests);
        Tester (L"wstringstream << test", Test_OperatorINSERT_ostream_<wstring>, L"wstring", Test_OperatorINSERT_ostream_<String>,
                L"Charactes::String", 6000, 1.4, &failedTests);
        Tester (L"String::substr()", Test_StringSubStr_<wstring>, L"wstring", Test_StringSubStr_<String>, L"Charactes::String", 2700000, 1.7, &failedTests);
        struct MemStreamOfChars_ : public MemoryStream<Characters::Character>::Ptr {
            MemStreamOfChars_ ()
                : Ptr{MemoryStream<Characters::Character>::New ()}
            {
            }
        };
        Tester (
            L"wstringstream versus BasicTextOutputStream",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, L"wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<MemStreamOfChars_> ([] (const MemStreamOfChars_& w) { return w.As<String> (); });
            },
            L"MemoryStream<Characters::Character>", 210000, 1.2, &failedTests);
        Tester (
            L"wstringstream versus StringBuilder",
            [] () { Test_StreamBuilderStringBuildingWithExtract_<wstringstream> ([] (const wstringstream& w) { return w.str (); }); }, L"wstringstream",
            [] () {
                Test_StreamBuilderStringBuildingWithExtract_<StringBuilder<>> ([] (const StringBuilder<>& w) { return w.As<String> (); });
            },
            L"StringBuilder", 220000, 0.48, &failedTests);
        Tester (L"Simple c_str() test", Test_String_cstr_call_<wstring>, L"wstring", Test_String_cstr_call_<String>, L"Charactes::String",
                51000, 1.3, &failedTests);
        Tester (L"Sequence<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<int>>, L"Sequence<int>", 125000, 0.75, &failedTests);
        Tester (L"Sequence<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Sequence<string>>, L"Sequence<string>", 9900, 0.33, &failedTests);
        Tester (L"Sequence_DoublyLinkedList<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<int>>,
                L"Sequence_DoublyLinkedList<int>", 120000, 5.1, &failedTests);
        Tester (L"Sequence_Array<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_Array<int>>, L"Sequence_Array<int>", 120000, 0.7, &failedTests);
        Tester (L"Sequence_stdvector<int> basics", Test_SequenceVectorAdditionsAndCopies_<vector<int>>, L"vector<int>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_stdvector<int>>, L"Sequence_stdvector<int>", 120000,
                1.1, &failedTests);
        Tester (L"Sequence_DoublyLinkedList<string> basics", Test_SequenceVectorAdditionsAndCopies_<vector<string>>, L"vector<string>",
                Test_SequenceVectorAdditionsAndCopies_<Containers::Concrete::Sequence_DoublyLinkedList<string>>,
                L"Sequence_DoublyLinkedList<string>", 9900, 0.55, &failedTests);
        Tester (
            L"Collection<int> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<int>> ([] (vector<int>* c) { c->push_back (2); }); }, L"vector<int>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<int>> ([] (Collection<int>* c) { c->Add (2); }); },
            L"Collection<int>", 113000, 4.9, &failedTests);
        Tester (
            L"Collection<string> basics",
            [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
            [] () { Test_CollectionVectorAdditionsAndCopies_<Collection<string>> ([] (Collection<string>* c) { c->Add (string{}); }); },
            L"Collection<string>", 9600, 0.79, &failedTests);
        {
            // In Stroika 2.1b15, we changed the default Collection factory to use Collection_stdmultiset. This is probably a good choice,
            // but is a small pessimization so include original Collection_stdforward_list for comparison (maybe orig was something else but this works).
            using Containers::Concrete::Collection_LinkedList;
            using Containers::Concrete::Collection_stdforward_list;
            using Containers::Concrete::Collection_stdmultiset;
            Tester (
                L"Collection_LinkedList<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_LinkedList<string>> (
                        [] (Collection_LinkedList<string>* c) { c->Add (string{}); });
                },
                L"Collection_LinkedList<string>", 9600, 0.6, &failedTests);
            Tester (
                L"Collection_stdforward_list<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_stdforward_list<string>> (
                        [] (Collection_stdforward_list<string>* c) { c->Add (string{}); });
                },
                L"Collection_stdforward_list<string>", 9600, 0.6, &failedTests);
            Tester (
                L"Collection_stdmultiset<string> basics",
                [] () { Test_CollectionVectorAdditionsAndCopies_<vector<string>> ([] (vector<string>* c) { c->push_back (string{}); }); }, L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_stdmultiset<string>> (
                        [] (Collection_stdmultiset<string>* c) { c->Add (string{}); });
                },
                L"Collection_stdmultiset<string>", 9600, 1.0, &failedTests);
        }
        {
            using Containers::Concrete::Collection_stdmultiset;
            // In Stroika 2.1b15, we changed the default Collection factory to use Collection_stdmultiset. This is probably a good choice,
            // but is a small pessimization when we have ALL IDENTICAL strings (so all conflicts). Just avoid that
            static const vector<string> kRandomStrings_ = [] () {
                vector<string> r;
                r.reserve (100);
                for (int i = 0; i < 100; ++i) {
                    char buf[1024];
                    snprintf (buf, NEltsOf (buf), "hello %d", i);
                    r.push_back (buf);
                }
                return r;
            }();
            // this would do much better if we cared about mem usage, or did lookups, remove, etc...
            Tester (
                L"Collection_stdmultiset<string> basics with rnd strings",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<vector<string>> (
                        [] (vector<string>* c) { c->push_back (kRandomStrings_[rand () % kRandomStrings_.size ()]); });
                },
                L"vector<string>",
                [] () {
                    Test_CollectionVectorAdditionsAndCopies_<Collection_stdmultiset<string>> (
                        [] (Collection_stdmultiset<string>* c) { c->Add (kRandomStrings_[rand () % kRandomStrings_.size ()]); });
                },
                L"Collection_stdmultiset<string>", 9600, 1.3, &failedTests);
        }
        Tester (L"std::set<int> vs Set<int>", Test_SetvsSet_<set<int>>, L"set<int>", Test_SetvsSet_<Set<int>>, L"Set<int>", 13000, 0.21, &failedTests);
        Tester (L"String Characters::Format ()", Test_String_Format_<wstring>, L"sprintf", Test_String_Format_<String>,
                L"String Characters::Format", 2100000, 1.8, &failedTests);
        Tester (L"BLOB versus vector<byte>", Test_BLOB_Versus_Vector_Byte<vector<byte>>, L"vector<byte>",
                Test_BLOB_Versus_Vector_Byte<Memory::BLOB>, L"BLOB", 13000, 1.0, &failedTests);
        Tester (L"BLOB versus vector<byte> ver#2", Test_BLOB_Versus_Vector_Byte_2<vector<byte>>, L"vector<byte>",
                Test_BLOB_Versus_Vector_Byte_2<Memory::BLOB>, L"BLOB", 5000, 0.85, &failedTests);
        Tester (L"Test_JSONReadWriteFile", Test_JSONReadWriteFile_::DoRunPerfTest, L"Test_JSONReadWriteFile",
                Debug::IsRunningUnderValgrind () ? 2 : 640, 0.5, &failedTests);
        Tester (L"Test_Optional_", Test_Optional_::DoRunPerfTest, L"Test_Optional_", 4875, 0.5, &failedTests);
        JSONTests_::Run ();

        GetOutStream_ () << "[[[Tests took: " << (DateTime::Now () - startedAt).PrettyPrint ().AsNarrowSDKString () << "]]]" << endl
                         << endl;

        // extra tests
        {
        }

        if (not failedTests.empty ()) {
            String listAsMsg;
            failedTests.Apply ([&listAsMsg] (String i) {
                if (not listAsMsg.empty ()) {
                    listAsMsg += L", ";
                }
                listAsMsg += i;
            });
            if (sShowOutput_) {
                Stroika::TestHarness::WarnTestIssue ((L"At least one test did not meet expected time constaint (see above): " + listAsMsg).c_str ());
            }
            else {
                Stroika::TestHarness::WarnTestIssue ((Format (L"At least one test (%s) did not meet expected time constraint (see %s)",
                                                              listAsMsg.c_str (), String{kDefaultPerfOutFile_}.c_str ()))
                                                         .c_str ());
            }
        }
    }
}

namespace {
    // just temp hack to test one thing or another - which is performance related
    namespace TemporaryTest_ {
        void DoTest_ ()
        {
        }
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    // NOTE: run with --show or look for output in PERF-OUT.txt
    try {
        Sequence<String> cmdLine = ParseCommandLine (argc, argv);
        sShowOutput_             = MatchesCommandLineArgument (cmdLine, L"show");

        {
            optional<String> arg = MatchesCommandLineArgumentWithValue (cmdLine, L"x");
            if (arg.has_value ()) {
                sTimeMultiplier_ = FloatConversion::ToFloat<double> (*arg);
            }
        }
    }
    catch (...) {
        auto exc = current_exception ();
        cerr << "Usage: " << Characters::ToString (exc).AsNarrowSDKString () << endl;
        exit (EXIT_FAILURE);
    }

    TemporaryTest_::DoTest_ ();

    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (RunPerformanceTests_);
}
