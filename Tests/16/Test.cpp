/*
* Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
*/
//  TEST    Foundation::Containers::SortedCollection
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedCollection.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"

#include    "../TestCommon/CommonTests_Collection.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/SortedCollection_LinkedList.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::SortedCollection_LinkedList;

using   Memory::Optional;


namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        typedef typename CONCRETE_CONTAINER::ElementType    T;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
        auto testFunc = [](const SortedCollection<T, TraitsType>& s) {
            // verify in sorted order
            Optional<T> last;
            for (T i : s) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::WellOrderCompareFunctionType::Compare (*last, i) <= 0);
                }
                last = i;
            }
        };
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER, SortedCollection<T, TraitsType>> (testFunc);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_Comparer_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static  int    Compare (ElementType v1, ElementType v2)
            {
                return v1.GetValue () - v2.GetValue ();
            }
        };
        typedef SortedCollection_DefaultTraits<SimpleClassWithoutComparisonOperators, MySimpleClassWithoutComparisonOperators_Comparer_>   SimpleClassWithoutComparisonOperators_CollectionTRAITS;

        RunTests_<SortedCollection<size_t>> ();
        RunTests_<SortedCollection<SimpleClass>> ();
        RunTests_<SortedCollection<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_CollectionTRAITS>> ();

        RunTests_<SortedCollection_LinkedList<size_t>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClass>> ();
        RunTests_<SortedCollection_LinkedList<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_CollectionTRAITS>> ();
    }

}




int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

