/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedTally
//      STATUS  very minimal/incomplete
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/SortedTally.h"
#include    "Stroika/Foundation/Containers/Concrete/SortedTally_stdmap.h"
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


using   Concrete::SortedTally_stdmap;


namespace   {

    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::TallyTests;

        auto testFunc1 = [] (const SortedTally<size_t>& s) {
            ///FIX TO DO SORT CHEKCING
        };
        auto testFunc2 = [] (const SortedTally<SimpleClass>& s) {
            ///FIX TO DO SORT CHEKCING
        };

        All_For_Type<SortedTally<size_t>> (testFunc1);
        All_For_Type<SortedTally<SimpleClass>> (testFunc2);

        All_For_Type<SortedTally_stdmap<size_t>> (testFunc1);
        All_For_Type<SortedTally_stdmap<SimpleClass>> (testFunc2);
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
