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

#include    "../TestCommon/CommonTests_Bag.h"
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


namespace {
    template    <typename CONCRETE_CONTAINER>
    void     RunTests_ ()
    {
        typedef typename CONCRETE_CONTAINER::ElementType T;
        auto testFunc = [] (const Bag<T>& s) {
        };

        CommonTests::BagTests::SimpleBagTest_All_For_Type<CONCRETE_CONTAINER> (testFunc);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test2_TallyOf_::RunBasicBagTestsOnEachConcreteType_ ();
        Test3_Equals_::RunBasicBagTestsOnEachConcreteType_ ();
        Test4_IteratorsBasics_::RunBasicBagTestsOnEachConcreteType_ ();
        Test5_Apply_::DoIt_ ();


        RunTests_<Bag<size_t>> ();
        RunTests_<Bag<SimpleClass>> ();
        RunTests_<Bag_LinkedList<size_t>> ();
        RunTests_<Bag_LinkedList<SimpleClass>> ();
        RunTests_<Bag_Array<size_t>> ();
        RunTests_<Bag_Array<SimpleClass>> ();

    }

}




int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

