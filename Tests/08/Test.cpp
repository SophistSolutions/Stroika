/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::DoublyLinkedList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/DataStructures/DoublyLinkedList.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;

namespace {
    static void Test1 ()
    {
        DataStructures::DoublyLinkedList<size_t> someLL;
        constexpr size_t                         kBigSize = 1001;

        Assert (kBigSize > 100);
        EXPECT_TRUE (someLL.size () == 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }
        someLL.RemoveAll ();
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }
        {
            for (size_t i = 1; i <= kBigSize - 10; i++) {
                someLL.RemoveFirst ();
            }
        }
        someLL.RemoveAll (); //  someLL.SetLength(kBigSize, 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }

        EXPECT_TRUE (someLL.size () == kBigSize);
        someLL.SetAt (55, 55);                      //  someLL [55] = 55;
        EXPECT_TRUE (someLL.GetAt (55) == 55); //  EXPECT_TRUE(someArray [55] == 55);
        EXPECT_TRUE (someLL.GetAt (55) != 56); //  EXPECT_TRUE(someArray [55] != 56);
        {
            size_t i = 1;
            for (DataStructures::DoublyLinkedList<size_t>::ForwardIterator it{&someLL}; not it.Done (); ++it, ++i) {
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

    static void Test2 ()
    {
        DataStructures::DoublyLinkedList<SimpleClass> someLL;
        const size_t                                  kBigSize = 1000;

        EXPECT_TRUE (someLL.size () == 0);

        Assert (kBigSize > 10);
        EXPECT_TRUE (someLL.size () == 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }
        someLL.RemoveAll ();
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }
        {
            for (size_t i = 1; i <= kBigSize - 10; i++) {
                someLL.RemoveFirst ();
            }
        }
        someLL.RemoveAll (); //  someLL.SetLength(kBigSize, 0);
        {
            for (size_t i = 1; i <= kBigSize; i++) {
                someLL.Prepend (0);
            }
        }

        EXPECT_TRUE (someLL.size () == kBigSize);

        someLL.SetAt (55, 55); //  someLL [55] = 55;
        EXPECT_TRUE (someLL.GetAt (55) == 55);
        EXPECT_TRUE (not(someLL.GetAt (55) == 56));

        someLL.RemoveAll ();
        EXPECT_TRUE (someLL.size () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            EXPECT_TRUE (not someLL.Contains (i));
            someLL.Prepend (i);
            EXPECT_TRUE (someLL.GetFirst () == i);
            EXPECT_TRUE (someLL.Contains (i));
        }
        for (size_t i = 1; i <= kBigSize; ++i) {
            EXPECT_TRUE (someLL.GetFirst () == i);
            someLL.RemoveFirst ();
            EXPECT_TRUE (not someLL.Contains (i));
        }
        EXPECT_TRUE (someLL.size () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            someLL.Prepend (i);
        }
        for (size_t i = kBigSize; i >= 1; --i) {
            //cerr << "i, getat(i-1) = " << i << ", " << someLL.GetAt (i-1).GetValue () << endl;
            EXPECT_TRUE (someLL.GetAt (i - 1) == i);
        }
    }
}

namespace {
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        Test1 ();
        Test2 ();
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature" << endl;
#endif
}
