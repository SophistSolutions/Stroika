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
//#include    "Stroika/Foundation/Containers/Concrete/Tally_LinkedList.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"


#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;


using   Concrete::Sequence_Array;
#if 0
using   Concrete::Tally_LinkedList;
#endif


#if 0
// OLD 1992 Stroika Sequence tests - look through and grab what we can
#define     qPrintProgress  0
#define     qPrintTimings   !qDebug


#if     qRealTemplatesAvailable
void    SequenceTimings (Sequence<UInt32>& s)
#else
void    SequenceTimings (Sequence(UInt32)& s)
#endif
{
#if     qPrintTimings
    Time t = GetCurrentTime ();
    cout << tab << "testing Sequence<UInt32> of length " << s.GetLength () << endl;
#endif

    for (int i = 1; i <= s.GetLength (); i++) {
        TestCondition (s.Contains (i));
        TestCondition (not s.Contains (0));
    }

    for (i = 1; i <= s.GetLength (); i++) {
        ForEach (UInt32, it, s) {
            if (it.Current () == i) {
                break;
            }
        }
    }
    ForEach (UInt32, it, s) {
        ForEach (UInt32, it1, s) {
            s.RemoveAll ();
        }
    }
    TestCondition (s.IsEmpty ());
    TestCondition (s.GetLength () == 0);

    ForEach (UInt32, it1, s) {
        ForEach (UInt32, it2, s) {
            TestCondition (False);
        }
    }

#if     qPrintTimings
    t = GetCurrentTime () - t;
    cout << tab << "finished testing Sequence<UInt32>; time elapsed = " << t << endl;
#endif
}

#if     qRealTemplatesAvailable
static  void    TestSimpleIteration (Sequence<UInt32>& s)
#else
static  void    TestSimpleIteration (Sequence(UInt32)& s)
#endif
{
    UInt32 index = 0;
    {
        ForEach (UInt32, it, s) {
            index++;
            TestCondition (it.Current () == index);
            TestCondition (s.IndexOf (it.Current ()) == index);
        }
    }
    TestCondition (s.GetLength () == index);

    {
        ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceBackward)) {
            TestCondition (it.Current () == index);
            TestCondition (s.IndexOf (it.Current ()) == index);
            index--;
        }
    }
}

#if     qRealTemplatesAvailable
void    SimpleSequenceTimings (Sequence<UInt32>& s)
#else
void    SimpleSequenceTimings (Sequence(UInt32)& s)
#endif
{

#if     qPrintTimings
    Time t = GetCurrentTime ();
    cout << tab << "timing Sequence<UInt32>..." << endl;
#endif

    const   K = 500;
    for (int i = 1; i <= K; i++) {
        s.Append (i);
        TestCondition (s.Contains (i));
        TestCondition (s.GetLength () == i);
    }
    for (i = K; i > 0; i--) {
        s.Remove (i);
        TestCondition (not s.Contains (i));
        TestCondition (s.GetLength () == (i - 1));
    }
    for (i = 1; i <= K; i++) {
        s.Append (i);
    }
    i = 0;
    ForEach (UInt32, it, s) {
        TestCondition (++i == it.Current ());
    }
    s.RemoveAll ();
    TestCondition (s.IsEmpty ());

#if     qPrintTimings
    t = GetCurrentTime () - t;
    cout << tab << "finished timing Sequence<UInt32>; time elapsed = " << t << endl;
#endif
}

#if     qRealTemplatesAvailable
void    SequenceIteratorTests (Sequence<UInt32>& s)
#else
void    SequenceIteratorTests (Sequence(UInt32)& s)
#endif
{
    const   int kTestSize   =   5;

    TestCondition (s.GetLength () == 0);
    /*
     * Try removes while iterating forward.
     */
    {
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }

        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (UInt32, it, s) {
            TestCondition (s.Contains (i));
            TestCondition (s.GetLength () == kTestSize - i + 1);
            s.Remove (i++);
            TestCondition (s.GetLength () == kTestSize - i + 1);
            TestCondition (not s.Contains (i - 1));
            if (i == kTestSize + 1) {
                TestCondition (s.IsEmpty ());
            }
        }
        TestCondition (s.IsEmpty ());
        TestCondition (s.GetLength () == 0);

        for (i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = kTestSize;
        ForEach (UInt32, it1, s) {
            TestCondition (s.Contains (i));
            s.Remove (i--);
        }
        TestCondition (s.GetLength () == kTestSize / 2);

        s.RemoveAll ();
        for (i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        ForEach (UInt32, it2, s) {
            s.Remove (it2.Current ());
        }
        TestCondition (s.GetLength () == 0);
    }

    /*
     * Try removes multiple iterators present.
     */
    {
        s.RemoveAll ();
        TestCondition (s.GetLength () == 0);
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (UInt32, it, s) {
            ForEachT (SequenceIterator, UInt32, it2, s.MakeSequenceIterator (eSequenceBackward)) {
                ForEachT (SequenceMutator, UInt32, it3, s.MakeSequenceMutator (eSequenceBackward)) {
                    TestCondition (it2.GetDirection () == eSequenceBackward);
                    TestCondition (it3.GetDirection () == eSequenceBackward);

                    TestCondition (s.GetLength () > 0);

                    it3.RemoveCurrent ();
                    s.Append (i);
                    s.Remove (i);
                }
            }
        }
    }
}

#if     qRealTemplatesAvailable
void    SimpleTests (Sequence<UInt32>& s)
#else
void    SimpleTests (Sequence(UInt32)& s)
#endif
{
    UInt32  three = 3;

#if     qRealTemplatesAvailable
    Sequence<UInt32>    s1 (s);
#else
    Sequence(UInt32)    s1 (s);
#endif
    TestCondition (s1 == s);
    TestCondition (s1 == s);
#if     qRealTemplatesAvailable
    Sequence<UInt32>    s2 = s1;
#else
    Sequence(UInt32)    s2 = s1;
#endif
    TestCondition (s2 == s);
    TestCondition (s2 == s1);
    s2.Append (three);
    TestCondition (s1 == s);
    TestCondition (s2 != s1);

    TestCondition (s.IsEmpty ());

    s.Append (three);
    TestCondition (s.GetLength () == 1);
    s.Append (4);
    TestCondition (s.GetLength () == 2);
    TestCondition (s.Contains (three));
    s.Remove (three);
    TestCondition (s.GetLength () == 1);
    s.RemoveAll ();
    TestCondition (s.IsEmpty ());
    s.Append (1);
    TestCondition (s.Contains (1));
    TestCondition (not s.Contains (3));
    s.Append (2);
    TestCondition (s.Contains (2));
    TestCondition (s.Contains (1));
    TestCondition (not s.Contains (3));
    TestCondition (s.GetLength () == 2);

    s.Remove (2);
    TestCondition (s.GetLength () == 1);

    s.RemoveAll ();

    const int kListLength = 50;

    size_t  count;
    for (count = 1; count <= kListLength; count++) {
        s.Append (count);
    }
    SequenceTimings (s);
    s.RemoveAll ();


#if     qPrintProgress
    cout << "Simple tests of Sequence of UInt32..." << endl;
    cout << "testing empty sequence" << endl;
#endif

    TestCondition (s.GetLength () == 0);
    {
        ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceBackward)) {
            TestCondition (False);
        }
    }
    {
        ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceForward)) {
            TestCondition (False);
        }
    }

#if     qPrintProgress
    cout << "testing iteration with appends" << endl;
#endif
    for (count = 1; count <= kListLength; count++) {
        s.Append (count);
        TestSimpleIteration (s);
    }

#if     qPrintProgress
    cout << "testing remove" << endl;
