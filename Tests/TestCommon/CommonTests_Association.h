/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Association_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Association_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/Association.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace AssociationTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace Test1_BasicConstruction {
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using ELEMENT_TYPE = typename USING_ASSOCIATION_CONTAINER::value_type;
                USING_ASSOCIATION_CONTAINER s;
                applyToContainer (s);
                USING_ASSOCIATION_CONTAINER s1 = s;
                applyToContainer (s1);
#if 0
                Association<ELEMENT_TYPE>   s2 = s;
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
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_ASSOCIATION_CONTAINER m;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                Verify (m.Lookup (1, nullptr));
                Verify (not m.Lookup (2, nullptr));
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                IterableTests::SimpleIterableTest_All_For_Type<USING_ASSOCIATION_CONTAINER> (m);
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
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using KeyEqualsCompareFunctionType = typename USING_ASSOCIATION_CONTAINER::KeyEqualsCompareFunctionType;
                USING_ASSOCIATION_CONTAINER m;
                using KeyType = typename USING_ASSOCIATION_CONTAINER::KeyType;
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (equal_to<KeyType>{}(i.fKey, KeyType{1}));
                }
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (equal_to<KeyType>{}(i.fKey, KeyType{1}));
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
                        VerifyTestResult (equal_to<KeyType>{}(i.fKey, KeyType{1}));
                    }
                    if (cnt == 2) {
                        VerifyTestResult (equal_to<KeyType>{}(i.fKey, KeyType{2}));
                    }
                    if (cnt == 3) {
                        VerifyTestResult (equal_to<KeyType>{}(i.fKey, KeyType{3}));
                    }
                }
                VerifyTestResult (cnt == 3);
                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
            }
        }

        namespace Test4_Equals {
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION, typename VALUE_EQUALS_COMPARER_TYPE>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_ASSOCIATION_CONTAINER m;
                USING_ASSOCIATION_CONTAINER m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);
                VerifyTestResult (m.size () == 2);
                USING_ASSOCIATION_CONTAINER m3 = m;
                applyToContainer (m);
                applyToContainer (m2);
                applyToContainer (m3);
                //VerifyTestResult (m == m3);
                VerifyTestResult ((typename USING_ASSOCIATION_CONTAINER::template EqualsComparer<VALUE_EQUALS_COMPARER_TYPE>{}(m, m3)));
                //VerifyTestResult (not (m != m3));

                //VerifyTestResult (m != m2);
                VerifyTestResult ((not typename USING_ASSOCIATION_CONTAINER::template EqualsComparer<VALUE_EQUALS_COMPARER_TYPE>{}(m, m2)));
                //VerifyTestResult (not (m == m2));
            }
        }

        namespace Test5_ToFromSTLMap {
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_ASSOCIATION_CONTAINER m;
                USING_ASSOCIATION_CONTAINER m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);

                using KeyType   = typename USING_ASSOCIATION_CONTAINER::KeyType;
                using ValueType = typename USING_ASSOCIATION_CONTAINER::ValueType;

                {
                    multimap<KeyType, ValueType> n = m.template As<multimap<KeyType, ValueType>> ();
                    VerifyTestResult (n.size () == 2);
                    USING_ASSOCIATION_CONTAINER tmp = USING_ASSOCIATION_CONTAINER (n);
                    VerifyTestResult (*tmp.Lookup (1) == 88);
                    multimap<KeyType, ValueType> nn = tmp.template As<multimap<KeyType, ValueType>> ();
                    VerifyTestResult (nn == n);
                }
            }
        }

        namespace Test6_AsSTLVector {
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_ASSOCIATION_CONTAINER m;
                USING_ASSOCIATION_CONTAINER m2 = m;
                m.Add (1, 88);
                m.Add (2, 101);
                VerifyTestResult (m.size () == 2);

                using KeyType   = typename USING_ASSOCIATION_CONTAINER::KeyType;
                using ValueType = typename USING_ASSOCIATION_CONTAINER::ValueType;

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
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                USING_ASSOCIATION_CONTAINER c;
                for (int i = 0; i < 100; ++i) {
                    c.Add (i, i);
                }
                VerifyTestResult (c.Keys ().length () == 100);
                {
                    // be sure copying and iterating multiple times over the iterable doesnt produce differnt results.
                    auto keys = c.Keys ();
                    VerifyTestResult (keys.length () == 100);
                    VerifyTestResult (keys.length () == 100);
                    int a = 0;
                    for (auto i : keys) {
                        a++;
                    }
                    VerifyTestResult (a == 100);
                    a = 0;
                    for (auto i : keys) {
                        a++;
                    }
                    VerifyTestResult (a == 100);
                }
            }
        }

        namespace Test_8_Iteration_With_Value_Comparer {
            template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION, typename VALUE_EQUALS_COMPARER_TYPE>
            void DoAllTests_ ([[maybe_unused]] TEST_FUNCTION applyToContainer)
            {
#if 0

                using KeyEqualsCompareFunctionType   = typename USING_ASSOCIATION_CONTAINER::KeyEqualsCompareFunctionType;
                using ValueEqualsCompareFunctionType = VALUE_EQUALS_COMPARER_TYPE;
                USING_ASSOCIATION_CONTAINER m;
                using KeyType = typename USING_ASSOCIATION_CONTAINER::KeyType;
                using ValueType = typename USING_ASSOCIATION_CONTAINER::ValueType;

                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (KeyEqualsCompareFunctionType{} (i.fKey, KeyType{ 1 }));
                    VerifyTestResult (ValueEqualsCompareFunctionType{} (i.fValue, 2));
                }
                m.Add (1, 2);
                VerifyTestResult (m.size () == 1);
                for (auto i : m) {
                    VerifyTestResult (KeyEqualsCompareFunctionType{} (i.fKey, KeyType{ 1 }));
                    VerifyTestResult (ValueEqualsCompareFunctionType{} (i.fValue, 2));
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
                        VerifyTestResult (KeyEqualsCompareFunctionType{} (i.fKey, KeyType{ 1 }));
                        VerifyTestResult (ValueEqualsCompareFunctionType{} (i.fValue, 2));
                    }
                    if (cnt == 2) {
                        VerifyTestResult (KeyEqualsCompareFunctionType{} (i.fKey, KeyType{ 2 }));
                        VerifyTestResult (ValueEqualsCompareFunctionType{} (i.fValue, 3));
                    }
                    if (cnt == 3) {
                        VerifyTestResult (KeyEqualsCompareFunctionType{} (i.fKey, KeyType{ 3 }));
                        VerifyTestResult (ValueEqualsCompareFunctionType{} (i.fValue, 4));
                    }
                }
                VerifyTestResult (cnt == 3);
                m.RemoveAll ();
                VerifyTestResult (m.size () == 0);
