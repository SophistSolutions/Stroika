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


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::SortedSet_stdset;



namespace {
    template <typename SetOfT>
    void    SimpleTest_1_ (SetOfT s)
    {
        SetOfT s2;
        SetOfT s3 = s;
    }
}


namespace {
    template <typename SetOfT>
    void    SimpleTest_2_ (SetOfT s)
    {
        s.Add (1);
        VerifyTestResult (s.size () == 1);
        VerifyTestResult (s.Contains (1));
        VerifyTestResult (not s.Contains (2));
        s.Add (1);
        VerifyTestResult (s.size () == 1);
        s.Remove (1);
        VerifyTestResult (s.size () == 0);
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
    }
}


namespace {
    template <typename SetOfT>
    void    SimpleTest_3_Iteration_ (SetOfT s)
    {
#if 0
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
#endif
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
    }
}


namespace {
    template <typename SetOfT>
    void    SimpleMappingTest_All_For_Type ()
    {
        SetOfT s;
        SimpleTest_1_ (s);
        SimpleTest_2_ (s);
        SimpleTest_3_Iteration_ (s);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        SimpleMappingTest_All_For_Type<SortedSet<size_t>> ();
        SimpleMappingTest_All_For_Type<SortedSet<SimpleClass>> ();

        SimpleMappingTest_All_For_Type<SortedSet_stdset<size_t>> ();
        SimpleMappingTest_All_For_Type<SortedSet_stdset<SimpleClass>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

