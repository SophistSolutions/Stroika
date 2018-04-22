/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Set
//      STATUS  PRELIMINARY
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Set_stdset.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "../TestCommon/CommonTests_Set.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Set_LinkedList;
using Concrete::Set_stdset;

namespace {
    template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
    void DoTestForConcreteContainer_ (CONCRETE_CONTAINER_FACTORY factory)
    {
        using T                  = typename CONCRETE_CONTAINER::value_type;
        auto extraChecksFunction = [](const Set<T>& s) {
            // only work todo on sorted sets
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, Set<T>> (factory, extraChecksFunction);
    }
    template <typename CONCRETE_CONTAINER>
    void DoTestForConcreteContainer_ ()
    {
        using T = typename CONCRETE_CONTAINER::value_type;
        DoTestForConcreteContainer_<CONCRETE_CONTAINER> ([]() { return CONCRETE_CONTAINER{}; });
    }
}

namespace {
    namespace ExampleCTORS_Test_2_ {
        void DoTest_examples_from_docs_ ()
        {
            // From Set<> CTOR docs
            Collection<int>  c;
            std::vector<int> v;

            Set<int> s1 = {1, 2, 3};
            Set<int> s2 = s1;
            Set<int> s3{s1};
            Set<int> s4{s1.begin (), s1.end ()};
            Set<int> s5{c};
            Set<int> s6{v};
            Set<int> s7{v.begin (), v.end ()};
            Set<int> s8{move (s1)};
            Set<int> s9{1, 2, 3};
            VerifyTestResult (s9.size () == 3);
            Set<int> s10{Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }), c};
        }

        void TEstCTORFromOtherContainer_ ()
        {
            using Characters::Character;
            using Characters::String;
            Sequence<String> x;
            Set<String>      cx{x};
            Set<String>      cx1  = Set<String> (x);
            Set<String>      tmp5 = Set<String> (String ().Trim ().Tokenize (Set<Character>{';', ' '}));
            auto             cccc = String ().Trim ().Tokenize ();
            Set<String>      tmp2 = Set<String> (cccc);
            Set<String>      tmp3 = Set<String> (Sequence<String>{});
            Set<String>      tmp4 = Set<String> (String ().Trim ().Tokenize ());
        }
        void DoTest ()
        {
            DoTest_examples_from_docs_ ();
            TEstCTORFromOtherContainer_ ();
        }
    }
}

namespace {
    namespace Where_Test_3_ {
        void DoAll ()
        {
            Set<int> s{1, 2, 3, 4, 5};
            VerifyTestResult ((s.Where ([](int i) { return Math::IsPrime (i); }) == Set<int>{2, 3, 5}));
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct MySimpleClassWithoutComparisonOperators_EQUAL_TO_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () == rhs.GetValue ();
            }
        };
        struct MySimpleClassWithoutComparisonOperators_LESS_ : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder> {
            bool operator() (const SimpleClassWithoutComparisonOperators& lhs, const SimpleClassWithoutComparisonOperators& rhs) const
            {
                return lhs.GetValue () < rhs.GetValue ();
            }
        };

        DoTestForConcreteContainer_<Set<size_t>> ();
        DoTestForConcreteContainer_<Set<SimpleClass>> ();
        DoTestForConcreteContainer_<Set<SimpleClassWithoutComparisonOperators>> ([]() { return Set<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_EQUAL_TO_ ()); });

        DoTestForConcreteContainer_<Set_LinkedList<size_t>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClass>> ();
        DoTestForConcreteContainer_<Set_LinkedList<SimpleClassWithoutComparisonOperators>> ([]() { return Set_LinkedList<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_EQUAL_TO_ ()); });

        DoTestForConcreteContainer_<Set_stdset<size_t>> ();
        DoTestForConcreteContainer_<Set_stdset<SimpleClass>> ();
        DoTestForConcreteContainer_<Set_stdset<SimpleClassWithoutComparisonOperators>> ([]() { return Set_stdset<SimpleClassWithoutComparisonOperators> (MySimpleClassWithoutComparisonOperators_LESS_ ()); });

        ExampleCTORS_Test_2_::DoTest ();

        Where_Test_3_::DoAll ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