#endif
    TestCondition (s.GetLength () == kListLength);
    for (count = 1; count <= kListLength; count++) {
        s.Remove (count);
        TestCondition (s.GetLength () == (kListLength - count));
    }
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "testing iteration with prepends" << endl;
#endif
    for (count = kListLength; count >= 1; count--) {
        s.Prepend (count);
    }
    TestSimpleIteration (s);

#if     qPrintProgress
    cout << "testing remove all" << endl;
#endif
    TestCondition (s.GetLength () == kListLength);
    s.RemoveAll ();
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "testing array indexing with appends" << endl;
#endif
    for (count = 1; count <= kListLength; count++) {
        s.Append (count);
        TestCondition (s[count] == count);
    }
    s.RemoveAll ();
    TestCondition (s.GetLength () == 0);

    for (count = 1; count <= kListLength; count++) {
        s.Append ((UInt32)1);
    }
    {
        count = 0;
        ForEach (UInt32, it, s) {
            count++;
        }
        TestCondition (count == kListLength);
    }
    for (count = kListLength; count >= 1; count--) {
        s.Remove (1);
    }
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "Finished simple tests of Sequence of UInt32..." << endl;
#endif

    SequenceIteratorTests (s);
    SimpleSequenceTimings (s);
}




#if     qRealTemplatesAvailable
void    Test_Deletions_During_Iteration (Sequence<UInt32>& s)
#else
void    Test_Deletions_During_Iteration (Sequence(UInt32)& s)
#endif
{
    SequenceIteratorTests (s);

    const   int kTestSize   =   100;

    TestCondition (s.GetLength () == 0);
    /*
     * Try removes while iterating forward.
     */
    {
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEachT (Iterator, UInt32, it, s) {
            s.Remove (i++);
        }
        TestCondition (s.GetLength () == 0);
    }

    /*
     * Try removeAts while iterating forward.
     */
    {
        TestCondition (s.GetLength () == 0);
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (UInt32, it, s) {
            if (i > s.GetLength ()) {
                i = 1;
            }
            s.RemoveAt (i++);
        }
        TestCondition (s.GetLength () == (kTestSize / 2) - 1);  /* since removing right of cursor */
        s.RemoveAll ();
        TestCondition (s.GetLength () == 0);
    }

    /*
     * Try removes while iterating backward.
     */
    {

    }

    /*
     * Try removes multiple iterators present.
     */
    {
        TestCondition (s.GetLength () == 0);
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (i);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (UInt32, it, s) {
            ForEach (UInt32, it2, s.MakeSequenceIterator (eSequenceBackward)) {
                ForEach (UInt32, it3, s) {
                    if (s.GetLength () != 0) {
                        if (i > s.GetLength ()) {
                            i = 1;
                        }
                        s.RemoveAt (i++);
                    }
                }
            }
        }
    }
}

#if     qRealTemplatesAvailable
static  void    TestSimpleIteration (Sequence<SimpleClass>& s)
#else
static  void    TestSimpleIteration (Sequence(SimpleClass)& s)
#endif
{
    UInt32 index = 0;
    {
        ForEach (SimpleClass, it, s) {
            index++;
            //  TestCondition (s.IndexOf (it.Current ()) == index);
        }
    }
    TestCondition (s.GetLength () == index);

    {
        ForEach (SimpleClass, it, s.MakeSequenceIterator (eSequenceBackward)) {
            //  TestCondition (s.IndexOf (it.Current ()) == index);
            index--;
        }
    }
}

#if     qRealTemplatesAvailable
void    SimpleTests (Sequence<SimpleClass>& s)
#else
void    SimpleTests (Sequence(SimpleClass)& s)
#endif
{
#if     qRealTemplatesAvailable
    Sequence<SimpleClass>   s1 (s);
#else
    Sequence(SimpleClass)   s1 (s);
#endif
    TestCondition (s1 == s);
    TestCondition (s1 == s);
#if     qRealTemplatesAvailable
    Sequence<SimpleClass>   s2 = s1;
#else
    Sequence(SimpleClass)   s2 = s1;
#endif
    TestCondition (s2 == s);
    TestCondition (s2 == s1);
    s2.Append (5);
    TestCondition (s1 == s);
    TestCondition (s2 != s1);

    const int kListLength = 10;
    size_t      count;

    for (count = 1; count <= kListLength; count++) {
        s.Append (1);
    }
    s.RemoveAll ();

#if     qPrintProgress
    cout << "Simple tests of Sequence of SimpleClass..." << endl;
    cout << "testing empty sequence" << endl;
#endif

    TestCondition (s.GetLength () == 0);
    {
        ForEach (SimpleClass, it, s) {
            TestCondition (False);
        }
    }
    {
        ForEach (SimpleClass, it, s.MakeSequenceIterator (eSequenceBackward)) {
            TestCondition (False);
        }
    }

#if     qPrintProgress
    cout << "testing iteration with appends" << endl;
#endif
    for (count = 1; count <= kListLength; count++) {
        s.Append (1);
        TestSimpleIteration (s);
    }

#if     qPrintProgress
    cout << "testing remove" << endl;
#endif
    TestCondition (s.GetLength () == kListLength);
    for (count = 1; count <= kListLength; count++) {
        s.Remove (1);
        TestCondition (s.GetLength () == (kListLength - count));
    }
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "testing iteration with prepends" << endl;
#endif
    for (count = kListLength; count >= 1; count--) {
        s.Prepend (2);
    }
    TestSimpleIteration (s);

#if     qPrintProgress
    cout << "testing remove all" << endl;
#endif
    TestCondition (s.GetLength () == kListLength);
    s.RemoveAll ();
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "testing array indexing with appends" << endl;
#endif
    for (count = 1; count <= kListLength; count++) {
        s.Append (2);
        TestCondition (s[count] == 2);
    }
    s.RemoveAll ();
    TestCondition (s.GetLength () == 0);

    for (count = 1; count <= kListLength; count++) {
        s.Append (3);
    }
    {
        count = 0;
        ForEach (SimpleClass, it, s) {
            count++;
        }
        TestCondition (count == kListLength);
    }

    for (count = kListLength; count >= 1; count--) {
        s.Remove (3);
    }
    TestCondition (s.GetLength () == 0);

#if     qPrintProgress
    cout << "Finished simple tests of Sequence of SimpleClass..." << endl;
#endif
}

#if     qRealTemplatesAvailable
void    Test_Deletions_During_Iteration (Sequence<SimpleClass>& s)
#else
void    Test_Deletions_During_Iteration (Sequence(SimpleClass)& s)
#endif
{
    const   int kTestSize   =   100;

    TestCondition (s.GetLength () == 0);
    /*
     * Try removes while iterating forward.
     */
    {
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (6);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (SimpleClass, it, s) {
            s.Remove (6);
        }
        TestCondition (s.GetLength () == 0);
    }

    /*
     * Try removeAts while iterating forward.
     */
    {
        TestCondition (s.GetLength () == 0);
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (6);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (SimpleClass, it, s) {
            if (i > s.GetLength ()) {
                i = 1;
            }
            s.RemoveAt (i++);
        }
        TestCondition (s.GetLength () == (kTestSize / 2) - 1);  /* since removing right of cursor */
        s.RemoveAll ();
        TestCondition (s.GetLength () == 0);
    }

    /*
     * Try removes while iterating backward.
     */
    {

    }

    /*
     * Try removes multiple iterators present.
     */
    {
        TestCondition (s.GetLength () == 0);
        for (int i = 1; i <= kTestSize; i++) {
            s.Append (8);
        }
        TestCondition (s.GetLength () == kTestSize);

        i = 1;
        ForEach (SimpleClass, it, s) {
            ForEach (SimpleClass, it2, s.MakeSequenceIterator (eSequenceBackward)) {
                ForEach (SimpleClass, it3, s) {
                    if (s.GetLength () != 0) {
                        if (i > s.GetLength ()) {
                            i = 1;
                        }
                        s.RemoveAt (i++);
                    }
                }
            }
        }
    }
}
#endif


namespace   {

#if 0
    void    TallyIteratorTests_ (Tally<size_t>& s)
    {
        const   size_t  kTestSize   =   6;

        VerifyTestResult (s.GetLength () == 0);

        for (TallyEntry<size_t> i : s) {
            VerifyTestResult (false);
        }

        /*
         * Try removes while iterating forward.
         */
        {
            for (size_t i = 1; i <= kTestSize; i++) {
                s.Add (i);
            }

            for (auto it = s.begin (); it != s.end (); ++it) {
                s.UpdateCount (it, 1);
            }

            VerifyTestResult (s.GetLength () == kTestSize);

            {
                for (TallyEntry<size_t> it : s) {
                    for (size_t i = 1; i <= kTestSize; i++) {
                        VerifyTestResult (s.Contains (i));
                        VerifyTestResult (s.GetLength () == kTestSize - i + 1);
                        s.Remove (i);
                        VerifyTestResult (not s.Contains (i - 1));
                    }
                }
                VerifyTestResult (s.IsEmpty ());
                VerifyTestResult (s.GetLength () == 0);
            }

            for (size_t i = 1; i <= kTestSize; i++) {
                s.Add (i);
            }
            VerifyTestResult (s.GetLength () == kTestSize);
            {
                for (auto it = s.begin (); it != s.end (); ++it) {
                    s.Remove (it);
                }
                VerifyTestResult (s.IsEmpty ());
                VerifyTestResult (s.GetLength () == 0);
            }

            for (size_t i = 1; i <= kTestSize; i++) {
                s.Add (i);
            }
            VerifyTestResult (s.GetLength () == kTestSize);
            for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                s.Remove (it2.Current ().fItem);
            }
            VerifyTestResult (s.GetLength () == 0);
        }