#endif
            }
        }

        template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
        void SimpleAssociationTest_AllTestsWhichDontRequireComparer_For_Type_ (TEST_FUNCTION applyToContainer)
        {
            Test1_BasicConstruction::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
            Test2_AddRemove::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
            Test_3_Iteration::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
            Test6_AsSTLVector::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
            Test7_Keys::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
        }

        template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
        void SimpleAssociationTest_AllTestsRequireComparer_For_Type_ (TEST_FUNCTION applyToContainer)
        {
            Test5_ToFromSTLMap::DoAllTests_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
        }

        template <typename USING_ASSOCIATION_CONTAINER, typename TEST_FUNCTION>
        void SimpleAssociationTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            SimpleAssociationTest_AllTestsWhichDontRequireComparer_For_Type_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
            SimpleAssociationTest_AllTestsRequireComparer_For_Type_<USING_ASSOCIATION_CONTAINER> (applyToContainer);
        }

        template <typename USING_ASSOCIATION_CONTAINER, typename VALUE_EQUALS_COMPARER_TYPE, typename TEST_FUNCTION>
        void SimpleAssociationTest_WhichRequiresExplcitValueComparer (TEST_FUNCTION applyToContainer)
        {
            Test_8_Iteration_With_Value_Comparer::DoAllTests_<USING_ASSOCIATION_CONTAINER, TEST_FUNCTION, VALUE_EQUALS_COMPARER_TYPE> (applyToContainer);
            Test4_Equals::DoAllTests_<USING_ASSOCIATION_CONTAINER, TEST_FUNCTION, VALUE_EQUALS_COMPARER_TYPE> (applyToContainer);
        }
    }
}
#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Association_h_ */
