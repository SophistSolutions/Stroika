/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Queue
//      STATUS  PRELIMINARY
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Queue.h"
#include    "Stroika/Foundation/Containers/Concrete/Queue_DoublyLinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Queue_DoublyLinkedList;



namespace {
    template <typename QueueOfT>
    void    SimpleTest_1_ (QueueOfT s)
    {
        QueueOfT s2;
        QueueOfT s3 = s;
    }
}


namespace {
    template <typename QueueOfT>
    void    SimpleTest_2_ (QueueOfT s)
    {
        s.Enqueue (1);
        VerifyTestResult (s.size () == 1);
        s.Enqueue (1);
        VerifyTestResult (s.size () == 2);
        s.Dequeue ();
        VerifyTestResult (s.size () == 1);
        s.RemoveAll ();
        VerifyTestResult (s.size () == 0);
    }
}


namespace {
    template <typename QueueOfT>
    void    SimpleTest_3_Iteration_ (QueueOfT s)
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
    template <typename QueueOfT>
    void    SimpleMappingTest_All_For_Type ()
    {
        QueueOfT s;
        SimpleTest_1_ (s);
        SimpleTest_2_ (s);
        SimpleTest_3_Iteration_ (s);
    }
}


namespace   {
    void    DoRegressionTests_ ()
    {
        SimpleMappingTest_All_For_Type<Queue<size_t>> ();
        SimpleMappingTest_All_For_Type<Queue<SimpleClass>> ();

        SimpleMappingTest_All_For_Type<Queue_DoublyLinkedList<size_t>> ();
        SimpleMappingTest_All_For_Type<Queue_DoublyLinkedList<SimpleClass>> ();
    }
}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

