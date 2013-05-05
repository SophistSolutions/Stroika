/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Mapping
//      STATUS  very minimal/incomplete
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Tally.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
//#include    "Stroika/Foundation/Containers/Concrete/Tally_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Mapping_Array;
//using   Concrete::Tally_LinkedList;



namespace {
    template <typename MappingOfKeyT>
    void    SimpleTest_1_ (MappingOfKeyT m)
    {
        MappingOfKeyT s;
    }
}


namespace {
    template <typename MappingOfKeyT>
    void    SimpleTest_2_ (MappingOfKeyT m)
    {
        m.Add (1, 2);
		VerifyTestResult (m.size () == 1);
		Verify (m.Lookup (1));
		Verify (not m.Lookup (2));
        m.Add (1, 2);
		VerifyTestResult (m.size () == 1);
		m.Remove (1);
		VerifyTestResult (m.size () == 0);
		m.RemoveAll ();
		VerifyTestResult (m.size () == 0);
    }
}



namespace {
    template <typename MappingOfKeyT>
    void    SimpleMappingTest_All_For_Type ()
    {
        MappingOfKeyT m;
        SimpleTest_1_ (m);
        SimpleTest_2_ (m);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
		SimpleMappingTest_All_For_Type<Mapping<size_t,size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping<size_t,size_t>> ();
        
		SimpleMappingTest_All_For_Type<Mapping_Array<size_t,size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping_Array<size_t,size_t>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

