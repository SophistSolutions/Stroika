/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
//  TEST    Foundation::Containers::DataStructures::HashTable
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/DataStructures/HashTable.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers::DataStructures;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_HashTable, Test1)
    {
        DataStructures::HashTable<int> ht1;
        ht1.Add (1);
        EXPECT_EQ (ht1.size (), 1u);

        EXPECT_EQ (ht1.Lookup (2), nullopt);
        EXPECT_EQ (ht1.Lookup (1), 1);
    }

    GTEST_TEST (Foundation_Containers_DataStructures_HashTable, Test2)
    {
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_DataStructures_HashTable, ToString)
    {
        Debug::TraceContextBumper ctx{"ToString"};
        // DataStructures::LinkedList<int> t;
        //    t.push_back (1);
        //  DbgTrace ("t={}"_f, t); // test using ranges support
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
