/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//  Foundation::Containers::Private::STLContainerWrapper
///             STATUS - ALL WRONG - REWRITE!!!!!
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

//#include    "Stroika/Foundation/Containers/Private/PatchingDataStructures/DoublyLinkedList.h"

#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
//using   namespace   Stroika::Foundation::Containers;




namespace   {
    static  void    Test1()
    {
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1();
    }
}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



