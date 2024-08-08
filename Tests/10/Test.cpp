/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::SkipList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

namespace {
    // removed from stdc++ but still handy here
    template <class RandomIt>
    void random_shuffle_ (RandomIt first, RandomIt last)
    {
        typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
        for (diff_t i = last - first - 1; i > 0; --i) {
            using std::swap;
            swap (first[i], first[std::rand () % (i + 1)]);
        }
    }
}

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicSmokeTest)
    {
        Debug::TraceContextBumper ctx{"BasicSmokeTest"};
        SkipList<int, int>        t;
        EXPECT_TRUE (not t.contains (1));
        t.Add (1, 2);
        EXPECT_TRUE (t.contains (1));
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicIteration)
    {
        Debug::TraceContextBumper ctx{"BasicIteration"};
        SkipList<int, int>        t;
        EXPECT_EQ (distance (t.begin (), t.end ()), 0);
        t.Add (1, 2);
        EXPECT_EQ (distance (t.begin (), t.end ()), 1);
    }
}

namespace {
    namespace Private_ {
        template <typename KEY_TYPE, typename MAPPED_TYPE, typename TRAITS>
        void BasicAddRemoveTestsHelper_ (SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> prototypeList, size_t testLength)
        {
            SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> t{prototypeList.GetComparer ()};
            Require (t.size () == 0);
            Debug::TraceContextBumper ctx{"BasicAddRemoveTests_", "Add and remove (len={}, forward direction)"_f, testLength};
            optional<KEY_TYPE>        biggestKey;
            for (int i = 1; i <= testLength; ++i) {
                KEY_TYPE key{i};
                EXPECT_TRUE (not t.contains (key));
                t.Add (key, i);
                EXPECT_EQ (t.size (), i);
                EXPECT_EQ (t.First (key), i);
                t.Invariant ();
                if (biggestKey == nullopt) {
                    biggestKey = key;
                }
                strong_ordering comp = t.GetComparer () (*biggestKey, key);
                if (comp == strong_ordering::greater or comp == strong_ordering::less) {
                    biggestKey = key;
                }
            }
            for (int i = 1; i <= testLength; ++i) {
                KEY_TYPE key{i};
                EXPECT_EQ (t.First (key), i);
                t.Remove (key);
                EXPECT_TRUE (not t.contains (key));
                EXPECT_EQ (t.size (), testLength - i);
                t.Invariant ();
            }
            EXPECT_EQ (t.size (), 0);
            DbgTrace ("Add and remove {} items, backwards direction"_f, testLength);
            for (int i = static_cast<int> (testLength); i >= 1; --i) {
                KEY_TYPE key{i};
                EXPECT_TRUE (not t.contains (key));
                t.Add (key, i);
                EXPECT_EQ (t.size (), testLength - i + 1);
                EXPECT_EQ (t.First (key), i);
                t.Invariant ();
                Assert (biggestKey);
                strong_ordering comp = t.GetComparer () (*biggestKey, key);
                if (i == testLength or comp < 0) {
                    biggestKey = key;
                }
            }
            for (int i = static_cast<int> (testLength); i >= 1; --i) {
                KEY_TYPE key{i};
                EXPECT_EQ (t.First (key), i);
                t.Remove (key);
                EXPECT_TRUE (not t.contains (key));
                EXPECT_EQ (t.size (), i - 1);
                t.Invariant ();
            }
            EXPECT_EQ (t.size (), 0);
        }
    }
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, BasicAddRemoveTest_)
    {
        Debug::TraceContextBumper ctx{"BasicAddRemoveTest_"};
        SkipList<int, int>        t;
        Private_::BasicAddRemoveTestsHelper_ (t, 1000);
    }
}

namespace {
    namespace Private_ {
        template <typename KEY_TYPE, typename MAPPED_TYPE, typename TRAITS>
        void RandomOrderAddRemoveTestsHelper_ (SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> prototypeList, size_t testLength)
        {
            SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> t{prototypeList.GetComparer ()};
            Require (t.size () == 0);
            Debug::TraceContextBumper ctx{"RandomOrderAddRemoveTestsHelper_", "Add and remove (len={}, items, in random order)"_f, testLength};

            vector<int> data;
            data.reserve (testLength);
            for (int i = 0; i < static_cast<int> (testLength); ++i) {
                data.push_back (i);
            }
            random_shuffle_ (data.begin (), data.end ());
            optional<KEY_TYPE> biggestKey;
            optional<KEY_TYPE> smallestKey;
            for (int i = 0; i < data.size (); ++i) {
                KEY_TYPE key = {data[i]};
                EXPECT_TRUE (not t.contains (key));
                t.Add (key, i);
                EXPECT_EQ (t.size (), i + 1);
                EXPECT_EQ (t.First (key), i);
                t.Invariant ();
                if (i == 0) {
                    smallestKey = key;
                    biggestKey  = key;
                }
                if (t.GetComparer () (*biggestKey, key) < 0) {
                    biggestKey = key;
                }
                if (t.GetComparer () (*smallestKey, key) > 0) {
                    smallestKey = key;
                }
            }
            random_shuffle_ (data.begin (), data.end ());
            for (int i = 0; i <= static_cast<int> (testLength - 1); ++i) {
                KEY_TYPE v = {data[i]};
                EXPECT_TRUE (t.contains (v));
                t.Remove (v);
                EXPECT_TRUE (not t.contains (v));
                EXPECT_EQ (t.size (), testLength - i - 1);
                t.Invariant ();
            }
            EXPECT_TRUE (t.size () == 0);
        }
    }
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, RandomOrderAddRemoveTests_)
    {
        Debug::TraceContextBumper ctx{"RandomOrderAddRemoveTests_"};
        SkipList<int, int>        t;
        Private_::RandomOrderAddRemoveTestsHelper_ (t, 25);
    }
}

namespace {
    namespace Private_ {
        template <typename KEY_TYPE, typename MAPPED_TYPE, typename TRAITS>
        void OptimizeTestsHelper_ (SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> prototypeList, size_t testLength)
        {
            SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> t{prototypeList.GetComparer ()};
            Require (t.size () == 0);
            Debug::TraceContextBumper ctx{"Balance Tests", "Add and remove (len={})"_f, testLength};
            vector<int>               data;
            data.reserve (testLength);
            for (int i = 0; i < static_cast<int> (testLength); ++i) {
                data.push_back (i);
            }
            random_shuffle_ (data.begin (), data.end ());
            for (int i = 0; i < static_cast<int> (data.size ()); ++i) {
                KEY_TYPE key{data[i]};
                t.Add (key, i);
            }
            EXPECT_EQ (t.size (), testLength);
            t.Invariant ();
            t.ReBalance ();
            t.Invariant ();
            Assert (t.size () == testLength);
            for (size_t i = 0; i <= static_cast<int> (testLength - 1); ++i) {
                KEY_TYPE key{data[i]};
                EXPECT_EQ (t.First (key), i);
            }
        }
    }
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, OptimizeTests_)
    {
        Debug::TraceContextBumper ctx{"OptimizeTests_"};
        SkipList<int, int>        t;
        Private_::OptimizeTestsHelper_ (t, 25);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_SkipList, ToString)
    {
        Debug::TraceContextBumper ctx{"ToString"};
        SkipList<int, int>        t;
        t.Add (1, 2);
        DbgTrace ("t={}"_f, t); // test using ranges support
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
