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



namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::MappingTests;

        auto testFunc1 = [] (const Mapping<size_t, size_t>& m) {
        };
        auto testFunc2 = [] (const Mapping<SimpleClass, SimpleClass>& m) {
        };

        SimpleMappingTest_All_For_Type<Mapping<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<Mapping<SimpleClass, SimpleClass>> (testFunc2);

        SimpleMappingTest_All_For_Type<Mapping_Array<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<Mapping_Array<SimpleClass, SimpleClass>> (testFunc2);

        SimpleMappingTest_All_For_Type<Mapping_LinkedList<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<Mapping_LinkedList<SimpleClass, SimpleClass>> (testFunc2);

        SimpleMappingTest_All_For_Type<Mapping_stdmap<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<Mapping_stdmap<SimpleClass, SimpleClass>> (testFunc2);
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

