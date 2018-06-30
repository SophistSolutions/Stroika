/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedSet
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "../TestCommon/CommonTests_Set.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Common;

using Concrete::SortedSet_stdset;

namespace {
    template <typename CONCRETE_CONTAINER, typename INORDER_COMPARER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (const INORDER_COMPARER& inorderComparer, CONCRETE_CONTAINER_FACTORY factory)
    {
        typedef typename CONCRETE_CONTAINER::value_type T;
        auto                                            testFunc = [&](const SortedSet<T>& s) {
            // verify in sorted order
            Memory::Optional<T> last;
            for (T i : s) {
                if (last.has_value ()) {
                    VerifyTestResult (inorderComparer (*last, i));
                }
                last = i;
            }
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, SortedSet<T>> (factory, testFunc);
    }
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        RunTests_<CONCRETE_CONTAINER> (std::less<typename CONCRETE_CONTAINER::value_type>{}, []() { return CONCRETE_CONTAINER (); });
    }
}

namespace {
    namespace Test2_InitalizeCTORs_ {
        void DoRun ()
        {
            {
                SortedSet<int> tmp{1, 3};
                VerifyTestResult (tmp.size () == 2);
                VerifyTestResult (tmp.Contains (1));
                VerifyTestResult (not tmp.Contains (2));
                VerifyTestResult (tmp.Contains (3));
            }
            {
                SortedSet<int> tmp{1, 3, 4, 5, 7};
                VerifyTestResult (tmp.size () == 5);
                VerifyTestResult (tmp.Contains (1));
                VerifyTestResult (not tmp.Contains (2));
                VerifyTestResult (tmp.Contains (3));
                VerifyTestResult (tmp.Contains (7));
            }
            {
                Set<int>       t1{1, 3, 4, 5, 7};
                SortedSet<int> tmp = SortedSet<int> (t1.begin (), t1.end ());
                //SortedSet<int> tmp  {t1.begin (), t1.end () };
                VerifyTestResult (tmp.size () == 5);
                VerifyTestResult (tmp.Contains (1));
                VerifyTestResult (not tmp.Contains (2));
                VerifyTestResult (tmp.Contains (3));
                VerifyTestResult (tmp.Contains (7));
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct MySimpleClassWithoutComparisonOperators_LESS_ : ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };
        RunTests_<SortedSet<size_t>> ();
        RunTests_<SortedSet<SimpleClass>> ();
        RunTests_<SortedSet<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, []() { return SortedSet<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{}); });

        RunTests_<SortedSet_stdset<size_t>> ();
        RunTests_<SortedSet_stdset<SimpleClass>> ();
        RunTests_<SortedSet_stdset<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, []() { return SortedSet_stdset<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{}); });

        Test2_InitalizeCTORs_::DoRun ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
