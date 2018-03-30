/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/MultiSet.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace MultiSetTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        template <typename CONCRETE_CONTAINER>
        struct DefaultFactory {
            CONCRETE_CONTAINER operator() () const { return CONCRETE_CONTAINER (); };
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

                    VerifyTestResult (s.GetLength () == 0);
                    applyToContainer (s);

                    for (CountedValue<T> i : s) {
                        VerifyTestResult (false);
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
                            s.UpdateCount (it, 1);
                        }
                        applyToContainer (s);

                        VerifyTestResult (s.GetLength () == kTestSize);

                        {
                            for (CountedValue<T> it : s) {
                                for (size_t i = 1; i <= kTestSize; i++) {
                                    VerifyTestResult (s.Contains (i));
                                    VerifyTestResult (s.GetLength () == kTestSize - i + 1);
                                    s.Remove (i);
                                    VerifyTestResult (not s.Contains (i - 1));
                                }
                            }
                            VerifyTestResult (s.IsEmpty ());
                            VerifyTestResult (s.GetLength () == 0);
                        }

                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        applyToContainer (s);
                        VerifyTestResult (s.GetLength () == kTestSize);
                        {
                            for (auto it = s.begin (); it != s.end (); ++it) {
                                s.Remove (it);
                                applyToContainer (s);
                            }
                            VerifyTestResult (s.IsEmpty ());
                            VerifyTestResult (s.GetLength () == 0);
                        }

                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        VerifyTestResult (s.GetLength () == kTestSize);
                        for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                            s.Remove (it2.Current ().fValue);
                        }
                        VerifyTestResult (s.GetLength () == 0);
                    }

                    /*
                    * Try removes multiple iterators present.
                    */
                    {
                        s.RemoveAll ();
                        applyToContainer (s);
                        VerifyTestResult (s.GetLength () == 0);
                        for (size_t i = 1; i <= kTestSize; i++) {
                            s.Add (i);
                            applyToContainer (s);
                        }
                        VerifyTestResult (s.GetLength () == kTestSize);
                        size_t i = 1;

                        for (auto it = s.begin (); it != s.end (); ++it) {
                            for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                                for (auto it3 = s.begin (); it3 != s.end (); ++it3) {
                                    if (s.GetLength () != 0) {
                                        applyToContainer (s);
                                        s.UpdateCount (it3, 3);
                                        applyToContainer (s);
                                        s.Remove (it3);
                                        applyToContainer (s);
                                        s.Add (i);
                                        applyToContainer (s);
                                        s.Remove (i);
                                        applyToContainer (s);
                                    }
                                }
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

                    VerifyTestResult (s1 == s);
                    TALLY_ARCHTYPE s2 = s1;

                    VerifyTestResult (s2 == s);
                    VerifyTestResult (s2 == s1);
                    s2.Add (three);
                    applyToContainer (mk_ (testingSchema.Factory, s2));
                    VerifyTestResult (s1 == s);
                    VerifyTestResult (s2 != s1);

                    MultiSetIteratorTests_ (testingSchema, s);

                    static const size_t K = qDebug ? (Debug::IsRunningUnderValgrind () ? 50 : 250) : 500;

                    VerifyTestResult (s.IsEmpty ());
                    s.Add (three);
                    VerifyTestResult (s.GetLength () == 1);
                    s += three;
                    VerifyTestResult (s.GetLength () == 1);
                    VerifyTestResult (s.Contains (three));
                    VerifyTestResult (s.OccurrencesOf (three) == 2);
                    s.Remove (three);
                    applyToContainer (s);
                    VerifyTestResult (s.GetLength () == 1);
                    VerifyTestResult (s.Contains (three));
                    VerifyTestResult (s.OccurrencesOf (three) == 1);
                    s.Remove (three);
                    VerifyTestResult (s.IsEmpty ());
                    s.RemoveAll ();
                    VerifyTestResult (s.IsEmpty ());
                    for (size_t i = 1; i <= K; i++) {
                        s.Add (i);
                        applyToContainer (s);
                    }

                    for (size_t i = 1; i <= s.GetLength (); i++) {
                        VerifyTestResult (s.Contains (i));
                        VerifyTestResult (not s.Contains (0));
                    }

                    for (size_t i = 1; i <= s.GetLength (); i++) {
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            if (testingSchema.fEqualsComparer (it.Current ().fValue, i)) {
                                break;
                            }
                        }
                    }

                    s.RemoveAll ();
                    VerifyTestResult (s.IsEmpty ());
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    applyToContainer (s);

                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.Elements ()) {
                            applyToContainer (s);
                        }
                    }
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.UniqueElements ()) {
                            applyToContainer (s);
                        }
                    }
                    {
                        auto saved = s;
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            s.RemoveAll ();
                        }
                        s = saved;
                    }
                    {
                        auto saved = s;
                        s.Add (32);
                        for (auto it1 : s) {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                        s = saved;
                    }

                    {
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            auto tmp = s;
                            s.Add (1);
                        }
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            auto tmp = s;
                            for (auto it1 : tmp) {
                            }
                            s.Add (1);
                        }
                        for (auto it = s.begin (); it != s.end (); ++it) {
                            auto tmp = s.Elements ();
                            for (auto it1 : tmp) {
                            }
                        }
                    }

                    // This next case fails - 2014-01-01
                    // Sent email to Sterl - I think the bug is that we must BreakReferences when
                    // copying a new object - if it has any active iterators. But think that through!
                    // -- LGP 2014-01-01
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        auto tmp = s.Elements ();
                        s.Add (1);
                    }

                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.Elements ()) {
                            if (s.TotalOccurrences () < 25) {
                                s.Add (1);
                            }
                            applyToContainer (s);
                            break;
                        }
                    }
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.Elements ()) {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                    }
                    s.Add (3);
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.UniqueElements ()) {
                            s.RemoveAll ();
                            applyToContainer (s);
                        }
                    }
                    VerifyTestResult (s.IsEmpty ());
                    VerifyTestResult (s.GetLength () == 0);

                    for (auto it1 = s.begin (); it1 != s.end (); ++it1) {
                        for (auto it2 = s.begin (); it2 != s.end (); ++it2) {
                            VerifyTestResult (false);
                        }
                    }
                    VerifyTestResult (s.IsEmpty ());

                    for (size_t i = 1; i <= K; i++) {
                        s.Add (i);
                        VerifyTestResult (s.Contains (i));
                        VerifyTestResult (s.OccurrencesOf (i) == 1);
                        VerifyTestResult (s.GetLength () == i);
                    }
                    for (size_t i = K; i > 0; i--) {
                        s.Remove (i);
                        VerifyTestResult (not s.Contains (i));
                        VerifyTestResult (s.GetLength () == (i - 1));
                    }
                    VerifyTestResult (s.IsEmpty ());

                    for (size_t i = 1; i <= K / 2; i++) {
                        s += 1;
                        VerifyTestResult (s.OccurrencesOf (1) == i);
                    }
                    size_t oldLength = s.GetLength ();
                    size_t oldTotal  = s.TotalOccurrences ();
                    applyToContainer (s);
                    s += s;
                    applyToContainer (s);
                    VerifyTestResult (s.GetLength () == oldLength);
                    VerifyTestResult (s.TotalOccurrences () == oldTotal * 2);
                }

                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    {
                        Debug::TraceContextBumper ctx{L"{}::Test1_MiscStarterTests_::DoAllTests_"};
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
                    Debug::TraceContextBumper ctx{L"{}::Test2_Equals::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    CONCRETE_CONTAINER s     = testingSchema.Factory ();
                    CONCRETE_CONTAINER s2    = s;
                    s.Add (1);
                    s.Add (1);
                    s.Add (2);
                    VerifyTestResult (s.size () == 2);
                    CONCRETE_CONTAINER s3 = s;
                    VerifyTestResult (s == s3);
                    VerifyTestResult (s.Equals (s3));
                    VerifyTestResult (not(s != s3));

                    VerifyTestResult (s != s2);
                    VerifyTestResult (not s.Equals (s2));
                    VerifyTestResult (not(s == s2));
                }
            }

            namespace Test3_Elements {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"{}::Test3_Elements::DoAllTests_"};
                    {
                        using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                        using TALLY_ARCHTYPE     = typename CONCRETE_CONTAINER::ArchetypeContainerType;
                        CONCRETE_CONTAINER s     = mk_ (testingSchema.Factory, initializer_list<int>{1, 2, 3, 4, 4, 4, 4});
                        VerifyTestResult (s.size () == 4);
                        VerifyTestResult (s.Elements ().size () == 7);
                        TALLY_ARCHTYPE tmp = mk_ (testingSchema.Factory, s.Elements ());
                        VerifyTestResult (tmp.OccurrencesOf (1) == 1);
                        VerifyTestResult (tmp.OccurrencesOf (0) == 0);
                        VerifyTestResult (tmp.OccurrencesOf (4) == 4);
                    }
                }
            }

            namespace Test4_UniqueElements {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"{}::Test4_UniqueElements::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    {
                        CONCRETE_CONTAINER s = mk_ (testingSchema.Factory, initializer_list<int>{1, 2, 3, 4, 4, 4, 4});
                        VerifyTestResult (s.size () == 4);
                        VerifyTestResult (s.UniqueElements ().size () == 4);
                    }
                }
            }

            namespace Test5_Elements_Crasher_ {
                template <typename DEFAULT_TESTING_SCHEMA>
                void T1_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"{}::Test5_Elements_Crasher_::DoAllTests_"};
                    using CONCRETE_CONTAINER = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    CONCRETE_CONTAINER s     = testingSchema.Factory ();
                    size_t             three = 3;
                    const size_t       K     = 500;
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.Elements ()) {
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
                    size_t             three = 3;
                    const size_t       K     = 500;
                    s.RemoveAll ();
                    VerifyTestResult (s.IsEmpty ());
                    for (size_t i = 1; i <= K / 50; i++) {
                        s.Add (i);
                    }
                    testingSchema.ApplyToContainerExtraTest (s);
                    for (auto it = s.begin (); it != s.end (); ++it) {
                        for (auto it1 : s.Elements ()) {
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
            Debug::TraceContextBumper ctx{L"{}::All_For_Type"};
            PRIVATE_::Test1_MiscStarterTests_::DoAllTests_ (testingSchema);
            PRIVATE_::Test2_Equals::DoAllTests_ (testingSchema);
            PRIVATE_::Test3_Elements::DoAllTests_ (testingSchema);
            PRIVATE_::Test4_UniqueElements::DoAllTests_ (testingSchema);
            PRIVATE_::Test5_Elements_Crasher_::DoAllTests_ (testingSchema);
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_MultiSet_h_ */
