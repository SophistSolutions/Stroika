/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Stack
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Stack.h"
//#include    "Stroika/Foundation/Containers/Concrete/Set_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


//using   Concrete::Set_LinkedList;



namespace {
    template <typename StackOfT>
    void    SimpleTest_1_ (StackOfT s)
    {
        StackOfT s2;
        StackOfT s3 = s;
    }
}


namespace {
    template <typename StackOfT>
    void    SimpleTest_2_ (StackOfT s)
    {
        s.Push (1);
        VerifyTestResult (s.size () == 1);
        s.Push (1);
        VerifyTestResult (s.size () == 2);
        s.Pop ();
        VerifyTestResult (s.size () == 1);
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
    }
}


namespace {
    template <typename StackOfT>
    void    SimpleTest_3_Iteration_ (StackOfT s)
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
    template <typename StackOfT>
    void    SimpleMappingTest_All_For_Type ()
    {
        StackOfT s;
        SimpleTest_1_ (s);
        SimpleTest_2_ (s);
        SimpleTest_3_Iteration_ (s);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        SimpleMappingTest_All_For_Type<Stack<size_t>> ();
        SimpleMappingTest_All_For_Type<Stack<SimpleClass>> ();

        //      SimpleMappingTest_All_For_Type<Set_LinkedList<size_t>> ();
        //      SimpleMappingTest_All_For_Type<Set_LinkedList<SimpleClass>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

