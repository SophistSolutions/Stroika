/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Caching
#include "Stroika/Foundation/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Cache/BloomFilter.h"
#include "Stroika/Foundation/Cache/CallerStalenessCache.h"
#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Cache/Memoizer.h"
#include "Stroika/Foundation/Cache/TimedCache.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/SuperFastHash.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/IO/Network/InternetAddressRange.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cache;

namespace {
    namespace Test1_Simple_ {
        namespace Private_ {
            void T1_ ()
            {
                LRUCache<string, string> tmp{3};
                tmp.Add ("a", "1");
                tmp.Add ("b", "2");
                tmp.Add ("c", "3");
                tmp.Add ("d", "4");
                VerifyTestResult (not tmp.Lookup ("a").has_value ());
                VerifyTestResult (tmp.Lookup ("b") == "2");
                VerifyTestResult (tmp.Lookup ("d") == "4");

                LRUCache<string, string> tmp2 = tmp;
                VerifyTestResult (not tmp2.Lookup ("a").has_value ());
                VerifyTestResult (tmp2.Lookup ("b") == "2");
                VerifyTestResult (tmp2.Lookup ("d") == "4");
            }
            void T2_ ()
            {
                using CACHE = LRUCache<string, string, equal_to<string>, hash<string>>;
                CACHE tmp{3, equal_to<string>{}, 10};
                tmp.Add ("a", "1");
                tmp.Add ("b", "2");
                tmp.Add ("c", "3");
                tmp.Add ("d", "4");

                VerifyTestResult (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
                VerifyTestResult (tmp.Lookup ("b") == "2");
                VerifyTestResult (tmp.Lookup ("d") == "4");

                CACHE tmp2 = tmp;
                VerifyTestResult (not tmp2.Lookup ("a").has_value () or *tmp2.Lookup ("a") == "1"); // could be missing or found but if found same value
                VerifyTestResult (tmp2.Lookup ("b") == "2");
                VerifyTestResult (tmp2.Lookup ("d") == "4");
            }
            void T3_ ()
            {
                // using C++17 deduction guides
                LRUCache tmp{pair<string, string>{}, 3, 10, hash<string>{}};
                tmp.Add ("a", "1");
                tmp.Add ("b", "2");
                tmp.Add ("c", "3");
                tmp.Add ("d", "4");

                VerifyTestResult (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
                VerifyTestResult (tmp.Lookup ("b") == "2");
                VerifyTestResult (tmp.Lookup ("d") == "4");

                LRUCache tmp2 = tmp;
                VerifyTestResult (not tmp2.Lookup ("a").has_value () or *tmp2.Lookup ("a") == "1"); // could be missing or found but if found same value
                VerifyTestResult (tmp2.Lookup ("b") == "2");
                VerifyTestResult (tmp2.Lookup ("d") == "4");
            }
        }
        void DoIt ()
        {
            Private_::T1_ ();
            Private_::T2_ ();
            Private_::T3_ ();
        }
    }
}

namespace {
    namespace Test2_LRUCache_ObjWithNoArgCTORs_ {
        namespace Private_ {
            struct TNoCTOR_ {
                TNoCTOR_ (int) {}
                TNoCTOR_ () = delete;
                bool operator== ([[maybe_unused]] const TNoCTOR_& rhs) const { return true; }
            };
        }
        void DoIt ()
        {
            using Private_::TNoCTOR_;
            LRUCache<TNoCTOR_, TNoCTOR_> test (10);
            test.Add (TNoCTOR_ (1), TNoCTOR_ (1));
            (void)test.Lookup (TNoCTOR_ (1));
        }
    }
}

namespace {
    namespace Test3_LRUCache_Elements {
        namespace Private_ {
            struct TNoCTOR_ {
                TNoCTOR_ (int) {}
                TNoCTOR_ () = delete;
                bool operator== ([[maybe_unused]] const TNoCTOR_& rhs) const { return true; }
            };
        }
        void DoIt ()
        {
            LRUCache<string, string> tmp{3};
            tmp.Add ("a", "1");
            tmp.Add ("b", "2");
            tmp.Add ("c", "3");
            tmp.Add ("d", "4");
            Containers::Mapping<string, string> x = tmp.Elements (); // subtle cuz converting traits type
            VerifyTestResult (x.length () <= 4);
            for (auto i : tmp.Elements ()) {
                VerifyTestResult (i.fKey == "a" or i.fKey == "b" or i.fKey == "c" or i.fKey == "d");
                VerifyTestResult (i.fValue == "1" or i.fValue == "2" or i.fValue == "3" or i.fValue == "4");
            }
        }
    }
}

namespace {
    namespace Test4_TimedCache_ {
        // FROM Example Usage in TimedCache<>
        namespace Private_ {
            using Characters::String;
            namespace Example1_ {
                struct DiskSpaceUsageType {
                    int size;
                };
                // do the actual lookup part which maybe slow
                auto LookupDiskStats_ ([[maybe_unused]] const String& filename) -> DiskSpaceUsageType { return DiskSpaceUsageType{33}; };

