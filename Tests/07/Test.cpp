/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::LinkedList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <span>
#include <vector>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/DataStructures/LinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_LinkedList, Test1)
    {
        DataStructures::LinkedList<size_t> someLL;
        const size_t                       kBigSize = 1001;

        Assert (kBigSize > 100);
        EXPECT_EQ (someLL.size (), 0u);
        for (size_t i = 1; i <= kBigSize; i++) {
            someLL.push_front (0);
        }
        someLL.clear ();
        for (size_t i = 1; i <= kBigSize; i++) {
            someLL.push_front (0);
        }
        for (size_t i = 1; i <= kBigSize - 10; i++) {
            someLL.RemoveFirst ();
        }
        someLL.clear (); //  someLL.SetLength(kBigSize, 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }

        EXPECT_EQ (someLL.size (), kBigSize);
        someLL.SetAt (55, 55);                 //  someLL [55] = 55;
        EXPECT_EQ (someLL.GetAt (55), 55u);    //  EXPECT_TRUE(someArray [55] == 55);
        EXPECT_TRUE (someLL.GetAt (55) != 56); //  EXPECT_TRUE(someArray [55] != 56);
        {
            size_t i = 1;
            for (DataStructures::LinkedList<size_t>::ForwardIterator it{&someLL}; not it.AtEnd (); ++it, ++i) {
                [[maybe_unused]] auto cur = *it;
                if (i == 100) {
                    someLL.AddAfter (it, 1);
                    break;
                }
            }
        }

        EXPECT_EQ (someLL.size (), kBigSize + 1u);
        EXPECT_TRUE (someLL.GetAt (100) == 1); //  EXPECT_TRUE(someArray [100] == 1);

        someLL.SetAt (someLL.GetAt (100) + 5, 101);

        EXPECT_EQ (someLL.GetAt (101), 6u);
        someLL.RemoveFirst ();
        EXPECT_EQ (someLL.GetAt (100), 6u);
    }

    GTEST_TEST (Foundation_Containers_DataStructures_LinkedList, Test2)
    {
        DataStructures::LinkedList<OnlyCopyableMoveableAndTotallyOrdered> someLL;
        constexpr size_t                                                  kBigSize = 1000;

        EXPECT_EQ (someLL.size (), 0u);

        Assert (kBigSize > 10);
        EXPECT_EQ (someLL.size (), 0u);
        for (size_t i = 1; i <= kBigSize; i++) {
            someLL.push_front (0);
        }
        someLL.clear ();
        for (size_t i = 1; i <= kBigSize; i++) {
            someLL.push_front (0);
        }
        Assert (kBigSize > 10);
        for (size_t i = 1; i <= kBigSize - 10; i++) {
            someLL.RemoveFirst ();
        }
        someLL.clear (); //  someLL.SetLength(kBigSize, 0);
        for (size_t i = 1; i <= kBigSize; i++) {
            someLL.push_front (0);
        }

        EXPECT_EQ (someLL.size (), kBigSize);

        someLL.SetAt (55, 55); //  someLL [55] = 55;
        EXPECT_EQ (someLL.GetAt (55), 55);
        EXPECT_TRUE (not(someLL.GetAt (55) == 56));

        someLL.clear ();
        EXPECT_EQ (someLL.size (), 0u);

        for (size_t i = kBigSize; i >= 1; --i) {
            EXPECT_EQ (someLL.Find (i), nullptr);
            someLL.push_front (i);
            EXPECT_EQ (someLL.GetFirst (), i);
            EXPECT_TRUE (someLL.Find (i) != nullptr);
        }
        for (size_t i = 1; i <= kBigSize; ++i) {
            EXPECT_EQ (someLL.GetFirst (), i);
            someLL.RemoveFirst ();
            EXPECT_EQ (someLL.Find (i), nullptr);
        }
        EXPECT_EQ (someLL.size (), 0u);

        for (size_t i = kBigSize; i >= 1; --i) {
            someLL.push_front (i);
        }
        for (size_t i = kBigSize; i >= 1; --i) {
            //cerr << "i, getat(i-1) = " << i << ", " << someLL.GetAt (i-1).GetValue () << endl;
            EXPECT_EQ (someLL.GetAt (i - 1), i);
        }
    }
}

namespace {
    /*
     *  push_back (span) / push_front (span) must both leave the elements in SPAN order - that is what
     *  their doc-notes promise. Only the single-item push_back () was covered before.
     */
    GTEST_TEST (Foundation_Containers_DataStructures_LinkedList, push_span_preserves_order)
    {
        Debug::TraceContextBumper ctx{"push_span_preserves_order"};
        using LL           = DataStructures::LinkedList<int>;
        const int kData_[] = {1, 2, 3};
        auto      contents = [] (const LL& l) { return vector<int>{l.begin (), l.end ()}; };
        { // push_back (span) onto an EMPTY list
            LL l;
            l.push_back (span<const int>{kData_});
            EXPECT_EQ ((vector<int>{1, 2, 3}), contents (l));
        }
        { // push_back (span) onto a NON-empty list
            LL l;
            l.push_back (0);
            l.push_back (span<const int>{kData_});
            EXPECT_EQ ((vector<int>{0, 1, 2, 3}), contents (l));
        }
        { // push_front (span) onto an empty and a non-empty list
            LL l;
            l.push_front (span<const int>{kData_});
            EXPECT_EQ ((vector<int>{1, 2, 3}), contents (l));
            l.push_front (span<const int>{kData_});
            EXPECT_EQ ((vector<int>{1, 2, 3, 1, 2, 3}), contents (l));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_LinkedList, ToString)
    {
        Debug::TraceContextBumper       ctx{"ToString"};
        DataStructures::LinkedList<int> t;
        t.push_back (1);
        DbgTrace ("t={}"_f, t); // test using ranges support
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
