/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedTally
//      STATUS  very minimal/incomplete
#include    "Stroika/Foundation/StroikaPreComp.h"


/// WRONG - we dont have SortedTally Yet - but when we do!!!
#include    "Stroika/Foundation/Containers/Tally.h"
#include    "Stroika/Foundation/Containers/Concrete/Tally_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Tally_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestCommon/CommonTests_Tally.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


#include    "../TestCommon/CommonTests_Tally.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Tally_Array;
using   Concrete::Tally_LinkedList;


namespace   {

    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::TallyTests;

        SimpleTallyTest_All_For_Type<Tally_LinkedList<size_t>> ();
        SimpleTallyTest_All_For_Type<Tally_LinkedList<SimpleClass>> ();

        SimpleTallyTest_All_For_Type<Tally_Array<size_t>> ();
        SimpleTallyTest_All_For_Type<Tally_Array<SimpleClass>> ();

        SimpleTallyTest_All_For_Type<Tally<size_t>> ();
        SimpleTallyTest_All_For_Type<Tally<SimpleClass>> ();
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
