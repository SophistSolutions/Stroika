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
        void    IterableIteratorTests_ (USING_ITERABLE_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }


        template <typename USING_ITERABLE_CONTAINER, typename TEST_FUNCTION>
        void    SimpleIterableTests (USING_ITERABLE_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }



        template <typename USING_ITERABLE_CONTAINER, typename TEST_FUNCTION>
        void    SimpleIterableTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            USING_ITERABLE_CONTAINER s;
            SimpleIterableTests<USING_ITERABLE_CONTAINER> (s, applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Iterable_h_ */
