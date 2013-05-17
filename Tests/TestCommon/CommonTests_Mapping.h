/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_    1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Mapping.h"

#include    "../TestHarness/TestHarness.h"
#include    "CommonTests_Iterable.h"


namespace CommonTests {
    namespace MappingTests {

        using   namespace   Stroika::Foundation;
        using   namespace   Stroika::Foundation::Containers;

        namespace Test1_BasicConstruction {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_MAPPING_CONTAINER::ElementType ELEMENT_TYPE;
                USING_MAPPING_CONTAINER   s;
                applyToContainer (s);
                USING_MAPPING_CONTAINER   s1 = s;
#if 0
                applyToContainer (s1);
                Mapping<ELEMENT_TYPE>   s2 = s;
                applyToContainer (s2);
                ELEMENT_TYPE kVec_[] = {1, 3, 4, 2 };
                Set<ELEMENT_TYPE> s3 = USING_SET_CONTAINER (kVec_);
                VerifyTestResult (s3.GetLength () == 4);
                VerifyTestResult (s3.Contains (1));
                VerifyTestResult (s3.Contains (2));
                VerifyTestResult (s3.Contains (3));
                VerifyTestResult (s3.Contains (4));
                VerifyTestResult (not s3.Contains (5));
#endif
            }
        }


        namespace Test2_AddRemove {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_MAPPING_CONTAINER m;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                Verify (m.Lookup (1, nullptr));
                Verify (not m.Lookup (2, nullptr));
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                IterableTests::SimpleIterableTest_All_For_Type<USING_MAPPING_CONTAINER> (m, applyToContainer);
                m.Remove (1);
                VerifyTestResult (m.size () == 0);
                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
            }
        }




        namespace Test_3_Iteration {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_MAPPING_CONTAINER m;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (i.first == 1);
                    VerifyTestResult (i.second == 2);
                }
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (i.first == 1);
                    VerifyTestResult (i.second == 2);
                }
                m.Remove (1);
                VerifyTestResult (m.size () == 0);
                for (auto i : m) {
                    VerifyTestResult (false);
                }
                m.Add (1, 2);
                m.Add (2, 3);
                m.Add (3, 4);
                unsigned int cnt = 0;
                for (auto i : m) {
                    cnt++;
                    if (cnt == 1) {
                        VerifyTestResult (i.first == 1);
                        VerifyTestResult (i.second == 2);
                    }
                    if (cnt == 2) {
                        VerifyTestResult (i.first == 2);
                        VerifyTestResult (i.second == 3);
                    }
                    if (cnt == 3) {
                        VerifyTestResult (i.first == 3);
                        VerifyTestResult (i.second == 4);
                    }
                }
                VerifyTestResult (cnt == 3);
                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
            }
        }

        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void    SimpleMappingTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_BasicConstruction::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test2_AddRemove::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test_3_Iteration::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
        }



    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ */
