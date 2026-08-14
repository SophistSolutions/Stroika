/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::DoublyLinkedList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>
#include <vector>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/DataStructures/DoublyLinkedList.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

// NB: these MUST be declared out here, not inside a GTEST_TEST body - in there, 'Test' names gtest's
// own ::testing::Test base class, so 'Test::ArchtypeClasses::X' does not compile
using Test::ArchtypeClasses::CopyableWithThrowingCopyCTOR;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, Test1)
    {
        DataStructures::DoublyLinkedList<size_t> someLL;
        constexpr size_t                         kBigSize = 1001;

        Assert (kBigSize > 100);
        EXPECT_TRUE (someLL.size () == 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }
        someLL.clear ();
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }
        {
            for (size_t i = 1; i <= kBigSize - 10; i++) {
                someLL.RemoveFirst ();
            }
        }
        someLL.clear (); //  someLL.SetLength(kBigSize, 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }

        EXPECT_TRUE (someLL.size () == kBigSize);
        someLL.SetAt (55, 55);                 //  someLL [55] = 55;
        EXPECT_TRUE (someLL.GetAt (55) == 55); //  EXPECT_TRUE(someArray [55] == 55);
        EXPECT_TRUE (someLL.GetAt (55) != 56); //  EXPECT_TRUE(someArray [55] != 56);
        {
            size_t i = 1;
            for (DataStructures::DoublyLinkedList<size_t>::ForwardIterator it{&someLL}; not it.AtEnd (); ++it, ++i) {
                if (i == 100) {
                    someLL.AddAfter (it, 1);
                    break;
                }
            }
        } //   someLL.InsertAt(1, 100);

        EXPECT_TRUE (someLL.size () == kBigSize + 1);
        EXPECT_TRUE (someLL.GetAt (100) == 1); //  EXPECT_TRUE(someArray [100] == 1);

        someLL.SetAt (101, someLL.GetAt (100) + 5);

        EXPECT_TRUE (someLL.GetAt (101) == 6);
        someLL.RemoveFirst ();
        EXPECT_TRUE (someLL.GetAt (100) == 6);
    }

    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, Test2)
    {
        DataStructures::DoublyLinkedList<OnlyCopyableMoveableAndTotallyOrdered> someLL;
        const size_t                                                            kBigSize = 1000;

        EXPECT_TRUE (someLL.size () == 0);

        Assert (kBigSize > 10);
        EXPECT_TRUE (someLL.size () == 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }
        someLL.clear ();
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }
        {
            for (size_t i = 1; i <= kBigSize - 10; i++) {
                someLL.RemoveFirst ();
            }
        }
        someLL.clear (); //  someLL.SetLength(kBigSize, 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.push_front (0);
            }
        }

        EXPECT_TRUE (someLL.size () == kBigSize);

        someLL.SetAt (55, 55); //  someLL [55] = 55;
        EXPECT_TRUE (someLL.GetAt (55) == 55);
        EXPECT_TRUE (not(someLL.GetAt (55) == 56));

        someLL.clear ();
        EXPECT_TRUE (someLL.size () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            EXPECT_TRUE (not someLL.Contains (i));
            someLL.push_front (i);
            EXPECT_EQ (someLL.GetFirst (), i);
            EXPECT_TRUE (someLL.Contains (i));
        }
        for (size_t i = 1; i <= kBigSize; ++i) {
            EXPECT_TRUE (someLL.GetFirst () == i);
            someLL.RemoveFirst ();
            EXPECT_TRUE (not someLL.Contains (i));
        }
        EXPECT_TRUE (someLL.size () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            someLL.push_front (i);
        }
        for (size_t i = kBigSize; i >= 1; --i) {
            //cerr << "i, getat(i-1) = " << i << ", " << someLL.GetAt (i-1).GetValue () << endl;
            EXPECT_TRUE (someLL.GetAt (i - 1) == i);
        }
    }
}

namespace {
    // walk the list BACKWARD, which is what exercises fPrev - forward-only iteration cannot see
    // corrupt back-links, which is how the Remove ()/operator= defects survived
    template <typename T>
    vector<T> ReverseContents_ (const DataStructures::DoublyLinkedList<T>& l)
    {
        vector<T> r;
        if (not l.empty ()) {
            typename DataStructures::DoublyLinkedList<T>::BidirectionalIterator i{&l};
            while (not i.AtEnd ()) { // walk to the last element
                auto next = i;
                ++next;
                if (next.AtEnd ()) {
                    break;
                }
                i = next;
            }
            for (;; --i) {
                r.push_back (*i);
                if (i.AtStart ()) {
                    break;
                }
            }
        }
        return r;
    }
    template <typename T>
    vector<T> ForwardContents_ (const DataStructures::DoublyLinkedList<T>& l)
    {
        return vector<T>{l.begin (), l.end ()};
    }
    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, Remove_by_value_maintains_backlinks_and_tail)
    {
        Debug::TraceContextBumper ctx{"Remove_by_value_maintains_backlinks_and_tail"};
        using LL = DataStructures::DoublyLinkedList<int>;
        auto mk  = [] () {
            LL l;
            for (int i : {1, 2, 3, 4, 5}) {
                l.push_back (i);
            }
            return l;
        };
        { // remove a MIDDLE element
            LL l = mk ();
            l.Remove (3);
            EXPECT_EQ (4u, l.size ());
            EXPECT_EQ ((vector<int>{1, 2, 4, 5}), ForwardContents_ (l));
            EXPECT_EQ ((vector<int>{5, 4, 2, 1}), ReverseContents_ (l)); // fPrev must still be sound
            EXPECT_EQ (5, *l.GetLast ());
            l.RemoveLast (); // needs fTail_ and fTail_->fPrev
            EXPECT_EQ ((vector<int>{1, 2, 4}), ForwardContents_ (l));
        }
        { // remove the LAST element - fTail_ must be updated
            LL l = mk ();
            l.Remove (5);
            EXPECT_EQ (4u, l.size ());
            EXPECT_EQ (4, *l.GetLast ());
            EXPECT_EQ ((vector<int>{4, 3, 2, 1}), ReverseContents_ (l));
        }
        { // remove the FIRST element
            LL l = mk ();
            l.Remove (1);
            EXPECT_EQ ((vector<int>{2, 3, 4, 5}), ForwardContents_ (l));
            EXPECT_EQ ((vector<int>{5, 4, 3, 2}), ReverseContents_ (l));
        }
        { // absent element - no change; and on an EMPTY list - must not dereference a null head
            LL l = mk ();
            l.Remove (99);
            EXPECT_EQ (5u, l.size ());
            LL e;
            e.Remove (42);
            EXPECT_EQ (0u, e.size ());
        }
    }
    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, operator_assign)
    {
        Debug::TraceContextBumper ctx{"operator_assign"};
        using LL = DataStructures::DoublyLinkedList<int>;
        LL src;
        for (int i : {1, 2, 3, 4}) {
            src.push_back (i);
        }
        LL dest;
        dest.push_back (99); // non-empty target, so operator= must clear first
        dest = src;
        EXPECT_EQ (4u, dest.size ());
        EXPECT_EQ ((vector<int>{1, 2, 3, 4}), ForwardContents_ (dest));
        EXPECT_EQ ((vector<int>{4, 3, 2, 1}), ReverseContents_ (dest)); // fPrev
        EXPECT_EQ (4, *dest.GetLast ());                                // fTail_
        dest.RemoveLast ();
        EXPECT_EQ ((vector<int>{1, 2, 3}), ForwardContents_ (dest));
        LL empty;
        dest = empty;
        EXPECT_EQ (0u, dest.size ());
        EXPECT_TRUE (dest.empty ());
    }
}

namespace {
    /*
     *  Link_ holds a T BY VALUE, so a Link_ that leaked is a T that was never destroyed - which is what
     *  CopyableWithThrowingCopyCTOR::sLiveCount sees.
     */
    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, copy_CTOR_does_not_leak_when_element_copy_throws)
    {
        Debug::TraceContextBumper ctx{"copy_CTOR_does_not_leak_when_element_copy_throws"};
        using T  = CopyableWithThrowingCopyCTOR;
        using LL = DataStructures::DoublyLinkedList<T>;
        LL src;
        for (int i = 1; i <= 5; ++i) {
            src.push_back (T{i});
        }
        const int kLiveBefore = T::sLiveCount;
        T::sCopiesUntilThrow  = 3; // so the 4th element's copy CTOR throws, 3 links already built
        EXPECT_THROW (LL{src}, runtime_error);
        T::sCopiesUntilThrow = -1;
        // a partly-built object never runs its destructor, so those 3 links have to be freed by the CTOR itself
        EXPECT_EQ (kLiveBefore, T::sLiveCount);
    }
}

namespace {
    GTEST_TEST (Foundation_DataStructures_DoublyLinkedList, ToString)
    {
        Debug::TraceContextBumper             ctx{"ToString"};
        DataStructures::DoublyLinkedList<int> t;
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
