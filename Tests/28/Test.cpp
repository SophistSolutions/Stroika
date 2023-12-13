/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Containers::SortedSet
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"
#include "Stroika/Foundation/Containers/SortedSet.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

#include "../TestCommon/CommonTests_Set.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Common;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::SimpleClass;
using Test::ArchtypeClasses::SimpleClassWithoutComparisonOperators;

using Concrete::SortedSet_stdset;

namespace {
    template <typename CONCRETE_CONTAINER, typename INORDER_COMPARER, typename CONCRETE_CONTAINER_FACTORY>
    void RunTests_ (const INORDER_COMPARER& inorderComparer, CONCRETE_CONTAINER_FACTORY factory)
    {
        typedef typename CONCRETE_CONTAINER::value_type T;
        auto                                            testFunc = [&] (const SortedSet<T>& s) {
            // verify in sorted order
            optional<T> last;
            for (T i : s) {
                if (last.has_value ()) {
                    EXPECT_TRUE (inorderComparer (*last, i));
                }
                last = i;
            }
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, SortedSet<T>> (factory, testFunc);
    }
    template <typename CONCRETE_CONTAINER>
    void RunTests_ ()
    {
        RunTests_<CONCRETE_CONTAINER> (less<typename CONCRETE_CONTAINER::value_type>{}, [] () { return CONCRETE_CONTAINER (); });
    }
}

namespace {
    namespace Test2_InitalizeCTORs_ {
        void DoRun ()
        {
            {
                SortedSet<int> tmp{1, 3};
                EXPECT_TRUE (tmp.size () == 2);
                EXPECT_TRUE (tmp.Contains (1));
                EXPECT_TRUE (not tmp.Contains (2));
                EXPECT_TRUE (tmp.Contains (3));
            }
            {
                SortedSet<int> tmp{1, 3, 4, 5, 7};
                EXPECT_TRUE (tmp.size () == 5);
                EXPECT_TRUE (tmp.Contains (1));
                EXPECT_TRUE (not tmp.Contains (2));
                EXPECT_TRUE (tmp.Contains (3));
                EXPECT_TRUE (tmp.Contains (7));
            }
            {
                Set<int>       t1{1, 3, 4, 5, 7};
                SortedSet<int> tmp = SortedSet<int>{t1.begin (), t1.end ()};
                //SortedSet<int> tmp  {t1.begin (), t1.end () };
                EXPECT_TRUE (tmp.size () == 5);
                EXPECT_TRUE (tmp.Contains (1));
                EXPECT_TRUE (not tmp.Contains (2));
                EXPECT_TRUE (tmp.Contains (3));
                EXPECT_TRUE (tmp.Contains (7));
            }
        }
    }
}

namespace {
    namespace Test3_ExplicitSortFunction_ {
        void DoRun ()
        {
            {
                using Characters::String;
                SortedSet<String> tmp{L"a", L"b", L"A"};
                EXPECT_TRUE (tmp.size () == 3);
                EXPECT_TRUE (tmp.Contains (L"A"));
                EXPECT_TRUE (not tmp.Contains (L"B"));
            }
            {
                using Characters::String;
                SortedSet<String> tmp{String::LessComparer{Characters::eCaseInsensitive}, {"a", L"b", "A"sv}};
                EXPECT_TRUE (tmp.size () == 2);
                EXPECT_TRUE (tmp.Contains (L"A"));
                EXPECT_TRUE (tmp.Contains (L"B"));
            }
        }
    }
}

namespace {
#if qHasFeature_GoogleTest
    GTEST_TEST (Foundation_Caching, all)
#else
    void DoRegressionTests_ ()
#endif
    {
        using namespace CommonTests::SetTests;

        struct MySimpleClassWithoutComparisonOperators_LESS_ : ComparisonRelationDeclarationBase<ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };
        RunTests_<SortedSet<size_t>> ();
        RunTests_<SortedSet<SimpleClass>> ();
        RunTests_<SortedSet<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, [] () {
            return SortedSet<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{});
        });

        RunTests_<SortedSet_stdset<size_t>> ();
        RunTests_<SortedSet_stdset<SimpleClass>> ();
        RunTests_<SortedSet_stdset<SimpleClassWithoutComparisonOperators>> (MySimpleClassWithoutComparisonOperators_LESS_{}, [] () {
            return SortedSet_stdset<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_{});
        });

        Test2_InitalizeCTORs_::DoRun ();
        Test3_ExplicitSortFunction_::DoRun ();

        EXPECT_TRUE (SimpleClass::GetTotalLiveCount () == 0 and SimpleClassWithoutComparisonOperators::GetTotalLiveCount () == 0); // simple portable leak check
    }
}

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature" << endl;
#endif
}
