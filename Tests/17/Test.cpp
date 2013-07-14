/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::SortedSet
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/SortedSet.h"
#include    "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Memory/Optional.h"

#include    "../TestCommon/CommonTests_Set.h"
#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::SortedSet_stdset;



namespace   {
    template    <typename T>
    void    Check_ (const SortedSet<T>& s)
    {
        // verify in sorted order
        Memory::Optional<T> last;
        for (T i : s) {
            if (last.IsPresent ()) {
                VerifyTestResult (typename SortedSet<T>::TraitsType::WellOrderCompareFunctionType::Compare (*last, i) <= 0);
                //VerifyTestResult (*last < i or (*last == i));
            }
            last = i;
        }
    }

    void    DoRegressionTests_ ()
    {
        using namespace CommonTests::SetTests;

#if 0
        typedef typename CONCRETE_CONTAINER::ElementType    T;
        typedef typename CONCRETE_CONTAINER::TraitsType     TraitsType;
#endif

        auto testFunc1 = [] (const SortedSet<size_t>& s) {
            Check_<size_t> (s);
        };
        auto testFunc2 = [] (const SortedSet<SimpleClass>& s) {
            Check_<SimpleClass> (s);
        };
        Test_All_For_Type<SortedSet_stdset<size_t>, SortedSet<size_t>> (testFunc1);
        Test_All_For_Type<SortedSet_stdset<SimpleClass>, SortedSet<SimpleClass>> (testFunc2);

        Test_All_For_Type<SortedSet<size_t>, SortedSet<size_t>> (testFunc1);
        Test_All_For_Type<SortedSet<SimpleClass>, SortedSet<SimpleClass>> (testFunc2);
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

