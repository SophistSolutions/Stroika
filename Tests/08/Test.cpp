/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Containers::ExternallySynchronizedDataStructures::SkipList
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
//using   namespace   Stroika::Foundation::Containers::ExternallySynchronizedDataStructures;
//using   namespace   Stroika::Foundation::Containers::Private::PatchingDataStructures;



namespace   {

}


namespace   {

    void    DoRegressionTests_ ()
    {
    }
}



int		main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
