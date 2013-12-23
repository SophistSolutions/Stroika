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
#include    "Stroika/Foundation/Memory/Optional.h"

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



namespace {
    template    <typename   CONCRETE_CONTAINER>
    void    DoTestForConcreteContainer_ ()
    {
        typedef typename CONCRETE_CONTAINER::TraitsType             TraitsType;
        typedef typename CONCRETE_CONTAINER::TallyOfElementType     TallyOfElementType;
        auto extraChecksFunction = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & t) {
            // verify in sorted order
            Memory::Optional<TallyOfElementType> last;
            for (TallyEntry<TallyOfElementType> i : t) {
                if (last.IsPresent ()) {
                    VerifyTestResult (TraitsType::WellOrderCompareFunctionType::Compare (*last, i.fItem) <= 0);
                }
                last = i.fItem;
            }
        };
        CommonTests::TallyTests::All_For_Type<CONCRETE_CONTAINER> (extraChecksFunction);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        struct  MySimpleClassWithoutComparisonOperators_ComparerWithComparer_ {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2)
            {
                return v1.GetValue () == v2.GetValue ();
            }
            static  int    Compare (ElementType v1, ElementType v2)
            {
                return v1.GetValue () - v2.GetValue ();
            }
        };
        typedef SortedTally_DefaultTraits <
        SimpleClassWithoutComparisonOperators,
        MySimpleClassWithoutComparisonOperators_ComparerWithComparer_
        >   SimpleClassWithoutComparisonOperators_SortedTallyTRAITS;

        DoTestForConcreteContainer_<SortedTally<size_t>> ();
        DoTestForConcreteContainer_<SortedTally<SimpleClass>> ();
        DoTestForConcreteContainer_<SortedTally<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedTallyTRAITS>> ();

        DoTestForConcreteContainer_<SortedTally_stdmap<size_t>> ();
        DoTestForConcreteContainer_<SortedTally_stdmap<SimpleClass>> ();
        DoTestForConcreteContainer_<SortedTally_stdmap<SimpleClassWithoutComparisonOperators, SimpleClassWithoutComparisonOperators_SortedTallyTRAITS>> ();
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
