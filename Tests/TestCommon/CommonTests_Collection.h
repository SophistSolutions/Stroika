/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

#include "CommonTests_Iterable.h"

/**
 *  @todo   SimpleCollectionTest_TestsWhichRequireEquals() never called
 */

namespace CommonTests {
    namespace CollectionTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

#if qHasFeature_GoogleTest
        namespace Test1_OldMiscBagTests_ {

            template <typename CONCRETE_CONTAINER>
            void IteratorTests_ (CONCRETE_CONTAINER& s)
            {
                typedef typename CONCRETE_CONTAINER::value_type T;

                const size_t kTestSize = 100;

                EXPECT_TRUE (s.size () == 0);
                /*
                * Try removes while iterating forward.
                */
                {
                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (i);
                    }
                    EXPECT_TRUE (s.size () == kTestSize);

                    IterableTests::SimpleIterableTest_All_For_Type<CONCRETE_CONTAINER> (s);

                    EXPECT_TRUE (s.size () == kTestSize);
                    s.RemoveAll ();
                    EXPECT_TRUE (s.size () == 0);
                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }

                    s.clear ();

                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    EXPECT_TRUE (s.size () == kTestSize);

                    s.clear ();

                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    EXPECT_TRUE (s.size () == kTestSize);

                    s.clear ();
                    EXPECT_TRUE (s.size () == 0);
                }
                /*
                * Try removes multiple iterators present.
                */
                {
                    s.RemoveAll ();
                    EXPECT_TRUE (s.size () == 0);
                    for (size_t i = 1; i <= kTestSize; i++) {
                        s.Add (i);
                    }
                    EXPECT_TRUE (s.size () == kTestSize);
                }

                s.RemoveAll ();
            }

            template <typename CONCRETE_CONTAINER>
            void CollectionTimings_ (CONCRETE_CONTAINER& s)
            {
#if qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t> of length " << s.size () << endl;
#endif

                for (size_t i = 1; i <= s.size (); i++) {
                    for ([[maybe_unused]] auto&& it : s) {
                    }
                }
                for ([[maybe_unused]] auto&& it : s) {
                    for ([[maybe_unused]] auto&& it1 : s) {
                    }
                }
                s.RemoveAll ();
                EXPECT_TRUE (s.empty ());
                EXPECT_TRUE (s.size () == 0);

                for ([[maybe_unused]] auto&& it1 : s) {
                    for ([[maybe_unused]] auto&& it2 : s) {
                        EXPECT_TRUE (false);
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
                size_t three = 3;

                typename CONCRETE_CONTAINER::ArchetypeContainerType s1 (s);

                typename CONCRETE_CONTAINER::ArchetypeContainerType s2 = s1;

                s2.Add (three);

                IteratorTests_ (s);

                static const size_t K = qDebug ? (Debug::IsRunningUnderValgrind () ? 50 : 200) : 500;

                size_t i;

                EXPECT_TRUE (s.empty ());
                s.Add (three);
                EXPECT_TRUE (s.size () == 1);
                s += three;
                EXPECT_TRUE (s.size () == 2);
                s.RemoveAll ();
                EXPECT_TRUE (s.empty ());

                for (i = 1; i <= K; ++i) {
                    s.Add (i);
                }
                CollectionTimings_ (s);
                EXPECT_TRUE (s.empty ());

#if qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t>..." << endl;
#endif

                for (i = 1; i <= K; ++i) {
                    s.Add (i);
                    EXPECT_TRUE (s.size () == i);
                }
#if qPrintTimings
                t = GetCurrentTime () - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, [[maybe_unused]] TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::Test1_OldMiscBagTests_"};
                CONCRETE_CONTAINER        s = factory ();
                On_Container_<CONCRETE_CONTAINER> (s);
            }
        }

        namespace Test2_TestsWithComparer_ {
            template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
            void On_Container_ (CONCRETE_CONTAINER& s)
            {
                size_t three = 3;

                typename CONCRETE_CONTAINER::ArchetypeContainerType s1 (s);

                typename CONCRETE_CONTAINER::ArchetypeContainerType s2 = s1;

                s2.Add (three);

#if qDebug
                const size_t K = 200;
#else
                const size_t           K = 500;
#endif
                size_t i;

                EXPECT_TRUE (s.empty ());
                s.Add (three);
                EXPECT_TRUE (s.size () == 1);
                s += three;
                EXPECT_TRUE (s.size () == 2);
                EXPECT_TRUE (s.Contains (three));
                s.Remove (three);
                EXPECT_TRUE (s.size () == 1);
                s.RemoveAll ();
                EXPECT_TRUE (s.empty ());

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                }
                EXPECT_TRUE (not s.empty ());

                EXPECT_TRUE (s.size () == K);
                for (i = 1; i <= K; i++) {
                    s.Add (i);
                    EXPECT_TRUE (s.Contains (i));
                    EXPECT_TRUE (s.size () == i + K);
                }
            }

            /*
             * React to sporadic failure with Collection_LinkedList<shared_ptr<Connection>>> objects - remove sometimes failing. NOT SURE if
             * is bug with the container code, or threading somehow. VERY rare. But at least try to eliminate question of bug in this code itself.
             * Issue manifests in WebServer (inside WTF app) - after running a couple days, remove of a Connection object in
             * Collection_LinkedList<shared_ptr<Connection>> - fails with the RemoveAt in linked list not finding item wthat was previous
             * found iwth Find (and definitely should eb there with add). COULD be a threading issue - but didnt look like it.
             */
            template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
            void TestLotsOfAddsAndRemovesByValue_ ()
            {
                using T            = typename CONCRETE_CONTAINER::value_type;
                auto createTestSet = [] () {
                    vector<T>               v;
                    static constexpr size_t kSize_ = 99;
                    for (size_t i = 0; i < kSize_; ++i) {
                        v.push_back (rand () % 100);
                    }
                    return v;
                };
                const auto         testSet = createTestSet ();
                CONCRETE_CONTAINER s;

                multiset<T> shouldBePresent; // double check we are doing this right

                // For N times, keep randomly adding an item (from test set) or removing an item (from s)
#if qDebug
                constexpr unsigned int kTimesToRepeat_{1000};
#else
                constexpr unsigned int kTimesToRepeat_{10000};
#endif
                for (unsigned int testNum = 0; testNum < kTimesToRepeat_; ++testNum) {
                    bool doingAdd = (s.size () < 1) ? true : ((s.size () < 10) ? (rand () % 2 == 1) : (rand () % 3 == 1));
                    if (doingAdd) {
                        T elt2Add = testSet[rand () % testSet.size ()];
                        s.Add (elt2Add);
                        shouldBePresent.insert (elt2Add);
                    }
                    else {
                        // pick a random elt from the collection
                        Assert (s.size () != 0);
                        size_t idx        = rand () % s.size ();
                        T      elt2Remove = s.Nth (idx);
                        EXPECT_TRUE (shouldBePresent.find (elt2Remove) != shouldBePresent.end ());
                        shouldBePresent.erase (shouldBePresent.find (elt2Remove)); // shouldBePresent.erase (elt2Remove);   confusingly, multiset erase removes all
                        s.Remove (elt2Remove);
                    }
                    EXPECT_TRUE (s.size () == shouldBePresent.size ());
                }
            }

            template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
            void DoAllTests_ ()
            {
                Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::Test2_TestsWithComparer_"};
                CONCRETE_CONTAINER        s;
                On_Container_<CONCRETE_CONTAINER, EQUALS_COMPARER> (s);
                TestLotsOfAddsAndRemovesByValue_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
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
                EXPECT_TRUE (collection.empty ());
                collection.Add (t1);
                collection.Add (t1);
#if 0
                {
                    CONCRETE_CONTAINER   bb = collection;
                    EXPECT_TRUE (bb.MakeIterator () != collection.MakeIterator ());
                    EXPECT_TRUE (bb.MakeIterator () != bb.MakeIterator ());        // WE may want to change the definition so this is allowed (-- LGP 2012-07-30)
                }
#endif
                {
                    Iterator<typename CONCRETE_CONTAINER::value_type> i  = collection.begin ();
                    Iterator<typename CONCRETE_CONTAINER::value_type> ii = i;
                    EXPECT_TRUE (i == ii);
                    EXPECT_TRUE (i != collection.end ()); // because bag wasn't empty
                    ++i;
                    ++ii;
                    EXPECT_TRUE (i == ii);
                }
                {
                    EXPECT_TRUE (collection.size () == 2); // cuz we said so above
                    Iterator<typename CONCRETE_CONTAINER::value_type> i = collection.begin ();
                    EXPECT_TRUE (not i.Done ());
                    EXPECT_TRUE (i != collection.end ());
                    ++i;
                    EXPECT_TRUE (not i.Done ());
                    EXPECT_TRUE (i != collection.end ());
                    ++i;
                    EXPECT_TRUE (i.Done ());
                    EXPECT_TRUE (i == collection.end ());
                }
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, [[maybe_unused]] TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::Test4_IteratorsBasics_"};
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
                    size_t count;
                    T      sum = 0;
                    count      = 0;
                    b.Apply ([&] (const T& i) {
                        count++;
                        sum = sum + i;
                    });
                    EXPECT_TRUE (count == LAST - FIRST);
                }

                applyToContainer (b);
            }

            template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::Test5_Apply_"};
                DoIt_<CONCRETE_CONTAINER> (factory, applyToContainer);
            }
        }

        /**
         */
        template <typename CONCRETE_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
        void SimpleCollectionTest_Generic (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::SimpleCollectionTest_Generic"};
            Test1_OldMiscBagTests_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
            Test4_IteratorsBasics_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
            Test5_Apply_::DoAllTests_<CONCRETE_CONTAINER> (factory, applyToContainer);
        }

        /**
        */
        template <typename CONCRETE_CONTAINER, typename WITH_COMPARE_EQUALS>
        void SimpleCollectionTest_TestsWhichRequireEquals ()
        {
            Debug::TraceContextBumper ctx{"CommonTests::CollectionTests::SimpleCollectionTest_TestsWhichRequireEquals"};
            Test2_TestsWithComparer_::DoAllTests_<CONCRETE_CONTAINER, WITH_COMPARE_EQUALS> ();
        }
#endif

    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_ */
