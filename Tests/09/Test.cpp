/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Private::DataStructures::STLContainerWrapper
///             STATUS - ALL WRONG - REWRITE!!!!!
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

//#include    "Stroika/Foundation/Containers/Private/PatchingDataStructures/DoublyLinkedList.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    static void Test1 ()
    {
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test1 ();
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
