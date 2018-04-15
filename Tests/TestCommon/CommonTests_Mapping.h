/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

        namespace Test1_BasicConstruction {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test1_BasicConstruction"};
                using key_type    = typename USING_MAPPING_CONTAINER::key_type;
                using mapped_type = typename USING_MAPPING_CONTAINER::mapped_type;
                USING_MAPPING_CONTAINER s;
                applyToContainer (s);
                USING_MAPPING_CONTAINER s1 = s;
                applyToContainer (s1);
                Mapping<key_type, mapped_type> s2 = s;
                applyToContainer (s2);
            }
        }

        namespace Test2_AddRemove {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test2_AddRemove"};
                USING_MAPPING_CONTAINER   m;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                Verify (m.Lookup (1, nullptr));
                Verify (not m.Lookup (2, nullptr));
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                IterableTests::SimpleIterableTest_All_For_Type<USING_MAPPING_CONTAINER> (m);
                m.Remove (1);
                VerifyTestResult (m.size () == 0);

                {
                    m.Add (1, 2);
                    m.Add (3, 66);
                    size_t oldLength = m.GetLength ();
                    m += m;
                    applyToContainer (m);
                    VerifyTestResult (m.GetLength () == oldLength);
                }

                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
            }
        }

        namespace Test_3_Iteration {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test_3_Iteration"};
                using KeyEqualsCompareFunctionType = typename USING_MAPPING_CONTAINER::KeyEqualsCompareFunctionType;
                USING_MAPPING_CONTAINER m;
                using key_type = typename USING_MAPPING_CONTAINER::key_type;
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
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION, typename VALUE_EQUALS_COMPARER_TYPE>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test4_Equals"};
                USING_MAPPING_CONTAINER   m;
                USING_MAPPING_CONTAINER   m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);
                VerifyTestResult (m.size () == 2);
                USING_MAPPING_CONTAINER m3 = m;
                applyToContainer (m);
                applyToContainer (m2);
                applyToContainer (m3);
                //VerifyTestResult (m == m3);
                VerifyTestResult (m.template Equals<VALUE_EQUALS_COMPARER_TYPE> (m3));
                //VerifyTestResult (not (m != m3));

                //VerifyTestResult (m != m2);
                VerifyTestResult (not m.template Equals<VALUE_EQUALS_COMPARER_TYPE> (m2));
                //VerifyTestResult (not (m == m2));
            }
        }

        namespace Test5_ToFromSTLMap {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test5_ToFromSTLMap"};
                USING_MAPPING_CONTAINER   m;
                USING_MAPPING_CONTAINER   m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);

                using KeyType   = typename USING_MAPPING_CONTAINER::key_type;
                using ValueType = typename USING_MAPPING_CONTAINER::mapped_type;

                {
                    map<KeyType, ValueType> n = m.template As<map<KeyType, ValueType>> ();
                    VerifyTestResult (n.size () == 2);
                    USING_MAPPING_CONTAINER tmp = USING_MAPPING_CONTAINER (n);
                    VerifyTestResult (*tmp.Lookup (1) == 88);
                    map<KeyType, ValueType> nn = tmp.template As<map<KeyType, ValueType>> ();
                    VerifyTestResult (nn == n);
                }
            }
        }

        namespace Test6_AsSTLVector {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test6_AsSTLVector"};
                USING_MAPPING_CONTAINER   m;
                USING_MAPPING_CONTAINER   m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);
                VerifyTestResult (m.size () == 2);

                using KeyType   = typename USING_MAPPING_CONTAINER::key_type;
                using ValueType = typename USING_MAPPING_CONTAINER::mapped_type;

                {
                    vector<KeyValuePair<KeyType, ValueType>> n = m.template As<vector<KeyValuePair<KeyType, ValueType>>> ();
                    VerifyTestResult (n.size () == m.size ());
                }
                {
                    vector<pair<KeyType, ValueType>> n = m.template As<vector<pair<KeyType, ValueType>>> ();
                    VerifyTestResult (n.size () == m.size ());
                }
            }
        }

        namespace Test7_Keys {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test7_Keys"};
                static const size_t       K = qDebug ? (Debug::IsRunningUnderValgrind () ? 25 : 50) : 100;
                USING_MAPPING_CONTAINER   c;
                for (size_t i = 0; i < K; ++i) {
                    c.Add (i, i);
                }
                VerifyTestResult (c.Keys ().length () == K);
                {
                    // be sure copying and iterating multiple times over the iterable doesnt produce differnt results.
                    auto keys = c.Keys ();
                    VerifyTestResult (keys.length () == K);
                    int a = 0;
                    for (auto i : keys) {
                        a++;
                    }
                    VerifyTestResult (a == K);
                    a = 0;
                    for (auto i : keys) {
                        a++;
                    }
                    VerifyTestResult (a == K);
                }
            }
        }

        namespace Test_8_Iteration_With_Value_Comparer {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION, typename VALUE_EQUALS_COMPARER_TYPE>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test_8_Iteration_With_Value_Comparer"};
                using KeyEqualsCompareFunctionType   = typename USING_MAPPING_CONTAINER::KeyEqualsCompareFunctionType;
                using ValueEqualsCompareFunctionType = VALUE_EQUALS_COMPARER_TYPE;
                USING_MAPPING_CONTAINER m;
                using key_type = typename USING_MAPPING_CONTAINER::key_type;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    VerifyTestResult (VALUE_EQUALS_COMPARER_TYPE{}(i.fValue, 2));
                }
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                    VerifyTestResult (VALUE_EQUALS_COMPARER_TYPE{}(i.fValue, 2));
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
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{1}));
                        VerifyTestResult (VALUE_EQUALS_COMPARER_TYPE{}(i.fValue, 2));
                    }
                    if (cnt == 2) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{2}));
                        VerifyTestResult (VALUE_EQUALS_COMPARER_TYPE{}(i.fValue, 3));
                    }
                    if (cnt == 3) {
                        VerifyTestResult (m.GetKeyEqualsComparer () (i.fKey, key_type{3}));
                        VerifyTestResult (VALUE_EQUALS_COMPARER_TYPE{}(i.fValue, 4));
                    }
                }
                VerifyTestResult (cnt == 3);
                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
            }
        }

        namespace Test9_RetainAll {
            template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::Test9_RetainAll"};
                USING_MAPPING_CONTAINER   c;
                for (int i = 0; i < 100; ++i) {
                    c.Add (i, i);
                }
                VerifyTestResult (c.Keys ().length () == 100);

                using KT = typename USING_MAPPING_CONTAINER::key_type;
                c.RetainAll (initializer_list<KT>{1, 3, 5});
                VerifyTestResult (c.Keys ().length () == 3);
                VerifyTestResult (c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}));

                c.RetainAll (Iterable<KT>{3});
                VerifyTestResult (not c.Keys ().SetEquals (Iterable<KT>{1, 3, 5}));
                VerifyTestResult (c.Keys ().SetEquals (Iterable<KT>{3}));
            }
        }

        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_ (TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_"};
            Test1_BasicConstruction::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test2_AddRemove::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test_3_Iteration::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test6_AsSTLVector::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test7_Keys::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
        }

        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void SimpleMappingTest_AllTestsRequireComparer_For_Type_ (TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_AllTestsRequireComparer_For_Type_"};
            Test5_ToFromSTLMap::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
            Test9_RetainAll::DoAllTests_<USING_MAPPING_CONTAINER> (applyToContainer);
        }

        template <typename USING_MAPPING_CONTAINER, typename TEST_FUNCTION>
        void SimpleMappingTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_All_For_Type"};
            SimpleMappingTest_AllTestsWhichDontRequireComparer_For_Type_<USING_MAPPING_CONTAINER> (applyToContainer);
            SimpleMappingTest_AllTestsRequireComparer_For_Type_<USING_MAPPING_CONTAINER> (applyToContainer);
        }

        template <typename USING_MAPPING_CONTAINER, typename VALUE_EQUALS_COMPARER_TYPE, typename TEST_FUNCTION>
        void SimpleMappingTest_WhichRequiresExplcitValueComparer (TEST_FUNCTION applyToContainer)
        {
            Debug::TraceContextBumper ctx{L"CommonTests::MappingTests::SimpleMappingTest_WhichRequiresExplcitValueComparer"};
            Test_8_Iteration_With_Value_Comparer::DoAllTests_<USING_MAPPING_CONTAINER, TEST_FUNCTION, VALUE_EQUALS_COMPARER_TYPE> (applyToContainer);
            Test4_Equals::DoAllTests_<USING_MAPPING_CONTAINER, TEST_FUNCTION, VALUE_EQUALS_COMPARER_TYPE> (applyToContainer);
        }
    }
}
#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Mapping_h_ */
