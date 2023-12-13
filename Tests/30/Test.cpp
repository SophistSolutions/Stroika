/*
* Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
*/
//  TEST    Foundation::Containers::STL
#include "Stroika/Foundation/StroikaPreComp.h"

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

namespace {
    namespace Test01_Concatenate_ {
        void DoTest ()
        {
            vector<int> v1 = {1, 2, 3};
            VerifyTestResult ((STL::Concatenate (v1) == v1));
            VerifyTestResult ((STL::Concatenate (v1, v1) == vector<int>{1, 2, 3, 1, 2, 3}));
            VerifyTestResult ((STL::Concatenate (v1, v1, v1) == vector<int>{1, 2, 3, 1, 2, 3, 1, 2, 3}));
        }
    }
}

namespace {
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        Test01_Concatenate_::DoTest ();

        VerifyTestResult (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    return Stroika::Frameworks::Test::PrintPassOrFail (DoRegressionTests_);
#endif
}
