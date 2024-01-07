/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/Mapping.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace MappingTests {

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
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test1_BasicConstruction"};
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
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test2_AddRemove"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    Verify (m.Lookup (1, nullptr));
                    Verify (not m.Lookup (2, nullptr));
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    IterableTests::SimpleIterableTest_All_For_Type<ConcreteContainerType> (m);
                    m.Remove (1);
                    EXPECT_TRUE (m.size () == 0);

                    {
                        m.Add (1, 2);
                        m.Add (3, 66);
                        size_t oldLength = m.size ();
                        m += m;
                        testingSchema.ApplyToContainerExtraTest (m);
                        EXPECT_TRUE (m.size () == oldLength);
                    }

                    {
                        m.RemoveAll ();
                        m.Add (1, 2);
                        m.Add (3, 66);
                        EXPECT_TRUE (m.size () == 2);
                        m.erase (1);
                        EXPECT_TRUE (m.size () == 1);
                        auto i = m.erase (m.begin ());
                        //
                        EXPECT_TRUE (m.size () == 0);
                        m.Add (1, 2);
                        m.Add (3, 66);
                        m.Add (5, 66);
                        EXPECT_TRUE (m.size () == 3);
                        i = m.begin ();
                        i = m.erase (i);
                        EXPECT_TRUE (m.size () == 2);
                    }

                    m.RemoveAll ();
                    EXPECT_TRUE (m.size () == 0);
                }
            }

            namespace Test_3_Iteration {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test_3_Iteration"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    for (auto i : m) {
                        EXPECT_TRUE (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    }
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    for (auto i : m) {
                        EXPECT_TRUE (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    }
                    m.Remove (1);
                    EXPECT_TRUE (m.size () == 0);
                    for ([[maybe_unused]] auto i : m) {
                        EXPECT_TRUE (false);
                    }
                    m.Add (1, 2);
                    m.Add (2, 3);
                    m.Add (3, 4);
                    vector<key_type> ss;
                    for (auto i : m) {
                        if (m.GetKeyEqualsComparer () (i.fKey, key_type{1})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 2));
                        }
                        else if (m.GetKeyEqualsComparer () (i.fKey, key_type{2})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 3));
                        }
                        else if (m.GetKeyEqualsComparer () (i.fKey, key_type{3})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 4));
                        }
                        else {
                            EXPECT_TRUE (false);
                        }
                        ss.push_back (i.fKey);
                    }
                    EXPECT_TRUE (ss.size () == 3);
                    m.RemoveAll ();
                    EXPECT_TRUE (m.size () == 0);
                }
            }

            namespace Test4_Equals {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test4_Equals"};
                    using key_type                       = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type                    = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType          = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    using ValueEqualsCompareFunctionType = typename DEFAULT_TESTING_SCHEMA::ValueEqualsCompareFunctionType;
                    ConcreteContainerType m              = testingSchema.Factory ();
                    ConcreteContainerType m2             = m;
                    m.Add (1, 88);
                    m.Add (2, 101);
                    EXPECT_TRUE (m.size () == 2);
                    ConcreteContainerType m3 = m;
                    testingSchema.ApplyToContainerExtraTest (m);
                    testingSchema.ApplyToContainerExtraTest (m2);
                    testingSchema.ApplyToContainerExtraTest (m3);
                    //EXPECT_TRUE (m == m3);
                    EXPECT_TRUE ((typename Mapping<key_type, mapped_type>::template EqualsComparer<ValueEqualsCompareFunctionType>{
                        testingSchema.fValueEqualsComparer}(m, m3)));
                    //EXPECT_TRUE (not (m != m3));

                    //EXPECT_TRUE (m != m2);
                    EXPECT_TRUE ((not typename Mapping<key_type, mapped_type>::template EqualsComparer<ValueEqualsCompareFunctionType>{
                        testingSchema.fValueEqualsComparer}(m, m2)));
                    //EXPECT_TRUE (not (m == m2));
                }
            }

            namespace Test5_ToFromSTLMap {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test5_ToFromSTLMap"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type           = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    ConcreteContainerType m2    = m;
                    m.Add (1, 88);
                    m.Add (2, 101);

                    {
                        map<key_type, mapped_type> n = m.template As<map<key_type, mapped_type>> ();
                        EXPECT_TRUE (n.size () == 2);
                        ConcreteContainerType tmp = ConcreteContainerType{n};
                        EXPECT_TRUE (testingSchema.fValueEqualsComparer (*tmp.Lookup (1), 88));
                        map<key_type, mapped_type> nn = tmp.template As<map<key_type, mapped_type>> ();
                        EXPECT_TRUE (nn == n);
                    }
                }
            }

            namespace Test6_AsSTLVector {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test6_AsSTLVector"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using mapped_type           = typename DEFAULT_TESTING_SCHEMA::mapped_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    ConcreteContainerType m2    = m;
                    m.Add (1, 88);
                    m.Add (2, 101);
                    EXPECT_TRUE (m.size () == 2);

                    {
                        vector<KeyValuePair<key_type, mapped_type>> n = m.template As<vector<KeyValuePair<key_type, mapped_type>>> ();
                        EXPECT_TRUE (n.size () == m.size ());
                    }
                    {
                        vector<pair<key_type, mapped_type>> n = m.template As<vector<pair<key_type, mapped_type>>> ();
                        EXPECT_TRUE (n.size () == m.size ());
                    }
                }
            }

            namespace Test7_Keys {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test7_Keys"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    static const size_t   K     = qDebug ? (Debug::IsRunningUnderValgrind () ? 25 : 50) : 100;
                    ConcreteContainerType c     = testingSchema.Factory ();
                    for (size_t i = 0; i < K; ++i) {
                        c.Add (i, i);
                    }
                    EXPECT_TRUE (c.Keys ().length () == K);
                    {
                        // be sure copying and iterating multiple times over the iterable doesnt produce differnt results.
                        auto keys = c.Keys ();
                        EXPECT_TRUE (keys.length () == K);
                        size_t a = 0;
                        for ([[maybe_unused]] auto i : keys) {
                            a++;
                        }
                        EXPECT_TRUE (a == K);
                        a = 0;
                        for ([[maybe_unused]] auto i : keys) {
                            a++;
                        }
                        EXPECT_TRUE (a == K);
                    }
                }
            }

            namespace Test_8_Iteration_With_Value_Comparer {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test_8_Iteration_With_Value_Comparer"};
                    using key_type              = typename DEFAULT_TESTING_SCHEMA::key_type;
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType m     = testingSchema.Factory ();
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    for (auto i : m) {
                        EXPECT_TRUE (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 2));
                    }
                    m.Add (1, 2);
                    EXPECT_TRUE (m.size () == 1);
                    for (auto i : m) {
                        EXPECT_TRUE (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 2));
                    }
                    m.Remove (1);
                    EXPECT_TRUE (m.size () == 0);
                    for ([[maybe_unused]] auto i : m) {
                        EXPECT_TRUE (false);
                    }
                    m.Add (1, 2);
                    m.Add (2, 3);
                    m.Add (3, 4);
                    for (auto i : m) {
                        if (m.GetKeyEqualsComparer () (i.fKey, key_type{1})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 2));
                        }
                        else if (m.GetKeyEqualsComparer () (i.fKey, key_type{2})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 3));
                        }
                        else if (m.GetKeyEqualsComparer () (i.fKey, key_type{3})) {
                            EXPECT_TRUE (testingSchema.fValueEqualsComparer (i.fValue, 4));
                        }
                        else {
                            EXPECT_TRUE (false);
                        }
                    }
                    m.RemoveAll ();
                    EXPECT_TRUE (m.size () == 0);
                }
            }

            namespace Test9_RetainAll {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test9_RetainAll"};
                    using ConcreteContainerType = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType c     = testingSchema.Factory ();
                    for (int i = 0; i < 100; ++i) {
                        c.Add (i, i);
                    }
                    EXPECT_TRUE (c.Keys ().length () == 100);

                    using KT = typename ConcreteContainerType::key_type;
                    c.RetainAll (initializer_list<KT>{1, 3, 5});
                    EXPECT_TRUE (c.Keys ().length () == 3);
                    EXPECT_TRUE (c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}, c.GetKeyEqualsComparer ()));

                    c.RetainAll (Iterable<KT>{3});
                    EXPECT_TRUE (not c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}, c.GetKeyEqualsComparer ()));
                    EXPECT_TRUE (c.Keys ().SetEquals (Iterable<KT>{3}, c.GetKeyEqualsComparer ()));
                }
            }
            namespace Test10_NewIteratorPatching {
                template <typename DEFAULT_TESTING_SCHEMA>
                void DoAllTests_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
                {
                    Debug::TraceContextBumper ctx{"CommonTests::MappingTests::Test10_NewIteratorPatching"};
                    using ConcreteContainerType   = typename DEFAULT_TESTING_SCHEMA::ConcreteContainerType;
                    ConcreteContainerType headers = testingSchema.Factory ();
                    headers.Add (1, 2);
                    headers.Add (2, 3);
                    ConcreteContainerType headers2 = headers; // up ref count before change
                    EXPECT_TRUE (headers.size () == 2);
                    EXPECT_TRUE (headers2.size () == 2);
                    for (auto hi = headers.begin (); hi != headers.end ();) {
                        hi = headers.erase (hi);
                    }
                    EXPECT_TRUE (headers2.size () == 2);
                    EXPECT_TRUE (headers.empty ());
                }
            }
        }
        template <typename DEFAULT_TESTING_SCHEMA>
        void SimpleMappingTest_All_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
        {
            Debug::TraceContextBumper ctx{"CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_"};
            Private_::Test1_BasicConstruction::DoAllTests_ (testingSchema);
            Private_::Test2_AddRemove::DoAllTests_ (testingSchema);
            Private_::Test_3_Iteration::DoAllTests_ (testingSchema);
            Private_::Test6_AsSTLVector::DoAllTests_ (testingSchema);
            Private_::Test7_Keys::DoAllTests_ (testingSchema);
            Private_::Test_8_Iteration_With_Value_Comparer::DoAllTests_ (testingSchema);
            Private_::Test4_Equals::DoAllTests_ (testingSchema);
            Private_::Test10_NewIteratorPatching::DoAllTests_ (testingSchema);
        }

        template <typename DEFAULT_TESTING_SCHEMA>
        void SimpleMappingTest_WithDefaultEqCompaerer_ (const DEFAULT_TESTING_SCHEMA& testingSchema)
        {
            Debug::TraceContextBumper ctx{"CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_"};
            Private_::Test5_ToFromSTLMap::DoAllTests_ (testingSchema);

            // @todo FIX - RetainAll has no good reason to require operator < etc to work o it but current impl does - LGP 2018-04-14
            Private_::Test9_RetainAll::DoAllTests_ (testingSchema);
        }
#endif

    }
}
#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ */
