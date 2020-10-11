/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedAssociation
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;

namespace {
    void Test1_ ()
    {
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
