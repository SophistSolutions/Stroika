/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Set.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace SetTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace {
            template <typename FACTORY, typename DEFAULT_VALUES_ITERABLE>
            auto mk_ (const FACTORY& f, const DEFAULT_VALUES_ITERABLE& il)
            {
                auto c = f ();
                for (auto i : il) {
                    c.Add (i);
                }
                return c;
            }
        }

        namespace Test1_IterableTests_ {
            template <typename USING_SET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory)
            {
                using value_type      = typename USING_SET_CONTAINER::value_type;
                USING_SET_CONTAINER s = factory ();
                s.Add (3);
                s.Add (9);
                IterableTests::SimpleIterableTest_All_For_Type<Iterable<value_type>> (s);
                IterableTests::SimpleIterableTest_RequiringEqualsComparer<Iterable<value_type>> (s, s.GetEqualsComparer ());
            }
        }

        namespace Test2_BasicConstruction_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                {
                    USING_SET_CONTAINER s = factory ();
                    applyToContainer (s);
                    USING_SET_CONTAINER s1 = s;
                    applyToContainer (s1);
                    USING_BASESET_CONTAINER s2 = s;
                    applyToContainer (s2);
                    IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s);
                    IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s2);
                    USING_BASESET_CONTAINER s3 = mk_ (factory, initializer_list<int>{1, 3, 4, 2});
                    VerifyTestResult (s3.GetLength () == 4);
                    VerifyTestResult (s3.Contains (1));
                    VerifyTestResult (s3.Contains (2));
                    VerifyTestResult (s3.Contains (3));
                    VerifyTestResult (s3.Contains (4));
                    VerifyTestResult (not s3.Contains (5));
                }
                {
                    USING_SET_CONTAINER     s1 = mk_ (factory, initializer_list<int>{1});
                    USING_SET_CONTAINER     s2 = mk_ (factory, initializer_list<int>{1});
                    USING_BASESET_CONTAINER s3 = mk_ (factory, initializer_list<int>{1});
                    VerifyTestResult (s1.length () == 1);
                    VerifyTestResult (s2.length () == 1);
                    VerifyTestResult (s3.length () == 1);
                }
            }
        }

        namespace Test3_AddRemove_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s = factory ();
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
                    size_t oldLength = s.GetLength ();
                    s += s;
                    applyToContainer (s);
                    VerifyTestResult (s.GetLength () == oldLength);
                }

                s.RemoveAll ();
                applyToContainer (s);
                VerifyTestResult (s.size () == 0);
            }
        }

        namespace Test4_Equals_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s  = factory ();
                USING_SET_CONTAINER s2 = s;
                s.Add (1);
                s.Add (2);
                VerifyTestResult (s.size () == 2);
                USING_SET_CONTAINER s3 = s;
                applyToContainer (s);
                applyToContainer (s2);
                applyToContainer (s3);
                VerifyTestResult (s == s3);
                VerifyTestResult (typename USING_SET_CONTAINER::EqualsComparer{}(s, s3));
                VerifyTestResult (not(s != s3));

                VerifyTestResult (s != s2);
                VerifyTestResult (not typename USING_SET_CONTAINER::EqualsComparer{}(s, s2));
                VerifyTestResult (not(s == s2));
            }
        }

        namespace Test5_UnionDifferenceIntersectionEtc_ {
            template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
            void DoAllTests_ (CONCRETE_CONTAINER_FACTORY factory, [[maybe_unused]] TEST_FUNCTION applyToContainer)
            {
                USING_SET_CONTAINER s1 = factory ();
                s1.Add (1);
                s1.Add (2);
                USING_SET_CONTAINER s2 = s1;

                VerifyTestResult (s2 == s1.Union (s2));
                VerifyTestResult (s2 == s1 + s2);
                VerifyTestResult ((s1 == mk_ (factory, initializer_list<int>{1, 2})));
                VerifyTestResult ((s2 == mk_ (factory, initializer_list<int>{1, 2})));

                VerifyTestResult (s1.Difference (s2).empty ());
                VerifyTestResult ((s1 - s2).empty ());
                s2.Add (3);
                VerifyTestResult ((s1 == mk_ (factory, initializer_list<int>{1, 2})));
                VerifyTestResult ((s2 == mk_ (factory, initializer_list<int>{1, 2, 3})));

                VerifyTestResult ((s1 - s2).empty ());
                VerifyTestResult ((s2 - s1).length () == 1);
                VerifyTestResult (((s1 - s2) == factory ()));
                VerifyTestResult (((s2 - s1) == mk_ (factory, initializer_list<int>{3})));

                Verify (s1.Intersects (s2));
                VerifyTestResult ((s1.Intersection (s2) == mk_ (factory, initializer_list<int>{1, 2})));
                VerifyTestResult (((s1 ^ s2) == mk_ (factory, initializer_list<int>{1, 2})));
                Verify (s1.Intersection (s2).length () == 2);
            }
        }

        template <typename USING_SET_CONTAINER, typename USING_BASESET_CONTAINER, typename CONCRETE_CONTAINER_FACTORY, typename TEST_FUNCTION>
        void Test_All_For_Type (CONCRETE_CONTAINER_FACTORY factory, TEST_FUNCTION applyToContainer)
        {
            Test1_IterableTests_::DoAllTests_<USING_SET_CONTAINER> (factory);
            Test2_BasicConstruction_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (factory, applyToContainer);
            Test3_AddRemove_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (factory, applyToContainer);
            Test4_Equals_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (factory, applyToContainer);
            Test5_UnionDifferenceIntersectionEtc_::DoAllTests_<USING_SET_CONTAINER, USING_BASESET_CONTAINER> (factory, applyToContainer);
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ */
