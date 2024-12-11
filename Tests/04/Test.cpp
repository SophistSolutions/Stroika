/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Common
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Endian.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Common/Version.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika-Current-Version.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qStroika_HasComponent_googletest

namespace {
    GTEST_TEST (OBSOLETEDELEMENE, all)
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
