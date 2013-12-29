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



        namespace   Test1_IterableTests_ {
            template    <typename USING_SET_CONTAINER>
            void    DoAllTests_ ()
            {
                typedef typename USING_SET_CONTAINER::ElementType ElementType;
                typedef typename USING_SET_CONTAINER::EqualsCompareFunctionType EqualsCompareFunctionType;
                USING_SET_CONTAINER   s;
                s.Add (3);
                s.Add (9);
                IterableTests::SimpleIterableTest_All_For_Type<Iterable<ElementType>> (s);
                IterableTests::SimpleIterableTest_RequiringEqualsComparer<Iterable<ElementType>, EqualsCompareFunctionType> (s, EqualsCompareFunctionType ());
            }
        }



        namespace Test2_BasicConstruction_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                typedef typename USING_SET_CONTAINER::ElementType ELEMENT_TYPE;
                USING_SET_CONTAINER   s;
                applyToContainer (s);
                USING_SET_CONTAINER   s1 = s;
                applyToContainer (s1);
                USING_BASESET_CONTAINER   s2 = s;
                applyToContainer (s2);
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s);
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s2);
                ELEMENT_TYPE kVec_[] = {1, 3, 4, 2 };
                USING_BASESET_CONTAINER s3 = USING_BASESET_CONTAINER (kVec_);
                VerifyTestResult (s3.GetLength () == 4);
                VerifyTestResult (s3.Contains (1));
                VerifyTestResult (s3.Contains (2));
                VerifyTestResult (s3.Contains (3));
                VerifyTestResult (s3.Contains (4));
                VerifyTestResult (not s3.Contains (5));
                {
                    USING_SET_CONTAINER s1 = {1};
                    USING_SET_CONTAINER s2 = USING_SET_CONTAINER ({1});
                    USING_BASESET_CONTAINER s3 = USING_SET_CONTAINER ({1});
                    VerifyTestResult (s1.length () == 1);
                    VerifyTestResult (s2.length () == 1);
                    VerifyTestResult (s3.length () == 1);
                }
            }
        }


        namespace Test3_AddRemove_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename TEST_FUNCTION>
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
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s);
                s.Remove (1);
                applyToContainer (s);
                VerifyTestResult (s.size () == 0);
                applyToContainer (s);

                {
                    s.Add (3);
                    s.Add (99);
                    size_t oldLength = s.GetLength();
                    s += s;
                    applyToContainer (s);
                    VerifyTestResult(s.GetLength() == oldLength);
                }

                s.RemoveAll ();
                applyToContainer (s);
                VerifyTestResult (s.size () == 0);
            }
        }


        namespace   Test4_Equals_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s;
                USING_SET_CONTAINER s2 = s;
                s.Add (1);
                s.Add (2);
                VerifyTestResult (s.size () == 2);
                USING_SET_CONTAINER s3 = s;
                applyToContainer (s);
                applyToContainer (s2);
                applyToContainer (s3);
                VerifyTestResult (s == s3);
                VerifyTestResult (s.Equals (s3));
                VerifyTestResult (not (s != s3));

                VerifyTestResult (s != s2);
                VerifyTestResult (not s.Equals (s2));
                VerifyTestResult (not (s == s2));
            }
        }


        namespace   Test5_UnionDifferenceIntersectionEtc_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename TEST_FUNCTION>
            void    DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s1;
                s1.Add (1);
                s1.Add (2);
                USING_SET_CONTAINER s2 = s1;

                VerifyTestResult (s2 == s1.Union (s2));
                VerifyTestResult (s2 == s1 + s2);
#if      !qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug
                VerifyTestResult (s1 == USING_SET_CONTAINER ({1, 2}));
                VerifyTestResult (s2 == USING_SET_CONTAINER ({1, 2}));
#endif

                VerifyTestResult (s1.Difference (s2).empty ());
                VerifyTestResult ((s1 - s2).empty ());
                s2.Add (3);
#if      !qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug
                VerifyTestResult (s1 == USING_SET_CONTAINER ({1, 2}));
                VerifyTestResult (s2 == USING_SET_CONTAINER ({1, 2, 3}));
#endif
                VerifyTestResult ((s1 - s2).empty ());
                VerifyTestResult ((s2 - s1).length () == 1);
#if      !qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug
                VerifyTestResult ((s1 - s2) == USING_SET_CONTAINER ({}));
                VerifyTestResult ((s2 - s1) == USING_SET_CONTAINER ({3}));
#endif

                Verify (s1.Intersects (s2));
#if      !qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug
                VerifyTestResult (s1.Intersection (s2) == USING_SET_CONTAINER ({1, 2}));
                VerifyTestResult ((s1 ^ s2) == USING_SET_CONTAINER ({1, 2}));
#endif
                Verify (s1.Intersection (s2).length () == 2);
            }
        }




        template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename TEST_FUNCTION>
        void    Test_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_IterableTests_::DoAllTests_<USING_SET_CONTAINER> ();
            Test2_BasicConstruction_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (applyToContainer);
            Test3_AddRemove_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (applyToContainer);
            Test4_Equals_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (applyToContainer);
            Test5_UnionDifferenceIntersectionEtc_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (applyToContainer);
        }


    }
}



#endif  /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ */
