/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Traversal/Iterable.h"

#include "../TestHarness/TestHarness.h"

namespace CommonTests {
    namespace IterableTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        template <typename USING_ITERABLE_CONTAINER>
        void SimpleIterableTests (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container)
        {
            {
                size_t l   = container.GetLength ();
                size_t cnt = 0;
                for ([[maybe_unused]] auto i : container) {
                    Lambda_Arg_Unused_BWA (i);
                    cnt++;
                }
                VerifyTestResult (cnt == l);
            }
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test2_Contains_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER equalsComparer)
        {
            (void)container.template Contains<EQUALS_COMPARER> (3, equalsComparer);
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test3_SetEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER equalsComparer)
        {
            auto iterableCopy = container;
            VerifyTestResult ((iterableCopy.template SetEquals<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer)));
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test4_MultiSetEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER equalsComparer)
        {
            auto iterableCopy = container;
            VerifyTestResult ((iterableCopy.template MultiSetEquals<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer)));
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test5_SequenceEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER equalsComparer)
        {
            auto iterableCopy = container;
            VerifyTestResult ((iterableCopy.template SequenceEquals<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer)));
        }

        template <typename USING_ITERABLE_CONTAINER>
        void SimpleIterableTest_All_For_Type (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container)
        {
            SimpleIterableTests<USING_ITERABLE_CONTAINER> (container);
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void SimpleIterableTest_RequiringEqualsComparer (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER equalsComparer)
        {
            Test2_Contains_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer);
            Test3_SetEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer);
            Test4_MultiSetEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer);
            Test5_SequenceEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, equalsComparer);
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ */
