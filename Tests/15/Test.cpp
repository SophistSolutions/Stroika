/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Containers::KeyedCollection
//      STATUS  Alpha-Late
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#if 0
#include "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#endif
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Mapping.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

namespace {
    void DoRegressionTests_ ()
    {
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
