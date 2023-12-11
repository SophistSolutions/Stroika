/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

namespace {
    static void Test1 ()
    {
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1 ();
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
    return Test::PrintPassOrFail (DoRegressionTests_);
}
