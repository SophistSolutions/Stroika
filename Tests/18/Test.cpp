/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedTally
//      STATUS  very minimal/incomplete
#include    "Stroika/Foundation/StroikaPreComp.h"


/// WRONG - we dont have SortedTally Yet - but when we do!!!
#include    "Stroika/Foundation/Containers/SortedTally.h"
//#include    "Stroika/Foundation/Containers/Concrete/Tally_Array.h"
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


//using   Concrete::Tally_Array;


namespace   {

    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::TallyTests;

        auto testFunc1 = [] (const Tally<size_t>& s) {
            ///FIX TO DO SORT CHEKCING
        };
        auto testFunc2 = [] (const Tally<SimpleClass>& s) {
            ///FIX TO DO SORT CHEKCING
        };

//        All_For_Type<Tally_Array<size_t>> (testFunc1);
//        All_For_Type<Tally_Array<SimpleClass>> (testFunc2);

#if 0
        // NOT WORKING YET - 2013-05-22
        All_For_Type<SortedTally<size_t>> (testFunc1);
        All_For_Type<SortedTally<SimpleClass>> (testFunc2);
#endif
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
