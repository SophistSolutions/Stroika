/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Set.h"

#include    "../TestHarness/TestHarness.h"


namespace CommonTests {
    namespace SetTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
        void    SetIteratorTests_ (USING_SET_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }


        template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
        void    SimpleSetTests (USING_SET_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }



        template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
        void    SimpleSetTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            USING_SET_CONTAINER s;
            SimpleSetTests<USING_SET_CONTAINER> (s, applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ */
