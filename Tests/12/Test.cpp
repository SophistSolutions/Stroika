/*
* Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
*/
//  TEST    Foundation::Containers::Collection
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Collection.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Collection.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Collection_stdforward_list.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::Collection_Array;
using   Concrete::Collection_LinkedList;
using   Concrete::Collection_stdforward_list;


namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        CommonTests::CollectionTests::SimpleCollectionTest_Generic<CONCRETE_CONTAINER> ([](const typename CONCRETE_CONTAINER::ArchetypeContainerType & c) {
        });
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithEquals_ {
            using   ElementType =   SimpleClassWithoutComparisonOperators;
            static  bool    Equals (ElementType v1, ElementType v2)
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
    }

}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

