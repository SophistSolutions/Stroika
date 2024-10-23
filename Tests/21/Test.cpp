/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Containers::Sequence
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <list>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Sequence_stdvector.h"
#include "Stroika/Foundation/Containers/STL/Utilities.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::AsIntsEqualsComparer;
using Test::ArchtypeClasses::AsIntsLessComparer;
using Test::ArchtypeClasses::OnlyCopyableMoveable;
using Test::ArchtypeClasses::OnlyCopyableMoveableAndTotallyOrdered;

using Concrete::Sequence_Array;
using Concrete::Sequence_DoublyLinkedList;
using Concrete::Sequence_LinkedList;
using Concrete::Sequence_stdvector;

using COMPARE_OnlyCopyableMoveableAndTotallyOrdered = equal_to<OnlyCopyableMoveableAndTotallyOrdered>;
using COMPARE_OnlyCopyableMoveable                  = AsIntsEqualsComparer<OnlyCopyableMoveable>;

/**
 *
 *  @todo   Move more tests into SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ... so
 *          that case gets more testing. Even if we have to breakup existing tests into parts.
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

#if qStroika_HasComponent_googletest
namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_1_ ()
    {
        Debug::TraceContextBumper ctx{"{}::SimpleSequenceTest_1_ ()"};
        static const size_t       K = Debug::IsRunningUnderValgrind () ? 100 : 1000;
        {
            CONCRETE_SEQUENCE_T s;
            EXPECT_EQ (s.size (), 0u);
            s.Append (1);
            EXPECT_TRUE (s.size () == 1);
            EXPECT_TRUE (EQUALS_COMPARER{}(s.GetAt (0), 1));
            s.Append (2);
            EXPECT_TRUE (s.size () == 2);
            EXPECT_TRUE (EQUALS_COMPARER{}(s.GetAt (0), 1));
            EXPECT_TRUE (EQUALS_COMPARER{}(s.GetAt (1), 2));
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
        {
            CONCRETE_SEQUENCE_T s;
            for (size_t i = 0; i < K; ++i) {
                s.Append (i);
            }
            EXPECT_TRUE (s.size () == K);
            EXPECT_TRUE (EQUALS_COMPARER{}(s[0], 0));
            EXPECT_TRUE (EQUALS_COMPARER{}(s[1], 1));
            EXPECT_TRUE (EQUALS_COMPARER{}(s[K - 1], K - 1));
            s.Remove (0);
            EXPECT_TRUE (s.size () == K - 1);
            for (size_t i = 0; i < K - 1; ++i) {
                EXPECT_TRUE (EQUALS_COMPARER{}(s[i], i + 1));
            }
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_2_Contains_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_2_Contains_ ()"};
        {
            CONCRETE_SEQUENCE_T s;
            EXPECT_EQ (s.size (), 0u);
            EXPECT_TRUE (not s.template Contains<EQUALS_COMPARER> (1));
            s.Append (1);
            EXPECT_TRUE (s.template Contains<EQUALS_COMPARER> (1));
            s.RemoveAll ();
            EXPECT_TRUE (not s.template Contains<EQUALS_COMPARER> (1));
            EXPECT_TRUE (s.empty ());
        }
        {
            static const size_t K = Debug::IsRunningUnderValgrind () ? 50 : 1000;
            CONCRETE_SEQUENCE_T s;
            for (size_t i = 0; i < K; ++i) {
                s.Append (i + K);
            }
            for (size_t i = 0; i < K; ++i) {
                EXPECT_TRUE (not s.template Contains<EQUALS_COMPARER> (i));
                EXPECT_TRUE (s.template Contains<EQUALS_COMPARER> (i + K));
            }
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename WELL_ORDER_COMPARER>
    void SimpleSequenceTest_3_Compare_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_3_Compare_ ()"};
#if 0
        // This is RIGHT but We need a way to use 'TRAITS' to extend the definition of Sequence<T> or some such - to make this work...
        {
            EXPECT_TRUE (s.size () == 0);
            s.Append (1);
            Sequence<T> s2 = s;
            s2.Append (2);
            EXPECT_TRUE (s.Compare (s2) < 0);
            EXPECT_TRUE (s2.Compare (s) > 0);
            s.Append (2);
            EXPECT_TRUE (s2.Compare (s) == 0);
            s.RemoveAll ();
            EXPECT_TRUE (s.Compare (s2) < 0);
            EXPECT_TRUE (s.empty ());
        }
#endif
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, Common::IEqualsComparer<typename CONCRETE_SEQUENCE_T::value_type> EQUALS_COMPARER>
    void SimpleSequenceTest_4_Equals_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_4_Equals_ ()"};
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        // This is RIGHT but We need a way to use 'TRAITS' to extend the definition of Sequence<T> or some such - to make this work...
        {
            CONCRETE_SEQUENCE_T s;
            EXPECT_TRUE (s.size () == 0);
            s.Append (1);
            Sequence<T> s2 = s;
            s2.Append (2);
            auto useEqualsComparer = typename CONCRETE_SEQUENCE_T::template EqualsComparer<EQUALS_COMPARER>{};
            EXPECT_TRUE (not useEqualsComparer (s, s2));
            EXPECT_TRUE (not useEqualsComparer (s2, s));
            s.Append (2);
            EXPECT_TRUE (useEqualsComparer (s2, s));
            s.RemoveAll ();
            EXPECT_TRUE (not useEqualsComparer (s, s2));
            EXPECT_TRUE (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_5_RemoveAll_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_5_RemoveAll_ ()"};
        CONCRETE_SEQUENCE_T       s;
        EXPECT_TRUE (s.empty ());
        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
        static const size_t K = Debug::IsRunningUnderValgrind () ? 50 : 1000;
        for (size_t i = 0; i < K; ++i) {
            s.Append (i + K);
        }
        EXPECT_TRUE (not s.empty ());
        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
        s.RemoveAll ();
        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_6_GetSetAt_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_6_GetSetAt_ ()"};
        static const size_t       K = Debug::IsRunningUnderValgrind () ? 50 : 1000;
        CONCRETE_SEQUENCE_T       s;
        EXPECT_TRUE (s.empty ());
        for (size_t i = 0; i < K; ++i) {
            s.Append (1);
            EXPECT_TRUE (EQUALS_COMPARER{}(s.GetAt (i), 1));
            EXPECT_TRUE (EQUALS_COMPARER{}(s[i], 1));
        }
        for (size_t i = 0; i < K; ++i) {
            s.SetAt (i, 5000 + i);
            EXPECT_TRUE (EQUALS_COMPARER{}(s[i], 5000 + i));
        }
        for (size_t i = 0; i < K; ++i) {
            EXPECT_TRUE (EQUALS_COMPARER{}(s.GetAt (i), 5000 + i));
        }
        EXPECT_TRUE (not s.empty ());
        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_7_IndexOf_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_7_IndexOf_ ()"};
        static const size_t       K = Debug::IsRunningUnderValgrind () ? 100 : 1000;
        using T                     = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        {
            EXPECT_TRUE (s.empty ());
            for (size_t i = 0; i < K; ++i) {
                s.Append (21 + i);
            }
            EXPECT_TRUE (not s.template IndexOf<EQUALS_COMPARER> (5).has_value ());
            EXPECT_TRUE (not s.empty ());

            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
        {
            for (size_t i = 0; i < K; ++i) {
                s.Append (i);
            }
            EXPECT_TRUE (not s.empty ());
            EXPECT_TRUE (s.size () == K);

            Sequence<T> s2 = s;
            EXPECT_TRUE (s.template IndexOf<EQUALS_COMPARER> (s2) == 0u);
            EXPECT_TRUE (s2.template IndexOf<EQUALS_COMPARER> (s) == 0u);

            Sequence<T> s3;
            s3.Append (3);
            s3.Append (4);
            EXPECT_TRUE (not s3.template IndexOf<EQUALS_COMPARER> (s).has_value ());
            EXPECT_TRUE (s.template IndexOf<EQUALS_COMPARER> (s3) == 3u);
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
        {
            for (size_t i = 0; i < K; ++i) {
                s.Append (i);
            }
            EXPECT_TRUE (s.size () == K);
            size_t j = 0;
            for (Iterator<T> i = s.MakeIterator (); i != s.end (); ++i, ++j) {
                EXPECT_TRUE (s.IndexOf (i) == j);
            }
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_8_InsertAppendPrepend_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_8_InsertAppendPrepend_ ()"};
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        static const size_t K = Debug::IsRunningUnderValgrind () ? 50 : 1000;
        {
            for (size_t i = 0; i < K; ++i) {
                s.Append (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                EXPECT_TRUE (EQUALS_COMPARER{}(*i, j));
            }
            EXPECT_TRUE (s.size () == K);
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
        {
            for (size_t i = 0; i < K; ++i) {
                s.Prepend (i);
            }
            size_t j = 0;
            for (Iterator<T> i = s.begin (); i != s.end (); ++i, ++j) {
                EXPECT_TRUE (EQUALS_COMPARER{}(*i, K - j - 1));
            }
            EXPECT_TRUE (s.size () == K);
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }

        {
            // primitive, but at least something - test of Insert() of sequence (cuz Prepend/Append call Insert internally)
            Sequence<T> x;
            x.Append (10);
            x.Append (11);
            x.Append (12);

            s.PrependAll (x);
            EXPECT_TRUE (typename CONCRETE_SEQUENCE_T::template EqualsComparer<EQUALS_COMPARER>{}(s, x));
            s.AppendAll (x);
            EXPECT_TRUE (EQUALS_COMPARER{}(s[1], 11));
            EXPECT_TRUE (EQUALS_COMPARER{}(s[2], 12));
            EXPECT_TRUE (EQUALS_COMPARER{}(s[3], 10));
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_9_Update_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_9_Update_ ()"};
        CONCRETE_SEQUENCE_T       s;
        {
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end (); ++i) {
                s.Update (i, 5, &i);
            }
            for (auto i : s) {
                EXPECT_TRUE (EQUALS_COMPARER{}(i, 5));
            }
            s.SetAt (16, 16);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (EQUALS_COMPARER{}(*i, 16)) {
                    s.Update (i, 17, &i);
                }
            }
            EXPECT_TRUE (EQUALS_COMPARER{}(s[16], 17));
            for (auto i = s.begin (); i != s.end (); ++i) {
                EXPECT_TRUE (EQUALS_COMPARER{}(*i, 5) or s.IndexOf (i) == 16);
            }

            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
        }
        {
            // Update just 'fixes' iterator for re-use but we still hit every item
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            unsigned int cnt{};
            for (auto i = s.begin (); i != s.end (); ++i) {
                s.Update (i, cnt * 2, &i);
                ++cnt;
            }
            for (size_t i = 0; i < 100; ++i) {
                EXPECT_TRUE (EQUALS_COMPARER{}(s[i], i * 2));
            }
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_10_Remove_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_10_Remove_ ()"};
        CONCRETE_SEQUENCE_T       s;
        {
            EXPECT_TRUE (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5);
            EXPECT_TRUE (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    EXPECT_TRUE (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 1));
                }
            }
        }
        {
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            s.Remove (5, 95);
            EXPECT_TRUE (s.size () == 10);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    EXPECT_TRUE (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 90));
                }
            }
        }
        {
            s.RemoveAll ();
            EXPECT_TRUE (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            for (auto i = s.begin (); i != s.end ();) {
                // remove the 5th element
                if (s.IndexOf (i) == 5 and s.size () == 100) {
                    i = s.erase (i);
                }
                else {
                    ++i;
                }
            }
            EXPECT_TRUE (s.size () == 99);
            for (auto i = s.begin (); i != s.end (); ++i) {
                if (s.IndexOf (i) < 5) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(*i, s.IndexOf (i)));
                }
                else {
                    EXPECT_TRUE (EQUALS_COMPARER{}((*i), s.IndexOf (i) + 1));
                }
            }
            s.RemoveAll ();
        }
        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_11_STLCompatWrappers_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_11_STLCompatWrappers_ ()"};
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        {
            EXPECT_TRUE (s.empty ());
            for (size_t i = 0; i < 100; ++i) {
                s.Append (i);
            }
            {
                vector<T> vs;
                s.As (&vs);
                EXPECT_TRUE (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(T (i - vs.begin ()), *i));
                }
            }
            {
                vector<T> vs = s.template As<vector<T>> ();
                EXPECT_TRUE (vs.size () == 100);
                for (auto i = vs.begin (); i != vs.end (); ++i) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(T (i - vs.begin ()), *i));
                }
            }
            {
                list<T> vs;
                s.As (&vs);
                EXPECT_TRUE (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(T (idx), *i));
                }
            }
            {
                list<T> vs = s.template As<list<T>> ();
                EXPECT_TRUE (vs.size () == 100);
                int idx = 0;
                for (auto i = vs.begin (); i != vs.end (); ++i, idx++) {
                    EXPECT_TRUE (EQUALS_COMPARER{}(T (idx), *i));
                }
            }
        }
        {
            vector<T>   vs  = s.template As<vector<T>> ();
            Sequence<T> tmp = Sequence<T> (vs);
            EXPECT_TRUE (tmp.size () == vs.size ());
            EXPECT_TRUE (STL::equal (tmp.template As<vector<T>> (), vs, EQUALS_COMPARER{}));
        }
        {
            list<T>     ls  = s.template As<list<T>> ();
            Sequence<T> tmp = Sequence<T> (ls);
            EXPECT_TRUE (tmp.size () == ls.size ());
            EXPECT_TRUE (STL::equal (tmp.template As<list<T>> (), ls, EQUALS_COMPARER{}));
        }

        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_12_ToFromSTLVector_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_12_ToFromSTLVector_ ()"};
        using T = typename CONCRETE_SEQUENCE_T::value_type;
        CONCRETE_SEQUENCE_T s;
        EXPECT_TRUE (s.empty ());

        {
            vector<T> n;
            n.push_back (3);
            n.push_back (5);
            n.push_back (9);
            s            = CONCRETE_SEQUENCE_T (n);
            vector<T> nn = s.template As<vector<T>> ();
            EXPECT_TRUE (STL::equal (nn, n, EQUALS_COMPARER{}));
        }

        s.RemoveAll ();
        EXPECT_TRUE (s.empty ());
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T, typename EQUALS_COMPARER>
    void SimpleSequenceTest_13_Initializers_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_13_Initializers_ ()"};
        CONCRETE_SEQUENCE_T       s;
        EXPECT_TRUE (s.empty ());

        // fix - once we have on all subclasses - do be basic test for aech
        {
            Sequence<int> x1 = {};
            EXPECT_TRUE (x1.size () == 0);
            Sequence<int> x2 = {1, 2, 3};
            EXPECT_TRUE (x2.size () == 3);
            EXPECT_TRUE (x2[0] == 1);
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T>
    void SimpleSequenceTest_15_CompareForTypesWithCompare_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_15_CompareForTypesWithCompare_ ()"};
        CONCRETE_SEQUENCE_T       tmp;
        tmp.Append (1);
        tmp.Append (2);

        CONCRETE_SEQUENCE_T tmp2 = tmp;
        EXPECT_TRUE (not(tmp < tmp2));
        EXPECT_TRUE (not(tmp > tmp2));
        EXPECT_TRUE (tmp <= tmp2);
        EXPECT_TRUE (tmp >= tmp2);
        EXPECT_EQ (tmp, tmp2);
        tmp.Append (3);
        EXPECT_TRUE (tmp > tmp2);
        tmp2.Append (4);
        EXPECT_TRUE (tmp < tmp2);
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_T>
    void SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_ ()"};
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

        auto counter = [] (SEQ s) -> size_t {
            size_t cnt = 0;
            for ([[maybe_unused]] auto i : s) {
                ++cnt;
            }
            return cnt;
        };
        auto counterI = [] (Iterator<ELTTYPE> i) -> size_t {
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
        EXPECT_TRUE (counter (tmp) == 3);
        EXPECT_TRUE (counterI (tmp.begin ()) == 3);

        CONCRETE_SEQUENCE_T tmp2 = tmp;
        EXPECT_TRUE (counter (tmp) == 3);
        EXPECT_TRUE (counterI (tmp.begin ()) == 3);
        EXPECT_TRUE (counter (tmp2) == 3);
        EXPECT_TRUE (counterI (tmp2.begin ()) == 3);

        {
            Iterator<ELTTYPE> i = tmp.begin ();
            tmp2.RemoveAll ();
            EXPECT_TRUE (counter (tmp) == 3);
            EXPECT_TRUE (counterI (tmp.begin ()) == 3);
            EXPECT_TRUE (counterI (i) == 3);
            EXPECT_TRUE (counter (tmp2) == 0);
            EXPECT_TRUE (counterI (tmp2.begin ()) == 0);
        }
    }
}

namespace {
    template <typename CONCRETE_SEQUENCE_TYPE, typename EQUALS_COMPARER>
    void SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_ ()"};
        SimpleSequenceTest_5_RemoveAll_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_6_GetSetAt_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_9_Update_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_10_Remove_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_13_Initializers_<CONCRETE_SEQUENCE_TYPE, EQUALS_COMPARER> ();
        SimpleSequenceTest_16_IteratorOwnerRespectedOnCopy_<CONCRETE_SEQUENCE_TYPE> ();
    }

    template <typename CONCRETE_SEQUENCE_TYPE, Common::IEqualsComparer<typename CONCRETE_SEQUENCE_TYPE::value_type> EQUALS_COMPARER>
    void SimpleSequenceTest_All_For_Type_ ()
    {
        Debug::TraceContextBumper traceCtx{"{}::SimpleSequenceTest_All_For_Type_ ()"};
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

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, DEFAULT_FACTORY)
    {
        Debug::TraceContextBumper ctx{"{}::DEFAULT_FACTORY"};
        SimpleSequenceTest_All_For_Type_<Sequence<size_t>, equal_to<size_t>> ();
        SimpleSequenceTest_All_For_Type_<Sequence<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_OnlyCopyableMoveableAndTotallyOrdered> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_All_For_Type_<Sequence<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence<size_t>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, Sequence_Array)
    {
        Debug::TraceContextBumper ctx{"{}::Sequence_Array"};
        SimpleSequenceTest_All_For_Type_<Sequence_Array<size_t>, equal_to<size_t>> ();
        SimpleSequenceTest_All_For_Type_<Sequence_Array<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_OnlyCopyableMoveableAndTotallyOrdered> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_Array<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_All_For_Type_<Sequence_Array<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_Array<size_t>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, Sequence_DoublyLinkedList)
    {
        Debug::TraceContextBumper ctx{"{}::Sequence_DoublyLinkedList"};
        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<size_t>, equal_to<size_t>> ();
        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_OnlyCopyableMoveableAndTotallyOrdered> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_All_For_Type_<Sequence_DoublyLinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_DoublyLinkedList<size_t>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, Sequence_LinkedList)
    {
        Debug::TraceContextBumper ctx{"{}::Sequence_LinkedList"};
        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<size_t>, equal_to<size_t>> ();
        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_OnlyCopyableMoveableAndTotallyOrdered> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_LinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_All_For_Type_<Sequence_LinkedList<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_LinkedList<size_t>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, Sequence_stdvector)
    {
        Debug::TraceContextBumper ctx{"{}::Sequence_stdvector"};
        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<size_t>, equal_to<size_t>> ();
        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<OnlyCopyableMoveableAndTotallyOrdered>, COMPARE_OnlyCopyableMoveableAndTotallyOrdered> ();
        SimpleSequenceTest_AllTestsWhichDontRequireComparer_For_Type_<Sequence_stdvector<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_All_For_Type_<Sequence_stdvector<OnlyCopyableMoveable>, COMPARE_OnlyCopyableMoveable> ();
        SimpleSequenceTest_15_CompareForTypesWithCompare_<Sequence_stdvector<size_t>> ();
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, SimpleSequenceTest_stdinitializer_complexType_)
    {
        Debug::TraceContextBumper ctx{"{}::SimpleSequenceTest_stdinitializer_complexType_"};
        using Characters::String;
        struct StructureFieldInfo_ {
            size_t     fOffset;
            type_index fTypeInfo;
            String     fSerializedFieldName;
            StructureFieldInfo_ (size_t fieldOffset = 0, type_index typeInfo = typeid (void), const String& serializedFieldName = {})
                : fOffset{fieldOffset}
                , fTypeInfo{typeInfo}
                , fSerializedFieldName{serializedFieldName}
            {
            }
        };
        Sequence<StructureFieldInfo_> t  = {StructureFieldInfo_{0, typeid (int), "fred"}};
        Sequence<StructureFieldInfo_> tt = t;
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, SequenceIndexing_)
    {
        Debug::TraceContextBumper ctx{"{}::SequenceIndexing_"};
        {
            Sequence<int> a;
            a += 1;
            [[maybe_unused]] int a0 = a[0];
            // Should fail to compile
            //a[0]   = 3; --- @todo - @see and maybe use http://stroika-bugs.sophists.com/browse/STK-583 AssertDoesntCompile
        }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        {
            Sequence<int> a;
            a += 1;
            int a0 = a[0];
            EXPECT_TRUE (a0 == 1);
            a (0) = 3;
            EXPECT_TRUE (a (0) == 3);
        }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        {
            Sequence<int> a;
            a += 1;
            int a0 = a[0];
            EXPECT_TRUE (a0 == 1);
            a[0] = 3;
            EXPECT_TRUE (a[0] == 3);
        }
#endif
        {
            using Characters::String;
            Sequence<String> a;
            a += "1";
            String a0 = a[0];
            // Should fail to compile
            //a[0]   = 3; --- @todo - @see and maybe use http://stroika-bugs.sophists.com/browse/STK-583 AssertDoesntCompile
        }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        {
            using Characters::String;
            Sequence<String> a;
            a += "1";
            EXPECT_TRUE (a (0) == "1");
            String a0 = a[0];
            a (0)     = "3";
            EXPECT_TRUE (a (0) == "3");
        }
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            String a0 = a[0];
            a (0)     = L"3";
            EXPECT_TRUE (a (0).Contains (L"3")); // can call '.' methods on result of a(n)
        }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        {
            using Characters::String;
            Sequence<String> a;
            a += L"1";
            EXPECT_TRUE (a[0] == "1");
            String a0 = a[0];
            a[0]      = L"3";
            EXPECT_TRUE (a[0] == "3");
        }
        {
            using Characters::String;
            Sequence<String> a;
            a += "1";
            String a0 = a[0];
            a[0]      = "3";
            EXPECT_TRUE (a[0].Contains ("3")); // can call '.' methods on result of a(n)
        }
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, ExampleCTORS_Tests_)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleCompare_Tests_"};
        // From Sequence<> CTOR docs
        Collection<int> c;
        vector<int>     v;

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

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, ExampleCompare_Tests_)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleCompare_Tests_"};
        // From Sequence<> CTOR docs
        {
            Sequence<int> s1 = {1, 2, 3};
            Sequence<int> s2 = s1;
            if (s1 == s2) {
            }
            if (s1 != s2) {
            }
            if (s1 < s2) {
            }
            if (s1 <= s2) {
            }
        }
        {
            Sequence<int> s1 = {1, 2, 3};
            Sequence<int> s2 = {4, 5, 6};
            if (s1 == s2) {
                EXPECT_TRUE (false);
            }
// todo get this type deduction working
#if 0
                if (not Sequence<int>::EqualsComparer{[](int l, int r) { return l % 3 == r % 3; }}(s1, s2)) {
                    EXPECT_TRUE (false);
                }
#endif
            auto cmp = Common::DeclareEqualsComparer ([] (int l, int r) { return l % 3 == r % 3; });
            if (not Sequence<int>::EqualsComparer<decltype (cmp)>{cmp}(s1, s2)) {
                EXPECT_TRUE (false);
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, ExampleOrderBy_)
    {
        Debug::TraceContextBumper ctx{"{}::ExampleOrderBy_"};
        // From Sequence<> CTOR docs
        {
            Sequence<int> c{3, 5, 9, 38, 3, 5};
            EXPECT_EQ (c.OrderBy (), (Sequence<int>{3, 3, 5, 5, 9, 38}));
            EXPECT_TRUE (c.OrderBy ().IsOrderedBy ());
        }
        {
            Sequence<int> c{3, 5, 9, 38, 3, 5};
            auto          cmp = [] (int lhs, int rhs) -> bool { return lhs < rhs; };
            EXPECT_EQ (c.OrderBy (cmp), (Sequence<int>{3, 3, 5, 5, 9, 38}));
            EXPECT_TRUE (c.OrderBy (cmp).IsOrderedBy (cmp));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, BugWithWhereCallingAdd_)
    {
        Debug::TraceContextBumper ctx{"{}::BugWithWhereCallingAdd_"};
        using IO::Network::InternetAddress;
        Sequence<InternetAddress> s1;
        auto s2 = s1.Where ([] (InternetAddress i) { return i.GetAddressFamily () == InternetAddress::AddressFamily::V4; });
        EXPECT_EQ (s2.size (), 0u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, Sequence_Array_ArrayAPITests_)
    {
        Debug::TraceContextBumper ctx{"{}::Sequence_Array_ArrayAPITests_"};
        Sequence_Array<int>       a;
        a.reserve (3);
        EXPECT_EQ (a.capacity (), 3u);
        a.shrink_to_fit ();
        EXPECT_EQ (a.capacity (), 0u);
    }
}

namespace {
    GTEST_TEST (Foundation_Containers_Sequence, CLEANUPS)
    {
        EXPECT_TRUE (OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount () == 0 and OnlyCopyableMoveable::GetTotalLiveCount () == 0); // simple portable leak check
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}