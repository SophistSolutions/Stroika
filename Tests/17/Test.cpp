/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Set.h"
#if 0
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#endif
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Mapping.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    namespace Test1_ {
        struct T1 {
            int key;
            int value;
        };
        struct T1_Key_Extractor {
            int operator() (const T1& t) const { return t.key; };
        };
        using T1_Traits = KeyedCollection_DefaultTraits<T1, int, equal_to<int>, T1_Key_Extractor>;
        void DoIt ()
        {
            auto test = [] (auto coll) {
                VerifyTestResult (coll.empty ());
                coll.Add (T1{1, 101});
                VerifyTestResult (coll.GetLength () == 1);
                VerifyTestResult (coll.Lookup (1)->value == 101);
                coll.Add (T1{1, 201});
                VerifyTestResult (coll.GetLength () == 1);
                VerifyTestResult (coll.Contains (1));
                VerifyTestResult (coll.Lookup (1)->value == 201);
                VerifyTestResult (not coll.Contains (2));
                auto prevValue = coll;
                VerifyTestResult (prevValue == coll);
                coll.Add (T1{2, 102});
                VerifyTestResult (prevValue != coll);
                VerifyTestResult (coll.Contains (2));
                VerifyTestResult (coll.GetLength () == 2);
                VerifyTestResult ((coll.Keys () == Set<int>{1, 2}));
                coll.Remove (99);
                VerifyTestResult (coll.GetLength () == 2);
                coll.Remove (1);
                VerifyTestResult (coll.GetLength () == 1);
            };
            KeyedCollection<T1, int> l1{[] (T1 e) { return e.key; }};
            test (l1);
            KeyedCollection<T1, int, T1_Traits> l2{[] (T1 e) { return e.key; }};
            test (l2);
            KeyedCollection<T1, int, T1_Traits> l3;
            test (l3);
        }
    }

    void DoRegressionTests_ ()
    {
        Test1_::DoIt ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