                Cache::TimedCache<String, DiskSpaceUsageType> sDiskUsageCache_{5.0};
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
                                                         [] (String diskName) -> DiskSpaceUsageType {
                                                             return LookupDiskStats_ (diskName);
                                                         });
                }
                // or still simpler
                DiskSpaceUsageType LookupDiskStats_Try3 (String diskName)
                {
                    return sDiskUsageCache_.LookupValue (diskName, LookupDiskStats_);
                }
                void DoIt ()
                {
                    VerifyTestResult (Memory::ValueOrDefault (LookupDiskStats_Try1 (L"xx")).size == 33);
                    VerifyTestResult (LookupDiskStats_Try2 (L"xx").size == 33);
                    VerifyTestResult (LookupDiskStats_Try3 (L"xx").size == 33);
                }
            }
            namespace Example2_ {
                using Execution::Synchronized;
                using Time::DurationSecondsType;

                using ScanFolderKey_ = String;
                static constexpr DurationSecondsType kAgeForScanPersistenceCache_{5 * 60.0};
                struct FolderDetails_ {
                    int size; // ...info to cache about a folder
                };
                Synchronized<Cache::TimedCache<
                    ScanFolderKey_,
                    shared_ptr<FolderDetails_>,
                    TimedCacheSupport::DefaultTraits<ScanFolderKey_, shared_ptr<FolderDetails_>, less<ScanFolderKey_>, true>>>
                    sCachedScanFoldersDetails_{kAgeForScanPersistenceCache_};

