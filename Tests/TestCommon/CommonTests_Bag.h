/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Bag.h"

#include    "../TestHarness/TestHarness.h"
#include    "CommonTests_Iterable.h"


namespace CommonTests {
    namespace BagTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        namespace Test1_ {

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    IteratorTests_ (USING_BAG_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType   T;

                const   size_t  kTestSize   = 100;

                VerifyTestResult(s.GetLength() == 0);
                /*
                * Try removes while iterating forward.
                */
                {
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add(i);
                        VerifyTestResult(s.Contains(i));
                        applyToContainer (s);
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    IterableTests::SimpleIterableTest_All_For_Type<USING_BAG_CONTAINER> (s, applyToContainer);

                    VerifyTestResult(s.GetLength() == kTestSize);
                    for (T i : s) {
                        size_t  oldLength = s.GetLength ();
                        VerifyTestResult(s.Contains(i));
                        VerifyTestResult(s.Contains(s.GetLength ()));
                        size_t item2Remove = s.GetLength ();
                        s.Remove (item2Remove);
                        VerifyTestResult(not s.Contains(item2Remove));
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
                                VerifyTestResult(s.Contains(i));
                                VerifyTestResult(s.GetLength() == kTestSize - i + 1);
                                s.Remove(i);
                                applyToContainer (s);
                                VerifyTestResult(not s.Contains(i - 1));
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

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    BagTimings_ (USING_BAG_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType       T;
                typedef typename USING_BAG_CONTAINER::TraitsType        TraitsType;
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;
#if     qPrintTimings
                Time t = GetCurrentTime();
                cout << tab << "testing Bag<size_t> of length " << s.GetLength() << endl;
#endif

                for(size_t i = 1; i <= s.GetLength(); i++) {
                    VerifyTestResult(s.Contains(i));
                    VerifyTestResult(not s.Contains(0));
                }

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
                cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
            }

            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void        On_Container_ (USING_BAG_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType   T;
                typedef typename USING_BAG_CONTAINER::TraitsType    TraitsType;
                size_t  three = 3;

                applyToContainer (s);
                USING_BASEBAG_CONTAINER s1(s);
                applyToContainer (s1);

                VerifyTestResult(s1 == s);
                VerifyTestResult(s1 == s);

                USING_BASEBAG_CONTAINER s2 = s1;
                applyToContainer (s2);

                VerifyTestResult(s2 == s);
                VerifyTestResult(s2 == s1);

                s2.Add(three);
                applyToContainer (s);
                applyToContainer (s1);
                applyToContainer (s2);
                VerifyTestResult(s1 == s);
                VerifyTestResult(s2 != s1);

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
                VerifyTestResult(s.Contains(three));
                s.Remove(three);
                VerifyTestResult(s.GetLength() == 1);
                s -= three;
                VerifyTestResult(s.IsEmpty());
                s.RemoveAll();
                VerifyTestResult(s.IsEmpty());

                for(i = 1; i <= K; i++) {
                    s.Add(i);
                    applyToContainer (s);
                }
                applyToContainer (s);
                BagTimings_(s, applyToContainer);
                applyToContainer (s);
                VerifyTestResult(s.IsEmpty());

#if     qPrintTimings
                Time t = GetCurrentTime();
                cout << tab << "testing Bag<size_t>..." << endl;
#endif

                for(i = 1; i <= K; i++) {
                    s.Add(i);
                    VerifyTestResult(s.Contains(i));
                    VerifyTestResult(s.TallyOf(i) == 1);
                    VerifyTestResult(s.GetLength() == i);
                }
                for(i = K; i > 0; i--) {
                    s -= i;
                    VerifyTestResult(not s.Contains(i));
                    VerifyTestResult(s.GetLength() == (i - 1));
                }
                VerifyTestResult(s.IsEmpty());

                for(i = 1; i <= K / 2; i++) {
                    s += 1;
                    applyToContainer (s);
                    VerifyTestResult(s.TallyOf(1) == i);
                }
                size_t oldLength = s.GetLength();
                s += s;
                applyToContainer (s);
                VerifyTestResult(s.GetLength() == oldLength * 2);
                s -= s;
                applyToContainer (s);
                VerifyTestResult(s.GetLength() == 0);

#if     qPrintTimings
                t = GetCurrentTime() - t;
                cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
            }


            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_BAG_CONTAINER s;
                On_Container_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (s, applyToContainer);
            }

        }


        namespace   Test2_TallyOf_ {

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    SimpleTallyTest_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType ELEMENT_TYPE;
                USING_BAG_CONTAINER   bag;
                ELEMENT_TYPE    t1  =   1;
                ELEMENT_TYPE    t2  =   2;
                ELEMENT_TYPE    t3  =   3;
                VerifyTestResult (bag.IsEmpty());
                bag.Add (t1);
                bag.Add (t1);
                applyToContainer (bag);
                VerifyTestResult (not bag.IsEmpty());
                VerifyTestResult (bag.TallyOf (t3) == 0);
                VerifyTestResult (bag.TallyOf (t1) == 2);
                {
                    USING_BAG_CONTAINER   bag2    =   bag;
                    VerifyTestResult (bag2.TallyOf (t3) == 0);
                    VerifyTestResult (bag2.TallyOf (t1) == 2);
                    bag.Add (t1);
                    applyToContainer (bag);
                    VerifyTestResult (bag2.TallyOf (t1) == 2);
                    VerifyTestResult (bag.TallyOf (t1) == 3);
                }
            }

            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                SimpleTallyTest_<USING_BAG_CONTAINER> (applyToContainer);
            }
        }




