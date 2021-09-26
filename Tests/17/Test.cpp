/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#if 0
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#endif
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_KeyedCollection.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void DoRegressionTests_ ()
    {
        {
            using T1        = CommonTests::KeyedCollectionTests::Test1_Basics_::T1;
            using T1_Traits = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Traits;
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; },
                [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int, T1_Traits>{[] (T1 e) { return e.key; }}; },
                [] (auto) {});

            static_assert (KeyedCollection_IsKeyExctractor<T1, int, T1_Traits::DefaultKeyExtractor> ());
#if 0
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int, T1_Traits>{}; },
                [] (auto) {});
#endif
        }
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
