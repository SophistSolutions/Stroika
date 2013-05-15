/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Tally_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Tally_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "../TestHarness/TestHarness.h"


namespace CommonTests {
    namespace TallyTests {

        template <typename USING_TALLY_CONTAINER>
        void    TallyIteratorTests_ (USING_TALLY_CONTAINER& s)
        {
            typedef USING_TALLY_CONTAINER::TallyOfElementType   T;
            const   size_t  kTestSize   =   6;

            VerifyTestResult (s.GetLength () == 0);

            for (TallyEntry<T> i : s) {
                VerifyTestResult (false);
            }

            /*
             * Try removes while iterating forward.
             */
            {
                for (int i = 1; i <= kTestSize; i++) {
                    s.Add (T (i));
                }

                for (auto it = s.begin (); it != s.end (); ++it) {
                    s.UpdateCount (it, 1);
                }

                VerifyTestResult (s.GetLength () == kTestSize);

                {
                    for (TallyEntry<T> it : s) {
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




        template <typename USING_TALLY_CONTAINER>
        void    SimpleTallyTests (USING_TALLY_CONTAINER& s)
        {
            typedef USING_TALLY_CONTAINER::TallyOfElementType   T;
            size_t  three = 3;

            Tally<T>   s1 (s);

            VerifyTestResult (s1 == s);
            VerifyTestResult (s1 == s);
            Tally<T>   s2 = s1;

            VerifyTestResult (s2 == s);
            VerifyTestResult (s2 == s1);
            s2.Add (three);
            VerifyTestResult (s1 == s);
            VerifyTestResult (s2 != s1);

            //TallyIteratorTests_ (s);
            CommonTests:: TallyTests::TallyIteratorTests_<USING_TALLY_CONTAINER> (s);


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




        template <typename USING_TALLY_CONTAINER>
        void    SimpleTallyTest_All_For_Type ()
        {
            USING_TALLY_CONTAINER s;
            SimpleTallyTests<USING_TALLY_CONTAINER> (s);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Tally_h_ */
