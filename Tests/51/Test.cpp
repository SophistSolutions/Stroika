/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/DisjointDiscreteRange.h"
#include "Stroika/Foundation/Traversal/DisjointRange.h"
#include "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include "Stroika/Foundation/Traversal/Generator.h"
#include "Stroika/Foundation/Traversal/Partition.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Traversal;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {

    void Test_1_BasicRange_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test_1_BasicRange_ ()"};
        {
            Range<int> r (3, 5);
            EXPECT_TRUE (not r.empty ());
            EXPECT_TRUE (r.Contains (3));
        }
        {
            Range<double> r (3, 5);
            EXPECT_TRUE (not r.empty ());
            EXPECT_TRUE (r.Contains (3));
        }
        {
            Range<double> r{3, 5, Openness::eOpen, Openness::eOpen};
            EXPECT_TRUE (not r.Contains (3));
        }
        {
            Range<int> rc (3, 7, Openness::eClosed, Openness::eClosed);
            Range<int> ro{3, 7, Openness::eOpen, Openness::eOpen};
            EXPECT_TRUE (rc.Contains (ro));
            EXPECT_TRUE (not ro.Contains (rc));
            Range<int> ri (4, 6);
            EXPECT_TRUE (rc.Contains (ri));
            EXPECT_TRUE (ro.Contains (ri));
            EXPECT_TRUE (not ri.Contains (rc));
            EXPECT_TRUE (not ri.Contains (ro));
            Range<int> e{};
            EXPECT_TRUE (rc.Contains (e)); // any set contains the empty set
        }
        {
            using namespace RangeTraits;
            using RT         = Explicit<int, DefaultOpenness<int>, ExplicitBounds<int, -3, 100>>;
            Range<int, RT> x = Range<int, RT>::FullRange ();
            EXPECT_TRUE (x.GetLowerBound () == -3);
            EXPECT_TRUE (x.GetUpperBound () == 100);
        }
        {
            // ALLOW THIS WHEN WE HAVE NEW CHECKED OPTINAL PARANM - EXPECT_TRUE ((Range<int>{1, 1, Openness::eOpen, Openness::eOpen} == Range<int>{3, 3, Openness::eOpen, Openness::eOpen}));
            EXPECT_TRUE ((Range<int>{1, 1, Openness::eClosed, Openness::eClosed} != Range<int>{3, 3, Openness::eClosed, Openness::eClosed}));
        }
        {
            auto r1  = Range<int>{3, 6};
            auto r2l = 2 * r1;
            auto r2r = r1 * 2;
            EXPECT_TRUE ((r2l == Range<int>{6, 12}));
            EXPECT_TRUE ((r2r == Range<int>{6, 12}));
        }
        {
            auto r1  = Range<int>{3, 6};
            auto r2l = 2 + r1;
            auto r2r = r1 + 2;
            EXPECT_TRUE ((r2l == Range<int>{5, 8}));
            EXPECT_TRUE ((r2r == Range<int>{5, 8}));
        }
        {
            using namespace RangeTraits;
            auto r1 = Range<int, Explicit<int, ExplicitOpenness<Openness::eOpen, Openness::eOpen>>>{3, 6};
            EXPECT_TRUE (r1.Pin (3) == 4);
            EXPECT_TRUE (r1.Pin (2) == 4);
            EXPECT_TRUE (r1.Pin (8) == 5);
        }
        {
            using namespace RangeTraits;
            auto r1 = Range<int, Explicit<int, ExplicitOpenness<Openness::eClosed, Openness::eClosed>>>{3, 6};
            EXPECT_TRUE (r1.Pin (3) == 3);
            EXPECT_TRUE (r1.Pin (2) == 3);
            EXPECT_TRUE (r1.Pin (4) == 4);
            EXPECT_TRUE (r1.Pin (8) == 6);
        }
        {
            using namespace RangeTraits;
            auto r1 = Range<int, Explicit<int, ExplicitOpenness<Openness::eClosed, Openness::eClosed>>>{3, 3};
            EXPECT_TRUE (r1.Pin (3) == 3);
            EXPECT_TRUE (r1.Pin (2) == 3);
            EXPECT_TRUE (r1.Pin (8) == 3);
        }
        {
            // test helper to assure answer for (A,B) is same as (B,A) - commutative
            auto verifyIntersectsIsCommutative = [] (const Range<int>& p1, const Range<int>& p2) {
                bool r = p1.Intersects (p2);
                EXPECT_TRUE (r == p2.Intersects (p1));
                return r;
            };
            constexpr auto eOpen   = Openness::eOpen;
            constexpr auto eClosed = Openness::eClosed;
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{1, 3}, Range<int>{2, 2, eClosed, eClosed}));
            EXPECT_TRUE (not verifyIntersectsIsCommutative (Range<int>{1, 3, eOpen, eOpen}, Range<int>{3, 4, eClosed, eClosed}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{1, 3, eClosed, eClosed}, Range<int>{0, 1, eClosed, eClosed}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{1, 3, eClosed, eClosed}, Range<int>{1, 1, eClosed, eClosed}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{1, 10}, Range<int>{3, 4}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{1, 10}, Range<int>{3, 3, eClosed, eClosed}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{5, 10}, Range<int>{3, 7}));
            EXPECT_TRUE (verifyIntersectsIsCommutative (Range<int>{5, 10, eClosed, eClosed}, Range<int>{5, 5, eClosed, eClosed}));
        }
    }

    void Test_2_BasicDiscreteRangeIteration_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test_2_BasicDiscreteRangeIteration_"};
        {
            DiscreteRange<int> r (3, 5);
            EXPECT_TRUE (not r.empty ());
            EXPECT_TRUE (r.Contains (3));
        }
        {
            DiscreteRange<int> r;
            EXPECT_TRUE (r.empty ());
        }
        {
            DiscreteRange<int> r (3, 3);
            EXPECT_TRUE (not r.empty ());
            EXPECT_TRUE (r.Elements ().size () == 1);
        }
        {
            int nItemsHit   = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5).Elements ()) {
                nItemsHit++;
                EXPECT_TRUE (lastItemHit < i);
                lastItemHit = i;
            }
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == 5); /// IN DISCUSSION - OPEN ENDED RHS?
        }
        {
            int nItemsHit   = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5)) {
                nItemsHit++;
                EXPECT_TRUE (lastItemHit < i);
                lastItemHit = i;
            }
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == 5); /// IN DISCUSSION - OPEN ENDED RHS?
        }
    }
}

namespace {
    void Test_3_SimpleDiscreteRangeWithEnumsTest_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test_3_SimpleDiscreteRangeWithEnumsTest_"};
        enum class Color {
            red,
            blue,
            green,

            Stroika_Define_Enum_Bounds (red, green)
        };

        {
            using namespace RangeTraits;
            Color min1 = Default<Color>::kLowerBound;
            Color max1 = Default<Color>::kUpperBound;
            Color min2 = Default_Enum<Color>::kLowerBound;
            Color max2 = Default_Enum<Color>::kUpperBound;
            Color min3 = Explicit<Color, DefaultOpenness<Color>, ExplicitBounds<Color, Color::eSTART, Color::eLAST>>::kLowerBound;
            Color max3 = Explicit<Color, DefaultOpenness<Color>, ExplicitBounds<Color, Color::eSTART, Color::eLAST>>::kUpperBound;
            EXPECT_TRUE (Color::red == Color::eSTART and Color::green == Color::eLAST);
            EXPECT_TRUE (min1 == Color::eSTART and max1 == Color::eLAST);
            EXPECT_TRUE (min2 == Color::eSTART and max2 == Color::eLAST);
            EXPECT_TRUE (min3 == Color::eSTART and max3 == Color::eLAST);
        }
        {
            using namespace RangeTraits;
            EXPECT_TRUE (Default_Enum<Color>::GetNext (Color::eSTART) == Color::blue);
            EXPECT_TRUE (Default_Enum<Color>::GetPrevious (Color::blue) == Color::red);
        }
        {
            int             nItemsHit = 0;
            optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color>::FullRange ().Elements ()) {
                nItemsHit++;
                EXPECT_TRUE (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == Color::green);
        }
        {
            using namespace RangeTraits;
            int             nItemsHit = 0;
            optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color, Default<Color>>::FullRange ().Elements ()) {
                nItemsHit++;
                EXPECT_TRUE (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == Color::green);
        }
        {
            int             nItemsHit = 0;
            optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color> (optional<Color>{}, optional<Color>{}).Elements ()) {
                nItemsHit++;
                EXPECT_TRUE (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == Color::green);
        }
        {
            int             nItemsHit = 0;
            optional<Color> lastItemHit;
            DiscreteRange<Color> (optional<Color> (), optional<Color>{}).Elements ().Apply ([&nItemsHit, &lastItemHit] (Color i) {
                nItemsHit++;
                EXPECT_TRUE (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            });
            EXPECT_TRUE (nItemsHit == 3);
            EXPECT_TRUE (lastItemHit == Color::green);
        }
    }
}

namespace {
    void Test4_FunctionalApplicationContext_MapTest_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test4_FunctionalApplicationContext_MapTest_"};
        {
            Containers::Sequence<int> n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            Containers::Sequence<int> n1 =
                Containers::Sequence<int> (FunctionalApplicationContext<int> (n).Map<int> ([] (int i) -> int { return i + 1; }));
            EXPECT_TRUE (n1.size () == 3);
            EXPECT_TRUE (n1[0] == 2);
            EXPECT_TRUE (n1[1] == 3);
            EXPECT_TRUE (n1[2] == 4);
        }
    }
}

namespace {
    void Test5_ReduceTest_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test5_ReduceTest_"};
        {
            Containers::Sequence<int> n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            int sum = FunctionalApplicationContext<int> (n).Reduce<int> ([] (int l, int r) -> int { return l + r; }, 0);
            EXPECT_TRUE (sum == 6);
        }
    }
}

namespace {
    void Test6_FunctionApplicationContext_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test6_FunctionApplicationContext_"};
        using Containers::Sequence;

        {
            Containers::Sequence<int> s = {1, 2, 3};
            {
                shared_ptr<int>         countSoFar = shared_ptr<int> (new int (0));
                [[maybe_unused]] size_t answer     = FunctionalApplicationContext<int> (s)
                                                     .Filter<int> ([countSoFar] (int) -> bool {
                                                         ++(*countSoFar);
                                                         return (*countSoFar) & 1;
                                                     })
                                                     .Map<int> ([] (int s) { return s + 5; })
                                                     .Reduce<size_t> ([] ([[maybe_unused]] int s, size_t memo) { return memo + 1; });
                EXPECT_TRUE (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer = FunctionalApplicationContext<int> (s)
                                    .Filter<int> ([&countSoFar] (int) -> bool {
                                        ++countSoFar;
                                        return countSoFar & 1;
                                    })
                                    .Map<int> ([] (int s) { return s + 5; })
                                    .Reduce<size_t> ([] ([[maybe_unused]] int s, size_t memo) { return memo + 1; });
                EXPECT_TRUE (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<int> r = Containers::Sequence<int> (FunctionalApplicationContext<int> (s)
                                                                             .Filter<int> ([&countSoFar] (int) -> bool {
                                                                                 ++countSoFar;
                                                                                 return countSoFar & 1;
                                                                             })
                                                                             .Map<int> ([] (int s) { return s + 5; }));
                EXPECT_TRUE (r.length () == 2);
                EXPECT_TRUE (r[0] == 6 and r[1] == 8);
            }
            {
                optional<int> answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([] (int i) -> bool { return (i & 1); }).Find<int> ([] (int i) -> bool {
                        return i == 1;
                    });
                EXPECT_TRUE (*answer == 1);
            }
            {
                optional<int> answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([] (int i) -> bool { return (i & 1); }).Find<int> ([] (int i) -> bool {
                        return i == 8;
                    });
                EXPECT_TRUE (not answer.has_value ());
            }
        }

        {
            using Characters::String;
            Sequence<String> s = {"alpha", "beta", "gamma"};
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer = FunctionalApplicationContext<String> (s)
                                    .Filter<String> ([&countSoFar] (String) -> bool {
                                        ++countSoFar;
                                        return countSoFar & 1;
                                    })
                                    .Map<String> ([] (String s) { return s + " hello"sv; })
                                    .Reduce<size_t> ([] ([[maybe_unused]] String s, size_t memo) { return memo + 1; });
                EXPECT_TRUE (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<String> r = Containers::Sequence<String> (FunctionalApplicationContext<String> (s)
                                                                                   .Filter<String> ([&countSoFar] (String) -> bool {
                                                                                       ++countSoFar;
                                                                                       return countSoFar & 1;
                                                                                   })
                                                                                   .Map<String> ([] (String s) { return s + " hello"sv; }));
                EXPECT_TRUE (r.length () == 2);
                EXPECT_TRUE (r[0] == "alpha hello"sv and r[1] == "gamma hello"sv);
            }
        }
    }
}

namespace {
    void Test7_FunctionApplicationContextWithDiscreteRangeEtc_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test7_FunctionApplicationContextWithDiscreteRangeEtc_"};
        using Containers::Sequence;
        {
            const uint32_t kRefCheck_[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
            auto           isPrimeCheck = [] (uint32_t n) -> bool { return Math::IsPrime (n); };
            for (auto i : FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t>{1, 100}.Elements ()).Filter<uint32_t> (isPrimeCheck)) {
                EXPECT_TRUE (Math::IsPrime (i));
            }
            Sequence<uint32_t> s = Sequence<uint32_t> (
                FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t>{1, 100}.Elements ()).Filter<uint32_t> (isPrimeCheck));
            EXPECT_TRUE (s == Sequence<uint32_t> (begin (kRefCheck_), end (kRefCheck_)));
            EXPECT_TRUE (
                Memory::NEltsOf (kRefCheck_) ==
                FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t>{1, 100}.Elements ()).Filter<uint32_t> (isPrimeCheck).size ());
        }
    }
}

namespace {
    void Test8_DiscreteRangeTestFromDocs_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test8_DiscreteRangeTestFromDocs_"};
        // From Docs in DiscreteRange<> class
        vector<int> v = DiscreteRange<int>{1, 10}.Elements ().As<vector<int>> ();
        EXPECT_TRUE ((v == vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
        for (auto i : DiscreteRange<int>{1, 10}.Elements ()) {
            EXPECT_TRUE (1 <= i and i <= 10); // rough verification
        }
    }
}

namespace {
    void Test9_Generators_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test9_Generators_"};
        {
            constexpr int kMin      = 1;
            constexpr int kMax      = 10;
            auto          myContext = shared_ptr<int> (new int (kMin - 1));
            auto          getNext   = [myContext] () -> optional<int> {
                (*myContext)++;
                if (*myContext > 10) {
                    return optional<int>{};
                }
                return *myContext;
            };

            int sum = 0;
            for (auto i : CreateGenerator<int> (getNext)) {
                EXPECT_TRUE (1 <= i and i <= 10);
                sum += i;
            }
            EXPECT_TRUE (sum == (kMax - kMin + 1) * (kMax + kMin) / 2);
        }
    }
}

namespace {
    void Test10_MakeIterableFromIterator_ ()
    {
        static_assert (ranges::range<Iterable<int>>);
        Debug::TraceContextBumper ctx{"{}::Test10_MakeIterableFromIterator_"};
        {
            Containers::Sequence<int> a    = {1, 3, 5, 7, 9};
            Iterator<int>             iter = a.MakeIterator ();

            int sum = 0;
            for (auto i : MakeIterableFromIterator (iter)) {
                sum += i;
            }
            EXPECT_TRUE (sum == 25);
        }
    }
}

namespace {
    void Test11_GetDistanceSpanned_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test11_GetDistanceSpanned_"};
        using IntRange = Range<unsigned int>;
        IntRange foo{3, 9};
        EXPECT_TRUE (foo.GetDistanceSpanned () == 6);
    }
}

namespace {
    void Test12_RangeConstExpr_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test12_RangeConstExpr_"};
        using IntRange = Range<unsigned int>;
        constexpr IntRange     kFoo_{3, 9};
        constexpr unsigned int l = kFoo_.GetLowerBound ();
        constexpr unsigned int u = kFoo_.GetUpperBound ();
        EXPECT_TRUE (l == 3);
        EXPECT_TRUE (u == 9);
        constexpr unsigned int m = kFoo_.GetMidpoint ();
        EXPECT_TRUE (m == 6);
    }
}

namespace {
    void Test13_DisjointRange_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test13_DisjointRange_"};
        {
            DisjointRange<float> dr{};
            EXPECT_TRUE (dr.empty ());
            EXPECT_TRUE (dr.GetBounds ().empty ());
            EXPECT_TRUE (dr.SubRanges ().empty ());
            EXPECT_TRUE (not dr.Contains (3));
        }
        {
            DisjointRange<float> dr{Range<float> (2.1f, 5.0f)};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE ((dr.GetBounds () == Range<float>{2.1f, 5.0f}));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
            EXPECT_TRUE (not dr.Contains (2));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointRange<int, RT>;
            DRT dr{};
            EXPECT_TRUE (dr.empty ());
            EXPECT_TRUE (dr.GetBounds ().empty ());
            EXPECT_TRUE (dr.SubRanges ().empty ());
            EXPECT_TRUE (not dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointRange<int, RT>;
            DRT dr{RT (2, 5)};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (2, 5));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
            EXPECT_TRUE (not dr.Contains (1));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 2}, RT{4, 5}};
            EXPECT_TRUE ((dr.GetBounds () == RT{1, 5}));
            EXPECT_TRUE (dr.SubRanges ().size () == 2);
            EXPECT_TRUE (dr.Contains (2));
            EXPECT_TRUE (not dr.Contains (3));
            EXPECT_TRUE (dr.Contains (4));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{4, 5}, RT{1, 2}};
            EXPECT_TRUE ((dr.GetBounds () == RT{1, 5}));
            EXPECT_TRUE (dr.SubRanges ().size () == 2);
            EXPECT_TRUE (dr.Contains (2));
            EXPECT_TRUE (not dr.Contains (3));
            EXPECT_TRUE (dr.Contains (4));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{};
            EXPECT_TRUE (dr.empty ());
            dr.Add (4);
            EXPECT_TRUE (dr.GetBounds () == RT (4, 4));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (not dr.Contains (3));
            EXPECT_TRUE (dr.Contains (4));
            for (int i = 5; i <= 100; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            EXPECT_TRUE (dr.GetBounds () == RT (4, 100));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            for (int i = 501; i < 600; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            dr.Add (3);
            EXPECT_TRUE (dr.SubRanges ().size () == 2);
            EXPECT_TRUE (dr.GetBounds () == RT (3, 600 - 1));
            EXPECT_TRUE (dr.Contains (3));
            EXPECT_TRUE (not dr.Contains (300));
            EXPECT_TRUE (dr.Contains (599));
            EXPECT_TRUE (not dr.Contains (600));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 1}, RT{3, 3}, RT{5, 5}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (1, 5));
            EXPECT_TRUE (dr.SubRanges ().size () == 3);
            EXPECT_TRUE (not dr.Contains (4));
            EXPECT_TRUE (dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{3, 7}, RT{5, 9}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (1, 9));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
        }
        {
            using RT  = Range<float>;
            using DRT = DisjointRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{3, 7}, RT{5, 9}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE ((dr.GetBounds () == RT{1, 9}));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
        }
        {
            using namespace RangeTraits;
            using RT  = Range<float, Explicit<float, ExplicitOpenness<Openness::eClosed, Openness::eClosed>>>;
            using DRT = DisjointRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{2, 2}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE ((dr.GetBounds () == RT{1, 5}));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 4}, RT{5, 9}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (1, 9));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Contains (3));
            dr.Add (10);
            EXPECT_TRUE (dr.GetBounds () == RT (1, 10));
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 1}, RT{3, 3}, RT{5, 5}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (1, 5));
            EXPECT_TRUE (dr.SubRanges ().size () == 3);
            EXPECT_TRUE (dr.Contains (3));
            EXPECT_TRUE (dr.GetNext (1) == 3);
            EXPECT_TRUE (dr.GetNext (2) == 3);
            EXPECT_TRUE (dr.GetNext (3) == 5);
            EXPECT_TRUE (dr.GetNext (4) == 5);
            EXPECT_TRUE (not dr.GetNext (5).has_value ());
            EXPECT_TRUE (not dr.GetPrevious (1).has_value ());
            EXPECT_TRUE (dr.GetPrevious (2) == 1);
            EXPECT_TRUE (dr.GetPrevious (3) == 1);
            EXPECT_TRUE (dr.GetPrevious (4) == 3);
            EXPECT_TRUE (dr.GetPrevious (5) == 3);
            EXPECT_TRUE (dr.GetPrevious (6) == 5);
            EXPECT_TRUE (dr.GetPrevious (7) == 5);
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 2}, RT{4, 5}, RT{7, 8}};
            EXPECT_TRUE (not dr.empty ());
            EXPECT_TRUE (dr.GetBounds () == RT (1, 8));
            EXPECT_TRUE (dr.SubRanges ().size () == 3);
            EXPECT_TRUE (not dr.Contains (3));
            EXPECT_TRUE (dr.GetNext (1) == 2);
            EXPECT_TRUE (dr.GetNext (2) == 4);
            EXPECT_TRUE (dr.GetNext (3) == 4);
            EXPECT_TRUE (dr.GetNext (4) == 5);
            EXPECT_TRUE (dr.GetNext (5) == 7);
            EXPECT_TRUE (dr.GetNext (6) == 7);
            EXPECT_TRUE (dr.GetNext (7) == 8);
            EXPECT_TRUE (not dr.GetNext (8).has_value ());
            EXPECT_TRUE (not dr.GetNext (99).has_value ());
            EXPECT_TRUE (not dr.GetPrevious (1).has_value ());
            EXPECT_TRUE (dr.GetPrevious (2) == 1);
            EXPECT_TRUE (dr.GetPrevious (3) == 2);
            EXPECT_TRUE (dr.GetPrevious (4) == 2);
            EXPECT_TRUE (dr.GetPrevious (5) == 4);
            EXPECT_TRUE (dr.GetPrevious (6) == 5);
            EXPECT_TRUE (dr.GetPrevious (7) == 5);
            EXPECT_TRUE (dr.GetPrevious (8) == 7);
            EXPECT_TRUE (dr.GetPrevious (9) == 8);
            {
                // test iterate over ranges
                int timeThru = 0;
                for (const RT& rng : dr.SubRanges ()) {
                    switch (timeThru++) {
                        case 0:
                            EXPECT_TRUE ((rng == RT{1, 2}));
                            break;
                        case 1:
                            EXPECT_TRUE ((rng == RT{4, 5}));
                            break;
                        case 2:
                            EXPECT_TRUE ((rng == RT{7, 8}));
                            break;
                        default:
                            EXPECT_TRUE (false);
                    }
                }
            }
            {
                // test iterate over elements
                EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({1, 2, 4, 5, 7, 8}));
            }
            {
                // Test intersection
                EXPECT_TRUE (Containers::Sequence<int> (dr.Intersection (RT{2, 4}).Elements ()) == Containers::Sequence<int> ({2, 4}));
            }
        }
        {
            using DRT = DisjointDiscreteRange<int>;
            DRT                       dr;
            static const unsigned int kMax_ = Debug::IsRunningUnderValgrind () ? 1000u : 10000u;
            for (int i = 0; i < (int)kMax_; ++i) {
                dr.Add (i);
            }
            EXPECT_TRUE (dr.SubRanges ().size () == 1);
            EXPECT_TRUE (dr.Elements ().size () == kMax_);
        }
        {
            using DRT = DisjointDiscreteRange<int>;
            DRT dr;
            dr.Add (872);
            EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({872}));
            dr.Add (231);
            EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 872}));
            dr.Add (329);
            EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 872}));
            dr.Add (665);
            EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 665, 872}));
            dr.Add (581);
            EXPECT_TRUE (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 581, 665, 872}));
        }
        {
            using Containers::Set;
            using Containers::SortedSet;
            using DRT      = DisjointDiscreteRange<int>;
            auto roundTrip = [] (const Set<int>& s) {
                DRT tmp{s};
                EXPECT_TRUE (tmp.Elements ().size () == s.size ());
                EXPECT_TRUE (Set<int>{tmp.Elements ()} == s);
            };
            roundTrip (Set<int>{3, 4});
            roundTrip (Set<int>{1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 100});
            roundTrip (Set<int>{4, 5, 6, 7, 8, 9, 10, 11, 100, 102, 103, 104});
            roundTrip (Set<int>{(DiscreteRange<int>{1, 1000}).Elements ()});
        }
        {
            // Check semantics of Range<>::Intersects
            using RT               = Range<int>;
            constexpr auto eOpen   = Openness::eOpen;
            constexpr auto eClosed = Openness::eClosed;
            EXPECT_TRUE ((RT{1, 2}.Intersects (RT{1, 2})));
            EXPECT_TRUE ((not RT{1, 2, eOpen, eOpen}.Intersects (RT{2, 3, eOpen, eOpen})));
            EXPECT_TRUE ((not RT{1, 2, eOpen, eClosed}.Intersects (RT{2, 3, eOpen, eOpen})));
            EXPECT_TRUE ((RT{1, 2, eOpen, eClosed}.Intersects (RT{2, 3, eClosed, eOpen})));
            EXPECT_TRUE ((RT{1, 2, eOpen, eClosed} ^ RT{2, 3, eClosed, eOpen}));
            EXPECT_TRUE (((RT{1, 2, eOpen, eClosed} ^ RT{2, 3, eClosed, eOpen}) == RT{2, 2, eClosed, eClosed}));
        }
    }
}

