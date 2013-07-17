/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Mapping
//      STATUS  Alpha-Late
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestCommon/CommonTests_Mapping.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Mapping_Array;
using   Concrete::Mapping_LinkedList;
using   Concrete::Mapping_stdmap;


namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::KeyType    KeyType;
        typedef typename CONCRETE_CONTAINER::ValueType  ValueType;
        typedef typename CONCRETE_CONTAINER::TraitsType TraitsType;
        auto extraChecksFunction = [] (const Mapping<KeyType, ValueType, TraitsType>& m) {
            // only work todo on sorted mappings
        };
        CommonTests::MappingTests::SimpleMappingTest_All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}




namespace   {
    void    DoRegressionTests_ ()
    {
        DoTestForConcreteContainer_<Mapping<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping<SimpleClass, SimpleClass>> ();

        DoTestForConcreteContainer_<Mapping_Array<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_Array<SimpleClass, SimpleClass>> ();

        DoTestForConcreteContainer_<Mapping_LinkedList<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_LinkedList<SimpleClass, SimpleClass>> ();

        DoTestForConcreteContainer_<Mapping_stdmap<size_t, size_t>> ();
        DoTestForConcreteContainer_<Mapping_stdmap<SimpleClass, SimpleClass>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

