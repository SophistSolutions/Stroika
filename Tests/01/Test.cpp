/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Caching
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Cache/Memoizer.h"
#include "Stroika/Foundation/Cache/TimedCache.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/Optional.h"

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
                LRUCache<string, string> tmp (3);
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
                CACHE tmp (3, equal_to<string>{}, 10);
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
        }
        void DoIt ()
        {
            Private_::T1_ ();
            Private_::T2_ ();
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
            LRUCache<string, string> tmp (3);
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

            struct DiskSpaceUsageType {
                int size;
            };
            auto LookupDiskStats_ ([[maybe_unused]] const String& filename) -> DiskSpaceUsageType { return DiskSpaceUsageType{33}; };

            Cache::TimedCache<String, DiskSpaceUsageType> sDiskUsageCache_{5.0};
            optional<DiskSpaceUsageType>                  LookupDiskStats (String diskName)
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
        }
        void DoIt ()
        {
            VerifyTestResult (Memory::ValueOrDefault (Private_::LookupDiskStats (L"xx")).size == 33);
            VerifyTestResult (Memory::ValueOrDefault (Private_::LookupDiskStats (L"xx")).size == 33);
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
            unsigned int                                            totalCallsCount{};
            Memoizer<int, MemoizerSupport::DEFAULT_CACHE, int, int> memoizer{[&totalCallsCount](int a, int b) { totalCallsCount++;  return a + b; }};
            VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
            VerifyTestResult (memoizer.Compute (1, 1) == 2 and totalCallsCount == 1);
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
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
