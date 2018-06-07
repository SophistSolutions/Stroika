/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::LinkedList
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/DataStructures/LinkedList.h"
#include "Stroika/Foundation/Containers/Private/PatchingDataStructures/LinkedList.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::DataStructures;
using namespace Stroika::Foundation::Containers::Private;
using namespace Stroika::Foundation::Containers::Private::PatchingDataStructures;

using Traversal::kUnknownIteratorOwnerID;

namespace {
    static void Test1 ()
    {
        PatchingDataStructures::LinkedList<size_t> someLL;
        const size_t                               kBigSize = 1001;

        Assert (kBigSize > 100);
        VerifyTestResult (someLL.GetLength () == 0);
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

        VerifyTestResult (someLL.GetLength () == kBigSize);
        someLL.SetAt (55, 55);                      //  someLL [55] = 55;
        VerifyTestResult (someLL.GetAt (55) == 55); //  VerifyTestResult(someArray [55] == 55);
        VerifyTestResult (someLL.GetAt (55) != 56); //  VerifyTestResult(someArray [55] != 56);
        {
            size_t i = 1;
            size_t cur;
            for (PatchingDataStructures::LinkedList<size_t>::ForwardIterator it (kUnknownIteratorOwnerID, &someLL); it.More (&cur, true); i++) {
                if (i == 100) {
                    someLL.AddAfter (it, 1);
                    break;
                }
            }
        } //   someLL.InsertAt(1, 100);

        VerifyTestResult (someLL.GetLength () == kBigSize + 1);
        VerifyTestResult (someLL.GetAt (100) == 1); //  VerifyTestResult(someArray [100] == 1);

        someLL.SetAt (someLL.GetAt (100) + 5, 101);

        VerifyTestResult (someLL.GetAt (101) == 6);
        someLL.RemoveFirst ();
        VerifyTestResult (someLL.GetAt (100) == 6);
    }

    static void Test2 ()
    {
        PatchingDataStructures::LinkedList<SimpleClass> someLL;
        const size_t                                    kBigSize = 1000;

        VerifyTestResult (someLL.GetLength () == 0);

        Assert (kBigSize > 10);
        VerifyTestResult (someLL.GetLength () == 0);
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

        VerifyTestResult (someLL.GetLength () == kBigSize);

        someLL.SetAt (55, 55); //  someLL [55] = 55;
        VerifyTestResult (someLL.GetAt (55) == 55);
        VerifyTestResult (not(someLL.GetAt (55) == 56));

        someLL.RemoveAll ();
        VerifyTestResult (someLL.GetLength () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            VerifyTestResult (someLL.Lookup (i) == nullptr);
            someLL.Prepend (i);
            VerifyTestResult (someLL.GetFirst () == i);
            VerifyTestResult (someLL.Lookup (i) != nullptr);
        }
        for (size_t i = 1; i <= kBigSize; ++i) {
            VerifyTestResult (someLL.GetFirst () == i);
            someLL.RemoveFirst ();
            VerifyTestResult (someLL.Lookup (i) == nullptr);
        }
        VerifyTestResult (someLL.GetLength () == 0);

        for (size_t i = kBigSize; i >= 1; --i) {
            someLL.Prepend (i);
        }
        for (size_t i = kBigSize; i >= 1; --i) {
            //cerr << "i, getat(i-1) = " << i << ", " << someLL.GetAt (i-1).GetValue () << endl;
            VerifyTestResult (someLL.GetAt (i - 1) == i);
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1 ();
        Test2 ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