        namespace   Test3_Equals_ {

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    SimpleOpEqualsTest_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType ELEMENT_TYPE;
                USING_BAG_CONTAINER   bag;
                ELEMENT_TYPE    t1  =   1;
                ELEMENT_TYPE    t2  =   2;
                ELEMENT_TYPE    t3  =   3;
                VerifyTestResult (bag.IsEmpty());
                bag.Add (t1);
                bag.Add (t1);
                applyToContainer (bag);
                {
                    USING_BAG_CONTAINER   bag2    =   bag;
                    VerifyTestResult (bag2 == bag);
                    VerifyTestResult (not (bag2 != bag));
                    bag.Add (t1);
                    applyToContainer (bag);
                    VerifyTestResult (not (bag2 == bag));
                    VerifyTestResult (bag2 != bag);
                }

                VerifyTestResult (bag.GetLength () == 3);
                bag.Add (t3);
                bag.Add (t1);
                bag.Add (t1);
                bag.Add (t3);
                applyToContainer (bag);
                {
                    USING_BAG_CONTAINER   bag2    =   bag;
                    VerifyTestResult (bag2 == bag);
                    VerifyTestResult (not (bag2 != bag));
                    bag.Add (t1);
                    VerifyTestResult (not (bag2 == bag));
                    VerifyTestResult (bag2 != bag);
                    bag.Remove (t1);
                    applyToContainer (bag);
                    VerifyTestResult (bag2 == bag);
                    VerifyTestResult (not (bag2 != bag));
                }

            }

            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                SimpleOpEqualsTest_<USING_BAG_CONTAINER> (applyToContainer);
            }

        }







        namespace   Test4_IteratorsBasics_ {

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    BasicIteratorTest_ (TEST_FUNCTION applyToContainer)
            {
                USING_BAG_CONTAINER   bag;
                typename USING_BAG_CONTAINER::ElementType t1  =   1;
                typename USING_BAG_CONTAINER::ElementType t2  =   2;
                typename USING_BAG_CONTAINER::ElementType t3  =   3;
                VerifyTestResult (bag.IsEmpty());
                bag.Add (t1);
                bag.Add (t1);
                applyToContainer (bag);
                {
                    USING_BAG_CONTAINER   bb  =   bag;
                    VerifyTestResult (bb.MakeIterator () != bag.MakeIterator ());
                    VerifyTestResult (bb.MakeIterator () != bb.MakeIterator ());        // WE may want to change the definition so this is allowed (-- LGP 2012-07-30)
                }
                {
                    Iterator<typename USING_BAG_CONTAINER::ElementType>   i   =   bag.begin ();
                    Iterator<typename USING_BAG_CONTAINER::ElementType>   ii  =   i;
                    VerifyTestResult (i == ii);
                    VerifyTestResult (i != bag.end ()); // because bag wasn't empty
                    ++i;
                    ++ii;
                    VerifyTestResult (i != ii);     // because bag wasn't empty and because of quirky (efficient) definition of Iterator<T>::operator== - may want to change this -- LGP 2012-07-30
                }
                {
                    VerifyTestResult (bag.size () == 2);    // cuz we said so above
                    Iterator<typename USING_BAG_CONTAINER::ElementType>   i   =   bag.begin ();
                    VerifyTestResult (not i.Done ());
                    VerifyTestResult (i != bag.end ());
                    ++i;
                    VerifyTestResult (not i.Done ());
                    VerifyTestResult (i != bag.end ());
                    ++i;
                    VerifyTestResult (i.Done ());
                    VerifyTestResult (i == bag.end ());
                }
            }


            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                BasicIteratorTest_<USING_BAG_CONTAINER> (applyToContainer);
            }

        }







        namespace   Test5_Apply_ {

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    DoIt_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType       T;
                typedef typename USING_BAG_CONTAINER::TraitsType        TraitsType;
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;
                USING_BAG_CONTAINER   b;

                constexpr int FIRST = 0;
                constexpr int LAST = 100;
                for (int i = FIRST; i < LAST; ++i) {
                    b.Add (i);
                    applyToContainer (b);
                }

                {
                    static size_t count;
                    static T sum;
                    count = 0;
                    sum = 0;
                    b.ApplyStatic ([] (const T & i) {
                        count++;
                        sum = sum + i;
                    });
                    VerifyTestResult (count == LAST - FIRST);
                    VerifyTestResult (EqualsCompareFunctionType::Equals (sum, T (((FIRST + (LAST - 1))) * (LAST - FIRST) / 2)));
                    applyToContainer (b);
                }
            }


            template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                DoIt_<USING_BAG_CONTAINER> (applyToContainer);
            }

        }


        template <typename USING_BAG_CONTAINER, typename USING_BASEBAG_CONTAINER, typename TEST_FUNCTION>
        void    SimpleBagTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_::DoAllTests_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (applyToContainer);
            Test2_TallyOf_::DoAllTests_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (applyToContainer);
            Test3_Equals_::DoAllTests_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (applyToContainer);
            Test4_IteratorsBasics_::DoAllTests_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (applyToContainer);
            Test5_Apply_::DoAllTests_<USING_BAG_CONTAINER, USING_BASEBAG_CONTAINER> (applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_ */
