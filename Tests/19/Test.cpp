/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::PriorityQueue
//      STATUS  TOTALLY WRONG - PLACEHOLDER
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Containers_PriorityQueue, all)
    {
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
