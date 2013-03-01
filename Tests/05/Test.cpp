/*
* Copyright(c) Sophist Solutions Inc. 1990-2013.  All rights reserved
*/
//  TEST    Foundation::Containers::Bag
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Bag.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/Bag_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Bag_LinkedList.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;

using   Concrete::Bag_Array;
using   Concrete::Bag_LinkedList;




namespace   {
    namespace   Test1_ {
        void    Test1_BagIteratorTests_ (Bag<size_t>& s)
        {
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
                for (size_t i : s) {
                    size_t  oldLength = s.GetLength ();
                    VerifyTestResult(s.Contains(i));
                    VerifyTestResult(s.Contains(s.GetLength ()));
                    s.Remove (s.GetLength ());
                    VerifyTestResult(s.GetLength () == oldLength - 1);
                }

                s.RemoveAll ();
                for(size_t i = 1; i <= kTestSize; i++) {
                    s.Add(i);
                }

                {
                    for (size_t it : s) {
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
                    s.Add(i);
                }
                VerifyTestResult(s.GetLength() == kTestSize);
                {
                    for (size_t i : s) {
                        s.Remove(i);
                    }
                    VerifyTestResult(s.IsEmpty());
                    VerifyTestResult(s.GetLength() == 0);
                }

                for(size_t i = 1; i <= kTestSize; i++) {
                    s.Add(i);
                }
                VerifyTestResult(s.GetLength() == kTestSize);

                for (size_t i : s) {
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
                for (size_t it : s) {
                    for (size_t it2 : s) {
                        for (Iterator<size_t> it3 = s.begin (); it3 != s.end (); ++it3) {
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

        void    Test1_BagTimings_ (Bag<size_t>& s)
        {
#if     qPrintTimings
            Time t = GetCurrentTime();
            cout << tab << "testing Bag<size_t> of length " << s.GetLength() << endl;
#endif

            for(size_t i = 1; i <= s.GetLength(); i++) {
                VerifyTestResult(s.Contains(i));
                VerifyTestResult(not s.Contains(0));
            }

            for(size_t i = 1; i <= s.GetLength(); i++) {
                for (size_t it : s) {
                    if(it == i) {
                        break;
                    }
                }
            }
            for (size_t it : s) {
                for (size_t it1 : s) {
                    s.RemoveAll();
                }
            }
            VerifyTestResult(s.IsEmpty());
            VerifyTestResult(s.GetLength() == 0);

            for (size_t it1 : s) {
                for (size_t it2 : s) {
                    VerifyTestResult(false);
                }
            }

#if     qPrintTimings
            t = GetCurrentTime() - t;
            cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
        }

        void    Test1_On_Container_ (Bag<size_t>& s)
        {
            size_t  three = 3;

            Bag<size_t> s1(s);

            VerifyTestResult(s1 == s);
            VerifyTestResult(s1 == s);

            Bag<size_t> s2 = s1;

            VerifyTestResult(s2 == s);
            VerifyTestResult(s2 == s1);

            s2.Add(three);
            VerifyTestResult(s1 == s);
            VerifyTestResult(s2 != s1);

            Test1_BagIteratorTests_ (s);

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
            Test1_BagTimings_(s);
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

        void    Test1_On_Container_ (Bag<SimpleClass>& s)
        {
            SimpleClass three = 3;

            Bag<SimpleClass>    s1(s);

            VerifyTestResult(s1 == s);
            VerifyTestResult(s1 == s);

            Bag<SimpleClass>    s2 = s1;

            VerifyTestResult(s2 == s);
            VerifyTestResult(s2 == s1);
            s2.Add(three);
            VerifyTestResult(s1 == s);
            VerifyTestResult(s2 != s1);

            VerifyTestResult(s.IsEmpty());
            s.Add(three);
            VerifyTestResult(s.GetLength() == 1);
            s.Add(three);
            VerifyTestResult(s.GetLength() >= 1);
            VerifyTestResult(s.Contains(three));
            s.Remove(three);
            VerifyTestResult(s.GetLength() <= 1);
            s.RemoveAll();
            VerifyTestResult(s.IsEmpty());
        }

        template    <typename   CONCRETE_TYPE>
        void    RunBasicBagTestsOnEachConcreteType_ ()
        {
            CONCRETE_TYPE   s;
            Test1_On_Container_ (s);
        }

        void    RunBasicBagTestsOnEachConcreteType_ ()
        {
            RunBasicBagTestsOnEachConcreteType_<Bag<size_t>> ();
            RunBasicBagTestsOnEachConcreteType_<Bag<SimpleClass>> ();
            RunBasicBagTestsOnEachConcreteType_<Bag_LinkedList<size_t>> ();
            RunBasicBagTestsOnEachConcreteType_<Bag_LinkedList<SimpleClass>> ();
            RunBasicBagTestsOnEachConcreteType_<Bag_Array<size_t>> ();
            RunBasicBagTestsOnEachConcreteType_<Bag_Array<SimpleClass>> ();
        }

    }
}





namespace   {
    namespace   Test2_TallyOf_ {


        template    <typename   CONCRETE_TYPE>
        void    SimpleTallyTest_ ()
        {
            typedef typename CONCRETE_TYPE::ElementType ELEMENT_TYPE;
            CONCRETE_TYPE   bag;
            ELEMENT_TYPE    t1  =   1;
            ELEMENT_TYPE    t2  =   2;
            ELEMENT_TYPE    t3  =   3;
            VerifyTestResult (bag.IsEmpty());
            bag.Add (t1);
            bag.Add (t1);
            VerifyTestResult (not bag.IsEmpty());
            VerifyTestResult (bag.TallyOf (t3) == 0);
            VerifyTestResult (bag.TallyOf (t1) == 2);
            {
                CONCRETE_TYPE   bag2    =   bag;
                VerifyTestResult (bag2.TallyOf (t3) == 0);
                VerifyTestResult (bag2.TallyOf (t1) == 2);
                bag.Add (t1);
                VerifyTestResult (bag2.TallyOf (t1) == 2);
                VerifyTestResult (bag.TallyOf (t1) == 3);
            }
        }

        void    RunBasicBagTestsOnEachConcreteType_ ()
        {
            SimpleTallyTest_<Bag<size_t>> ();
            SimpleTallyTest_<Bag<SimpleClass>> ();
            SimpleTallyTest_<Bag_LinkedList<size_t>> ();
            SimpleTallyTest_<Bag_LinkedList<SimpleClass>> ();
            SimpleTallyTest_<Bag_Array<size_t>> ();
            SimpleTallyTest_<Bag_Array<SimpleClass>> ();
        }

    }
}







namespace   {
    namespace   Test3_Equals_ {

        template    <typename   CONCRETE_TYPE>
        void    SimpleOpEqualsTest_ ()
        {
            typedef typename CONCRETE_TYPE::ElementType ELEMENT_TYPE;
            CONCRETE_TYPE   bag;
            ELEMENT_TYPE    t1  =   1;
            ELEMENT_TYPE    t2  =   2;
            ELEMENT_TYPE    t3  =   3;
            VerifyTestResult (bag.IsEmpty());
            bag.Add (t1);
            bag.Add (t1);
            {
                CONCRETE_TYPE   bag2    =   bag;
                VerifyTestResult (bag2 == bag);
                VerifyTestResult (not (bag2 != bag));
                bag.Add (t1);
                VerifyTestResult (not (bag2 == bag));
                VerifyTestResult (bag2 != bag);
            }

            VerifyTestResult (bag.GetLength () == 3);
            bag.Add (t3);
            bag.Add (t1);
            bag.Add (t1);
            bag.Add (t3);
            {
                CONCRETE_TYPE   bag2    =   bag;
                VerifyTestResult (bag2 == bag);
                VerifyTestResult (not (bag2 != bag));
                bag.Add (t1);
                VerifyTestResult (not (bag2 == bag));
                VerifyTestResult (bag2 != bag);
                bag.Remove (t1);
                VerifyTestResult (bag2 == bag);
                VerifyTestResult (not (bag2 != bag));
            }

        }

        void    RunBasicBagTestsOnEachConcreteType_ ()
        {
            SimpleOpEqualsTest_<Bag<size_t>> ();
            SimpleOpEqualsTest_<Bag<SimpleClass>> ();
            SimpleOpEqualsTest_<Bag_LinkedList<size_t>> ();
            SimpleOpEqualsTest_<Bag_LinkedList<SimpleClass>> ();
            SimpleOpEqualsTest_<Bag_Array<size_t>> ();
            SimpleOpEqualsTest_<Bag_Array<SimpleClass>> ();
        }

    }
}












namespace   {
    namespace   Test4_IteratorsBasics_ {

        template    <typename   CONCRETE_TYPE>
        void    BasicIteratorTest_ ()
        {
            CONCRETE_TYPE   bag;
            typename CONCRETE_TYPE::ElementType t1  =   1;
            typename CONCRETE_TYPE::ElementType t2  =   2;
            typename CONCRETE_TYPE::ElementType t3  =   3;
            VerifyTestResult (bag.IsEmpty());
            bag.Add (t1);
            bag.Add (t1);
            {
                CONCRETE_TYPE   bb  =   bag;
                VerifyTestResult (bb.MakeIterator () != bag.MakeIterator ());
                VerifyTestResult (bb.MakeIterator () != bb.MakeIterator ());        // WE may want to change the definition so this is allowed (-- LGP 2012-07-30)
            }
            {
                Iterator<typename CONCRETE_TYPE::ElementType>   i   =   bag.begin ();
                Iterator<typename CONCRETE_TYPE::ElementType>   ii  =   i;
                VerifyTestResult (i == ii);
                VerifyTestResult (i != bag.end ()); // because bag wasn't empty
                ++i;
                ++ii;
                VerifyTestResult (i != ii);     // because bag wasn't empty and because of quirky (efficient) definition of Iterator<T>::operator== - may want to change this -- LGP 2012-07-30
            }
            {
                VerifyTestResult (bag.size () == 2);    // cuz we said so above
                Iterator<typename CONCRETE_TYPE::ElementType>   i   =   bag.begin ();
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

        void    RunBasicBagTestsOnEachConcreteType_ ()
        {
            BasicIteratorTest_<Bag<size_t>> ();
            BasicIteratorTest_<Bag<SimpleClass>> ();
            BasicIteratorTest_<Bag_LinkedList<size_t>> ();
            BasicIteratorTest_<Bag_LinkedList<SimpleClass>> ();
            BasicIteratorTest_<Bag_Array<size_t>> ();
            BasicIteratorTest_<Bag_Array<SimpleClass>> ();
        }

    }
}


namespace   {
    namespace   Test5_Apply_ {
        void    DoIt_ ()
        {
            Bag<int>    b;

            constexpr int FIRST = 0;
            constexpr int LAST = 100;
            for (int i = FIRST; i < LAST; ++i) {
                b.Add (i);
            }

            {
                static size_t count;
                static size_t sum;
                count = 0;
                sum = 0;
                b.ApplyStatic ([] (const int & i) {
                    count++;
                    sum += i;
                });
                VerifyTestResult (count == LAST - FIRST);
                VerifyTestResult (sum == ((FIRST + (LAST - 1))) * (LAST - FIRST) / 2);
            }
        }
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_::RunBasicBagTestsOnEachConcreteType_ ();
        Test2_TallyOf_::RunBasicBagTestsOnEachConcreteType_ ();
        Test3_Equals_::RunBasicBagTestsOnEachConcreteType_ ();
        Test4_IteratorsBasics_::RunBasicBagTestsOnEachConcreteType_ ();
        Test5_Apply_::DoIt_ ();
    }

}




int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