        /*
         * Try removes multiple iterators present.
         */
        {
            s.RemoveAll ();
            VerifyTestResult (s.GetLength () == 0);
            for (size_t i = 1; i <= kTestSize; i++) {
                s.Add (i);
            }
            VerifyTestResult (s.GetLength () == kTestSize);
            size_t i =  1;

            for (auto it = s.begin (); it != s.end (); ++it) {
                for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                    for (auto it3 = s.begin (); it3 != s.end (); ++it3) {
                        if (s.GetLength () != 0) {
                            s.UpdateCount (it3, 3);
                            s.Remove (it3);
                            s.Add (i);
                            s.Remove (i);
                        }
                    }
                }
            }
        }
    }

    void    SimpleTallyTests (Tally<size_t>& s)

    {
        size_t  three = 3;

        Tally<size_t>   s1 (s);

        VerifyTestResult (s1 == s);
        VerifyTestResult (s1 == s);
        Tally<size_t>   s2 = s1;

        VerifyTestResult (s2 == s);
        VerifyTestResult (s2 == s1);
        s2.Add (three);
        VerifyTestResult (s1 == s);
        VerifyTestResult (s2 != s1);

        TallyIteratorTests_ (s);

        const   size_t  K = 500;

        VerifyTestResult (s.IsEmpty ());
        s.Add (three);
        VerifyTestResult (s.GetLength () == 1);
        s += three;
        VerifyTestResult (s.GetLength () == 1);
        VerifyTestResult (s.Contains (three));
        VerifyTestResult (s.TallyOf (three) == 2);
        s.Remove (three);
        VerifyTestResult (s.GetLength () == 1);
        VerifyTestResult (s.Contains (three));
        VerifyTestResult (s.TallyOf (three) == 1);
        s.Remove (three);
        VerifyTestResult (s.IsEmpty ());
        s.RemoveAll ();
        VerifyTestResult (s.IsEmpty ());
        for (size_t i = 1; i <= K; i++) {
            s.Add (i);
        }

        for (size_t i = 1; i <= s.GetLength (); i++) {
            VerifyTestResult (s.Contains (i));
            VerifyTestResult (not s.Contains (0));
        }

        for (size_t i = 1; i <= s.GetLength (); i++) {
            for (auto it = s.begin (); it != s.end (); ++it) {
                if (it.Current ().fItem == i) {
                    break;
                }
            }
        }
        for (auto it = s.begin (); it != s.end (); ++it) {
            for (auto it1 = s.bagbegin (); it1 != s.bagend (); ++it1) {
                s.RemoveAll ();
            }
        }
        VerifyTestResult (s.IsEmpty ());
        VerifyTestResult (s.GetLength () == 0);

        for (auto it1 = s.begin (); it1 != s.end (); ++it1) {
            for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                VerifyTestResult (false);
            }
        }
        VerifyTestResult (s.IsEmpty ());


        for (size_t i = 1; i <= K; i++) {
            s.Add (i);
            VerifyTestResult (s.Contains (i));
            VerifyTestResult (s.TallyOf (i) == 1);
            VerifyTestResult (s.GetLength () == i);
        }
        for (size_t i = K; i > 0; i--) {
            s.Remove (i);
            VerifyTestResult (not s.Contains (i));
            VerifyTestResult (s.GetLength () == (i - 1));
        }
        VerifyTestResult (s.IsEmpty ());

        for (size_t i = 1; i <= K / 2; i++) {
            s += 1;
            VerifyTestResult (s.TallyOf (1) == i);
        }
        size_t oldLength = s.GetLength ();
        size_t oldTotal = s.TotalTally ();
        s += s;
        VerifyTestResult (s.GetLength () == oldLength);
        VerifyTestResult (s.TotalTally () == oldTotal * 2);
    }

    void    SimpleTallyTests (Tally<SimpleClass>& s)
    {
        SimpleClass three = 3;

        Tally<SimpleClass>  s1 (s);

        VerifyTestResult (s1 == s);
        VerifyTestResult (s1 == s);
        Tally<SimpleClass>  s2 = s1;

        VerifyTestResult (s2 == s);
        VerifyTestResult (s2 == s1);
        s2.Add (three);
        VerifyTestResult (s1 == s);
        VerifyTestResult (s2 != s1);

        VerifyTestResult (s.IsEmpty ());
        s.Add (three);
        VerifyTestResult (s.GetLength () == 1);
        s += three;
        VerifyTestResult (s.GetLength () == 1);
        VerifyTestResult (s.Contains (three));
        VerifyTestResult (s.TallyOf (three) == 2);
        s.Remove (three);
        VerifyTestResult (s.GetLength () == 1);
        VerifyTestResult (s.Contains (three));
        VerifyTestResult (s.TallyOf (three) == 1);
        s.Remove (three);
        VerifyTestResult (s.IsEmpty ());
        s.RemoveAll ();
        VerifyTestResult (s.IsEmpty ());
    }
#endif


    template <typename T>
    void    SimpleSequenceTest_1_ (Sequence<T>& s)
    {
        {
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            VerifyTestResult (s.size () == 1);
            VerifyTestResult (s.GetAt (0) == 1);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (s.size () == 1000);
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
        // NYI =- well a little implemnted - but terrible and needs alot of work
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
        // NYI
#if 0
        nonvirtual  void    Remove (size_t i);
        nonvirtual  void    Remove (size_t start, size_t end);
        nonvirtual  void    Remove (const Iterator<T>& i);
#endif
    }


    template <typename T>
    void    SimpleSequenceTest_10_STLCompatWrappers_ (Sequence<T>& s)
    {
        // NYI
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
    }

}


int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