namespace {
    void Test14_ToString_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test14_ToString_"};
        EXPECT_TRUE ((Range<int>{3, 4}.ToString ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4]"));
        EXPECT_TRUE ((Range<int>{3, 4}.ToString () == L"[3 ... 4]"));
        {
            using namespace Time;
            EXPECT_TRUE (
                (Range<DateTime>{DateTime{Date (Year{1903}, April, DayOfMonth{4})}, DateTime{Date (Year{1903}, April, DayOfMonth{5})}}.ToString () ==
                 L"[4/4/03 ... 4/5/03]"));
        }
        {
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            using namespace Time;
            EXPECT_TRUE (
                (Range<DateTime>{DateTime{Date (Year{1903}, April, DayOfMonth{4})}, DateTime{Date (Year{1903}, April, DayOfMonth{5})}}.ToString () ==
                 "[4/4/1903 ... 4/5/1903]"));
        }
    }
}

namespace {
    void Test15_Partition_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test15_Partition_"};
        {
            using Containers::Sequence;
            using RangeTraits::Explicit;
            using RangeTraits::ExplicitOpenness;
            using RT = Range<double>;
            EXPECT_TRUE (not IsPartition (Sequence<RT>{RT{1, 2}, RT{3, 4}}));
            EXPECT_TRUE (IsPartition (Sequence<RT>{RT{1, 2}, RT{2, 4}}));
        }
        {
            using Containers::Sequence;
            using RangeTraits::Explicit;
            using RangeTraits::ExplicitOpenness;
            using RT = Range<int, Explicit<int, ExplicitOpenness<Openness::eClosed, Openness::eOpen>>>; // half open intervals best for partitions
            EXPECT_TRUE (not IsPartition (Sequence<RT>{RT{1, 2}, RT{3, 4}}));
            EXPECT_TRUE (IsPartition (Sequence<RT>{RT{1, 2}, RT{2, 4}}));
        }
    }
}

