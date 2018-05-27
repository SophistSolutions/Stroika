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
    template <typename CONCRETE_CONTAINER, typename INORDER_COMPARER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (const INORDER_COMPARER& inorderComparer, CONCRETE_CONTAINER_FACTORY factory)
    {
        typedef typename CONCRETE_CONTAINER::value_type T;
        auto                                            testFunc = [&](const SortedCollection<T>& s) {
            // verify in sorted order
            Memory::Optional<T> last;
            for (T i : s) {
                if (last.IsPresent ()) {
                    VerifyTestResult (inorderComparer (*last, i));
                }
                last = i;
            }
        };
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> (factory, testFunc);
    }
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        RunTests_<CONCRETE_CONTAINER> (std::less<typename CONCRETE_CONTAINER::value_type>{}, []() { return CONCRETE_CONTAINER (); });
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
        struct MySimpleClassWithoutComparisonOperators_LESS_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        RunTests_<SortedCollection<size_t>> ();
        RunTests_<SortedCollection<SimpleClass>> ();
        RunTests_<SortedCollection<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, []() { return SortedCollection<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{}); });

        RunTests_<SortedCollection_LinkedList<size_t>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClass>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, []() { return SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{}); });

        TestOverwriteContainerWhileIteratorRunning_ ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
