/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Set
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Set.h"
#include    "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Set.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Set_LinkedList;



namespace   {
    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

        auto testFunc1 = [] (const Set<size_t>& s) {
        };
        auto testFunc2 = [] (const Set<SimpleClass>& s) {
        };
        Test_All_For_Type<Set_LinkedList<size_t>> (testFunc1);
        Test_All_For_Type<Set_LinkedList<SimpleClass>> (testFunc2);

        Test_All_For_Type<Set<size_t>> (testFunc1);
        Test_All_For_Type<Set<SimpleClass>> (testFunc2);
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

