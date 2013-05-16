/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Bag.h"

#include    "../TestHarness/TestHarness.h"


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
                    }

                    VerifyTestResult(s.GetLength() == kTestSize);
                    for (T i : s) {
                        size_t  oldLength = s.GetLength ();
                        VerifyTestResult(s.Contains(i));
                        VerifyTestResult(s.Contains(s.GetLength ()));
                        s.Remove (s.GetLength ());
                        VerifyTestResult(s.GetLength () == oldLength - 1);
                    }

                    s.RemoveAll ();
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T(i));
                    }

                    {
                        for (T it : s) {
                            for(size_t i = 1; i <= kTestSize; i++) {
                                VerifyTestResult(s.Contains(i));
                                VerifyTestResult(s.GetLength() == kTestSize - i + 1);
                                s.Remove(i);
                                VerifyTestResult(not s.Contains(i - 1));
                                VerifyTestResult(s.GetLength() == kTestSize - i);
                            }
                        }
                        VerifyTestResult(s.IsEmpty());
                        VerifyTestResult(s.GetLength() == 0);
                    }

                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);
                    {
                        for (T i : s) {
                            s.Remove(i);
                        }
                        VerifyTestResult(s.IsEmpty());
                        VerifyTestResult(s.GetLength() == 0);
                    }

                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add (T (i));
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    for (T i : s) {
                        s.Remove(i);
                    }
                    VerifyTestResult(s.GetLength() == 0);
                }
                /*
                * Try removes multiple iterators present.
                */
                {
                    s.RemoveAll();
                    VerifyTestResult(s.GetLength() == 0);
                    for(size_t i = 1; i <= kTestSize; i++) {
                        s.Add(i);
                    }
                    VerifyTestResult(s.GetLength() == kTestSize);

                    size_t i =  1;
                    for (T it : s) {
                        for (T it2 : s) {
                            for (Iterator<T> it3 = s.begin (); it3 != s.end (); ++it3) {
                                s.Update (it3, i);
                                s.Remove(it3);
                                s.Add(i);
                                s.Remove(i);
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
                typedef typename USING_BAG_CONTAINER::ElementType   T;
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
                        if(it == i) {
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

            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void        On_Container_ (USING_BAG_CONTAINER& s, TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_BAG_CONTAINER::ElementType   T;
                size_t  three = 3;

                Bag<T> s1(s);

                VerifyTestResult(s1 == s);
                VerifyTestResult(s1 == s);

                Bag<T> s2 = s1;

                VerifyTestResult(s2 == s);
                VerifyTestResult(s2 == s1);

                s2.Add(three);
                VerifyTestResult(s1 == s);
                VerifyTestResult(s2 != s1);

                IteratorTests_ (s, applyToContainer);

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
                }
                BagTimings_(s, applyToContainer);
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
                    VerifyTestResult(s.TallyOf(1) == i);
                }
                size_t oldLength = s.GetLength();
                s += s;
                VerifyTestResult(s.GetLength() == oldLength * 2);
                s -= s;
                VerifyTestResult(s.GetLength() == 0);

#if     qPrintTimings
                t = GetCurrentTime() - t;
                cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
            }


            template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_BAG_CONTAINER s;
                On_Container_<USING_BAG_CONTAINER> (s, applyToContainer);
            }

        }



        template <typename USING_BAG_CONTAINER, typename TEST_FUNCTION>
        void    SimpleBagTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_::DoAllTests_<USING_BAG_CONTAINER> (applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bag_h_ */