namespace {
    void Test16_LinqLikeFunctions_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test16_LinqLikeFunctions_"};
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Where ([] (int i) { return i % 2 == 0; }).SequentialEquals (Iterable<int>{2, 4, 6}));
            EXPECT_TRUE (c.Where ([] (int i) { return i % 2 == 1; }).SequentialEquals (Iterable<int>{1, 3, 5}));
            {
                Iterable<int> w = c.Where ([] (int i) { return i % 2 == 0; });
                EXPECT_TRUE (w.SequentialEquals ({2, 4, 6}));
                EXPECT_TRUE (w.SequentialEquals ({2, 4, 6}));
            }
        }
        {
            Iterable<int> c{1, 2, 2, 5, 9, 4, 5, 6};
            EXPECT_TRUE (c.Distinct ().SetEquals ({1, 2, 4, 5, 6, 9}));
            auto resultsEqualMod5 = c.Distinct ([] (int l, int r) { return l % 5 == r % 5; });
            DbgTrace (L"x=%s", Characters::ToString (resultsEqualMod5).c_str ());
            EXPECT_TRUE (resultsEqualMod5.size () == 4);
            for (auto i : resultsEqualMod5) {
                EXPECT_TRUE (i != 3);
            }
        }
        {
            Iterable<pair<int, char>> c{{1, 'a'}, {2, 'b'}, {3, 'c'}};
            EXPECT_TRUE (c.Map<Iterable<int>> ([] (pair<int, char> p) { return p.first; }).SequentialEquals (Iterable<int>{1, 2, 3}));
        }
        {
            Iterable<pair<int, char>> c{{1, 'a'}, {2, 'b'}, {3, 'c'}};
            EXPECT_TRUE (c.Map<Iterable<int>> ([] (pair<int, char> p) {
                              return (p.first & 1) ? optional<int>{p.first} : nullopt;
                          }).SequentialEquals (Iterable<int>{1, 3}));
        }
        {
            using Characters::String;
            Iterable<int> c{3, 4, 7};
            EXPECT_TRUE (c.Map<Iterable<String>> ([] (int i) { return Characters::Format (L"%d", i); }).SequentialEquals (Iterable<String>{"3", "4", "7"}));
        }
        {
            using Characters::String;
            Iterable<int> c{3, 4, 7};
            EXPECT_TRUE ((c.Map<vector<String>> ([] (int i) { return Characters::Format (L"%d", i); }) == vector<String>{"3", "4", "7"}));
        }
        {
            Iterable<int> c = {1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Any ([] (int i) { return i % 2 == 0; }));
            EXPECT_TRUE (not c.Any ([] (int i) { return i > 7; }));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Skip (3).SequentialEquals ({4, 5, 6}));
        }
        {
            using Containers::Sequence;
            using Containers::Set;
            Sequence<int> a{1, 3, 5, 7, 9};
            a = Sequence<int>{a.Skip (2)}; // https://stroika.atlassian.net/browse/STK-532 - crash
            EXPECT_TRUE ((a == Sequence<int>{5, 7, 9}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Take (3).SequentialEquals (Iterable<int>{1, 2, 3}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Slice (3, 5).SequentialEquals ({4, 5}));
            EXPECT_TRUE (c.Slice (3, 9999).SequentialEquals ({4, 5, 6}));
        }
        {
            Iterable<int> c{3, 5, 9, 38, 3, 5};
            EXPECT_TRUE (c.OrderBy ().SequentialEquals ({3, 3, 5, 5, 9, 38}));
            EXPECT_TRUE (c.OrderBy ([] (int lhs, int rhs) -> bool { return lhs < rhs; }).SequentialEquals ({3, 3, 5, 5, 9, 38}));
            EXPECT_TRUE (c.OrderBy ([] (int lhs, int rhs) -> bool { return lhs > rhs; }).SequentialEquals ({38, 9, 5, 5, 3, 3}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Min () == 1);
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.Max () == 6);
        }
        {
            using Math::NearlyEquals;
            Iterable<int> c{1, 2, 9, 4, 5, 3};
            EXPECT_TRUE (c.Median () == 3);
            EXPECT_TRUE (NearlyEquals (c.Median<double> (), 3.5));
        }
        {
            Iterable<int> c{1, 2, 9, 4, 5, 3};
            EXPECT_TRUE (c.MeanValue () == 4);
        }
        {
            // From Iterable::First/Iterable::Last docs
            Iterable<int> c{3, 5, 9, 38, 3, 5};
            EXPECT_TRUE (*c.First () == 3);
            EXPECT_TRUE (*c.First<int> ([] (int i) { return (i % 2 == 0) ? i : optional<int>{}; }) == 38);
            EXPECT_TRUE (*c.Last () == 5);
            EXPECT_TRUE (*c.Last<int> ([] (int i) { return i % 2 == 0 ? i : optional<int>{}; }) == 38);
        }
        {
            // From Iterable::All docs
            Iterable<int> c{3, 5, 9, 3, 5};
            EXPECT_TRUE (c.All ([] (int i) { return i % 2 == 1; }));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.NthValue (1) == 2);
            EXPECT_TRUE ((c.NthValue (99) == int{}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            EXPECT_TRUE (c.First () == 1);
            EXPECT_TRUE (c.First ([] (int i) { return i == 4; }) == 4);
        }
    }
}

namespace {
    void Test17_DurationRange_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test17_DurationRange_"};
        using Time::Duration;
        using Traversal::Range;

        {
            Range<Duration> a{Duration{"PT.5S"sv}, Duration{"PT2M"}};
            Range<Duration> b{Duration{"PT1S"}, Duration{"PT2M"}};
            Verify ((a ^ b) == b);
        }
        {
            Range<Duration> a{Duration{"PT.5S"}, Duration{"PT2M"}};
            EXPECT_TRUE (a.Pin (Duration{"PT.5S"}) == Duration{"PT.5S"});
            EXPECT_TRUE (a.Pin (Duration{"PT0S"}) == Duration{"PT.5S"});
            EXPECT_TRUE (a.Pin (Duration{"PT5M"}) == Duration{"PT2M"});
            EXPECT_TRUE (a.Pin (Duration{"PT10S"}) == Duration{"PT10S"});
        }
    }
}

