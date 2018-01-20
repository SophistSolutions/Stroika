/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_
#define _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"

#include "../TestHarness/TestHarness.h"
#include "CommonTests_Iterable.h"

namespace CommonTests {
    namespace BijectionTests {

        using namespace Stroika::Foundation;
        using namespace Stroika::Foundation::Containers;

        namespace Test1_VeryBasics_ {

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using DomainType                      = typename USING_BIJECTION_CONTAINER::DomainType;
                using RangeType                       = typename USING_BIJECTION_CONTAINER::RangeType;
                using DomainEqualsCompareFunctionType = typename USING_BIJECTION_CONTAINER::DomainEqualsCompareFunctionType;
                using RangeEqualsCompareFunctionType  = typename USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType;
                USING_BIJECTION_CONTAINER s;
                s.Add (3, 5);
                VerifyTestResult (s.length () == 1);
                VerifyTestResult (s.ContainsDomainElement (3));
                VerifyTestResult (not s.ContainsDomainElement (5));
                VerifyTestResult (s.ContainsRangeElement (5));
                VerifyTestResult (not s.ContainsRangeElement (3));
                VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (3), 5));
                VerifyTestResult (DomainEqualsCompareFunctionType::Equals (*s.InverseLookup (5), 3));
            }
        }

        namespace Test2_MultipeItems_ {

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using DomainType                      = typename USING_BIJECTION_CONTAINER::DomainType;
                using RangeType                       = typename USING_BIJECTION_CONTAINER::RangeType;
                using DomainEqualsCompareFunctionType = typename USING_BIJECTION_CONTAINER::DomainEqualsCompareFunctionType;
                using RangeEqualsCompareFunctionType  = typename USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType;
                USING_BIJECTION_CONTAINER s;
                for (int i = 0; i < 100; ++i) {
                    s.Add (3 + i, 5 + i);
                }
                VerifyTestResult (s.length () == 100);
                VerifyTestResult (s.ContainsDomainElement (3));
                VerifyTestResult (s.ContainsRangeElement (5));
                VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (3), 5));
                VerifyTestResult (DomainEqualsCompareFunctionType::Equals (*s.InverseLookup (5), 3));
            }
        }

        namespace Test3_ConstructFromOtherTypes_ {

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using DomainEqualsCompareFunctionType = typename USING_BIJECTION_CONTAINER::DomainEqualsCompareFunctionType;
                using RangeEqualsCompareFunctionType  = typename USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType;
                {
                    map<int, int> t;
                    t.insert (map<int, int>::value_type (2, 4));
                    typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s = USING_BIJECTION_CONTAINER (t);
                    VerifyTestResult (s.length () == 1);
                    VerifyTestResult (s.ContainsDomainElement (2));
                    VerifyTestResult (s.ContainsRangeElement (4));
                    VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (2), 4));
                    VerifyTestResult (DomainEqualsCompareFunctionType::Equals (*s.InverseLookup (4), 2));
                }
                {
                    pair<int, int> t[] = {
                        {2, 4}};
                    typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s = USING_BIJECTION_CONTAINER (t);
                    VerifyTestResult (s.length () == 1);
                    VerifyTestResult (s.ContainsDomainElement (2));
                    VerifyTestResult (s.ContainsRangeElement (4));
                    VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (2), 4));
                    VerifyTestResult (DomainEqualsCompareFunctionType::Equals (*s.InverseLookup (4), 2));
                }
                {
                    Mapping<int, int> t;
                    t.Add (2, 4);
                    typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s = USING_BIJECTION_CONTAINER (t);
                    VerifyTestResult (s.length () == 1);
                    VerifyTestResult (s.ContainsDomainElement (2));
                    VerifyTestResult (s.ContainsRangeElement (4));
                    VerifyTestResult (RangeEqualsCompareFunctionType::Equals (*s.Lookup (2), 4));
                    VerifyTestResult (DomainEqualsCompareFunctionType::Equals (*s.InverseLookup (4), 2));
                }
            }
        }

        namespace Test4_As_ {

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using DomainEqualsCompareFunctionType = typename USING_BIJECTION_CONTAINER::DomainEqualsCompareFunctionType;
                using RangeEqualsCompareFunctionType  = typename USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType;
                using DomainType                      = typename USING_BIJECTION_CONTAINER::DomainType;
                using RangeType                       = typename USING_BIJECTION_CONTAINER::RangeType;
                {
                    USING_BIJECTION_CONTAINER b;
                    b.Add (3, 5);
                    b.Add (4, 19);
#if 0
                    {
                        map<DomainType, RangeType>  m = b.As<map<DomainType, RangeType>> ();
                        VerifyTestResult (m.size () == 2);
                        VerifyTestResult (m.find (3) != m.end ());
                        VerifyTestResult (m.find (9) == m.end ());
                    }
#endif
                    {
                        using MAPPING_TRAITS                             = DefaultTraits::Mapping<DomainType, RangeType, DomainEqualsCompareFunctionType>;
                        Mapping<DomainType, RangeType, MAPPING_TRAITS> m = b.template As<Mapping<DomainType, RangeType, MAPPING_TRAITS>> ();
                        VerifyTestResult (m.size () == 2);
                        VerifyTestResult (m.ContainsKey (3));
                        VerifyTestResult (not m.ContainsKey (9));
                    }
                    {
                        vector<pair<DomainType, RangeType>> m = b.template As<vector<pair<DomainType, RangeType>>> ();
                        VerifyTestResult (m.size () == 2);
                    }
                    {
                        Sequence<pair<DomainType, RangeType>> m = b.template As<Sequence<pair<DomainType, RangeType>>> ();
                        VerifyTestResult (m.size () == 2);
                    }
                }
            }
        }

        namespace Test5_Inverse_ {

            template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
            void DoAllTests_ (TEST_FUNCTION applyToContainer)
            {
                using DomainEqualsCompareFunctionType = typename USING_BIJECTION_CONTAINER::DomainEqualsCompareFunctionType;
                using RangeEqualsCompareFunctionType  = typename USING_BIJECTION_CONTAINER::RangeEqualsCompareFunctionType;
                using DomainType                      = typename USING_BIJECTION_CONTAINER::DomainType;
                using RangeType                       = typename USING_BIJECTION_CONTAINER::RangeType;
                {
                    USING_BIJECTION_CONTAINER b;
                    b.Add (3, 5);
                    b.Add (4, 19);
#if 0
                    {
                        map<DomainType, RangeType>  m = b.As<map<DomainType, RangeType>> ();
                        VerifyTestResult (m.size () == 2);
                        VerifyTestResult (m.find (3) != m.end ());
                        VerifyTestResult (m.find (9) == m.end ());
                    }
#endif
#if 0
                    {
                        using   MAPPING_TRAITS      =   typename DefaultTraits::Mapping<RangeType, DomainType, RangeEqualsCompareFunctionType, DomainEqualsCompareFunctionType>;
                        Mapping<RangeType, DomainType, MAPPING_TRAITS>  m = b.Inverse<Mapping<RangeType, DomainType, MAPPING_TRAITS>> ();
                        VerifyTestResult (m.size () == 2);
                        //VerifyTestResult (m.ContainsKey (5));
                        //VerifyTestResult (not m.ContainsKey (9));
                    }
#endif
                }
            }
        }

        template <typename USING_BIJECTION_CONTAINER, typename TEST_FUNCTION>
        void SimpleTest_All_For_Type (TEST_FUNCTION applyToContainer)
        {
            Test1_VeryBasics_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
            Test2_MultipeItems_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
            Test3_ConstructFromOtherTypes_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
            Test4_As_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
            Test5_Inverse_::DoAllTests_<USING_BIJECTION_CONTAINER> (applyToContainer);
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_ */
