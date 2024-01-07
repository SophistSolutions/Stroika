/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Traversal/Iterable.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

namespace CommonTests {
    namespace IterableTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

#if qHasFeature_GoogleTest
        template <typename USING_ITERABLE_CONTAINER>
        void SimpleIterableTests (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container)
        {
            {
                size_t l   = container.size ();
                size_t cnt = 0;
                for ([[maybe_unused]] auto i : container) {
                    ++cnt;
                }
                EXPECT_TRUE (cnt == l);
            }
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test2_Contains_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER&& equalsComparer)
        {
            (void)container.template Contains<EQUALS_COMPARER> (3, forward<EQUALS_COMPARER> (equalsComparer));
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test3_SetEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER&& equalsComparer)
        {
            auto iterableCopy = container;
            EXPECT_TRUE ((iterableCopy.template SetEquals<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, forward<EQUALS_COMPARER> (equalsComparer))));
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test4_MultiSetEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER&& equalsComparer)
        {
            auto iterableCopy = container;
            EXPECT_TRUE ((iterableCopy.template MultiSetEquals<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (
                container, forward<EQUALS_COMPARER> (equalsComparer))));
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void Test5_SequenceEquals_ (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container, EQUALS_COMPARER&& equalsComparer)
        {
            auto iterableCopy = container;
#if qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy
            EXPECT_TRUE ((iterableCopy.SequentialEquals (container, forward<EQUALS_COMPARER> (equalsComparer))));
#else
            EXPECT_TRUE ((iterableCopy.template SequentialEquals (container, forward<EQUALS_COMPARER> (equalsComparer))));
#endif
        }

        template <typename USING_ITERABLE_CONTAINER>
        void SimpleIterableTest_All_For_Type (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container)
        {
            SimpleIterableTests<USING_ITERABLE_CONTAINER> (container);
        }

        template <typename USING_ITERABLE_CONTAINER, typename EQUALS_COMPARER>
        void SimpleIterableTest_RequiringEqualsComparer (const Iterable<typename USING_ITERABLE_CONTAINER::value_type>& container,
                                                         EQUALS_COMPARER&&                                              equalsComparer)
        {
            Test2_Contains_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, forward<EQUALS_COMPARER> (equalsComparer));
            Test3_SetEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, forward<EQUALS_COMPARER> (equalsComparer));
            Test4_MultiSetEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, forward<EQUALS_COMPARER> (equalsComparer));
            Test5_SequenceEquals_<USING_ITERABLE_CONTAINER, EQUALS_COMPARER> (container, forward<EQUALS_COMPARER> (equalsComparer));
        }
#endif

    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ */
