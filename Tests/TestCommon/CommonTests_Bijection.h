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

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename    USING_BIJECTION_CONTAINER::DomainType   DomainType;
                typedef typename    USING_BIJECTION_CONTAINER::RangeType    RangeType;
                typedef typename    USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType RangeEqualsCompareFunctionType;
                USING_BIJECTION_CONTAINER s;
                s.Add (3, 5);
                VerifyTestResult (s.length () == 1);
                VerifyTestResult (s.ContainsDomainElement (3));
                VerifyTestResult (not s.ContainsDomainElement (5));
                VerifyTestResult (s.ContainsRangeElement (5));
                VerifyTestResult (not s.ContainsRangeElement (3));
                VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (3), 5));
            }

        }


        template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
        void    SimpleBagTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
        }

    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_ */
