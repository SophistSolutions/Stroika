/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
//  TEST    Foundation::Containers::STL
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Containers/STL/Utilities.h"
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

#if qHasFeature_GoogleTest
namespace {
    namespace Test01_Concatenate_ {
        void DoTest ()
        {
            vector<int> v1 = {1, 2, 3};
            EXPECT_TRUE ((STL::Concatenate (v1) == v1));
            EXPECT_TRUE ((STL::Concatenate (v1, v1) == vector<int>{1, 2, 3, 1, 2, 3}));
            EXPECT_TRUE ((STL::Concatenate (v1, v1, v1) == vector<int>{1, 2, 3, 1, 2, 3, 1, 2, 3}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Test01_Concatenate_::DoTest ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
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
