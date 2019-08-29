/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

        template <typename CONCRETE_CONTAINER>
        struct DefaultFactory {
            CONCRETE_CONTAINER operator() () const { return CONCRETE_CONTAINER (); };
        };

        template <typename CONCRETE_CONTAINER, typename FACTORY = DefaultFactory<CONCRETE_CONTAINER>>
        struct DEFAULT_TESTING_SCHEMA {
            constexpr inline DEFAULT_TESTING_SCHEMA (FACTORY factory = {})
                : Factory (factory)
            {
            }
            using ConcreteContainerType = CONCRETE_CONTAINER;
            FACTORY     Factory;
            static void ApplyToContainerExtraTest (const ConcreteContainerType&){};
        };

        template <typename TESTING_SCHEMA>
        void All_For_Type (const TESTING_SCHEMA& testingSchema);

        namespace Private_ {

            namespace Test1_VeryBasics_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ (const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    using DomainType                = typename USING_BIJECTION_CONTAINER::DomainType;
                    using RangeType                 = typename USING_BIJECTION_CONTAINER::RangeType;
                    USING_BIJECTION_CONTAINER s     = testingSchema.Factory ();
                    s.Add (3, 5);
                    VerifyTestResult (s.length () == 1);
                    VerifyTestResult (s.ContainsDomainElement (3));
                    VerifyTestResult (not s.ContainsDomainElement (5));
                    VerifyTestResult (s.ContainsRangeElement (5));
                    VerifyTestResult (not s.ContainsRangeElement (3));
                    VerifyTestResult (s.GetRangeEqualsComparer () (*s.Lookup (3), RangeType{5}));
                    VerifyTestResult (s.GetDomainEqualsComparer () (*s.InverseLookup (5), DomainType{3}));
                }
            }

            namespace Test2_MultipeItems_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ (const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    using DomainType                = typename USING_BIJECTION_CONTAINER::DomainType;
                    using RangeType                 = typename USING_BIJECTION_CONTAINER::RangeType;
                    USING_BIJECTION_CONTAINER s     = testingSchema.Factory ();
                    for (int i = 0; i < 100; ++i) {
                        s.Add (3 + i, 5 + i);
                    }
                    VerifyTestResult (s.length () == 100);
                    VerifyTestResult (s.ContainsDomainElement (3));
                    VerifyTestResult (s.ContainsRangeElement (5));
                    VerifyTestResult (s.GetRangeEqualsComparer () (*s.Lookup (3), RangeType{5}));
                    VerifyTestResult (s.GetDomainEqualsComparer () (*s.InverseLookup (5), DomainType{3}));
                }
            }

            namespace Test3_ConstructFromOtherTypes_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ ([[maybe_unused]] const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    using DomainType                = typename USING_BIJECTION_CONTAINER::DomainType;
                    using RangeType                 = typename USING_BIJECTION_CONTAINER::RangeType;
                    {
                        map<int, int> t;
                        t.insert (map<int, int>::value_type (2, 4));
                        typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s = USING_BIJECTION_CONTAINER (t);
                        VerifyTestResult (s.length () == 1);
                        VerifyTestResult (s.ContainsDomainElement (2));
                        VerifyTestResult (s.ContainsRangeElement (4));
                        VerifyTestResult (s.GetRangeEqualsComparer () (*s.Lookup (2), RangeType{4}));
                        VerifyTestResult (s.GetDomainEqualsComparer () (*s.InverseLookup (4), DomainType{2}));
                    }
                    {
                        pair<int, int>                                             t[] = {{2, 4}};
                        typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s   = USING_BIJECTION_CONTAINER (t);
                        VerifyTestResult (s.length () == 1);
                        VerifyTestResult (s.ContainsDomainElement (2));
                        VerifyTestResult (s.ContainsRangeElement (4));
                        VerifyTestResult (s.GetRangeEqualsComparer () (*s.Lookup (2), RangeType{4}));
                        VerifyTestResult (s.GetDomainEqualsComparer () (*s.InverseLookup (4), DomainType{2}));
                    }
                    {
                        Mapping<int, int> t;
                        t.Add (2, 4);
                        typename USING_BIJECTION_CONTAINER::ArchetypeContainerType s = USING_BIJECTION_CONTAINER (t);
                        VerifyTestResult (s.length () == 1);
                        VerifyTestResult (s.ContainsDomainElement (2));
                        VerifyTestResult (s.ContainsRangeElement (4));
                        VerifyTestResult (s.GetRangeEqualsComparer () (*s.Lookup (2), RangeType{4}));
                        VerifyTestResult (s.GetDomainEqualsComparer () (*s.InverseLookup (4), DomainType{2}));
                    }
                }
            }

            namespace Test4_As_Generic_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ (const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    using DomainType                = typename USING_BIJECTION_CONTAINER::DomainType;
                    using RangeType                 = typename USING_BIJECTION_CONTAINER::RangeType;
                    {
                        USING_BIJECTION_CONTAINER b = testingSchema.Factory ();
                        b.Add (3, 5);
                        b.Add (4, 19);
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

            namespace Test5_As_WithDefaultComparers_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ (const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    using DomainType                = typename USING_BIJECTION_CONTAINER::DomainType;
                    using RangeType                 = typename USING_BIJECTION_CONTAINER::RangeType;
                    {
                        USING_BIJECTION_CONTAINER b = testingSchema.Factory ();
                        b.Add (3, 5);
                        b.Add (4, 19);
                        {
                            map<DomainType, RangeType> m = b.template As<map<DomainType, RangeType>> ();
                            VerifyTestResult (m.size () == 2);
                            VerifyTestResult (m.find (3) != m.end ());
                            VerifyTestResult (m.find (9) == m.end ());
                        }
                        {
                            Mapping<DomainType, RangeType> m = b.template As<Mapping<DomainType, RangeType>> ();
                            VerifyTestResult (m.size () == 2);
                            VerifyTestResult (m.ContainsKey (3));
                            VerifyTestResult (not m.ContainsKey (9));
                        }
                    }
                }
            }

            namespace Test6_Inverse_ {

                template <typename TESTING_SCHEMA>
                void DoAllTests_ (const TESTING_SCHEMA& testingSchema)
                {
                    using USING_BIJECTION_CONTAINER = typename TESTING_SCHEMA::ConcreteContainerType;
                    {
                        USING_BIJECTION_CONTAINER b = testingSchema.Factory ();
                        b.Add (3, 5);
                        b.Add (4, 19);
                    }
                }
            }
        }

        template <typename TESTING_SCHEMA>
        void All_For_Type (const TESTING_SCHEMA& testingSchema)
        {
            Private_::Test1_VeryBasics_::DoAllTests_ (testingSchema);
            Private_::Test2_MultipeItems_::DoAllTests_ (testingSchema);
            Private_::Test4_As_Generic_::DoAllTests_ (testingSchema);
            Private_::Test6_Inverse_::DoAllTests_ (testingSchema);
        }

        template <typename TESTING_SCHEMA>
        void For_TypesWithDefaultFactory (const TESTING_SCHEMA& testingSchema)
        {
            Private_::Test3_ConstructFromOtherTypes_::DoAllTests_ (testingSchema);
            Private_::Test5_As_WithDefaultComparers_::DoAllTests_ (testingSchema);
        }
    }
}

#endif /* _Stroika_Foundation_Tests_TestCommon_CommonTests_Bijection_h_ */
