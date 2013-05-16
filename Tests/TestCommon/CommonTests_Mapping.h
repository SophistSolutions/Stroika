/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Mapping.h"

#include    "../TestHarness/TestHarness.h"


namespace CommonTests {
    namespace MappingTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void    MappingIteratorTests_ (USING_MAPPING_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }


        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void    SimpleMappingTests (USING_MAPPING_CONTAINER& s, TEST_FUNCTION applyToContainer)
        {
        }



        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void    SimpleMappingTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            USING_MAPPING_CONTAINER s;
            SimpleMappingTests<USING_MAPPING_CONTAINER> (s, applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ */
