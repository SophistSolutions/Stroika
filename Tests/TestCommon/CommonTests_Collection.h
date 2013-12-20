/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Collection.h"

#include    "../TestHarness/TestHarness.h"
#include    "CommonTests_Iterable.h"


namespace CommonTests {
    namespace CollectionTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        namespace Test1_ {

            template <typename USING_Collection_CONTAINER, typename TEST_FUNCTION>
            void    IteratorTests_ (USING_Collection_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_Collection_CONTAINER::ElementType   T;

                const   size_t  kTestSize   = 100;

                VerifyTestResult(s.GetLength() == 0);
                /*
                * Try removes while iterating forward.
                */
                {
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add(i);
                        applyToContainer (s);
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    IterableTests::SimpleIterableTest_All_For_Type<USING_Collection_CONTAINER> (s, applyToContainer);

                    VerifyTestResult(s.GetLength() == kTestSize);
                    for (T i : s) {
                        size_t  oldLength = s.GetLength ();
                        size_t item2Remove = s.GetLength ();
                        s.Remove (item2Remove);
                        applyToContainer (s);
                        VerifyTestResult(s.GetLength () == oldLength - 1);
                    }

                    s.RemoveAll ();
                    applyToContainer (s);
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T(i));
                        applyToContainer (s);
                    }

                    {
                        for (T it : s) {
                            for(size_t i = 1; i <= kTestSize; i++) {
                                VerifyTestResult(s.GetLength() == kTestSize - i + 1);
                                s.Remove(i);
                                applyToContainer (s);
                                VerifyTestResult(s.GetLength() == kTestSize - i);
                            }
                        }
                        VerifyTestResult(s.IsEmpty());
                        VerifyTestResult(s.GetLength() == 0);
                    }

                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                        applyToContainer (s);
                    }
                    applyToContainer (s);
                    VerifyTestResult(s.GetLength() == kTestSize);
                    {
                        for (T i : s) {
                            s.Remove(i);
                            applyToContainer (s);
                        }
                        VerifyTestResult(s.IsEmpty());
                        VerifyTestResult(s.GetLength() == 0);
                    }

                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                        applyToContainer (s);
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    for (T i : s) {
                        s.Remove(i);
                        applyToContainer (s);
                    }
                    VerifyTestResult(s.GetLength() == 0);
                }
                /*
                * Try removes multiple iterators present.
                */
                {
                    s.RemoveAll();
                    applyToContainer (s);
                    VerifyTestResult(s.GetLength() == 0);
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add(i);
                        applyToContainer (s);
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    size_t i =  1;
                    for (T it : s) {
                        for (T it2 : s) {
                            for (Iterator<T> it3 = s.begin (); it3 != s.end (); ++it3) {
                                s.Update (it3, i);
                                applyToContainer (s);
                                s.Remove(it3);
                                applyToContainer (s);
                                s.Add(i);
                                applyToContainer (s);
                                s.Remove(i);
                                applyToContainer (s);
                                i++;
                            }
                        }
                    }
                }

                s.RemoveAll ();
            }

            template <typename USING_Collection_CONTAINER, typename TEST_FUNCTION>
            void    CollectionTimings_ (USING_Collection_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_Collection_CONTAINER::ElementType       T;
                typedef typename USING_Collection_CONTAINER::TraitsType        TraitsType;
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;
#if     qPrintTimings
                Time t = GetCurrentTime();
                cout << tab << "testing Collection<size_t> of length " << s.GetLength() << endl;
#endif

                for(size_t i = 1; i <= s.GetLength(); i++) {
                    for (T it : s) {
                        if (EqualsCompareFunctionType::Equals (it, i)) {
                            break;
                        }
                    }
                }
                for (T it : s) {
                    for (T it1 : s) {
                        s.RemoveAll();
                    }
                }
                VerifyTestResult(s.IsEmpty());
                VerifyTestResult(s.GetLength() == 0);

                for (T it1 : s) {
                    for (T it2 : s) {
                        VerifyTestResult(false);
                    }
                }

#if     qPrintTimings
                t = GetCurrentTime() - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION>
            void        On_Container_ (USING_Collection_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_Collection_CONTAINER::ElementType   T;
                typedef typename USING_Collection_CONTAINER::TraitsType    TraitsType;
                size_t  three = 3;

                applyToContainer (s);
                USING_BASECollection_CONTAINER s1 (s);
                applyToContainer (s1);

                USING_BASECollection_CONTAINER s2 = s1;
                applyToContainer (s2);

                s2.Add(three);
                applyToContainer (s);
                applyToContainer (s1);
                applyToContainer (s2);

                IteratorTests_ (s, applyToContainer);
                applyToContainer (s);

#if     qDebug
                const   size_t  K = 200;
#else
                const   size_t  K = 500;
#endif
                size_t i;

                VerifyTestResult(s.IsEmpty());
                s.Add(three);
                VerifyTestResult(s.GetLength() == 1);
                s += three;
                VerifyTestResult(s.GetLength() == 2);
                s.Remove(three);
                VerifyTestResult(s.GetLength() == 1);
                s.RemoveAll();
                VerifyTestResult(s.IsEmpty());

                for(i = 1; i <= K; i++) {
                    s.Add(i);
                    applyToContainer (s);
                }
                applyToContainer (s);
                CollectionTimings_ (s, applyToContainer);
                applyToContainer (s);
                VerifyTestResult(s.IsEmpty());

#if     qPrintTimings
                Time t = GetCurrentTime();
                cout << tab << "testing Collection<size_t>..." << endl;
#endif

                for(i = 1; i <= K; i++) {
                    s.Add(i);
                    VerifyTestResult(s.GetLength() == i);
                }
#if     qPrintTimings
                t = GetCurrentTime() - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }


            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_Collection_CONTAINER s;
                On_Container_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (s, applyToContainer);
            }

        }



        namespace   Test2_TestsWithComparer_ {
            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION, typename EQUALS_COMPARER>
            void        On_Container_ (USING_Collection_CONTAINER& s, TEST_FUNCTION applyToContainer, EQUALS_COMPARER equals_comparer)
            {
                typedef typename USING_Collection_CONTAINER::ElementType   T;
                typedef typename USING_Collection_CONTAINER::TraitsType    TraitsType;
                size_t  three = 3;

                applyToContainer (s);
                USING_BASECollection_CONTAINER s1 (s);
                applyToContainer (s1);

                USING_BASECollection_CONTAINER s2 = s1;
                applyToContainer (s2);

                s2.Add (three);
                applyToContainer (s);
                applyToContainer (s1);
                applyToContainer (s2);

                IteratorTests_ (s, applyToContainer);
                applyToContainer (s);

#if     qDebug
                const   size_t  K = 200;
#else
                const   size_t  K = 500;
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
                CollectionTimings_ (s, applyToContainer);
                applyToContainer (s);
                VerifyTestResult (s.IsEmpty ());

#if     qPrintTimings
                Time t = GetCurrentTime ();
                cout << tab << "testing Collection<size_t>..." << endl;
#endif

                for (i = 1; i <= K; i++) {
                    s.Add (i);
                    VerifyTestResult (s.Contains (i));
                    VerifyTestResult (s.GetLength () == i);
                }
#if     qPrintTimings
                t = GetCurrentTime () - t;
                cout << tab << "finished testing Collection<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION, typename EQUALS_COMPARER>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer, EQUALS_COMPARER equals_comparer)
            {
                USING_Collection_CONTAINER s;
                On_Container_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (s, applyToContainer);
            }
        }


        namespace   Test4_IteratorsBasics_ {

            template <typename USING_Collection_CONTAINER, typename TEST_FUNCTION>
            void    BasicIteratorTest_ (TEST_FUNCTION applyToContainer)
            {
                USING_Collection_CONTAINER   collection;
                typename USING_Collection_CONTAINER::ElementType t1 = 1;
                typename USING_Collection_CONTAINER::ElementType t2 = 2;
                typename USING_Collection_CONTAINER::ElementType t3 = 3;
                VerifyTestResult (collection.IsEmpty ());
                collection.Add (t1);
                collection.Add (t1);
                applyToContainer (collection);
                {
                    USING_Collection_CONTAINER   bb = collection;
                    VerifyTestResult (bb.MakeIterator () != collection.MakeIterator ());
                    VerifyTestResult (bb.MakeIterator () != bb.MakeIterator ());        // WE may want to change the definition so this is allowed (-- LGP 2012-07-30)
                }
                {
                    Iterator<typename USING_Collection_CONTAINER::ElementType>   i = collection.begin ();
                    Iterator<typename USING_Collection_CONTAINER::ElementType>   ii = i;
                    VerifyTestResult (i == ii);
                    VerifyTestResult (i != collection.end ()); // because bag wasn't empty
                    ++i;
                    ++ii;
                    VerifyTestResult (i != ii);     // because bag wasn't empty and because of quirky (efficient) definition of Iterator<T>::operator== - may want to change this -- LGP 2012-07-30
                }
                {
                    VerifyTestResult (collection.size () == 2);    // cuz we said so above
                    Iterator<typename USING_Collection_CONTAINER::ElementType>   i = collection.begin ();
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


            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                BasicIteratorTest_<USING_Collection_CONTAINER> (applyToContainer);
            }

        }







        namespace   Test5_Apply_ {

            template <typename USING_Collection_CONTAINER, typename TEST_FUNCTION>
            void    DoIt_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_Collection_CONTAINER::ElementType       T;
                typedef typename USING_Collection_CONTAINER::TraitsType        TraitsType;
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;
                USING_Collection_CONTAINER   b;

                constexpr int FIRST = 0;
                constexpr int LAST = 100;
                for (int i = FIRST; i < LAST; ++i) {
                    b.Add (i);
                    applyToContainer (b);
                }

                {
                    static size_t   count;
                    static T        sum =   0;
                    count = 0;
                    b.ApplyStatic ([] (const T & i) {
                        count++;
                        sum = sum + i;
                    });
                    VerifyTestResult (count == LAST - FIRST);
                    VerifyTestResult (EqualsCompareFunctionType::Equals (sum, T (((FIRST + (LAST - 1))) * (LAST - FIRST) / 2)));
                    applyToContainer (b);
                }
            }


            template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                DoIt_<USING_Collection_CONTAINER> (applyToContainer);
            }

        }


        template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION>
        void    SimpleCollectionTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_::DoAllTests_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (applyToContainer);
            Test4_IteratorsBasics_::DoAllTests_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (applyToContainer);
            Test5_Apply_::DoAllTests_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (applyToContainer);
        }


        template <typename USING_Collection_CONTAINER, typename USING_BASECollection_CONTAINER, typename TEST_FUNCTION, typename WITH_COMPARE_EQUALS>
        void    SimpleCollectionTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test2_TestsWithComparer_::DoAllTests_<USING_Collection_CONTAINER, USING_BASECollection_CONTAINER> (applyToContainer, WITH_COMPARE_EQUALS ());
        }

    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Collection_h_ */
