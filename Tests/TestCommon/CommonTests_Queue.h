/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Queue_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Queue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Queue.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace QueueTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace Test1_BasicConstruction {
            template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
            void DoAllTests_ ()
            {
                CONCRETE_CONTAINER s;
                CONCRETE_CONTAINER s3 = s;
            }
        }

        namespace Test2_SimpleAddRemove {
            template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
            void DoAllTests_ ()
            {
                typedef typename CONCRETE_CONTAINER::value_type ELEMENT_TYPE;
                CONCRETE_CONTAINER                              s;
                s.Enqueue (1);
                EXPECT_TRUE (s.size () == 1);
                s.Enqueue (1);
                EXPECT_TRUE (s.size () == 2);
                EXPECT_TRUE (EQUALS_COMPARER{}(s.Dequeue (), ELEMENT_TYPE{1}));
                EXPECT_TRUE (s.size () == 1);
                s.RemoveAll ();
                EXPECT_TRUE (s.size () == 0);
            }
        }

        namespace Test3_Equals {
            template <typename CONCRETE_CONTAINER, Common::IEqualsComparer<typename CONCRETE_CONTAINER::value_type> EQUALS_COMPARER>
            void DoAllTests_ ()
            {
                CONCRETE_CONTAINER s;
                CONCRETE_CONTAINER s2 = s;
                s.Enqueue (1);
                s.Enqueue (2);
                EXPECT_TRUE (s.size () == 2);
                CONCRETE_CONTAINER s3 = s;
                //EXPECT_TRUE (s == s3);
                EXPECT_TRUE (typename CONCRETE_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s3));
                //EXPECT_TRUE (not (s != s3));

                //EXPECT_TRUE (s != s2);
                EXPECT_TRUE (not typename CONCRETE_CONTAINER::template EqualsComparer<EQUALS_COMPARER>{}(s, s2));
                //EXPECT_TRUE (not (s == s2));
            }
        }

        template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
        void SimpleQueueTest_All_NotRequiringEquals_For_Type ()
        {
            Test1_BasicConstruction::DoAllTests_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
            Test2_SimpleAddRemove::DoAllTests_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
        }

        template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER>
        void SimpleQueueTest_All_For_Type ()
        {
            SimpleQueueTest_All_NotRequiringEquals_For_Type<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
            Test3_Equals::DoAllTests_<CONCRETE_CONTAINER, EQUALS_COMPARER> ();
        }
    }
}
#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Queue_h_ */