                shared_ptr<FolderDetails_> AccessFolder_ (const ScanFolderKey_& folder)
                {
                    auto lockedCache = sCachedScanFoldersDetails_.rwget ();
                    if (optional<shared_ptr<FolderDetails_>> o = lockedCache->Lookup (folder)) {
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
        void DoIt ()
        {
            Private_::Example1_::DoIt ();
            Private_::Example2_::DoIt ();
        }
    }
}

namespace {
    namespace Test5_Memoizer_ {
        // FROM Example Usage in ???
        namespace Private_ {
        }
        void DoIt ()
        {
            {
                unsigned int totalCallsCount{};
#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
                Memoizer<int, MemoizerSupport::DEFAULT_CACHE_BWA_, int, int> memoizer{[&totalCallsCount] (int a, int b) { totalCallsCount++;  return a + b; }};
#else
                Memoizer<int, LRUCache, int, int> memoizer{[&totalCallsCount] (int a, int b) { totalCallsCount++;  return a + b; }};
#endif
                VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
                VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
            }
#if 0
            {
                // trying to figure out how to do...
                unsigned int totalCallsCount{};
                Memoizer     memoizer{[&totalCallsCount](int a, int b) { totalCallsCount++;  return a + b; }};
                VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
                VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
            }
#endif
        }
    }
}

namespace {
    namespace Test6_CallerStalenessCache_ {
        // FROM Example Usage in ???
        namespace Private_ {
            unsigned int  sCalls1_{0};
            optional<int> LookupExternalInternetAddress_ (optional<Time::DurationSecondsType> allowedStaleness = {})
            {
                using Cache::CallerStalenessCache;
                static CallerStalenessCache<void, optional<int>> sCache_;
                return sCache_.LookupValue (sCache_.Ago (allowedStaleness.value_or (30)), [] () -> optional<int> {
                    sCalls1_++;
                    return 1;
                });
            }
            void Test_void_ ()
            {
                VerifyTestResult (Private_::LookupExternalInternetAddress_ () == 1 and Private_::sCalls1_ == 1);
                VerifyTestResult (Private_::LookupExternalInternetAddress_ () == 1 and Private_::sCalls1_ == 1);
            }
        }
        namespace Private_ {
            unsigned int  sCalls2_{0};
            optional<int> MapValue_ (int value, optional<Time::DurationSecondsType> allowedStaleness = {})
            {
                using Cache::CallerStalenessCache;
                static CallerStalenessCache<int, optional<int>> sCache_;
                return sCache_.LookupValue (value, sCache_.Ago (allowedStaleness.value_or (30)), [=] (int v) -> optional<int> {
                    sCalls2_++;
                    return v;
                });
            }
            void Test_keyed_ ()
            {
                VerifyTestResult (Private_::MapValue_ (1) == 1 and Private_::sCalls2_ == 1);
                VerifyTestResult (Private_::MapValue_ (2) == 2 and Private_::sCalls2_ == 2);
                VerifyTestResult (Private_::MapValue_ (1) == 1 and Private_::sCalls2_ == 2);
                VerifyTestResult (Private_::MapValue_ (2) == 2 and Private_::sCalls2_ == 2);
            }
        }
        void DoIt ()
        {
            Private_::Test_void_ ();
            Private_::Test_keyed_ ();
        }
    }
}

namespace {
    namespace Test7_BloomFilter_ {
        namespace Private_ {
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
                        VerifyTestResult (f.Contains (i));
                    }
                    else {
                        if (f.Contains (i)) {
                            falsePositives++;
                        }
                    }
                }
                auto falsePositivesMax = kTotalEntries_ / 2;    // total number that should be false
                DbgTrace (L"stats: %s", Characters::ToString (f.GetStatistics ()).c_str ());
                DbgTrace (L"Probability of false positives = %f", f.ProbabilityOfFalsePositive (kTotalEntries_));
                DbgTrace (L"false positives: %d, expected: %f", falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (kTotalEntries_));
                VerifyTestResultWarning (falsePositives < 100); // last measured was 60, no matter how things change
                auto pfp                        = f.ProbabilityOfFalsePositive (kTotalEntries_);
                auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.25, 1.5}); // my probs estimate not perfect, so add some wiggle around it
                DbgTrace (L"expectedFalsePositiveRange: %s", Characters::ToString (expectedFalsePositiveRange).c_str ());
                VerifyTestResult (expectedFalsePositiveRange.Contains (falsePositives));
            }
            void SimpleInternetAddressTest ()
            {
                Debug::TraceContextBumper ctx{"SimpleInternetAddressTest"};
                using Characters::String;
                using IO::Network::CIDR;
                using IO::Network::InternetAddress;
                auto                             hashFunction = [] (const InternetAddress& a) -> int { return hash<string>{}(a.As<String> ().AsUTF8 ()); };
                CIDR                             cidr{L"192.168.243.0/24"};
                BloomFilter<InternetAddress>     f{BloomFilter<InternetAddress>{cidr.GetRange ().GetNumberOfContainedPoints (), hashFunction}}; // way more than needed so SB small # of false positives
                Containers::Set<InternetAddress> oracle;
                for (InternetAddress ia : cidr.GetRange ()) {
                    default_random_engine      gen (random_device{}()); //Standard mersenne_twister_engine seeded with rd()
                    uniform_int_distribution<> uniformDist{0, 1};
                    if (uniformDist (gen) == 0) {
                        f.Add (ia);
                        oracle.Add (ia);
                    }
                }
                unsigned int falsePositives{};
                for (InternetAddress ia : cidr.GetRange ()) {
                    if (oracle.Contains (ia)) {
                        VerifyTestResult (f.Contains (ia));
                    }
                    else {
                        if (f.Contains (ia)) {
                            falsePositives++;
                        }
                    }
                }
                auto totalEntries = cidr.GetRange ().GetNumberOfContainedPoints ();
                auto falsePositivesMax = totalEntries - oracle.size (); // total number that should be false
                DbgTrace (L"stats: %s", Characters::ToString (f.GetStatistics ()).c_str ());
                DbgTrace (L"Probability of false positives = %f", f.ProbabilityOfFalsePositive (totalEntries));
                DbgTrace (L"false positives: %d, expected: %f", falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (totalEntries));
                VerifyTestResultWarning (falsePositives < 50); // typically 15, but anything over 50 probably buggy, no matter how things change
                auto pfp                        = f.ProbabilityOfFalsePositive (totalEntries);
                auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.2, 1.4}); // my probs estimate not perfect, so add some wiggle around it
                DbgTrace (L"expectedFalsePositiveRange: %s", Characters::ToString (expectedFalsePositiveRange).c_str ());
                VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
            }
            void SimpleBloomTestWithStroikaDigester ()
            {
                Debug::TraceContextBumper ctx{"SimpleBloomTestWithStroikaDigester"};
                using namespace Cryptography::Digest;
                using Characters::String;
                using IO::Network::CIDR;
                using IO::Network::InternetAddress;
                auto                             hashFunction = [] (const InternetAddress& a) -> int { return Digester<Algorithm::SuperFastHash>::ComputeDigest (Memory::BLOB{a.As<vector<uint8_t>> ()}); };
                CIDR                             cidr{L"192.168.243.0/24"};
                BloomFilter<InternetAddress>     f{BloomFilter<InternetAddress>{cidr.GetRange ().GetNumberOfContainedPoints (), hashFunction}};
                Containers::Set<InternetAddress> oracle;
                for (InternetAddress ia : cidr.GetRange ()) {
                    default_random_engine      gen (random_device{}()); //Standard mersenne_twister_engine seeded with rd()
                    uniform_int_distribution<> uniformDist{0, 1};
                    if (uniformDist (gen) == 0) {
                        f.Add (ia);
                        oracle.Add (ia);
                    }
                }
                unsigned int falsePositives{};
                for (InternetAddress ia : cidr.GetRange ()) {
                    if (oracle.Contains (ia)) {
                        VerifyTestResult (f.Contains (ia));
                    }
                    else {
                        if (f.Contains (ia)) {
                            falsePositives++;
                        }
                    }
                }
                auto totalEntries = cidr.GetRange ().GetNumberOfContainedPoints ();
                auto falsePositivesMax = totalEntries - oracle.size (); // total number that should be false
                DbgTrace (L"stats: %s", Characters::ToString (f.GetStatistics ()).c_str ());
                DbgTrace (L"Probability of false positives = %f", f.ProbabilityOfFalsePositive (totalEntries));
                DbgTrace (L"false positives: %d, expected: %f", falsePositives, falsePositivesMax * f.ProbabilityOfFalsePositive (totalEntries));
                VerifyTestResultWarning (falsePositives < 25); // typically around 14
                auto pfp                        = f.ProbabilityOfFalsePositive (totalEntries);
                auto expectedFalsePositiveRange = falsePositivesMax * pfp * (Traversal::Range<double>{.1, 1.2}); // my probs estimate not perfect, so add some wiggle around it
                DbgTrace (L"expectedFalsePositiveRange: %s", Characters::ToString (expectedFalsePositiveRange).c_str ());
                VerifyTestResultWarning (expectedFalsePositiveRange.Contains (falsePositives));
            }
        }

        void DoIt ()
        {
            Debug::TraceContextBumper ctx{"Test7_BloomFilter_"};
            Private_::SimpleBasic ();
            Private_::SimpleInternetAddressTest ();
            Private_::SimpleBloomTestWithStroikaDigester ();
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_Simple_::DoIt ();
        Test2_LRUCache_ObjWithNoArgCTORs_::DoIt ();
        Test3_LRUCache_Elements::DoIt ();
        Test4_TimedCache_::DoIt ();
        Test5_Memoizer_::DoIt ();
        Test6_CallerStalenessCache_::DoIt ();
        Test7_BloomFilter_::DoIt ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
