/*
 * Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
 */
//      TEST    Foundation::Containers::Sequence
//      STATUS  TOTALLY WRONG - PLACEHOLDER
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <list>
#include    <sstream>
#include    <vector>

#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_LinkedList.h"
#include    "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
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
using   Concrete::Sequence_stdvector;



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

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_1_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        {
            CONCRETE_SEQUENCE_T s;
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            VerifyTestResult (s.size () == 1);
            VerifyTestResult (EQUALS_COMPARER::Equals (s.GetAt (0), 1));
            s.Append (2);
            VerifyTestResult (s.size () == 2);
            VerifyTestResult (EQUALS_COMPARER::Equals (s.GetAt (0), 1));
            VerifyTestResult (EQUALS_COMPARER::Equals (s.GetAt (1), 2));
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            CONCRETE_SEQUENCE_T s;
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (s.size () == 1000);
            VerifyTestResult (EQUALS_COMPARER::Equals (s[0], 0));
            VerifyTestResult (EQUALS_COMPARER::Equals (s[1], 1));
            VerifyTestResult (EQUALS_COMPARER::Equals (s[100], 100));
            s.Remove (0);
            VerifyTestResult (s.size () == 999);
            for (size_t i = 0; i < 999; ++i) {
                VerifyTestResult (EQUALS_COMPARER::Equals (s[i], i + 1));
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_2_Contains_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        {
            CONCRETE_SEQUENCE_T s;
            VerifyTestResult (s.size () == 0);
            VerifyTestResult (not s.Contains (1));
            s.Append (1);
            VerifyTestResult (s.Contains (1));
            s.RemoveAll ();
            VerifyTestResult (not s.Contains (1));
            VerifyTestResult (s.empty ());
        }
        {
            CONCRETE_SEQUENCE_T s;
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

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_3_Compare_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
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

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_4_Equals_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        // This is RIGHT but We need a way to use 'TRAITS' to extend the defintiion of Sequence<T> or some such - to make this work...
        {
            CONCRETE_SEQUENCE_T s;
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

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_5_RemoveAll_ ()
    {
        CONCRETE_SEQUENCE_T s;
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

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_6_GetSetAt_ ()
    {
        CONCRETE_SEQUENCE_T s;
        VerifyTestResult (s.empty ());
        for (size_t i = 0; i < 1000; ++i) {
            s.Append (1);
            VerifyTestResult (EQUALS_COMPARER::Equals (s.GetAt (i), 1));
            VerifyTestResult (EQUALS_COMPARER::Equals (s[i], 1));
        }
        for (size_t i = 0; i < 1000; ++i) {
            s.SetAt (i, 5000 + i);
            VerifyTestResult (EQUALS_COMPARER::Equals (s[i], 5000 + i));
        }
        for (size_t i = 0; i < 1000; ++i) {
            VerifyTestResult (EQUALS_COMPARER::Equals (s.GetAt (i), 5000 + i));
        }
        VerifyTestResult (not s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_7_IndexOf_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        CONCRETE_SEQUENCE_T s;
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

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_8_InsertAppendPrepend_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        CONCRETE_SEQUENCE_T s;
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                VerifyTestResult (EQUALS_COMPARER::Equals (*i, j));
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
                VerifyTestResult (EQUALS_COMPARER::Equals (*i, 1000 - j - 1));
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

            s.PrependAll (x);
            VerifyTestResult (s.Equals (x));
            s.AppendAll (x);
            VerifyTestResult (EQUALS_COMPARER::Equals (s[1], 11));
            VerifyTestResult (EQUALS_COMPARER::Equals (s[2], 12));
            VerifyTestResult (EQUALS_COMPARER::Equals (s[3], 10));
        }
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_9_Update_ ()
    {
        CONCRETE_SEQUENCE_T s;
        {
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end (); ++i) {
                s.Update (i, 5);
            }
            for (auto i : s) {
                VerifyTestResult (EQUALS_COMPARER::Equals (i, 5));
            }
            s.SetAt (16, 16);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (EQUALS_COMPARER::Equals (*i, 16)) {
                    s.Update (i, 17);
                }
            }
            VerifyTestResult (EQUALS_COMPARER::Equals (s[16], 17));
            for (auto i = s.begin (); i != s.end (); ++i) {
                VerifyTestResult (EQUALS_COMPARER::Equals (*i, 5) or s.IndexOf (i) == 16);
            }

            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_10_Remove_ ()
    {
        CONCRETE_SEQUENCE_T s;
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5);
            VerifyTestResult (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    VerifyTestResult (EQUALS_COMPARER::Equals (*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER::Equals ((*i), s.IndexOf (i) + 1));
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
                    VerifyTestResult (EQUALS_COMPARER::Equals (*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER::Equals ((*i), s.IndexOf (i) + 90));
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
                    VerifyTestResult (EQUALS_COMPARER::Equals (*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER::Equals ((*i), s.IndexOf (i) + 1));
                }
            }
            s.RemoveAll ();
        }
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_11_STLCompatWrappers_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        CONCRETE_SEQUENCE_T s;
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            {
                vector<T>   vs;
                s.As (&vs);
                VerifyTestResult (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    VerifyTestResult (EQUALS_COMPARER::Equals (T (i - vs.begin ()), *i));
                }
            }
            {
                vector<T>   vs = s.template As<vector<T>> ();
                VerifyTestResult (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    VerifyTestResult (EQUALS_COMPARER::Equals (T (i - vs.begin ()), *i));
                }
            }
            {
                list<T> vs;
                s.As (&vs);
                VerifyTestResult (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    VerifyTestResult (EQUALS_COMPARER::Equals (T (idx), *i));
                }
            }
            {
                list<T> vs = s.template As<list<T>> ();
                VerifyTestResult (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    VerifyTestResult (EQUALS_COMPARER::Equals (T (idx), *i));
                }
            }
        }
        {
            vector<T>   vs = s.template As<vector<T>> ();
            Sequence<T> tmp = Sequence<T> (vs);
            VerifyTestResult (tmp.size () == vs.size ());
            VerifyTestResult (tmp.template As<vector<T>> () == vs);
        }
        {
            list<T>     ls = s.template As<list<T>> ();
            Sequence<T> tmp = Sequence<T> (ls);
            VerifyTestResult (tmp.size () == ls.size ());
            VerifyTestResult (tmp.template As<list<T>> () == ls);
        }

        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_12_ToFromSTLVector_ ()
    {
        typedef typename CONCRETE_SEQUENCE_T::ElementType       T;
        CONCRETE_SEQUENCE_T s;
        VerifyTestResult (s.empty ());

        {
            vector<T>   n;
            n.push_back (3);
            n.push_back (5);
            n.push_back (9);
            s = CONCRETE_SEQUENCE_T (n);
            vector<T>   nn = s.template As<vector<T>> ();
            VerifyTestResult (nn == n);
        }

        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }

}


namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_13_Initializers_ ()
    {
        CONCRETE_SEQUENCE_T s;
        typedef typename CONCRETE_SEQUENCE_T::ElementType T;
        VerifyTestResult (s.empty ());

#if      qCompilerAndStdLib_Supports_initializer_lists
        // fix - once we have on all subclasses - do be basic test for aech
        {
            Sequence<int> x1 = { };
            VerifyTestResult (x1.size () == 0);
            Sequence<int> x2 = { 1, 2, 3 };
            VerifyTestResult (x2.size () == 3);
            VerifyTestResult (x2[0] == 1);
        }
#endif

    }

}




namespace   {

    template <typename CONCRETE_SEQUENCE_TYPE, typename EQUALS_COMPARER>
    void    SimpleSequenceTest_All_For_Type ()
    {
        SimpleSequenceTest_1_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_2_Contains_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_3_Compare_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_4_Equals_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_5_RemoveAll_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_6_GetSetAt_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_7_IndexOf_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_8_InsertAppendPrepend_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_9_Update_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_10_Remove_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_11_STLCompatWrappers_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_12_ToFromSTLVector_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_13_Initializers_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        typedef Common::ComparerWithEquals<size_t>  COMPARE_SIZET;
        typedef Common::ComparerWithEquals<SimpleClass>  COMPARE_SimpleClass;
        struct  COMPARE_SimpleClassWithoutComparisonOperators {
            typedef SimpleClassWithoutComparisonOperators ElementType;
            static  bool    Equals (ElementType v1, ElementType v2) {
                return v1.GetValue () == v2.GetValue ();
            }
        };


        SimpleSequenceTest_All_For_Type<Sequence<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type<Sequence<SimpleClass>, COMPARE_SimpleClass> ();
        //SimpleSequenceTest_All_For_Type<Sequence<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_All_For_Type<Sequence_Array<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type<Sequence_Array<SimpleClass>, COMPARE_SimpleClass> ();
        //SimpleSequenceTest_All_For_Type<Sequence_Array<SimpleClassWithoutComparisonOperators>> ();

        SimpleSequenceTest_All_For_Type<Sequence_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type<Sequence_DoublyLinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        //SimpleSequenceTest_All_For_Type<Sequence_DoublyLinkedList<SimpleClassWithoutComparisonOperators>> ();

        SimpleSequenceTest_All_For_Type<Sequence_LinkedList<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type<Sequence_LinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        //SimpleSequenceTest_All_For_Type<Sequence_LinkedList<SimpleClassWithoutComparisonOperators>> ();

        SimpleSequenceTest_All_For_Type<Sequence_stdvector<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type<Sequence_stdvector<SimpleClass>, COMPARE_SimpleClass> ();
        //SimpleSequenceTest_All_For_Type<Sequence_stdvector<SimpleClassWithoutComparisonOperators>> ();
    }

}



int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
