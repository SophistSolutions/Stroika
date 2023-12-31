/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Frameworks::WebServer
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"


using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using DataStructures::Array;

#if qHasFeature_GoogleTest

namespace {
    GTEST_TEST (Frameworks_WebServer, all)
    {
      
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
