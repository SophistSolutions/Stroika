/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Private::DataStructures::STLContainerWrapper
///             STATUS - ALL WRONG - REWRITE!!!!!
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <vector>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_DataStructures_STLContainerWrapper, Test1)
    {
    }
}

namespace {
    GTEST_TEST (Foundation_DataStructures_STLContainerWrapper, ToString)
    {
        Debug::TraceContextBumper                        ctx{"ToString"};
        DataStructures::STLContainerWrapper<vector<int>> t;
        t.push_back (1);
        DbgTrace ("t={}"_f, t); // test using ranges support
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
