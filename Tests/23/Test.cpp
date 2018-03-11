/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
//  TEST    Foundation::Containers::SortedCollection
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Containers/SortedCollection.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "../TestCommon/CommonTests_Collection.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

#include "Stroika/Foundation/Containers/Concrete/SortedCollection_LinkedList.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::SortedCollection_LinkedList;

using Memory::Optional;

namespace {
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        using T          = typename CONCRETE_CONTAINER::value_type;
        using TraitsType = typename CONCRETE_CONTAINER::TraitsType;
        auto testFunc    = [](const typename CONCRETE_CONTAINER::ArchetypeContainerType& s) {
            // verify in sorted order
            Optional<T> last;
            for (T i : s) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::WellOrderCompareFunctionType::Compare (*last, i) <= 0);
                }
                last = i;
            }
        };
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> (testFunc);
    }
}

namespace {
    void TestOverwriteContainerWhileIteratorRunning_ ()
    {
        SortedCollection<int>    a = {1, 2, 3};
        Traversal::Iterator<int> i = a.begin ();
#if qStroika_Foundation_Traveral_OverwriteContainerWhileIteratorRunning_Buggy
        i.clear ();
#endif
        a = SortedCollection<int>{3, 4, 5};
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        struct MySimpleClassWithoutComparisonOperators_Comparer_ {
            using value_type = SimpleClassWithoutComparisonOperators;
            static bool Equals (value_type v1, value_type v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static int Compare (value_type v1, value_type v2)
            {
                return Common::CompareNormalizer (v1.GetValue (), v2.GetValue ());
            }
            struct NEW_EQUALS_COMPARER {
                bool operator() (value_type v1, value_type v2) const
                {
                    return Equals (v1, v2);
                }
            };
        };
        using SimpleClassWithoutComparisonOperators_CollectionTRAITS = DefaultTraits::SortedCollection<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_Comparer_>;

        RunTests_<SortedCollection<size_t>> ();
        RunTests_<SortedCollection<SimpleClass>> ();
        RunTests_<SortedCollection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_CollectionTRAITS>> ();

        RunTests_<SortedCollection_LinkedList<size_t>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClass>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_CollectionTRAITS>> ();

        TestOverwriteContainerWhileIteratorRunning_ ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
