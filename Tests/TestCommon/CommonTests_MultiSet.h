/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/MultiSet.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace MultiSetTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

#if qHasFeature_GoogleTest
        template <typename CONCRETE_CONTAINER>
        struct DefaultFactory {
            CONCRETE_CONTAINER operator() () const
            {
                return CONCRETE_CONTAINER ();
            };
        };

        template <typename CONCRETE_CONTAINER, typename EQUALS_COMPARER = std::equal_to<typename CONCRETE_CONTAINER::MultiSetOfElementType>, typename FACTORY = DefaultFactory<CONCRETE_CONTAINER>>
        struct DEFAULT_TESTING_SCHEMA {
            constexpr inline DEFAULT_TESTING_SCHEMA (FACTORY factory = {})
                : Factory (factory)
            {
            }
            using ConcreteContainerType = CONCRETE_CONTAINER;
            using MultiSetOfElementType = typename CONCRETE_CONTAINER::MultiSetOfElementType;
            EQUALS_COMPARER fEqualsComparer;
            using ArchetypeContainerType = typename CONCRETE_CONTAINER::ArchetypeContainerType;
            static void ApplyToContainerExtraTest (const ConcreteContainerType&){};
            FACTORY     Factory;
        };

        template <typename DEFAULT_TESTING_SCHEMA>
        void All_For_Type (const DEFAULT_TESTING_SCHEMA& testingSchema);

        namespace PRIVATE_ {

            template <typename FACTORY, typename DEFAULT_VALUES_ITERABLE>
            auto mk_ (const FACTORY& f, const DEFAULT_VALUES_ITERABLE& il)
            {
                auto c = f ();
                for (auto i : il) {
                    c.Add (i);
                }
                return c;
            }

            namespace Test1_MiscStarterTests_ {
                template <typename DEFAULT_TESTING_SCHEMA>
                void MultiSetIteratorTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema, typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType& s)
                {
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    auto applyToContainer    = testingSchema.ApplyToContainerExtraTest;
                    using T                  = typename CONCRETE_CONTAINER::MultiSetOfElementType;
                    const size_t kTestSize   = 6;

                    EXPECT_TRUE (s.size () == 0);
                    applyToContainer (s);

                    for ([[maybe_unused]] CountedValue<T> i : s) {
                        EXPECT_TRUE (false);
                    }
                    IterableTests::SimpleIterableTest_All_For_Type<CONCRETE_CONTAINER> (s);

                    /*
                     * Try removes while iterating forward.
                     */
                    {
                        for (int i = 1; i <= kTestSize; i++) {
                            s.Add (T (i));
                        }
                        applyToContainer (s);
                        IterableTests::SimpleIterableTest_All_For_Type<CONCRETE_CONTAINER> (s);

                        for (auto it = s.begin (); it != s.end (); ++it) {
                            s.UpdateCount (it, 1, &it);
                        }
                        applyToContainer (s);

                        EXPECT_TRUE (s.size () == kTestSize);

                        {
                            for ([[maybe_unused]] CountedValue<T> it : s) {
                            }
                            for (size_t i = 1; i <= kTestSize; i++) {
                                EXPECT_TRUE (s.Contains (i));
                                EXPECT_TRUE (s.size () == kTestSize - i + 1);
                                s.Remove (i);
                                EXPECT_TRUE (not s.Contains (i - 1));
                            }
                            EXPECT_TRUE (s.empty ());
                            EXPECT_TRUE (s.size () == 0);
                        }

                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        applyToContainer (s);
                        EXPECT_TRUE (s.size () == kTestSize);
                        {
                            for (auto it = s.begin (); it != s.end ();) {
                                it = s.erase (it);
                                applyToContainer (s);
                            }
                            EXPECT_TRUE (s.empty ());
                            EXPECT_TRUE (s.size () == 0);
                        }

                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        EXPECT_TRUE (s.size () == kTestSize);
                        for (auto it2 = s.begin (); it2 != s.end ();) {
                            auto current = it2.Current ().fValue;
                            it2          = s.end (); // cannot keep live iterator during remove
                            s.Remove (current);
                            it2 = s.begin ();
                        }
                        EXPECT_TRUE (s.size () == 0);
                    }

                    /*
                    * Try removes multiple with iteration
                    */
                    {
                        s.RemoveAll ();
                        applyToContainer (s);
                        EXPECT_TRUE (s.size () == 0);
                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        EXPECT_TRUE (s.size () == kTestSize);
                        for (auto it3 = s.begin (); it3 != s.end ();) {
                            if (s.size () != 0) {
                                applyToContainer (s);
                                s.UpdateCount (it3, 3, &it3);
                                applyToContainer (s);
                                it3 = s.erase (it3);
                                applyToContainer (s);
                            }
                        }
                    }
                }

                template <typename DEFAULT_TESTING_SCHEMA>
                void SimpleMultiSetTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema, typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType& s)
                {
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    auto applyToContainer    = testingSchema.ApplyToContainerExtraTest;
                    using TALLY_ARCHTYPE     = typename CONCRETE_CONTAINER::ArchetypeContainerType;

                    size_t three = 3;

                    applyToContainer (s);

                    TALLY_ARCHTYPE s1 (s);

                    EXPECT_TRUE (s1 == s);
                    TALLY_ARCHTYPE s2 = s1;

                    EXPECT_TRUE (s2 == s);
                    EXPECT_TRUE (s2 == s1);
                    s2.Add (three);
                    applyToContainer (mk_ (testingSchema.Factory, s2));
                    EXPECT_TRUE (s1 == s);
                    EXPECT_TRUE (s2 != s1);

                    MultiSetIteratorTests_ (testingSchema, s);

                    static const size_t K = qDebug ? (Debug::IsRunningUnderValgrind () ? 50 : 250) : 500;

                    EXPECT_TRUE (s.empty ());
                    s.Add (three);
                    EXPECT_TRUE (s.size () == 1);
                    s += three;
                    EXPECT_TRUE (s.size () == 1);
                    EXPECT_TRUE (s.Contains (three));
                    EXPECT_TRUE (s.OccurrencesOf (three) == 2);
                    s.Remove (three);
                    applyToContainer (s);
                    EXPECT_TRUE (s.size () == 1);
                    EXPECT_TRUE (s.Contains (three));
                    EXPECT_TRUE (s.OccurrencesOf (three) == 1);
                    s.Remove (three);
                    EXPECT_TRUE (s.empty ());
                    s.RemoveAll ();
                    EXPECT_TRUE (s.empty ());
                    for (size_t i = 1; i <= K; i++) {
                        s.Add (i);
                        applyToContainer (s);
                    }

                    for (size_t i = 1; i <= s.size (); i++) {
                        EXPECT_TRUE (s.Contains (i));
                        EXPECT_TRUE (not s.Contains (0));
                    }

                    for (size_t i = 1; i <= s.size (); i++) {
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            if (testingSchema.fEqualsComparer (it.Current ().fValue, i)) {
                                break;
                            }
                        }
                    }

                    s.RemoveAll ();
                    EXPECT_TRUE (s.empty ());
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    applyToContainer (s);

                    for (
#if !qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy
                        [[maybe_unused]]
#endif
                        auto it = s.begin ();
                        it != s.end (); ++it) {
                        for (auto it1 [[maybe_unused]] : s.Elements ()) {
                            applyToContainer (s);
                        }
                    }
                    for (auto it [[maybe_unused]] = s.begin (); it != s.end (); ++it) {
                        for (auto it1 [[maybe_unused]] : s.UniqueElements ()) {
                            applyToContainer (s);
                        }
                    }
                    {
                        auto saved = s;
                        for (auto it [[maybe_unused]] = s.begin (); it != s.end (); ++it) {
                            s.RemoveAll ();
                        }
                        s = saved;
                    }
                    {
                        auto saved = s;
                        s.Add (32);
                        {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                        s = saved;
                    }

                    {
                        {
                            [[maybe_unused]] auto tmp = s;
                            s.Add (1);
                        }
                        {
                            auto tmp = s;
                            s.Add (1);
                            for (auto it1 [[maybe_unused]] : tmp) {
                            }
                        }
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            auto tmp = s.Elements ();
                            for (auto it1 [[maybe_unused]] : tmp) {
                            }
                        }
                    }

                    {
                        auto tmp = s.Elements ();
                        s.Add (1);
                    }

                    {
                        for (auto it1 [[maybe_unused]] : s.Elements ()) {
                            if (s.TotalOccurrences () < 25) {
                                s.Add (1);
                            }
                            applyToContainer (s);
                            break;
                        }
                    }
                    {
                        for (auto it1 [[maybe_unused]] : s.Elements ()) {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                    }
                    s.Add (3);
                    {
                        for (auto it1 [[maybe_unused]] : s.UniqueElements ()) {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                    }
                    EXPECT_TRUE (s.empty ());
                    EXPECT_TRUE (s.size () == 0);

                    for (auto it1 = s.begin (); it1 != s.end (); ++it1) {
                        for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                            EXPECT_TRUE (false);
                        }
                    }
                    EXPECT_TRUE (s.empty ());

                    for (size_t i = 1; i <= K; i++) {
                        s.Add (i);
                        EXPECT_TRUE (s.Contains (i));
                        EXPECT_TRUE (s.OccurrencesOf (i) == 1);
                        EXPECT_TRUE (s.size () == i);
                    }
                    for (size_t i = K; i > 0; --i) {
                        s.Remove (i);
                        EXPECT_TRUE (not s.Contains (i));
                        EXPECT_TRUE (s.size () == (i - 1));
                    }
                    EXPECT_TRUE (s.empty ());

                    for (size_t i = 1; i <= K / 2; i++) {
                        s += 1;
                        EXPECT_TRUE (s.OccurrencesOf (1) == i);
                    }
                    size_t oldLength = s.size ();
                    size_t oldTotal  = s.TotalOccurrences ();
                    applyToContainer (s);
                    s += s;
                    applyToContainer (s);
                    EXPECT_TRUE (s.size () == oldLength);
                    EXPECT_TRUE (s.TotalOccurrences () == oldTotal * 2);
                }

                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    {
                        Debug::TraceContextBumper ctx{"{}::Test1_MiscStarterTests_::DoAllTests_"};
                        using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                        CONCRETE_CONTAINER s     = testingSchema.Factory ();
                        SimpleMultiSetTests_ (testingSchema, s);
                    }
                }
            }

            namespace Test2_Equals {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"{}::Test2_Equals::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    CONCRETE_CONTAINER s     = testingSchema.Factory ();
                    CONCRETE_CONTAINER s2    = s;
                    s.Add (1);
                    s.Add (1);
                    s.Add (2);
                    EXPECT_TRUE (s.size () == 2);
                    CONCRETE_CONTAINER s3 = s;
                    EXPECT_TRUE (s == s3);
                    EXPECT_TRUE (not(s != s3));

                    EXPECT_TRUE (s != s2);
                    EXPECT_TRUE (not(s == s2));
                }
            }

            namespace Test3_Elements {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"{}::Test3_Elements::DoAllTests_"};
                    {
                        using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                        using TALLY_ARCHTYPE     = typename CONCRETE_CONTAINER::ArchetypeContainerType;
                        CONCRETE_CONTAINER s     = mk_ (testingSchema.Factory, initializer_list<int>{1, 2, 3, 4, 4, 4, 4});
                        EXPECT_TRUE (s.size () == 4);
                        EXPECT_TRUE (s.Elements ().size () == 7);
                        TALLY_ARCHTYPE tmp = mk_ (testingSchema.Factory, s.Elements ());
                        EXPECT_TRUE (tmp.OccurrencesOf (1) == 1);
                        EXPECT_TRUE (tmp.OccurrencesOf (0) == 0);
                        EXPECT_TRUE (tmp.OccurrencesOf (4) == 4);
                    }
                }
            }

            namespace Test4_UniqueElements {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"{}::Test4_UniqueElements::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    {
                        CONCRETE_CONTAINER s = mk_ (testingSchema.Factory, initializer_list<int>{1, 2, 3, 4, 4, 4, 4});
                        EXPECT_TRUE (s.size () == 4);
                        EXPECT_TRUE (s.UniqueElements ().size () == 4);
                    }
                }
            }

            namespace Test5_Elements_Crasher_ {
                template <typename DEFAULT_TESTING_SCHEMA>
                void T1_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"{}::Test5_Elements_Crasher_::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    CONCRETE_CONTAINER s     = testingSchema.Factory ();
                    const size_t       K     = 500;
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    {
                        for ([[maybe_unused]] auto it1 : s.Elements ()) {
                            if (s.TotalOccurrences () < 25) {
                                s.Add (1);
                            }
                            break;
                        }
                    }
                }
                template <typename DEFAULT_TESTING_SCHEMA>
                void T2_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    CONCRETE_CONTAINER s     = testingSchema.Factory ();
                    const size_t       K     = 500;
                    s.RemoveAll ();
                    EXPECT_TRUE (s.empty ());
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    testingSchema.ApplyToContainerExtraTest (s);
                    {
                        for ([[maybe_unused]] auto it1 : s.Elements ()) {
                            if (s.TotalOccurrences () < 25) {
                                s.Add (1);
                            }
                            testingSchema.ApplyToContainerExtraTest (s);
                            break;
                        }
                    }
                }
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    T1_ (testingSchema);
                    T2_ (testingSchema);
                }
            }
        }

        template <typename DEFAULT_TESTING_SCHEMA>
        void All_For_Type (const DEFAULT_TESTING_SCHEMA& testingSchema)
        {
            Debug::TraceContextBumper ctx{"{}::All_For_Type"};
            PRIVATE_::Test1_MiscStarterTests_::DoAllTests_ (testingSchema);
            PRIVATE_::Test2_Equals::DoAllTests_ (testingSchema);
            PRIVATE_::Test3_Elements::DoAllTests_ (testingSchema);
            PRIVATE_::Test4_UniqueElements::DoAllTests_ (testingSchema);
            PRIVATE_::Test5_Elements_Crasher_::DoAllTests_ (testingSchema);
        }
        #endif
        
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_ */
