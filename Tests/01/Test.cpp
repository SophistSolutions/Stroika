/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Caching
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <random>

#include "Stroika/Foundation/Cache/BloomFilter.h"
#include "Stroika/Foundation/Cache/CallerStalenessCache.h"
#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Cache/Memoizer.h"
#include "Stroika/Foundation/Cache/SynchronizedCallerStalenessCache.h"
#include "Stroika/Foundation/Cache/TimedCache.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/SuperFastHash.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cache;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    namespace Test1_Simple_Private_ {
        void T1_ ()
        {
            LRUCache<string, string> tmp{3};
            tmp.Add ("a", "1");
            tmp.Add ("b", "2");
            tmp.Add ("c", "3");
            tmp.Add ("d", "4");
            EXPECT_TRUE (not tmp.Lookup ("a").has_value ());
            EXPECT_EQ (tmp.Lookup ("b"), "2");
            EXPECT_EQ (tmp.Lookup ("d"), "4");

            LRUCache<string, string> tmp2 = tmp;
            EXPECT_TRUE (not tmp2.Lookup ("a").has_value ());
            EXPECT_EQ (tmp2.Lookup ("b"), "2");
            EXPECT_EQ (tmp2.Lookup ("d"), "4");
        }
        void T2_ ()
        {
            using CACHE = LRUCache<string, string, equal_to<string>, hash<string>>;
            CACHE tmp{10, equal_to<string>{}, 10};
            tmp.Add ("a", "1");
            tmp.Add ("b", "2");
            tmp.Add ("c", "3");
            tmp.Add ("d", "4");

            EXPECT_TRUE (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
            EXPECT_TRUE (tmp.Lookup ("b") == "2");
            EXPECT_TRUE (tmp.Lookup ("d") == "4");

            CACHE tmp2 = tmp;
            EXPECT_TRUE (not tmp2.Lookup ("a").has_value () or *tmp2.Lookup ("a") == "1"); // could be missing or found but if found same value
            EXPECT_EQ (tmp2.Lookup ("b"), "2");
            EXPECT_EQ (tmp2.Lookup ("d"), "4");
        }
        void T3_ ()
        {
            // using C++17 deduction guides
            //LRUCache tmp{pair<string, string>{}, 10, 10, hash<string>{}};
#if qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
            auto tmp = Cache::Factory::LRUCache_WithHash<string, string>{}(10, 10, hash<string>{});
#else
            LRUCache tmp = Cache::Factory::LRUCache_WithHash<string, string>{}(10, 10, hash<string>{});
#endif
            tmp.Add ("a", "1");
            tmp.Add ("b", "2");
            tmp.Add ("c", "3");
            tmp.Add ("d", "4");

            EXPECT_TRUE (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
            EXPECT_TRUE (tmp.Lookup ("b") == "2");
            EXPECT_TRUE (tmp.Lookup ("d") == "4");

            LRUCache tmp2 = tmp;
            EXPECT_TRUE (not tmp2.Lookup ("a").has_value () or *tmp2.Lookup ("a") == "1"); // could be missing or found but if found same value
            EXPECT_EQ (tmp2.Lookup ("b"), "2");
            EXPECT_EQ (tmp2.Lookup ("d"), "4");
        }
    }
    GTEST_TEST (Foundation_Caching, Simple_LRUCache_)
    {
        Test1_Simple_Private_::T1_ ();
        Test1_Simple_Private_::T2_ ();
        Test1_Simple_Private_::T3_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, LRUCache_ObjWithNoArgCTORs_)
    {
        struct TNoCTOR_ {
            TNoCTOR_ (int)
            {
            }
            TNoCTOR_ () = delete;
            bool operator== ([[maybe_unused]] const TNoCTOR_& rhs) const
            {
                return true;
            }
        };
        LRUCache<TNoCTOR_, TNoCTOR_> test{10};
        test.Add (TNoCTOR_{1}, TNoCTOR_{1});
        (void)test.Lookup (TNoCTOR_{1});
    }
}

namespace {
#if 0
    namespace Test3_LRUCache_Elements {
        namespace Private_ {
            struct TNoCTOR_ {
                TNoCTOR_ (int)
                {
                }
                TNoCTOR_ () = delete;
                bool operator== ([[maybe_unused]] const TNoCTOR_& rhs) const
                {
                    return true;
                }
            };
        }
        
    }
#endif
    GTEST_TEST (Foundation_Caching, Test3_LRUCache_Elements)
    {
        LRUCache<string, string> tmp{3};
        tmp.Add ("a", "1");
        tmp.Add ("b", "2");
        tmp.Add ("c", "3");
        tmp.Add ("d", "4");
        Containers::Mapping<string, string> x = tmp.Elements (); // subtle cuz converting traits type
        EXPECT_TRUE (x.length () <= 4);
        for (auto i : tmp.Elements ()) {
            EXPECT_TRUE (i.fKey == "a" or i.fKey == "b" or i.fKey == "c" or i.fKey == "d");
            EXPECT_TRUE (i.fValue == "1" or i.fValue == "2" or i.fValue == "3" or i.fValue == "4");
        }
    }
}

namespace {
    namespace Test4_TimedCache_Private_ {
        // FROM Example Usage in TimedCache<>
        using Characters::String;
        namespace Example1_ {
            struct DiskSpaceUsageType {
                int size;
            };
            // do the actual lookup part which maybe slow
            auto LookupDiskStats_ ([[maybe_unused]] const String& filename) -> DiskSpaceUsageType
            {
                return DiskSpaceUsageType{33};
            };

            using namespace Time;
            Cache::TimedCache<String, DiskSpaceUsageType> sDiskUsageCache_{5.0_duration};
            // explicitly caller maintaining the cache
            optional<DiskSpaceUsageType> LookupDiskStats_Try1 (String diskName)
            {
                optional<DiskSpaceUsageType> o = sDiskUsageCache_.Lookup (diskName);
                if (not o.has_value ()) {
                    o = LookupDiskStats_ (diskName);
                    if (o) {
                        sDiskUsageCache_.Add (diskName, *o);
                    }
                }
                return o;
            }
            // more automatic maintainance of that update pattern
            DiskSpaceUsageType LookupDiskStats_Try2 (String diskName)
            {
                return sDiskUsageCache_.LookupValue (diskName,
                                                     [] (String diskName) -> DiskSpaceUsageType { return LookupDiskStats_ (diskName); });
            }
            // or still simpler
            DiskSpaceUsageType LookupDiskStats_Try3 (String diskName)
            {
#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
                if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
                    // Arm/Raspi g++-11 at least, broken here and generate either badcode for below, or more likely just spurious warning from ubsan...
                    // -- LGP 2023-11-08
                    return LookupDiskStats_Try2 (diskName);
                }
#endif
                return sDiskUsageCache_.LookupValue (diskName, LookupDiskStats_);
            }
            void DoIt ()
            {
                EXPECT_TRUE (Memory::NullCoalesce (LookupDiskStats_Try1 ("xx")).size == 33);
                EXPECT_TRUE (LookupDiskStats_Try2 ("xx").size == 33);
                EXPECT_TRUE (LookupDiskStats_Try3 ("xx").size == 33);
                [[maybe_unused]] auto n = sDiskUsageCache_.Elements ().size ();
                for (const auto& ci : sDiskUsageCache_.Elements ()) {
                    sDiskUsageCache_.Add (ci.fKey, ci.fValue, ci.fLastRefreshedAt);
                }
            }
        }
        namespace Example2_ {
            using Execution::Synchronized;
            using Time::Duration;

            using ScanFolderKey_ = String;
            const Duration kAgeForScanPersistenceCache_{5min};
            struct FolderDetails_ {
                int size; // ...info to cache about a folder
            };
            Synchronized<Cache::TimedCache<ScanFolderKey_, shared_ptr<FolderDetails_>>> sCachedScanFoldersDetails_{kAgeForScanPersistenceCache_};

            shared_ptr<FolderDetails_> AccessFolder_ (const ScanFolderKey_& folder)
            {
                auto lockedCache = sCachedScanFoldersDetails_.rwget ();
                if (optional<shared_ptr<FolderDetails_>> o =
                        lockedCache->Lookup (folder, TimedCacheSupport::LookupMarksDataAsRefreshed::eTreatFoundThroughLookupAsRefreshed)) {
                    return *o;
                }
                else {
                    shared_ptr<FolderDetails_> fd = make_shared<FolderDetails_> (); // and fill in default values looking at disk
                    lockedCache->Add (folder, fd);
                    return fd;
                }
            }

            void DoIt ()
            {
                auto f1      = AccessFolder_ (L"folder1");
                auto f2      = AccessFolder_ (L"folder2");
                auto f1again = AccessFolder_ (L"folder1"); // if you trace through the debug code you'll see this is a cache hit
            }
        }
    }
    GTEST_TEST (Foundation_Caching, Test4_TimedCache_)
    {
        Test4_TimedCache_Private_::Example1_::DoIt ();
        Test4_TimedCache_Private_::Example2_::DoIt ();
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, Test5_Memoizer_)
    {
        {
            unsigned int totalCallsCount{};
#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
            Memoizer<int, MemoizerSupport::DEFAULT_CACHE_BWA_, int, int> memoizer{[&totalCallsCount] (int a, int b) {
                totalCallsCount++;
                return a + b;
            }};
#else
            Memoizer<int, LRUCache, int, int> memoizer{[&totalCallsCount] (int a, int b) {
                totalCallsCount++;
                return a + b;
            }};
#endif
            EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
            EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
        }
#if 0
            {
                // http://stroika-bugs.sophists.com/browse/STK-812
                // trying to figure out how to do...
                unsigned int totalCallsCount{};
                Memoizer     memoizer{[&totalCallsCount](int a, int b) { totalCallsCount++;  return a + b; }};
                EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
                EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
            }
#endif
    }
}

namespace {
    namespace Test6_CallerStalenessCache_ {
        // FROM Example Usage in ???
        namespace Private_ {
            unsigned int  sCalls1_{0};
            optional<int> LookupExternalInternetAddress_ (optional<Time::DurationSeconds> allowedStaleness = {})
            {
                using Cache::CallerStalenessCache;
                static CallerStalenessCache<void, optional<int>> sCache_;
                return sCache_.LookupValue (sCache_.Ago (allowedStaleness.value_or (30s)), [] () -> optional<int> {
                    sCalls1_++;
                    return 1;
                });
            }
            void Test_void_ ()
            {
                EXPECT_TRUE (LookupExternalInternetAddress_ () == 1 and Private_::sCalls1_ == 1);
                EXPECT_TRUE (LookupExternalInternetAddress_ () == 1 and Private_::sCalls1_ == 1);
            }
        }
        namespace Private_ {
            unsigned int  sCalls2_{0};
            optional<int> MapValue_ (int value, optional<Time::DurationSeconds> allowedStaleness = {})
            {
                using Cache::CallerStalenessCache;
                static CallerStalenessCache<int, optional<int>> sCache_;
                return sCache_.LookupValue (value, sCache_.Ago (allowedStaleness.value_or (30s)), [=] (int v) -> optional<int> {
                    sCalls2_++;
                    return v;
                });
            }
            void Test_keyed_ ()
            {
                EXPECT_TRUE (MapValue_ (1) == 1 and sCalls2_ == 1);
                EXPECT_TRUE (MapValue_ (2) == 2 and sCalls2_ == 2);
                EXPECT_TRUE (MapValue_ (1) == 1 and sCalls2_ == 2);
                EXPECT_TRUE (MapValue_ (2) == 2 and sCalls2_ == 2);
            }
        }
        namespace Private_Sync_ {
            unsigned int  sCalls1_{0};
            optional<int> LookupExternalInternetAddress_ (optional<Time::DurationSeconds> allowedStaleness = {})
            {
                using Cache::SynchronizedCallerStalenessCache;
                static SynchronizedCallerStalenessCache<void, optional<int>> sCache_;
                return sCache_.LookupValue (sCache_.Ago (allowedStaleness.value_or (30s)), [] () -> optional<int> {
                    sCalls1_++;
                    return 1;
                });
            }
            void Test_void_ ()
            {
                EXPECT_TRUE (LookupExternalInternetAddress_ () == 1 and sCalls1_ == 1);
                EXPECT_TRUE (LookupExternalInternetAddress_ () == 1 and sCalls1_ == 1);
            }
        }
        namespace Private_Sync_ {
            unsigned int  sCalls2_{0};
            optional<int> MapValue_ (int value, optional<Time::DurationSeconds> allowedStaleness = {})
            {
                using Cache::SynchronizedCallerStalenessCache;
                static SynchronizedCallerStalenessCache<int, optional<int>> sCache_;
                return sCache_.LookupValue (value, sCache_.Ago (allowedStaleness.value_or (30s)), [=] (int v) -> optional<int> {
                    sCalls2_++;
                    return v;
                });
            }
            void Test_keyed_ ()
            {
                EXPECT_TRUE (MapValue_ (1) == 1 and sCalls2_ == 1);
                EXPECT_TRUE (MapValue_ (2) == 2 and sCalls2_ == 2);
                EXPECT_TRUE (MapValue_ (1) == 1 and sCalls2_ == 2);
                EXPECT_TRUE (MapValue_ (2) == 2 and sCalls2_ == 2);
            }
        }
    }
    GTEST_TEST (Foundation_Caching, CallerStalenessCache_)
    {
        using namespace Test6_CallerStalenessCache_;
        Private_::Test_void_ ();
        Private_::Test_keyed_ ();
        Private_Sync_::Test_void_ ();
        Private_Sync_::Test_keyed_ ();
    }
}

namespace {
    namespace Test7_BloomFilter_PRivate_ {
        void SimpleBasic ()
        {
            Debug::TraceContextBumper ctx{"SimpleBasic"};
            constexpr int             kTotalEntries_{1000};
            BloomFilter<int>          f{BloomFilter<int>{kTotalEntries_}};
            for (auto i : Traversal::DiscreteRange<int>{1, kTotalEntries_}) {
                if (i & 1) {
                    f.Add (i);
                }
            }
            unsigned int falsePositives{};
            for (auto i : Traversal::DiscreteRange<int>{1, kTotalEntries_}) {
                if (i & 1) {
                    EXPECT_TRUE (f.ProbablyContains (i));
                }
                else {
                    if (f.ProbablyContains (i)) {
                        falsePositives++;
                    }
                }
            }
            auto falsePositivesMax = kTotalEntries_ / 2; // total number that should be false
            DbgTrace ("stats: {}"_f, Characters::ToString (f.GetStatistics ()));
            DbgTrace ("Probability of false positives = {}"_f, f.ProbabilityOfFalsePositive (kTotalEntries_));
            DbgTrace ("false positives: {}, expected: {}"_f, falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (kTotalEntries_));
            VerifyTestResultWarning (falsePositives < 100); // last measured was 75 (was 60 with old hash function) no matter how things change
            auto pfp = f.ProbabilityOfFalsePositive (kTotalEntries_);
            auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.1, 1.1}); // my probs estimate not perfect, so add some wiggle around it
            DbgTrace ("expectedFalsePositiveRange: {}"_f, Characters::ToString (expectedFalsePositiveRange));
            VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
        }
        void SimpleInternetAddressTest ()
        {
            Debug::TraceContextBumper ctx{"SimpleInternetAddressTest"};
            using Characters::String;
            using IO::Network::CIDR;
            using IO::Network::InternetAddress;
            CIDR cidr{L"192.168.243.0/24"};
            BloomFilter<InternetAddress> f{BloomFilter<InternetAddress>{cidr.GetRange ().GetNumberOfContainedPoints ()}}; // way more than needed so SB small # of false positives
            Containers::Set<InternetAddress> oracle;
            for (const InternetAddress& ia : cidr.GetRange ()) {
                default_random_engine      gen (random_device{}()); //Standard mersenne_twister_engine seeded with rd()
                uniform_int_distribution<> uniformDist{0, 1};
                if (uniformDist (gen) == 0) {
                    f.Add (ia);
                    oracle.Add (ia);
                }
            }
            unsigned int falsePositives{};
            for (const InternetAddress& ia : cidr.GetRange ()) {
                if (oracle.Contains (ia)) {
                    EXPECT_TRUE (f.ProbablyContains (ia));
                }
                else {
                    if (f.ProbablyContains (ia)) {
                        falsePositives++;
                    }
                }
            }
            auto totalEntries      = cidr.GetRange ().GetNumberOfContainedPoints ();
            auto falsePositivesMax = totalEntries - oracle.size (); // total number that should be false
            DbgTrace ("stats: {}"_f, f.GetStatistics ());
            DbgTrace ("Probability of false positives = {}"_f, f.ProbabilityOfFalsePositive (totalEntries));
            DbgTrace ("false positives: {}, expected: {}"_f, falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (totalEntries));
            VerifyTestResultWarning (falsePositives < 75); // typically 15, but anything over 75 probably buggy, no matter how things change
            auto pfp = f.ProbabilityOfFalsePositive (totalEntries);
            auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.1, 1.1}); // my probs estimate not perfect, so add some wiggle around it
            DbgTrace ("expectedFalsePositiveRange: {}"_f, expectedFalsePositiveRange);
            VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
        }
        void SimpleInternetAddressTestWithExplicitHash ()
        {
            Debug::TraceContextBumper ctx{"SimpleInternetAddressTestWithExplicitHash"};
            using Characters::String;
            using IO::Network::CIDR;
            using IO::Network::InternetAddress;
            auto hashFunction = [] (const InternetAddress& a) -> size_t { return hash<string>{}(a.As<String> ().AsUTF8<string> ()); };
            CIDR cidr{L"192.168.243.0/24"};
            BloomFilter<InternetAddress> f{BloomFilter<InternetAddress>{cidr.GetRange ().GetNumberOfContainedPoints (), hashFunction}}; // way more than needed so SB small # of false positives
            Containers::Set<InternetAddress> oracle;
            for (const InternetAddress& ia : cidr.GetRange ()) {
                default_random_engine      gen{random_device{}()}; //Standard mersenne_twister_engine seeded with rd()
                uniform_int_distribution<> uniformDist{0, 1};
                if (uniformDist (gen) == 0) {
                    f.Add (ia);
                    oracle.Add (ia);
                }
            }
            unsigned int falsePositives{};
            for (const InternetAddress& ia : cidr.GetRange ()) {
                if (oracle.Contains (ia)) {
                    EXPECT_TRUE (f.ProbablyContains (ia));
                }
                else {
                    if (f.ProbablyContains (ia)) {
                        falsePositives++;
                    }
                }
            }
            auto totalEntries      = cidr.GetRange ().GetNumberOfContainedPoints ();
            auto falsePositivesMax = totalEntries - oracle.size (); // total number that should be false
            DbgTrace ("stats: {}"_f, Characters::ToString (f.GetStatistics ()));
            DbgTrace ("Probability of false positives = {}"_f, f.ProbabilityOfFalsePositive (totalEntries));
            DbgTrace ("false positives: {}, expected: {}"_f, falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (totalEntries));
            VerifyTestResultWarning (falsePositives < 75); // typically 15, but anything over 75 probably buggy, no matter how things change
            auto pfp = f.ProbabilityOfFalsePositive (totalEntries);
            auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.1, 1.1}); // my probs estimate not perfect, so add some wiggle around it
            DbgTrace ("expectedFalsePositiveRange: {}"_f, Characters::ToString (expectedFalsePositiveRange));
            VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
        }
        void SimpleBloomTestWithStroikaDigester ()
        {
            Debug::TraceContextBumper ctx{"SimpleBloomTestWithStroikaDigester"};
            using namespace Cryptography::Digest;
            using Characters::String;
            using IO::Network::CIDR;
            using IO::Network::InternetAddress;
            auto hashFunction = [] (const InternetAddress& a) -> int {
                return Digester<Algorithm::SuperFastHash>{}(Memory::BLOB{a.As<vector<uint8_t>> ()});
            };
            CIDR                             cidr{L"192.168.243.0/24"};
            BloomFilter<InternetAddress>     f{BloomFilter<InternetAddress>{cidr.GetRange ().GetNumberOfContainedPoints (), hashFunction}};
            Containers::Set<InternetAddress> oracle;
            for (const InternetAddress& ia : cidr.GetRange ()) {
                default_random_engine      gen (random_device{}()); //Standard mersenne_twister_engine seeded with rd()
                uniform_int_distribution<> uniformDist{0, 1};
                if (uniformDist (gen) == 0) {
                    f.Add (ia);
                    oracle.Add (ia);
                }
            }
            unsigned int falsePositives{};
            for (const InternetAddress& ia : cidr.GetRange ()) {
                if (oracle.Contains (ia)) {
                    EXPECT_TRUE (f.ProbablyContains (ia));
                }
                else {
                    if (f.ProbablyContains (ia)) {
                        falsePositives++;
                    }
                }
            }
            auto totalEntries      = cidr.GetRange ().GetNumberOfContainedPoints ();
            auto falsePositivesMax = totalEntries - oracle.size (); // total number that should be false
            DbgTrace ("stats: {}"_f, f.GetStatistics ());
            DbgTrace ("Probability of false positives = {}"_f, f.ProbabilityOfFalsePositive (totalEntries));
            DbgTrace ("false positives: {}, expected: {}"_f, falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (totalEntries));
            VerifyTestResultWarning (falsePositives < 75); // typically around 14 (now 20)
            auto pfp = f.ProbabilityOfFalsePositive (totalEntries);
            auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.1, 1.1}); // my probs estimate not perfect, so add some wiggle around it
            DbgTrace ("expectedFalsePositiveRange: {}"_f, Characters::ToString (expectedFalsePositiveRange));
            VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
        }
        void TestSuggestionsForFilterSize ()
        {
            Debug::TraceContextBumper ctx{"TestSuggestionsForFilterSize"};
            using IO::Network::CIDR;
            using IO::Network::InternetAddress;
            using Traversal::DiscreteRange;

            auto runTest = [] (CIDR cidr, double runToProbOfFalsePositive, double runToFractionFull, double bitSizeFactor = 1.0) {
                Debug::TraceContextBumper                 ctx{"runTest",
                                              "cidr={}, runToProbOfFalsePositive={}, runToFractionFull={}, bitSizeFactor={}"_f,
                                              Characters::ToString (cidr),
                                              runToProbOfFalsePositive,
                                              runToFractionFull,
                                              bitSizeFactor};
                Containers::Set<InternetAddress>          oracle;
                Traversal::DiscreteRange<InternetAddress> scanAddressRange = cidr.GetRange ();
                BloomFilter<InternetAddress>              addressesProbablyUsed{
                    BloomFilter<InternetAddress>{static_cast<size_t> (bitSizeFactor * scanAddressRange.GetNumberOfContainedPoints ())}};
                [[maybe_unused]] unsigned int nLoopIterations{};
                [[maybe_unused]] unsigned int nActualCollisions{};
                [[maybe_unused]] unsigned int nContainsMistakes{};
                while (true) {
                    nLoopIterations++;
                    auto bloomFilterStats = addressesProbablyUsed.GetStatistics ();
                    //DbgTrace (L"***addressesProbablyUsed.GetStatistics ()=%s", Characters::ToString (bloomFilterStats).c_str ());
                    if (bloomFilterStats.ProbabilityOfFalsePositive () < runToProbOfFalsePositive and
                        double (bloomFilterStats.fApparentlyDistinctAddCalls) / scanAddressRange.GetNumberOfContainedPoints () < runToFractionFull) {
                        static mt19937 sRng_{std::random_device{}()};
                        unsigned int selected = uniform_int_distribution<unsigned int>{1, scanAddressRange.GetNumberOfContainedPoints () - 2}(sRng_);
                        InternetAddress ia                    = scanAddressRange.GetLowerBound ().Offset (selected);
                        bool            wasAlreadyPresent     = oracle.Contains (ia);
                        bool            appearsAlreadyPresent = addressesProbablyUsed.ProbablyContains (ia);
                        if (wasAlreadyPresent != appearsAlreadyPresent) {
                            nContainsMistakes++;
                        }
                        if (wasAlreadyPresent) {
                            nActualCollisions++;
                        }
                        addressesProbablyUsed.Add (ia);
                        oracle.Add (ia);
                    }
                    else {
                        DbgTrace ("Completed full scan: nIterations={}, nActualCollisions={}, nContainsMistakes={}, pctActualCoverage={}"_f,
                                  nLoopIterations, nActualCollisions, nContainsMistakes,
                                  double (oracle.size ()) / scanAddressRange.GetNumberOfContainedPoints ());
                        DbgTrace ("addressesProbablyUsed.GetStatistics ()={}"_f, Characters::ToString (addressesProbablyUsed.GetStatistics ()));
                        break;
                    }
                }
            };
            runTest (CIDR{L"192.168.243.0/24"}, .5, .5);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .6);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .7);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .5, 2);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .6, 2);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .7, 2);
            runTest (CIDR{L"192.168.243.0/24"}, .5, .8, 2);
        }
    }
    GTEST_TEST (Foundation_Caching, BloomFilter_)
    {
        Debug::TraceContextBumper ctx{"Test7_BloomFilter_"};
        Test7_BloomFilter_PRivate_::SimpleBasic ();
        Test7_BloomFilter_PRivate_::SimpleInternetAddressTestWithExplicitHash ();
        Test7_BloomFilter_PRivate_::SimpleInternetAddressTest ();
        Test7_BloomFilter_PRivate_::SimpleBloomTestWithStroikaDigester ();
        Test7_BloomFilter_PRivate_::TestSuggestionsForFilterSize ();
    }
}

