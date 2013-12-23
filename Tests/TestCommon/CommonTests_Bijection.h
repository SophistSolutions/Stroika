/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Bijection.h"

#include    "../TestHarness/TestHarness.h"
#include    "CommonTests_Iterable.h"


namespace CommonTests {
    namespace BijectionTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        namespace Test1_ {

            template <typename USING_BIJECTION_CONTAINER, typename USING_BASEBIJECTION_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_BIJECTION_CONTAINER s;
            }

        }


        template <typename USING_BIJECTION_CONTAINER, typename USING_BASEBIJECTION_CONTAINER, typename TEST_FUNCTION>
        void    SimpleBagTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_::DoAllTests_<USING_BIJECTION_CONTAINER, USING_BASEBIJECTION_CONTAINER> (applyToContainer);
        }

    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_ */
