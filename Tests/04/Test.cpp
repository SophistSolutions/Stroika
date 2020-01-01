/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::Array
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/DataStructures/Array.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using DataStructures::Array;

namespace {
    static void Test1 ()
    {
        Array<size_t> someArray;

        const size_t kBigSize = 1001;

        VerifyTestResult (someArray.GetLength () == 0);

        someArray.SetLength (kBigSize, 0);
        someArray.RemoveAll ();
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        VerifyTestResult (someArray.GetLength () == kBigSize);
        someArray[55] = 55;
        VerifyTestResult (someArray[55] == 55);
        VerifyTestResult (someArray[55] != 56);

        someArray.InsertAt (100, 1);
        VerifyTestResult (someArray.GetLength () == kBigSize + 1);
        VerifyTestResult (someArray[100] == 1);

        someArray[101] = someArray[100] + 10;
        VerifyTestResult (someArray[101] == 11);
        someArray.RemoveAt (0);
        VerifyTestResult (someArray[100] == 11);
        someArray.RemoveAt (1);

        VerifyTestResult (someArray[99] == 11);
    }

    static void Test2 ()
    {
        {
            Array<SimpleClass> someArray;
            someArray.InsertAt (0, 100);
            // for (size_t i = 0; i < someArray.GetLength (); ++i) { cerr << "someArray[" << i << "] = " << someArray[i].GetValue () << endl; }
            someArray.RemoveAt (0);
            someArray.InsertAt (0, 2);
            someArray.InsertAt (0, 1);
            someArray.InsertAt (0, 3);
            someArray.InsertAt (someArray.GetLength (), 4);
            someArray.RemoveAt (someArray.GetLength () - 1);
            someArray.RemoveAt (1);
        }

        Array<SimpleClass> someArray;

        const size_t kBigSize = 1001;

        VerifyTestResult (someArray.GetLength () == 0);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (0, 0);
        someArray.SetLength (kBigSize, 0);
        someArray.SetLength (10, 0);
        someArray.SetLength (kBigSize, 0);

        VerifyTestResult (someArray.GetLength () == kBigSize);
        someArray[55] = 55;
        VerifyTestResult (someArray[55] == 55);
        VerifyTestResult (not(someArray[55] == 56));

        someArray.RemoveAt (100);

        while (someArray.GetLength () > 0) {
            someArray.RemoveAt (0);
        }
        while (someArray.GetLength () < kBigSize) {
            someArray.InsertAt (someArray.GetLength (), 1);
        }

        someArray.InsertAt (100, 1);
        VerifyTestResult (someArray.GetLength () == kBigSize + 1);
        VerifyTestResult (someArray[100] == 1);
        someArray[101] = 1 + someArray[100].GetValue ();
        someArray.RemoveAt (1);
        VerifyTestResult (someArray[100].GetValue () == 2);
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
