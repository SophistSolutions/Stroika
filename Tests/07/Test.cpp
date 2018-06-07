/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Private::DataStructures::STLContainerWrapper
///             STATUS - ALL WRONG - REWRITE!!!!!
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

//#include    "Stroika/Foundation/Containers/Private/PatchingDataStructures/DoublyLinkedList.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
//using   namespace   Stroika::Foundation::Containers;

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

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
