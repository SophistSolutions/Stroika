/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedMapping
//      STATUS  Alpha-Late
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedMapping.h"
#include    "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestCommon/CommonTests_Mapping.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::SortedMapping_stdmap;



namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::MappingTests;

        auto testFunc1 = [] (const SortedMapping<size_t, size_t>& m) {
            // fix to check for inorder sorting
        };
        auto testFunc2 = [] (const SortedMapping<SimpleClass, SimpleClass>& m) {
            // fix to check for inorder sorting
        };

        SimpleMappingTest_All_For_Type<SortedMapping<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<SortedMapping<SimpleClass, SimpleClass>> (testFunc2);

        SimpleMappingTest_All_For_Type<SortedMapping_stdmap<size_t, size_t>> (testFunc1);
        SimpleMappingTest_All_For_Type<SortedMapping_stdmap<SimpleClass, SimpleClass>> (testFunc2);
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

