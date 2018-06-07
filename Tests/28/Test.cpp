/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
//  TEST    Foundation::Containers::STL
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/STL/Utilities.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

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

    void DoRegressionTests_ ()
    {
        Test01_Concatenate_::DoTest ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