namespace {
    void Test18_IterableConstructors_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test18_IterableConstructors_"};

        {
            vector<int>   a = {1, 3, 5};
            Iterable<int> aa1{vector<int>{1, 3, 5}};
            Iterable<int> aa2{a};
            Iterable<int> aa3{move (a)};
            Iterable<int> aa4{aa3};
            Iterable<int> aa5{Iterable<int>{}};
            Iterable<int> aa6{3, 4, 6};
        }
    }
}

namespace {
    void Test19_CreateGeneratorBug_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test19_CreateGeneratorBug_"};
        auto                      t1 = [] () {
            Containers::Bijection<int, int> seeIfReady;
            seeIfReady.Add (1, 1);
            seeIfReady.Add (2, 2);
            EXPECT_TRUE (seeIfReady.size () == 2);
            DbgTrace (L"seeIfReady=%s", Characters::ToString (seeIfReady).c_str ());
            EXPECT_TRUE (seeIfReady.size () == 2);

            const bool               kFails_ = true;
            Traversal::Iterable<int> fds     = kFails_ ? seeIfReady.Image () : Traversal::Iterable<int>{1, 2};

            EXPECT_TRUE (fds.size () == 2);
            EXPECT_TRUE (seeIfReady.size () == 2);
            DbgTrace (L"fds=%s", Characters::ToString (fds).c_str ()); // note this is critical step in reproducing old bug - iterating over fds
            EXPECT_TRUE (fds.size () == 2);
            EXPECT_TRUE (seeIfReady.size () == 2);
        };
        t1 ();
        auto t11 = [] () {
            Containers::Bijection<int, int> seeIfReady;
            seeIfReady.Add (1, 1);
            seeIfReady.Add (2, 2);
            seeIfReady.Add (3, 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            DbgTrace (L"seeIfReady=%s", Characters::ToString (seeIfReady).c_str ());
            EXPECT_TRUE (seeIfReady.size () == 3);

            Traversal::Iterable<int> fds = seeIfReady.Image ();

            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            DbgTrace (L"fds=%s", Characters::ToString (fds).c_str ()); // note this is critical step in reproducing old bug - iterating over fds
            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            DbgTrace (L"fds=%s", Characters::ToString (fds).c_str ()); // note this is critical step in reproducing old bug - iterating over fds
            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
        };
        t11 ();
        auto t2 = [] () {
            Containers::Bijection<int, int> seeIfReady;
            seeIfReady.Add (1, 1);
            seeIfReady.Add (2, 2);
            seeIfReady.Add (3, 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            DbgTrace (L"seeIfReady=%s", Characters::ToString (seeIfReady).c_str ());
            EXPECT_TRUE (seeIfReady.size () == 3);

            const bool               kFails_ = true;
            Traversal::Iterable<int> fds     = kFails_ ? seeIfReady.Image () : Traversal::Iterable<int>{1, 2, 3};

            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);
            DbgTrace (L"fds=%s", Characters::ToString (fds).c_str ()); // not this is critical step in reproducing old bug - iterating over fds
            EXPECT_TRUE (fds.size () == 3);
            EXPECT_TRUE (seeIfReady.size () == 3);

            const bool               kFails2_ = true;
            Traversal::Iterable<int> o1       = kFails2_ ? fds.Map ([&] (const int& t) { return t; }) : fds;

            EXPECT_TRUE (seeIfReady.size () == 3);
            EXPECT_TRUE (o1.size () == 3);

            // this iteration seems to break the list o1 if kFails_
            [[maybe_unused]] size_t a = o1.size ();
            //DbgTrace (L"o1=%s", Characters::ToString (o1).c_str ());

            EXPECT_TRUE (o1.size () == 3);

            auto c1 = Containers::Collection<int>{o1};
            (void)Characters::ToString (o1);
            EXPECT_TRUE (o1.size () == 3);
            (void)Characters::ToString (c1);
            EXPECT_TRUE (c1.size () == 3);
        };
        t2 ();
    }
}

namespace {
    void Test20_Join_ ()
    {
        using IO::Network::InternetAddress;
        Debug::TraceContextBumper ctx{"{}::Test20_Join_"};
        Iterable<InternetAddress> c{IO::Network::V4::kLocalhost, IO::Network::V4::kAddrAny};
        EXPECT_TRUE (c.Join () == "localhost, INADDR_ANY");
        EXPECT_TRUE (c.Join (L"; ") == "localhost; INADDR_ANY");
    }
}

namespace {
    void Test21_Repeat_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test21_Repeat_"};
        {
            Iterable<int> c{1};
            EXPECT_TRUE (c.Repeat (5).SequentialEquals (Iterable<int>{1, 1, 1, 1, 1}));
        }
        {
            using IO::Network::InternetAddress;
            Iterable<InternetAddress> c{IO::Network::V4::kLocalhost, IO::Network::V4::kAddrAny};
            EXPECT_TRUE (c.Repeat (0).size () == 0);
            EXPECT_TRUE (c.Repeat (1).SequentialEquals (c));
            EXPECT_TRUE (c.Repeat (10).size () == 20);
        }
    }
}

namespace {
    void Test22_Top_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test22_Top_"};
        {
            Iterable<int> c{3, 5, 9, 38, 3, 5};
            EXPECT_TRUE (c.Top ().SequentialEquals (c.OrderBy (std::greater<int>{})));
            EXPECT_TRUE (c.Top (2).SequentialEquals ({38, 9}));
            EXPECT_TRUE (c.Top (2, std::greater<int>{}).SequentialEquals ({38, 9}));
            EXPECT_TRUE (c.Top (3, std::less<int>{}).SequentialEquals ({3, 3, 5}));
        }
    }
}

namespace {
    void Test23_Iterable_Map_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test23_Iterable_Map_"};
        {
            using Traversal::Iterable;
            Iterable<int> t{1, 2, 3};
            vector<int>   r1 = t.Map<vector<int>> ([] (int i) { return i; });
            EXPECT_TRUE (t.SequentialEquals (r1));
            auto r2 = t.Map<Iterable<int>> ([] (int i) { return i; });
            EXPECT_TRUE (t.SequentialEquals (r2));
            auto r4 = t.Map ([] (int i) { return i; });
            EXPECT_TRUE (t.SequentialEquals (r4));
            auto r5 = t.Map ([] (int i) -> optional<int> { return i == 1 ? optional<int>{} : i; });
            EXPECT_TRUE (r5.SequentialEquals ({2, 3}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Debug::TraceContextBumper ctx{"{}::DoRegressionTests_"};
        Test_1_BasicRange_ ();
        Test_2_BasicDiscreteRangeIteration_ ();
        Test_3_SimpleDiscreteRangeWithEnumsTest_ ();
        Test4_FunctionalApplicationContext_MapTest_ ();
        Test5_ReduceTest_ ();
        Test6_FunctionApplicationContext_ ();
        Test7_FunctionApplicationContextWithDiscreteRangeEtc_ ();
        Test8_DiscreteRangeTestFromDocs_ ();
        Test9_Generators_ ();
        Test10_MakeIterableFromIterator_ ();
        Test11_GetDistanceSpanned_ ();
        Test12_RangeConstExpr_ ();
        Test13_DisjointRange_ ();
        Test14_ToString_ ();
        Test15_Partition_ ();
        Test16_LinqLikeFunctions_ ();
        Test17_DurationRange_ ();
        Test18_IterableConstructors_ ();
        Test19_CreateGeneratorBug_ ();
        Test20_Join_ ();
        Test21_Repeat_ ();
        Test22_Top_ ();
        Test23_Iterable_Map_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
