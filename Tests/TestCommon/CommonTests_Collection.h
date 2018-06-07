/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

/**
 *  @todo   SimpleCollectionTest_TestsWhichRequireEquals() never called
 */

namespace CommonTests {
    namespace CollectionTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace Test1_OldMiscBagTests_ {

            template <typename CONCRETE_CONTAINER>
            void IteratorTests_ (CONCRETE_CONTAINER& s)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;

                const size_t kTestSize = 100;

                VerifyTestResult (s.GetLength () == 0);
                /*
                * Try removes while iterating forward.
                */
                {
                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (i);
                    }
                    VerifyTestResult (s.GetLength () == kTestSize);

                    IterableTests::SimpleIterableTest_All_For_Type<CONCRETE_CONTAINER> (s);

                    VerifyTestResult (s.GetLength () == kTestSize);
                    s.RemoveAll ();
                    VerifyTestResult (s.size () == 0);
                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }

                    s.clear ();

                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    VerifyTestResult (s.GetLength () == kTestSize);

                    s.clear ();

                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    VerifyTestResult (s.GetLength () == kTestSize);

                    s.clear ();
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
                }

                s.RemoveAll ();
            }

            template <typename CONCRETE_CONTAINER>
            void CollectionTimings_ (CONCRETE_CONTAINER& s)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;
#if qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t> of length " << s.GetLength () << endl;
#endif

                for (size_t i = 1; i <= s.GetLength (); i++) {
                    for (T it : s) {
                    }
                }
                for (T it : s) {
                    for (T it1 : s) {
                        s.RemoveAll ();
                    }
                }
                VerifyTestResult (s.IsEmpty ());
                VerifyTestResult (s.GetLength () == 0);

                for (T it1 : s) {
                    for (T it2 : s) {
                        VerifyTestResult (false);
                    }
                }

#if qPrintTimings
                t = GetCurrentTime () - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename CONCRETE_CONTAINER>
            void On_Container_ (CONCRETE_CONTAINER& s)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;
                size_t                                          three = 3;

                typename CONCRETE_CONTAINER::ArchetypeContainerType s1 (s);

                typename CONCRETE_CONTAINER::ArchetypeContainerType s2 = s1;

                s2.Add (three);

                IteratorTests_ (s);

                static const size_t K = qDebug ? (Debug::IsRunningUnderValgrind () ? 50 : 200) : 500;

                size_t i;

                VerifyTestResult (s.IsEmpty ());
                s.Add (three);
                VerifyTestResult (s.GetLength () == 1);
                s += three;
                VerifyTestResult (s.GetLength () == 2);
                s.RemoveAll ();
                VerifyTestResult (s.IsEmpty ());

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                }
                CollectionTimings_ (s);
                VerifyTestResult (s.IsEmpty ());

#if qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t>..." << endl;
#endif

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                    VerifyTestResult (s.GetLength () == i);
                }
#if qPrintTimings
                t = GetCurrentTime () - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, [[maybe_unused]] TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::Test1_OldMiscBagTests_"};
                CONCRETE_CONTAINER        s = factory ();
                On_Container_<CONCRETE_CONTAINER> (s);
            }
        }

        namespace Test2_TestsWithComparer_ {
            template <typename CONCRETE_CONTAINER, typename TEST_FUNCTION, typename EQUALS_COMPARER>
            void On_Container_ (CONCRETE_CONTAINER& s, TEST_FUNCTION applyToContainer, EQUALS_COMPARER equals_comparer)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;
                typedef typename CONCRETE_CONTAINER::TraitsType TraitsType;
                size_t                                          three = 3;

                applyToContainer (s);
                typename CONCRETE_CONTAINER::ArchetypeContainerType s1 (s);
                applyToContainer (s1);

                typename CONCRETE_CONTAINER::ArchetypeContainerType s2 = s1;
                applyToContainer (s2);

                s2.Add (three);
                applyToContainer (s);
                applyToContainer (s1);
                applyToContainer (s2);

                IteratorTests_ (s);
                applyToContainer (s);

#if qDebug
                const size_t K = 200;
#else
                const size_t K = 500;
#endif
                size_t i;

                VerifyTestResult (s.IsEmpty ());
                s.Add (three);
                VerifyTestResult (s.GetLength () == 1);
                s += three;
                VerifyTestResult (s.GetLength () == 2);
                VerifyTestResult (s.Contains (three));
                s.Remove (three);
                VerifyTestResult (s.GetLength () == 1);
                s.RemoveAll ();
                VerifyTestResult (s.IsEmpty ());

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                    applyToContainer (s);
                }
                applyToContainer (s);
                CollectionTimings_ (s);
                applyToContainer (s);
                VerifyTestResult (s.IsEmpty ());

#if qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t>..." << endl;
#endif

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                    VerifyTestResult (s.Contains (i));
                    VerifyTestResult (s.GetLength () == i);
                }
#if qPrintTimings
                t = GetCurrentTime () - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename CONCRETE_CONTAINER, typename TEST_FUNCTION, typename EQUALS_COMPARER>
            void DoAllTests_ (TEST_FUNCTION applyToContainer, EQUALS_COMPARER equals_comparer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::Test2_TestsWithComparer_"};
                CONCRETE_CONTAINER        s;
                On_Container_<CONCRETE_CONTAINER> (s, applyToContainer);
            }
        }

        namespace Test4_IteratorsBasics_ {

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
            void BasicIteratorTest_ (CONCRETE_CONTAINER_FACTORY factory)
            {
                CONCRETE_CONTAINER                                       collection = factory ();
                [[maybe_unused]] typename CONCRETE_CONTAINER::value_type t1         = 1;
                [[maybe_unused]] typename CONCRETE_CONTAINER::value_type t2         = 2;
                [[maybe_unused]] typename CONCRETE_CONTAINER::value_type t3         = 3;
                VerifyTestResult (collection.IsEmpty ());
                collection.Add (t1);
                collection.Add (t1);
#if 0
                {
                    CONCRETE_CONTAINER   bb = collection;
                    VerifyTestResult (bb.MakeIterator () != collection.MakeIterator ());
                    VerifyTestResult (bb.MakeIterator () != bb.MakeIterator ());        // WE may want to change the definition so this is allowed (-- LGP 2012-07-30)
                }
#endif
                {
                    Iterator<typename CONCRETE_CONTAINER::value_type> i  = collection.begin ();
                    Iterator<typename CONCRETE_CONTAINER::value_type> ii = i;
                    VerifyTestResult (i == ii);
                    VerifyTestResult (i != collection.end ()); // because bag wasn't empty
                    ++i;
                    ++ii;
                    VerifyTestResult (i == ii);
                }
                {
                    VerifyTestResult (collection.size () == 2); // cuz we said so above
                    Iterator<typename CONCRETE_CONTAINER::value_type> i = collection.begin ();
                    VerifyTestResult (not i.Done ());
                    VerifyTestResult (i != collection.end ());
                    ++i;
                    VerifyTestResult (not i.Done ());
                    VerifyTestResult (i != collection.end ());
                    ++i;
                    VerifyTestResult (i.Done ());
                    VerifyTestResult (i == collection.end ());
                }
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::Test4_IteratorsBasics_"};
                BasicIteratorTest_<CONCRETE_CONTAINER> (factory);
            }
        }

        namespace Test5_Apply_ {

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoIt_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;
                CONCRETE_CONTAINER                              b = factory ();

                constexpr int FIRST = 0;
                constexpr int LAST  = 100;
                for (int i = FIRST; i < LAST; ++i) {
                    b.Add (i);
                }

                applyToContainer (b);

                {
                    static size_t count;
                    static T      sum = 0;
                    count             = 0;
                    b.Apply ([](const T& i) {
                        count++;
                        sum = sum + i;
                    });
                    VerifyTestResult (count == LAST - FIRST);
                }

                applyToContainer (b);
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::Test5_Apply_"};
                DoIt_<CONCRETE_CONTAINER> (factory, applyToContainer);
            }
        }

        /**
         */
        template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
        void SimpleCollectionTest_Generic (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::SimpleCollectionTest_Generic"};
            Test1_OldMiscBagTests_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
            Test4_IteratorsBasics_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
            Test5_Apply_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
        }

        /**
        */
        template <typename USING_Collection_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION, typename WITH_COMPARE_EQUALS>
        void SimpleCollectionTest_TestsWhichRequireEquals (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::CollectionTests::SimpleCollectionTest_TestsWhichRequireEquals"};
            Test2_TestsWithComparer_::DoAllTests_<USING_Collection_CONTAINER> (applyToContainer, WITH_COMPARE_EQUALS ());
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_ */
