/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Iterable.h"

#include    "../TestHarness/TestHarness.h"


namespace CommonTests {
    namespace IterableTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        template <typename USING_ITERABLE_CONTAINER, typename TEST_FUNCTION>
        void    SimpleIterableTests (const USING_ITERABLE_CONTAINER& container, TEST_FUNCTION applyToContainer)
        {
            {
                size_t  l   =   container.GetLength ();
                size_t  cnt =   0;
                for (auto i : container) {
                    cnt++;
                }
                VerifyTestResult (cnt == l);
            }
        }


        template <typename USING_ITERABLE_CONTAINER, typename TEST_FUNCTION>
        void    SimpleIterableTest_All_For_Type (const USING_ITERABLE_CONTAINER& container, TEST_FUNCTION applyToContainer)
        {
            SimpleIterableTests<USING_ITERABLE_CONTAINER> (container, applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ */
