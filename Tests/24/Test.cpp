/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedSet
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
//#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedSet.h"
#include    "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"

#include    "../TestCommon/CommonTests_Set.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::SortedSet_stdset;



namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        typedef typename CONCRETE_CONTAINER::ElementType    T;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto testFunc = [] (const SortedSet<T, TraitsType>& s) {
            // verify in sorted order
            Memory::Optional<T> last;
            for (T i : s) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::WellOrderCompareFunctionType::Compare (*last, i) <= 0);
                }
                last = i;
            }
        };
        CommonTests::SetTests::Test_All_For_Type<CONCRETE_CONTAINER, SortedSet<T, TraitsType>> (testFunc);
    }
}


namespace {
    namespace Test2_InitalizeCTORs_ {
        void    DoRun ()
        {
            {
                SortedSet<int> tmp {1, 3};
                VerifyTestResult (tmp.size () == 2);
                VerifyTestResult (tmp.Contains (1));
                VerifyTestResult (not tmp.Contains (2));
                VerifyTestResult (tmp.Contains (3));
            }
            {
                SortedSet<int> tmp {1, 3, 4, 5, 7};
                VerifyTestResult (tmp.size () == 5);
                VerifyTestResult (tmp.Contains (1));
                VerifyTestResult (not tmp.Contains (2));
                VerifyTestResult (tmp.Contains (3));
                VerifyTestResult (tmp.Contains (7));
            }
            {
                Set<int> t1 {1, 3, 4, 5, 7};
                SortedSet<int> tmp = SortedSet<int> (t1.begin (), t1.end () );
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


namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        struct  MySimpleClassWithoutComparisonOperators_CompareEquals_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };
        struct  MySimpleClassWithoutComparisonOperators_Comparer_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static  int    Compare (ElementType v1, ElementType v2)
            {
                return static_cast<int> (v1.GetValue ()) - static_cast<int> (v2.GetValue ());
            }
        };
        typedef DefaultTraits::SortedSet<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_CompareEquals_, MySimpleClassWithoutComparisonOperators_Comparer_>   SimpleClassWithoutComparisonOperators_SETTRAITS;

        RunTests_<SortedSet<size_t>> ();
        RunTests_<SortedSet<SimpleClass>> ();
        RunTests_<SortedSet<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETTRAITS>> ();

        RunTests_<SortedSet_stdset<size_t>> ();
        RunTests_<SortedSet_stdset<SimpleClass>> ();
        RunTests_<SortedSet_stdset<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SETTRAITS>> ();

        Test2_InitalizeCTORs_::DoRun();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