namespace {
    namespace Test8_NewLRUCacheConstructors_Private_ {
        void T_NoHashTableCTORs1_ ()
        {
            using namespace Characters;
            constexpr auto kStringCIComparer_ = String::EqualsComparer{eCaseInsensitive};
            {
                // explicit or defaulted params
                LRUCache<string, string>                         t0{};
                LRUCache<string, string>                         t1{3};
                LRUCache<String, string, String::EqualsComparer> t2{3, kStringCIComparer_};
            }
            {
                // DEDUCTION using Factory approach
                auto t0{Factory::LRUCache_NoHash<string, string>{}()};
                auto t1{Factory::LRUCache_NoHash<string, string>{}(3)};
                auto t2{Factory::LRUCache_NoHash<String, string>{}(3, kStringCIComparer_)};
            }
            {
                // DEDUCTION alt syntax
#if !qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
                LRUCache t0{Factory::LRUCache_NoHash<string, string>{}()};
                LRUCache t1{Factory::LRUCache_NoHash<string, string>{}(3)};
#endif
                auto t2{Factory::LRUCache_NoHash<String, string>{}(3, kStringCIComparer_)};
            }
        }
        void T_WithHashTableCTORs1_ ()
        {
            using namespace Characters;
            constexpr auto kStringCIComparer_ = String::EqualsComparer{eCaseInsensitive};
            auto           hashFunction       = [] (const String& a) -> size_t { return hash<string>{}(a.AsUTF8<string> ()); };

            {
                // explicit or defaulted params
                LRUCache<string, string, equal_to<string>, hash<string>>            t0{3, 3};
                LRUCache<string, string, equal_to<string>, decltype (hashFunction)> t1{3, 3, hashFunction};
                LRUCache<string, string, equal_to<string>, hash<string>>            t2{3, equal_to<string>{}, 3};
            }
            {
                // DEDUCTION using Factory approach
                auto t0{Factory::LRUCache_WithHash<string, string>{}(3, 3)};
                auto t1{Factory::LRUCache_WithHash<String, string>{}(3, 3, hashFunction)};
#if !qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
                LRUCache t2{Factory::LRUCache_WithHash<String, string>{}(3, equal_to<String>{}, 3)};
                LRUCache t3{Factory::LRUCache_WithHash<String, string, Statistics::Stats_Basic>{}(3, equal_to<String>{}, 3)}; // throw in stats object
#endif
                auto t4{Factory::LRUCache_WithHash<String, string>{}(3, kStringCIComparer_, 3)}; // alt equality comparer
            }
        }
    }
    GTEST_TEST (Foundation_Caching, NewLRUCacheConstructors_)
    {
        Test8_NewLRUCacheConstructors_Private_::T_NoHashTableCTORs1_ ();
        Test8_NewLRUCacheConstructors_Private_::T_WithHashTableCTORs1_ ();
    }
}

#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
