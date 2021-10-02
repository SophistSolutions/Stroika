/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Concrete/KeyedCollection_LinkedList.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
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
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return KeyedCollection<T1, int, T1_Traits>{}; },
                [] (auto) {});
            CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
                [] () { return Concrete::KeyedCollection_LinkedList<T1, int>{[] (T1 e) { return e.key; }}; },
                [] (auto) {});
        }
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
