/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Containers::Sequence
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <list>
#include <sstream>
#include <typeindex>
#include <vector>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/Containers/STL/Compare.h"
#include "Stroika/Foundation/Containers/STL/Utilities.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using Concrete::Sequence_Array;
using Concrete::Sequence_DoublyLinkedList;
using Concrete::Sequence_LinkedList;
using Concrete::Sequence_stdvector;

/**
 *
 *  @todo   Move more tests into SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ... so
 *          that case gets more testing. Even if we have to breakup exsiting tests into parts.
 *
 *  @todo   Fix COMPARE (test case 3) support
 *
 *  @todo   Look at Container tests for Bag/MultiSet, and see if any of them can be lifted and applied here.
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
    void SimpleSequenceTest_1_ ()
    {
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        {
            Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_1_ ()");
            CONCRETE_SEQUENCE_T       s;
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            VerifyTestResult (s.size () == 1);
            VerifyTestResult (EQUALS_COMPARER{}(s.GetAt (0), 1));
            s.Append (2);
            VerifyTestResult (s.size () == 2);
            VerifyTestResult (EQUALS_COMPARER{}(s.GetAt (0), 1));
            VerifyTestResult (EQUALS_COMPARER{}(s.GetAt (1), 2));
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            CONCRETE_SEQUENCE_T s;
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (i);
            }
            VerifyTestResult (s.size () == 1000);
            VerifyTestResult (EQUALS_COMPARER{}(s[0], 0));
            VerifyTestResult (EQUALS_COMPARER{}(s[1], 1));
            VerifyTestResult (EQUALS_COMPARER{}(s[100], 100));
            s.Remove (0);
            VerifyTestResult (s.size () == 999);
            for (size_t i = 0; i < 999; ++i) {
                VerifyTestResult (EQUALS_COMPARER{}(s[i], i + 1));
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }
}

namespace {

    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_2_Contains_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_2_Contains_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        {
            CONCRETE_SEQUENCE_T s;
            VerifyTestResult (s.size () == 0);
            VerifyTestResult (not s.template Contains<EQUALS_COMPARER> (1));
            s.Append (1);
            VerifyTestResult (s.template Contains<EQUALS_COMPARER> (1));
            s.RemoveAll ();
            VerifyTestResult (not s.template Contains<EQUALS_COMPARER> (1));
            VerifyTestResult (s.empty ());
        }
        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 100 : 1000;
            CONCRETE_SEQUENCE_T s;
            for (size_t i = 0; i < K; ++i) {
                s.Append (i + K);
            }
            for (size_t i = 0; i < K; ++i) {
                VerifyTestResult (not s.template Contains<EQUALS_COMPARER> (i));
                VerifyTestResult (s.template Contains<EQUALS_COMPARER> (i + K));
            }
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }
}

namespace {

    template <typename CONCRETE_SEQUENCE_T, typename WELL_ORDER_COMPARER>
    void SimpleSequenceTest_3_Compare_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_3_Compare_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
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
    void SimpleSequenceTest_4_Equals_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_4_Equals_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        // This is RIGHT but We need a way to use 'TRAITS' to extend the defintiion of Sequence<T> or some such - to make this work...
        {
            CONCRETE_SEQUENCE_T s;
            VerifyTestResult (s.size () == 0);
            s.Append (1);
            Sequence<T> s2 = s;
            s2.Append (2);
            VerifyTestResult (not s.template Equals<EQUALS_COMPARER> (s2));
            VerifyTestResult (not s2.template Equals<EQUALS_COMPARER> (s));
            s.Append (2);
            VerifyTestResult (s2.template Equals<EQUALS_COMPARER> (s));
            s.RemoveAll ();
            VerifyTestResult (not s.template Equals<EQUALS_COMPARER> (s2));
            VerifyTestResult (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_5_RemoveAll_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_5_RemoveAll_ ()");
        CONCRETE_SEQUENCE_T       s;
        VerifyTestResult (s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
        static const size_t K = Debug::IsRunningUnderValgrind () ? 200 : 1000;
        for (size_t i = 0; i < K; ++i) {
            s.Append (i + K);
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
    void SimpleSequenceTest_6_GetSetAt_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_6_GetSetAt_ ()");
        static const size_t       K = Debug::IsRunningUnderValgrind () ? 100 : 1000;
        CONCRETE_SEQUENCE_T       s;
        VerifyTestResult (s.empty ());
        for (size_t i = 0; i < K; ++i) {
            s.Append (1);
            VerifyTestResult (EQUALS_COMPARER{}(s.GetAt (i), 1));
            VerifyTestResult (EQUALS_COMPARER{}(s[i], 1));
        }
        for (size_t i = 0; i < K; ++i) {
            s.SetAt (i, 5000 + i);
            VerifyTestResult (EQUALS_COMPARER{}(s[i], 5000 + i));
        }
        for (size_t i = 0; i < K; ++i) {
            VerifyTestResult (EQUALS_COMPARER{}(s.GetAt (i), 5000 + i));
        }
        VerifyTestResult (not s.empty ());
        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_7_IndexOf_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_7_IndexOf_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 1000; ++i) {
                s.Append (21 + i);
            }
            VerifyTestResult (s.template IndexOf<EQUALS_COMPARER> (5).IsMissing ());
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
            VerifyTestResult (s.template IndexOf<EQUALS_COMPARER> (s2) == 0);
            VerifyTestResult (s2.template IndexOf<EQUALS_COMPARER> (s) == 0);

            Sequence<T> s3;
            s3.Append (3);
            s3.Append (4);
            VerifyTestResult (s3.template IndexOf<EQUALS_COMPARER> (s).IsMissing ());
            VerifyTestResult (s.template IndexOf<EQUALS_COMPARER> (s3) == 3);
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
    void SimpleSequenceTest_8_InsertAppendPrepend_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_8_InsertAppendPrepend_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        static const size_t K = Debug::IsRunningUnderValgrind () ? 100 : 1000;
        {
            for (size_t i = 0; i < K; ++i) {
                s.Append (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                VerifyTestResult (EQUALS_COMPARER{}(*i, j));
            }
            VerifyTestResult (s.size () == K);
            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
        {
            for (size_t i = 0; i < K; ++i) {
                s.Prepend (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                VerifyTestResult (EQUALS_COMPARER{}(*i, K - j - 1));
            }
            VerifyTestResult (s.size () == K);
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
            VerifyTestResult (s.template Equals<EQUALS_COMPARER> (x));
            s.AppendAll (x);
            VerifyTestResult (EQUALS_COMPARER{}(s[1], 11));
            VerifyTestResult (EQUALS_COMPARER{}(s[2], 12));
            VerifyTestResult (EQUALS_COMPARER{}(s[3], 10));
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_9_Update_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_9_Update_ ()");
        CONCRETE_SEQUENCE_T       s;
        {
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end (); ++i) {
                s.Update (i, 5);
            }
            for (auto i : s) {
                VerifyTestResult (EQUALS_COMPARER{}(i, 5));
            }
            s.SetAt (16, 16);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (EQUALS_COMPARER{}(*i, 16)) {
                    s.Update (i, 17);
                }
            }
            VerifyTestResult (EQUALS_COMPARER{}(s[16], 17));
            for (auto i = s.begin (); i != s.end (); ++i) {
                VerifyTestResult (EQUALS_COMPARER{}(*i, 5) or s.IndexOf (i) == 16);
            }

            s.RemoveAll ();
            VerifyTestResult (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_10_Remove_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_10_Remove_ ()");
        CONCRETE_SEQUENCE_T       s;
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5);
            VerifyTestResult (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    VerifyTestResult (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 1));
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
                    VerifyTestResult (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 90));
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
                    VerifyTestResult (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    VerifyTestResult (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 1));
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
    void SimpleSequenceTest_11_STLCompatWrappers_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_11_STLCompatWrappers_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        {
            VerifyTestResult (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            {
                vector<T> vs;
                s.As (&vs);
                VerifyTestResult (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    VerifyTestResult (EQUALS_COMPARER{}(T (i - vs.begin ()), *i));
                }
            }
            {
                vector<T> vs = s.template As<vector<T>> ();
                VerifyTestResult (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    VerifyTestResult (EQUALS_COMPARER{}(T (i - vs.begin ()), *i));
                }
            }
            {
                list<T> vs;
                s.As (&vs);
                VerifyTestResult (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    VerifyTestResult (EQUALS_COMPARER{}(T (idx), *i));
                }
            }
            {
                list<T> vs = s.template As<list<T>> ();
                VerifyTestResult (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    VerifyTestResult (EQUALS_COMPARER{}(T (idx), *i));
                }
            }
        }
        {
            vector<T>   vs  = s.template As<vector<T>> ();
            Sequence<T> tmp = Sequence<T> (vs);
            VerifyTestResult (tmp.size () == vs.size ());
            VerifyTestResult (STL::equal (tmp.template As<vector<T>> (), vs, EQUALS_COMPARER{}));
        }
        {
            list<T>     ls  = s.template As<list<T>> ();
            Sequence<T> tmp = Sequence<T> (ls);
            VerifyTestResult (tmp.size () == ls.size ());
            VerifyTestResult (STL::equal (tmp.template As<list<T>> (), ls, EQUALS_COMPARER{}));
        }

        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_12_ToFromSTLVector_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_12_ToFromSTLVector_ ()");
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        VerifyTestResult (s.empty ());

        {
            vector<T> n;
            n.push_back (3);
            n.push_back (5);
            n.push_back (9);
            s            = CONCRETE_SEQUENCE_T (n);
            vector<T> nn = s.template As<vector<T>> ();
            VerifyTestResult (STL::equal (nn, n, EQUALS_COMPARER{}));
        }

        s.RemoveAll ();
        VerifyTestResult (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_13_Initializers_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_13_Initializers_ ()");
        CONCRETE_SEQUENCE_T       s;
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        VerifyTestResult (s.empty ());

        // fix - once we have on all subclasses - do be basic test for aech
        {
            Sequence<int> x1 = {};
            VerifyTestResult (x1.size () == 0);
            Sequence<int> x2 = {1, 2, 3};
            VerifyTestResult (x2.size () == 3);
            VerifyTestResult (x2[0] == 1);
        }
    }
}

namespace {
    void SimpleSequenceTest_14_Sequence_stdinitializer_complexType_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_14_Sequence_stdinitializer_complexType_ ()");
        using Characters::String;
        struct StructureFieldInfo_ {
            size_t     fOffset;
            type_index fTypeInfo;
            String     fSerializedFieldName;

            StructureFieldInfo_ (size_t fieldOffset = 0, type_index typeInfo = typeid (void), const String& serializedFieldName = String ())
                : fOffset (fieldOffset)
                , fTypeInfo (typeInfo)
                , fSerializedFieldName (serializedFieldName)
            {
            }
        };
        Sequence<StructureFieldInfo_> t  = {StructureFieldInfo_ (0, typeid (int), L"fred")};
        Sequence<StructureFieldInfo_> tt = t;
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T>
    void SimpleSequenceTest_15_CompareForTypesWithCompare_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_15_CompareForTypesWithCompare_ ()");
        CONCRETE_SEQUENCE_T       tmp;
        tmp.Append (1);
        tmp.Append (2);

        CONCRETE_SEQUENCE_T tmp2 = tmp;
        VerifyTestResult (not(tmp < tmp2));
        VerifyTestResult (not(tmp > tmp2));
        VerifyTestResult (tmp <= tmp2);
        VerifyTestResult (tmp >= tmp2);
        VerifyTestResult (tmp == tmp2);
        tmp.Append (3);
        VerifyTestResult (tmp > tmp2);
        tmp2.Append (4);
        VerifyTestResult (tmp < tmp2);
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T>
    void SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_ ()");
        // TESTS NEW IteratorOwner functionality - so when we make changes to shared container,
        // rep, iteration sticks to the envelope (owner)
        //
        // Pretty sure this test would have failed for the last 25 years, and was just fixed around
        // Stroika v2.0a18
        //
        //  @todo   consider putting a variation on this test in Collection<T> and other
        //          container-type regtests?
        //
        using SEQ     = typename CONCRETE_SEQUENCE_T::ArchetypeContainerType;
        using ELTTYPE = typename CONCRETE_SEQUENCE_T::value_type;

        auto counter = [](SEQ s) -> size_t {
            size_t cnt = 0;
            for ([[maybe_unused]] auto i : s) {
                Lambda_Arg_Unused_BWA (i);
                ++cnt;
            }
            return cnt;
        };
        auto counterI = [](Iterator<ELTTYPE> i) -> size_t {
            size_t cnt = 0;
            while (i) {
                ++cnt;
                ++i;
            }
            return cnt;
        };

        CONCRETE_SEQUENCE_T tmp;
        tmp.Append (1);
        tmp.Append (2);
        tmp.Append (3);
        VerifyTestResult (counter (tmp) == 3);
        VerifyTestResult (counterI (tmp.begin ()) == 3);

        CONCRETE_SEQUENCE_T tmp2 = tmp;
        VerifyTestResult (counter (tmp) == 3);
        VerifyTestResult (counterI (tmp.begin ()) == 3);
        VerifyTestResult (counter (tmp2) == 3);
        VerifyTestResult (counterI (tmp2.begin ()) == 3);

        {
            Iterator<ELTTYPE> i = tmp.begin ();
            tmp2.RemoveAll ();
            VerifyTestResult (counter (tmp) == 3);
            VerifyTestResult (counterI (tmp.begin ()) == 3);
            VerifyTestResult (counterI (i) == 3);
            VerifyTestResult (counter (tmp2) == 0);
            VerifyTestResult (counterI (tmp2.begin ()) == 0);
        }
    }
}

namespace {

    template <typename CONCRETE_SEQUENCE_TYPE, typename EQUALS_COMPARER>
    void SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ()");
        SimpleSequenceTest_5_RemoveAll_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_6_GetSetAt_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_9_Update_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_10_Remove_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_13_Initializers_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_<CONCRETE_SEQUENCE_TYPE> ();
    }

    template <typename CONCRETE_SEQUENCE_TYPE, typename EQUALS_COMPARER>
    void SimpleSequenceTest_All_For_Type_ ()
    {
        Debug::TraceContextBumper traceCtx ("{}::SimpleSequenceTest_All_For_Type_ ()");
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_1_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_2_Contains_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_3_Compare_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_4_Equals_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_7_IndexOf_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_8_InsertAppendPrepend_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_11_STLCompatWrappers_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_12_ToFromSTLVector_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
    }
}

namespace SequenceIndexing_Test_16_ {
    void DoTest ()
    {
        {
            Sequence<int> a;
            a += 1;
            [[maybe_unused]] int a0 = a[0];
            // Should fail to compile
            //a[0]   = 3; --- @todo - @see and maybe use https://stroika.atlassian.net/browse/STK-583 AssertDoesntCompile
        }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        {
            Sequence<int> a;
            a += 1;
            int a0 = a[0];
            VerifyTestResult (a0 == 1);
            a (0) = 3;
            VerifyTestResult (a (0) == 3);
        }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        {
            Sequence<int> a;
            a += 1;
            int a0 = a[0];
            VerifyTestResult (a0 == 1);
            a[0] = 3;
            VerifyTestResult (a[0] == 3);
        }
#endif
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            String a0 = a[0];
            // Should fail to compile
            //a[0]   = 3; --- @todo - @see and maybe use https://stroika.atlassian.net/browse/STK-583 AssertDoesntCompile
        }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            VerifyTestResult (a (0) == L"1");
            String a0 = a[0];
            a (0)     = L"3";
            VerifyTestResult (a (0) == L"3");
        }
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            String a0 = a[0];
            a (0)     = L"3";
            VerifyTestResult (a (0).Contains (L"3")); // can call '.' methods on result of a(n)
        }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            VerifyTestResult (a[0] == L"1");
            String a0 = a[0];
            a[0]      = L"3";
            VerifyTestResult (a[0] == L"3");
        }
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            String a0 = a[0];
            a[0]      = L"3";
            VerifyTestResult (a[0].Contains (L"3")); // can call '.' methods on result of a(n)
        }
#endif
    }
}

namespace {
    namespace ExampleCTORS_Test_17_ {
        void DoTest ()
        {
            // From Sequence<> CTOR docs
            Collection<int>  c;
            std::vector<int> v;

            Sequence<int> s1 = {1, 2, 3};
            Sequence<int> s2 = s1;
            Sequence<int> s3{s1};
            Sequence<int> s4{s1.begin (), s1.end ()};
            Sequence<int> s5{c};
            Sequence<int> s6{v};
            Sequence<int> s7{v.begin (), v.end ()};
            Sequence<int> s8{move (s1)};
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        using COMPARE_SIZET       = std::equal_to<size_t>;
        using COMPARE_SimpleClass = std::equal_to<SimpleClass>;
        struct COMPARE_SimpleClassWithoutComparisonOperators {
            using value_type = SimpleClassWithoutComparisonOperators;
            bool operator() (value_type v1, value_type v2) const
            {
                return v1.GetValue () == v2.GetValue ();
            }
        };

        SimpleSequenceTest_All_For_Type_<Sequence<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type_<Sequence<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleSequenceTest_All_For_Type_<Sequence<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_All_For_Type_<Sequence_Array<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type_<Sequence_Array<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_Array<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleSequenceTest_All_For_Type_<Sequence_Array<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_DoublyLinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_LinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<size_t>, COMPARE_SIZET> ();
        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<SimpleClass>, COMPARE_SimpleClass> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_stdvector<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();
        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<SimpleClassWithoutComparisonOperators>, COMPARE_SimpleClassWithoutComparisonOperators> ();

        SimpleSequenceTest_14_Sequence_stdinitializer_complexType_ ();

        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence<size_t>> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_Array<size_t>> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_DoublyLinkedList<size_t>> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_LinkedList<size_t>> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_stdvector<size_t>> ();

        SequenceIndexing_Test_16_::DoTest ();

        ExampleCTORS_Test_17_::DoTest ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
