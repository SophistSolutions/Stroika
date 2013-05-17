/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Set.h"

#include    "../TestHarness/TestHarness.h"
#include    "CommonTests_Iterable.h"


namespace CommonTests {
    namespace SetTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        namespace Test1_BasicConstruction {
            template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_SET_CONTAINER::ElementType ELEMENT_TYPE;
                USING_SET_CONTAINER   s;
                applyToContainer (s);
                USING_SET_CONTAINER   s1 = s;
                applyToContainer (s1);
                Set<ELEMENT_TYPE>   s2 = s;
                applyToContainer (s2);
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s2, applyToContainer);
                ELEMENT_TYPE kVec_[] = {1, 3, 4, 2 };
                Set<ELEMENT_TYPE> s3 = USING_SET_CONTAINER (kVec_);
                VerifyTestResult (s3.GetLength () == 4);
                VerifyTestResult (s3.Contains (1));
                VerifyTestResult (s3.Contains (2));
                VerifyTestResult (s3.Contains (3));
                VerifyTestResult (s3.Contains (4));
                VerifyTestResult (not s3.Contains (5));
            }
        }


        namespace Test2_AddRemove {
            template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s;
                s.Add (1);
                applyToContainer (s);
                VerifyTestResult (s.size () == 1);
                VerifyTestResult (s.Contains (1));
                VerifyTestResult (not s.Contains (2));
                applyToContainer (s);
                s.Add (1);
                applyToContainer (s);
                VerifyTestResult (s.size () == 1);
                applyToContainer (s);
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s, applyToContainer);
                s.Remove (1);
                applyToContainer (s);
                VerifyTestResult (s.size () == 0);
                applyToContainer (s);
                s.RemoveAll ();
                applyToContainer (s);
                VerifyTestResult (s.size () == 0);
            }
        }


        template <typename USING_SET_CONTAINER, typename TEST_FUNCTION>
        void    SimpleSetTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_BasicConstruction::DoAllTests_<USING_SET_CONTAINER> (applyToContainer);
            Test2_AddRemove::DoAllTests_<USING_SET_CONTAINER> (applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ */
