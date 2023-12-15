/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Set.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace SetTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

#if qHasFeature_GoogleTest
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
                IterableTests::SimpleIterableTest_RequiringEqualsComparer<Iterable<value_type>> (s, s.GetElementEqualsComparer ());
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
                    EXPECT_TRUE (s3.size () == 4);
                    EXPECT_TRUE (s3.Contains (1));
                    EXPECT_TRUE (s3.Contains (2));
                    EXPECT_TRUE (s3.Contains (3));
                    EXPECT_TRUE (s3.Contains (4));
                    EXPECT_TRUE (not s3.Contains (5));
                }
                {
                    USING_SET_CONTAINER     s1 = mk_ (factory, initializer_list<int>{1});
                    USING_SET_CONTAINER     s2 = mk_ (factory, initializer_list<int>{1});
                    USING_BASESET_CONTAINER s3 = mk_ (factory, initializer_list<int>{1});
                    EXPECT_TRUE (s1.length () == 1);
                    EXPECT_TRUE (s2.length () == 1);
                    EXPECT_TRUE (s3.length () == 1);
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
                EXPECT_TRUE (s.size () == 1);
                EXPECT_TRUE (s.Contains (1));
                EXPECT_TRUE (not s.Contains (2));
                applyToContainer (s);
                s.Add (1);
                applyToContainer (s);
                EXPECT_TRUE (s.size () == 1);
                applyToContainer (s);
                IterableTests::SimpleIterableTest_All_For_Type<USING_SET_CONTAINER> (s);
                s.Remove (1);
                applyToContainer (s);
                EXPECT_TRUE (s.size () == 0);
                applyToContainer (s);

                {
                    s.Add (3);
                    s.Add (99);
                    size_t oldLength = s.size ();
                    s += s;
                    applyToContainer (s);
                    EXPECT_TRUE (s.size () == oldLength);
                }

                s.RemoveAll ();
                applyToContainer (s);
                EXPECT_TRUE (s.size () == 0);
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
                EXPECT_TRUE (s.size () == 2);
                USING_SET_CONTAINER s3 = s;
                applyToContainer (s);
                applyToContainer (s2);
                applyToContainer (s3);
                EXPECT_TRUE (s == s3);
                EXPECT_TRUE (typename USING_SET_CONTAINER::EqualsComparer{}(s, s3));
                EXPECT_TRUE (not(s != s3));

                EXPECT_TRUE (s != s2);
                EXPECT_TRUE (not typename USING_SET_CONTAINER::EqualsComparer{}(s, s2));
                EXPECT_TRUE (not(s == s2));
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

                EXPECT_TRUE (s2 == s1.Union (s2));
                EXPECT_TRUE (s2 == s1 + s2);
                EXPECT_TRUE ((s1 == mk_ (factory, initializer_list<int>{1, 2})));
                EXPECT_TRUE ((s2 == mk_ (factory, initializer_list<int>{1, 2})));

                EXPECT_TRUE (s1.Difference (s2).empty ());
                EXPECT_TRUE ((s1 - s2).empty ());
                s2.Add (3);
                EXPECT_TRUE ((s1 == mk_ (factory, initializer_list<int>{1, 2})));
                EXPECT_TRUE ((s2 == mk_ (factory, initializer_list<int>{1, 2, 3})));

                EXPECT_TRUE ((s1 - s2).empty ());
                EXPECT_TRUE ((s2 - s1).length () == 1);
                EXPECT_TRUE (((s1 - s2) == factory ()));
                EXPECT_TRUE (((s2 - s1) == mk_ (factory, initializer_list<int>{3})));

                Verify (s1.Intersects (s2));
                EXPECT_TRUE ((s1.Intersection (s2) == mk_ (factory, initializer_list<int>{1, 2})));
                EXPECT_TRUE (((s1 ^ s2) == mk_ (factory, initializer_list<int>{1, 2})));
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
#endif

    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Set_h_ */
