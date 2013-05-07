/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Mapping
//      STATUS  very minimal/incomplete
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Mapping_stdmap.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Mapping_Array;
using   Concrete::Mapping_LinkedList;
using   Concrete::Mapping_stdmap;



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
        Verify (m.Lookup (1, nullptr));
        Verify (not m.Lookup (2, nullptr));
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
    void    SimpleTest_3_Iteration_ (MappingOfKeyT m)
    {
        m.Add (1, 2);
        VerifyTestResult (m.size () == 1);
        for (auto i : m) {
            VerifyTestResult (i.first == 1);
            VerifyTestResult (i.second == 2);
        }
        m.Add (1, 2);
        VerifyTestResult (m.size () == 1);
        for (auto i : m) {
            VerifyTestResult (i.first == 1);
            VerifyTestResult (i.second == 2);
        }
        m.Remove (1);
        VerifyTestResult (m.size () == 0);
        for (auto i : m) {
            VerifyTestResult (false);
        }
        m.Add (1, 2);
        m.Add (2, 3);
        m.Add (3, 4);
        unsigned int cnt = 0;
        for (auto i : m) {
            cnt++;
            if (cnt == 1) {
                VerifyTestResult (i.first == 1);
                VerifyTestResult (i.second == 2);
            }
            if (cnt == 2) {
                VerifyTestResult (i.first == 2);
                VerifyTestResult (i.second == 3);
            }
            if (cnt == 3) {
                VerifyTestResult (i.first == 3);
                VerifyTestResult (i.second == 4);
            }
        }
        VerifyTestResult (cnt == 3);
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
        SimpleTest_3_Iteration_ (m);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        SimpleMappingTest_All_For_Type<Mapping<size_t, size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping<SimpleClass, SimpleClass>> ();

        SimpleMappingTest_All_For_Type<Mapping_Array<size_t, size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping_Array<SimpleClass, SimpleClass>> ();

        SimpleMappingTest_All_For_Type<Mapping_LinkedList<size_t, size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping_LinkedList<SimpleClass, SimpleClass>> ();

        SimpleMappingTest_All_For_Type<Mapping_stdmap<size_t, size_t>> ();
        SimpleMappingTest_All_For_Type<Mapping_stdmap<SimpleClass, SimpleClass>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

