/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/
//  TEST    Foundation::Containers::Collection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Collection.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Collection_Array;
using Concrete::Collection_LinkedList;
using Concrete::Collection_stdforward_list;

namespace {
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::RunTests_"};
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> (
            []() { return CONCRETE_CONTAINER (); },
            []([[maybe_unused]] const typename CONCRETE_CONTAINER::ArchetypeContainerType& c) { Lambda_Arg_Unused_BWA (c); });
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx{L"{}::ExampleCTORS_Test_2_"};
            // From Collection<> CTOR docs
            Sequence<int> s;
            vector<int>   v;

            Collection<int> c1 = {1, 2, 3};
            Collection<int> c2 = c1;
            Collection<int> c3{c1};
            Collection<int> c4{c1.begin (), c1.end ()};
            Collection<int> c5{s};
            Collection<int> c6{v};
            Collection<int> c7{v.begin (), v.end ()};
            Collection<int> c8{move (c1)};
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        RunTests_<Collection<size_t>> ();
        RunTests_<Collection<SimpleClass>> ();
        RunTests_<Collection<SimpleClassWithoutComparisonOperators>> ();

        RunTests_<Collection_LinkedList<size_t>> ();
        RunTests_<Collection_LinkedList<SimpleClass>> ();
        RunTests_<Collection_LinkedList<SimpleClassWithoutComparisonOperators>> ();

        RunTests_<Collection_Array<size_t>> ();
        RunTests_<Collection_Array<SimpleClass>> ();
        RunTests_<Collection_Array<SimpleClassWithoutComparisonOperators>> ();

        RunTests_<Collection_stdforward_list<size_t>> ();
        RunTests_<Collection_stdforward_list<SimpleClass>> ();
        RunTests_<Collection_stdforward_list<SimpleClassWithoutComparisonOperators>> ();

        ExampleCTORS_Test_2_::DoTest ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
