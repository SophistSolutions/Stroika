/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Sequence
//      STATUS  TOTALLY WRONG - PLACEHOLDER
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Sequence_Array;
using   Concrete::Sequence_DoublyLinkedList;
using   Concrete::Sequence_LinkedList;



/**
 *
 *  @todo   Fix COMPARE (test case 3) support (test cases) once we better support the feautre with traits!
 *
 *  @todo   Look at Container tests for Bag/Tally, and see if any of them can be lifted and applied here.
 *
 *  @todo   Look at Iterable<T> stuff and write test cases for those APIs. Write in a way so that
 *          they can be re-applied for other classes that are  Iterable - such as Bag/etc.
 *
 *  @todo   Review Stroika v1 regression tests, and see if any of that can usefully be applied here
 *          as well.
 *
 *  @todo   Write PERFORMANCE TESTS - to compare between Stroika and STL containers. This may be better
 *          as a DEMO, than as a regression test.
 */


namespace {

    template <typename T>
    void    SimpleSequenceTest_1_ (Sequence<T>& s)
    {
        {
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            VerifyTestResult (s.size () == 1);
            VerifyTestResult (s.GetAt (0) == 1);
            s.Append (2);
            VerifyTestResult (s.size () == 2);
            VerifyTestResult (s.GetAt (0) == 1);
            VerifyTestResult (s.GetAt (1) == 2);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (s.size () == 1000);
            VerifyTestResult (s[0] == 0);
            VerifyTestResult (s[1] == 1);
            VerifyTestResult (s[100] == 100);
            s.Remove (0);
            VerifyTestResult (s.size () == 999);
            for (size_t i = 0; i < 999; ++i) {
                VerifyTestResult (s[i] == i + 1);
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }


    template <typename T>
    void    SimpleSequenceTest_2_Contains_ (Sequence<T>& s)
    {
        {
            VerifyTestResult (s.size () == 0);
            VerifyTestResult (not s.Contains (1));
            s.Append (1);
            VerifyTestResult (s.Contains (1));
            s.RemoveAll ();
            VerifyTestResult (not s.Contains (1));
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i + 1000);
            }
            for (size_t i = 0; i < 1000; ++i) {
                VerifyTestResult (not s.Contains (i));
                VerifyTestResult (s.Contains (i + 1000));
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }


    template <typename T>
    void    SimpleSequenceTest_3_Compare_ (Sequence<T>& s)
    {
#if 0
        // This is RIGHT but We need a way to use 'TRAITS' to extend the defintiion of Sequence<T> or some such - to make this work...
        {
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            Sequence<T> s2 = s;
            s2.Append (2);
            VerifyTestResult (s.Compare (s2) < 0);
            VerifyTestResult (s2.Compare (s) > 0);
            s.Append (2);
            VerifyTestResult (s2.Compare (s) == 0);
            s.RemoveAll ();
            VerifyTestResult (s.Compare (s2) < 0);
            VerifyTestResult (s.empty ());
        }
#endif
    }


    template <typename T>
    void    SimpleSequenceTest_4_Equals_ (Sequence<T>& s)
    {
        // This is RIGHT but We need a way to use 'TRAITS' to extend the defintiion of Sequence<T> or some such - to make this work...
        {
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            Sequence<T> s2 = s;
            s2.Append (2);
            VerifyTestResult (not s.Equals (s2));
            VerifyTestResult (not s2.Equals (s));
            s.Append (2);
            VerifyTestResult (s2.Equals (s));
            s.RemoveAll ();
            VerifyTestResult (not s.Equals (s2));
            VerifyTestResult (s.empty ());
        }
    }


    template <typename T>
    void    SimpleSequenceTest_4_RemoveAll_ (Sequence<T>& s)
    {
        VerifyTestResult (s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
        for (size_t i = 0; i < 1000; ++i) {
            s.Append (i + 1000);
        }
        VerifyTestResult (not s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
        s.RemoveAll ();
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }


    template <typename T>
    void    SimpleSequenceTest_5_GetSetAt_ (Sequence<T>& s)
    {
        VerifyTestResult (s.empty ());
        for (size_t i = 0; i < 1000; ++i) {
            s.Append (1);
            VerifyTestResult (s.GetAt (i) == 1);
            VerifyTestResult (s[i] == 1);
        }
        for (size_t i = 0; i < 1000; ++i) {
            s.SetAt (i, 5000 + i);
            VerifyTestResult (s[i] == 5000 + i);
        }
        for (size_t i = 0; i < 1000; ++i) {
            VerifyTestResult (s.GetAt (i) == 5000 + i);
        }
        VerifyTestResult (not s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }


    template <typename T>
    void    SimpleSequenceTest_6_IndexOf_ (Sequence<T>& s)
    {
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (21 + i);
            }
            VerifyTestResult (s.IndexOf (5) == kBadSequenceIndex);
            VerifyTestResult (not s.empty ());

            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (not s.empty ());
            VerifyTestResult (s.size () == 1000);

            Sequence<T> s2 = s;
            VerifyTestResult (s.IndexOf (s2) == 0);
            VerifyTestResult (s2.IndexOf (s) == 0);

            Sequence<T> s3;
            s3.Append (3);
            s3.Append (4);
            VerifyTestResult (s3.IndexOf (s) == kBadSequenceIndex);
            VerifyTestResult (s.IndexOf (s3) == 3);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (s.size () == 1000);
            size_t j = 0;
            for (Iterator<T> i = s.MakeIterator (); i != s.end (); ++i, ++j) {
                VerifyTestResult (s.IndexOf (i) == j);
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }


    template <typename T>
    void    SimpleSequenceTest_7_InsertAppendPrepend_ (Sequence<T>& s)
    {
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                VerifyTestResult (*i == j);
            }
            VerifyTestResult (s.size () == 1000);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Prepend (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                VerifyTestResult (*i == 1000 - j - 1);
            }
            VerifyTestResult (s.size () == 1000);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }

        {
            // primitive, but at least somthing - test of Insert() of sequence (cuz Prepend/Append call Insert internally)
            Sequence<T> x;
            x.Append (10);
            x.Append (11);
            x.Append (12);

            s.Prepend (x);
            VerifyTestResult (s.Equals (x));
            s.Append (x);
            VerifyTestResult (s[1] == 11);
            VerifyTestResult (s[2] == 12);
            VerifyTestResult (s[3] == 10);
        }
    }




    template <typename T>
    void    SimpleSequenceTest_8_Update_ (Sequence<T>& s)
    {
        {
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end (); ++i) {
                s.Update (i, 5);
            }
            for (auto i : s) {
                VerifyTestResult (i == 5);
            }
            s.SetAt (16, 16);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (*i == 16) {
                    s.Update (i, 17);
                }
            }
            VerifyTestResult (s[16] == 17);
            for (auto i = s.begin (); i != s.end (); ++i) {
                VerifyTestResult (*i == 5 or s.IndexOf (i) == 16);
            }

            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }




    template <typename T>
    void    SimpleSequenceTest_9_Remove_ (Sequence<T>& s)
    {
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5);
            VerifyTestResult (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    VerifyTestResult (*i == s.IndexOf (i));
                }
                else {
                    VerifyTestResult ((*i) == s.IndexOf (i) + 1);
                }
            }
        }
        {
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5, 95);
            VerifyTestResult (s.size () == 10);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    VerifyTestResult (*i == s.IndexOf (i));
                }
                else {
                    VerifyTestResult ((*i) == s.IndexOf (i) + 90);
                }
            }
        }
        {
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end (); ++i) {
                // remove the 5th element, but note after deletion, all index will be for the 5th elt if we keep deleting the 5th
                if (s.IndexOf (i) == 5 and s.size () == 100) {
                    s.Remove (i);
                }
            }
            VerifyTestResult (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    VerifyTestResult (*i == s.IndexOf (i));
                }
                else {
                    VerifyTestResult ((*i) == s.IndexOf (i) + 1);
                }
            }
            s.RemoveAll ();
        }
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }


    template <typename T>
    void    SimpleSequenceTest_10_STLCompatWrappers_ (Sequence<T>& s)
    {
        // NYI -
        // but just trival wrappers on other things already tested so no biggie
    }



}



namespace   {

    template <typename SequenceOfT>
    void    SimpleSequenceTest_All_For_Type ()
    {
        SequenceOfT s;
        SimpleSequenceTest_1_ (s);
        SimpleSequenceTest_2_Contains_ (s);
        SimpleSequenceTest_3_Compare_ (s);
        SimpleSequenceTest_4_Equals_ (s);
        SimpleSequenceTest_4_RemoveAll_ (s);
        SimpleSequenceTest_5_GetSetAt_ (s);
        SimpleSequenceTest_6_IndexOf_ (s);
        SimpleSequenceTest_7_InsertAppendPrepend_ (s);
        SimpleSequenceTest_8_Update_ (s);
        SimpleSequenceTest_9_Remove_ (s);
        SimpleSequenceTest_10_STLCompatWrappers_ (s);
    }

    void    DoRegressionTests_ ()
    {
        SimpleSequenceTest_All_For_Type<Sequence<size_t>> ();
        SimpleSequenceTest_All_For_Type<Sequence<SimpleClass>> ();

        SimpleSequenceTest_All_For_Type<Sequence_Array<size_t>> ();
        SimpleSequenceTest_All_For_Type<Sequence_Array<SimpleClass>> ();

        SimpleSequenceTest_All_For_Type<Sequence_DoublyLinkedList<size_t>> ();
        SimpleSequenceTest_All_For_Type<Sequence_DoublyLinkedList<SimpleClass>> ();

        SimpleSequenceTest_All_For_Type<Sequence_LinkedList<size_t>> ();
        SimpleSequenceTest_All_For_Type<Sequence_LinkedList<SimpleClass>> ();
    }

}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

