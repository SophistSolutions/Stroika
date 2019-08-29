/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/Mapping.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace MappingTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        template <typename CONCRETE_CONTAINER>
        struct DefaultFactory {
            CONCRETE_CONTAINER operator() () const { return CONCRETE_CONTAINER (); };
        };

        template <typename CONCRETE_CONTAINER, typename FACTORY = DefaultFactory<CONCRETE_CONTAINER>, typename VALUE_EQUALS_COMPARER_TYPE = equal_to<typename CONCRETE_CONTAINER::mapped_type>>
        struct DEFAULT_TESTING_SCHEMA {
            constexpr inline DEFAULT_TESTING_SCHEMA (FACTORY factory = {}, VALUE_EQUALS_COMPARER_TYPE valueEqualsComparer = {})
                : Factory (factory)
                , fValueEqualsComparer (valueEqualsComparer)
            {
            }
            using ConcreteContainerType                                              = CONCRETE_CONTAINER;
            using ArchetypeContainerType                                             = typename CONCRETE_CONTAINER::ArchetypeContainerType;
            using key_type                                                           = typename CONCRETE_CONTAINER::key_type;
            using mapped_type                                                        = typename CONCRETE_CONTAINER::mapped_type;
            function<void (const ArchetypeContainerType&)> ApplyToContainerExtraTest = [] (const ArchetypeContainerType&) {};
            using ValueEqualsCompareFunctionType                                     = VALUE_EQUALS_COMPARER_TYPE;

            FACTORY                    Factory;
            VALUE_EQUALS_COMPARER_TYPE fValueEqualsComparer;
        };

        namespace Private_ {

            namespace Test1_BasicConstruction {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test1_BasicConstruction"};
                    using ConcreteContainerType  = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    using ArchetypeContainerType = typename DEFAULT_TESTING_SCHEMA::ArchetypeContainerType;
                    ConcreteContainerType s      = testingSchema.Factory ();
                    testingSchema.ApplyToContainerExtraTest (s);
                    ConcreteContainerType s1 = s;
                    testingSchema.ApplyToContainerExtraTest (s1);
                    ArchetypeContainerType s2 = s;
                    testingSchema.ApplyToContainerExtraTest (s2);
                }
            }

            namespace Test2_AddRemove {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test2_AddRemove"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    Verify (m.Lookup (1, nullptr));
                    Verify (not m.Lookup (2, nullptr));
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    IterableTests::SimpleIterableTest_All_For_Type<ConcreteContainerType> (m);
                    m.Remove (1);
                    VerifyTestResult (m.size () == 0);

                    {
                        m.Add (1, 2);
                        m.Add (3, 66);
                        size_t oldLength = m.GetLength ();
                        m += m;
                        testingSchema.ApplyToContainerExtraTest (m);
                        VerifyTestResult (m.GetLength () == oldLength);
                    }

                    m.RemoveAll ();
                    VerifyTestResult (m.size () == 0);
                }
            }

            namespace Test_3_Iteration {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test_3_Iteration"};
                    using key_type                     = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using ConcreteContainerType        = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    using KeyEqualsCompareFunctionType = typename ConcreteContainerType::KeyEqualsCompareFunctionType;
                    ConcreteContainerType m            = testingSchema.Factory ();
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    for (auto i : m) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    }
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    for (auto i : m) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    }
                    m.Remove (1);
                    VerifyTestResult (m.size () == 0);
                    for ([[maybe_unused]] auto i : m) {
                        VerifyTestResult (false);
                    }
                    m.Add (1, 2);
                    m.Add (2, 3);
                    m.Add (3, 4);
                    unsigned int cnt = 0;
                    for ([[maybe_unused]] auto i : m) {
                        cnt++;
                        if (cnt == 1) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        }
                        if (cnt == 2) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{2}));
                        }
                        if (cnt == 3) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{3}));
                        }
                    }
                    VerifyTestResult (cnt == 3);
                    m.RemoveAll ();
                    VerifyTestResult (m.size () == 0);
                }
            }

            namespace Test4_Equals {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test4_Equals"};
                    using key_type                       = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type                    = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType          = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    using ValueEqualsCompareFunctionType = typename DEFAULT_TESTING_SCHEMA::ValueEqualsCompareFunctionType;
                    ConcreteContainerType m              = testingSchema.Factory ();
                    ConcreteContainerType m2             = m;
                    m.Add (1, 88);
                    m.Add (2, 101);
                    VerifyTestResult (m.size () == 2);
                    ConcreteContainerType m3 = m;
                    testingSchema.ApplyToContainerExtraTest (m);
                    testingSchema.ApplyToContainerExtraTest (m2);
                    testingSchema.ApplyToContainerExtraTest (m3);
                    //VerifyTestResult (m == m3);
                    VerifyTestResult ((typename Mapping<key_type, mapped_type>::template EqualsComparer<ValueEqualsCompareFunctionType>{testingSchema.fValueEqualsComparer}(m, m3)));
                    //VerifyTestResult (not (m != m3));

                    //VerifyTestResult (m != m2);
                    VerifyTestResult ((not typename Mapping<key_type, mapped_type>::template EqualsComparer<ValueEqualsCompareFunctionType>{testingSchema.fValueEqualsComparer}(m, m2)));
                    //VerifyTestResult (not (m == m2));
                }
            }

            namespace Test5_ToFromSTLMap {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test5_ToFromSTLMap"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type           = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    ConcreteContainerType m2    = m;
                    m.Add (1, 88);
                    m.Add (2, 101);

                    {
                        map<key_type, mapped_type> n = m.template As<map<key_type, mapped_type>> ();
                        VerifyTestResult (n.size () == 2);
                        ConcreteContainerType tmp = ConcreteContainerType (n);
                        VerifyTestResult (testingSchema.fValueEqualsComparer (*tmp.Lookup (1), 88));
                        map<key_type, mapped_type> nn = tmp.template As<map<key_type, mapped_type>> ();
                        VerifyTestResult (nn == n);
                    }
                }
            }

            namespace Test6_AsSTLVector {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test6_AsSTLVector"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type           = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    ConcreteContainerType m2    = m;
                    m.Add (1, 88);
                    m.Add (2, 101);
                    VerifyTestResult (m.size () == 2);

                    {
                        vector<KeyValuePair<key_type, mapped_type>> n = m.template As<vector<KeyValuePair<key_type, mapped_type>>> ();
                        VerifyTestResult (n.size () == m.size ());
                    }
                    {
                        vector<pair<key_type, mapped_type>> n = m.template As<vector<pair<key_type, mapped_type>>> ();
                        VerifyTestResult (n.size () == m.size ());
                    }
                }
            }

            namespace Test7_Keys {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test7_Keys"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    static const size_t   K     = qDebug ? (Debug::IsRunningUnderValgrind () ? 25 : 50) : 100;
                    ConcreteContainerType c     = testingSchema.Factory ();
                    for (size_t i = 0; i < K; ++i) {
                        c.Add (i, i);
                    }
                    VerifyTestResult (c.Keys ().length () == K);
                    {
                        // be sure copying and iterating multiple times over the iterable doesnt produce differnt results.
                        auto keys = c.Keys ();
                        VerifyTestResult (keys.length () == K);
                        size_t a = 0;
                        for ([[maybe_unused]] auto i : keys) {
                            Lambda_Arg_Unused_BWA (i);
                            a++;
                        }
                        VerifyTestResult (a == K);
                        a = 0;
                        for ([[maybe_unused]] auto i : keys) {
                            Lambda_Arg_Unused_BWA (i);
                            a++;
                        }
                        VerifyTestResult (a == K);
                    }
                }
            }

            namespace Test_8_Iteration_With_Value_Comparer {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test_8_Iteration_With_Value_Comparer"};
                    using key_type                     = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using ConcreteContainerType        = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    using KeyEqualsCompareFunctionType = typename ConcreteContainerType::KeyEqualsCompareFunctionType;
                    ConcreteContainerType m            = testingSchema.Factory ();
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    for (auto i : m) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        VerifyTestResult (testingSchema.fValueEqualsComparer (i.fValue, 2));
                    }
                    m.Add (1, 2);
                    VerifyTestResult (m.size () == 1);
                    for (auto i : m) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        VerifyTestResult (testingSchema.fValueEqualsComparer (i.fValue, 2));
                    }
                    m.Remove (1);
                    VerifyTestResult (m.size () == 0);
                    for ([[maybe_unused]] auto i : m) {
                        VerifyTestResult (false);
                    }
                    m.Add (1, 2);
                    m.Add (2, 3);
                    m.Add (3, 4);
                    unsigned int cnt = 0;
                    for (auto i : m) {
                        cnt++;
                        if (cnt == 1) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                            VerifyTestResult (testingSchema.fValueEqualsComparer (i.fValue, 2));
                        }
                        if (cnt == 2) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{2}));
                            VerifyTestResult (testingSchema.fValueEqualsComparer (i.fValue, 3));
                        }
                        if (cnt == 3) {
                            VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{3}));
                            VerifyTestResult (testingSchema.fValueEqualsComparer (i.fValue, 4));
                        }
                    }
                    VerifyTestResult (cnt == 3);
                    m.RemoveAll ();
                    VerifyTestResult (m.size () == 0);
                }
            }

            namespace Test9_RetainAll {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test9_RetainAll"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType c     = testingSchema.Factory ();
                    for (int i = 0; i < 100; ++i) {
                        c.Add (i, i);
                    }
                    VerifyTestResult (c.Keys ().length () == 100);

                    using KT = typename ConcreteContainerType::key_type;
                    c.RetainAll (initializer_list<KT>{1, 3, 5});
                    VerifyTestResult (c.Keys ().length () == 3);
                    VerifyTestResult (c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}, c.GetKeyEqualsComparer ()));

                    c.RetainAll (Iterable<KT>{3});
                    VerifyTestResult (not c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}, c.GetKeyEqualsComparer ()));
                    VerifyTestResult (c.Keys ().SetEquals (Iterable<KT>{3}, c.GetKeyEqualsComparer ()));
                }
            }
        }
        template <typename DEFAULT_TESTING_SCHEMA>
        void SimpleMappingTest_All_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_"};
            Private_::Test1_BasicConstruction::DoAllTests_ (testingSchema);
            Private_::Test2_AddRemove::DoAllTests_ (testingSchema);
            Private_::Test_3_Iteration::DoAllTests_ (testingSchema);
            Private_::Test6_AsSTLVector::DoAllTests_ (testingSchema);
            Private_::Test7_Keys::DoAllTests_ (testingSchema);
            Private_::Test_8_Iteration_With_Value_Comparer::DoAllTests_ (testingSchema);
            Private_::Test4_Equals::DoAllTests_ (testingSchema);
        }

        template <typename DEFAULT_TESTING_SCHEMA>
        void SimpleMappingTest_WithDefaultEqCompaerer_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_"};
            Private_::Test5_ToFromSTLMap::DoAllTests_ (testingSchema);

            // @todo FIX - RetainAll has no good reason to require operator < etc to work o it but current impl does - LGP 2018-04-14
            Private_::Test9_RetainAll::DoAllTests_ (testingSchema);
        }
    }
}
#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ */
