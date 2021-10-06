/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedKeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/SortedKeyedCollection.h"

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
        using T1        = CommonTests::KeyedCollectionTests::Test1_Basics_::T1;
        using T1_Traits = CommonTests::KeyedCollectionTests::Test1_Basics_::T1_Traits;
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int>{[] (T1 e) { return e.key; }}; },
            [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{[] (T1 e) { return e.key; }}; },
            [] (auto) {});
        CommonTests::KeyedCollectionTests::SimpleKeyedCollectionTest_TestBasics (
            [] () { return SortedKeyedCollection<T1, int, T1_Traits>{}; },
            [] (auto) {});
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
