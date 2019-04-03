/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Time/DateTimeRange.h"
#include "Stroika/Foundation/Time/DurationRange.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"
#include "Stroika/Foundation/Traversal/DisjointDiscreteRange.h"
#include "Stroika/Foundation/Traversal/DisjointRange.h"
#include "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include "Stroika/Foundation/Traversal/Generator.h"
#include "Stroika/Foundation/Traversal/Partition.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Traversal;

namespace {

    void Test_1_BasicRange_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test_1_BasicRange_ ()"};
        {
            Range<int> r (3, 5);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Contains (3));
        }
        {
            Range<double> r (3, 5);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Contains (3));
        }
        {
            Range<double> r{3, 5, Openness::eOpen, Openness::eOpen};
            VerifyTestResult (not r.Contains (3));
        }
        {
#if 0
            ////// MAYBE GET RID OF THIS???
            Range<int> r1 (3, 5);
            Range<int> r2 (5, 6);
            VerifyTestResult (not r1.Overlaps (r2));
            VerifyTestResult (not r2.Overlaps (r1));
            Range<int> r3  = r1;
            VerifyTestResult (r1.Overlaps (r3));
            VerifyTestResult (r3.Overlaps (r1));
#endif
        }
        {
            using RT         = RangeTraits::ExplicitRangeTraits_Integral<int, -3, 100, Openness::eClosed, Openness::eClosed, int, unsigned int>;
            Range<int, RT> x = Range<int, RT>::FullRange ();
            VerifyTestResult (x.GetLowerBound () == -3);
            VerifyTestResult (x.GetUpperBound () == 100);
        }
    }

    void Test_2_BasicDiscreteRangeIteration_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test_2_BasicDiscreteRangeIteration_"};
        {
            DiscreteRange<int> r (3, 5);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Contains (3));
        }
        {
            DiscreteRange<int> r;
            VerifyTestResult (r.empty ());
        }
        {
            DiscreteRange<int> r (3, 3);
            VerifyTestResult (not r.empty ());
            VerifyTestResult (r.Elements ().size () == 1);
        }
        {
            int nItemsHit   = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5).Elements ()) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == 5); /// IN DISCUSSION - OPEN ENDED RHS?
        }
        {
            int nItemsHit   = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5)) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == 5); /// IN DISCUSSION - OPEN ENDED RHS?
        }
    }
}

namespace {
    void Test_3_SimpleDiscreteRangeWithEnumsTest_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test_3_SimpleDiscreteRangeWithEnumsTest_"};
        enum class Color {
            red,
            blue,
            green,

            Stroika_Define_Enum_Bounds (red, green)
        };

        using Memory::Optional;

        {
            Color min1 = RangeTraits::DefaultDiscreteRangeTraits<Color>::kLowerBound;
            Color max1 = RangeTraits::DefaultDiscreteRangeTraits<Color>::kUpperBound;
            Color min2 = RangeTraits::DefaultDiscreteRangeTraits_Enum<Color>::kLowerBound;
            Color max2 = RangeTraits::DefaultDiscreteRangeTraits_Enum<Color>::kUpperBound;
            Color min3 = RangeTraits::ExplicitDiscreteRangeTraits<Color, Color::eSTART, Color::eLAST, int, unsigned int>::kLowerBound;
            Color max3 = RangeTraits::ExplicitDiscreteRangeTraits<Color, Color::eSTART, Color::eLAST, int, unsigned int>::kUpperBound;
            Color min4 = RangeTraits::ExplicitRangeTraits_Integral<Color, Color::eSTART, Color::eLAST, Openness::eClosed, Openness::eClosed, int, unsigned int>::kLowerBound;
            Color max4 = RangeTraits::ExplicitRangeTraits_Integral<Color, Color::eSTART, Color::eLAST, Openness::eClosed, Openness::eClosed, int, unsigned int>::kUpperBound;
            VerifyTestResult (Color::red == Color::eSTART and Color::green == Color::eLAST);
            VerifyTestResult (min1 == Color::eSTART and max1 == Color::eLAST);
            VerifyTestResult (min2 == Color::eSTART and max2 == Color::eLAST);
            VerifyTestResult (min3 == Color::eSTART and max3 == Color::eLAST);
            VerifyTestResult (min4 == Color::eSTART and max4 == Color::eLAST);
        }
        {
            int             nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color>::FullRange ().Elements ()) {
                nItemsHit++;
                VerifyTestResult (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int             nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color, RangeTraits::DefaultDiscreteRangeTraits<Color>>::FullRange ().Elements ()) {
                nItemsHit++;
                VerifyTestResult (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int             nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color> (Optional<Color> (), Optional<Color> ()).Elements ()) {
                nItemsHit++;
                VerifyTestResult (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int             nItemsHit = 0;
            Optional<Color> lastItemHit;
            DiscreteRange<Color> (Optional<Color> (), Optional<Color> ()).Elements ().Apply ([&nItemsHit, &lastItemHit] (Color i) {
                nItemsHit++;
                VerifyTestResult (not lastItemHit.has_value () or *lastItemHit < i);
                lastItemHit = i;
            });
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
    }
}

namespace {
    void Test4_MapTest_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test4_MapTest_"};
        {
            Containers::Sequence<int> n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            Containers::Sequence<int> n1 = Containers::Sequence<int> (FunctionalApplicationContext<int> (n).Map<int> ([] (int i) -> int { return i + 1; }));
            VerifyTestResult (n1.size () == 3);
            VerifyTestResult (n1[0] == 2);
            VerifyTestResult (n1[1] == 3);
            VerifyTestResult (n1[2] == 4);
        }
    }
}

namespace {
    void Test5_ReduceTest_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test5_ReduceTest_"};
        {
            Containers::Sequence<int> n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            int sum = FunctionalApplicationContext<int> (n).Reduce<int> ([] (int l, int r) -> int { return l + r; }, 0);
            VerifyTestResult (sum == 6);
        }
    }
}

namespace {
    void Test6_FunctionApplicationContext_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test6_FunctionApplicationContext_"};
        using Containers::Sequence;

        {
            Containers::Sequence<int> s = {1, 2, 3};
            {
                shared_ptr<int>         countSoFar = shared_ptr<int> (new int (0));
                [[maybe_unused]] size_t answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([countSoFar] (int) -> bool { ++(*countSoFar); return (*countSoFar) & 1; }).Map<int> ([] (int s) { return s + 5; }).Reduce<size_t> ([] ([[maybe_unused]] int s, size_t memo) { Lambda_Arg_Unused_BWA (s); return memo + 1; });
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([&countSoFar] (int) -> bool { ++countSoFar; return countSoFar & 1; }).Map<int> ([] (int s) { return s + 5; }).Reduce<size_t> ([] ([[maybe_unused]] int s, size_t memo) { Lambda_Arg_Unused_BWA (s); return memo + 1; });
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<int> r = Containers::Sequence<int> (
                    FunctionalApplicationContext<int> (s).Filter<int> ([&countSoFar] (int) -> bool { ++countSoFar; return countSoFar & 1; }).Map<int> ([] (int s) { return s + 5; }));
                VerifyTestResult (r.length () == 2);
                VerifyTestResult (r[0] == 6 and r[1] == 8);
            }
            {
                Memory::Optional<int> answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([] (int i) -> bool { return (i & 1); }).Find<int> ([] (int i) -> bool { return i == 1; });
                VerifyTestResult (*answer == 1);
            }
            {
                Memory::Optional<int> answer =
                    FunctionalApplicationContext<int> (s).Filter<int> ([] (int i) -> bool { return (i & 1); }).Find<int> ([] (int i) -> bool { return i == 8; });
                VerifyTestResult (not answer.has_value ());
            }
        }

        {
            using Characters::String;
            Sequence<String> s = {L"alpha", L"beta", L"gamma"};
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer =
                    FunctionalApplicationContext<String> (s).Filter<String> ([&countSoFar] (String) -> bool { ++countSoFar; return countSoFar & 1; }).Map<String> ([] (String s) { return s + L" hello"; }).Reduce<size_t> ([] ([[maybe_unused]] String s, size_t memo) { Lambda_Arg_Unused_BWA (s); return memo + 1; });
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<String> r = Containers::Sequence<String> (
                    FunctionalApplicationContext<String> (s).Filter<String> ([&countSoFar] (String) -> bool { ++countSoFar; return countSoFar & 1; }).Map<String> ([] (String s) { return s + L" hello"; }));
                VerifyTestResult (r.length () == 2);
                VerifyTestResult (r[0] == L"alpha hello" and r[1] == L"gamma hello");
            }
        }
    }
}

namespace {
    void Test7_FunctionApplicationContextWithDiscreteRangeEtc_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test7_FunctionApplicationContextWithDiscreteRangeEtc_"};
        using Containers::Sequence;
        {
            const uint32_t kRefCheck_[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
            auto           isPrimeCheck = [] (uint32_t n) -> bool { return Math::IsPrime (n); };
            for (auto i : FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100).Elements ()).Filter<uint32_t> (isPrimeCheck)) {
                VerifyTestResult (Math::IsPrime (i));
            }
            Sequence<uint32_t> s = Sequence<uint32_t> (FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100).Elements ()).Filter<uint32_t> (isPrimeCheck));
            VerifyTestResult (s == Sequence<uint32_t> (begin (kRefCheck_), end (kRefCheck_)));
            VerifyTestResult (NEltsOf (kRefCheck_) == FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100).Elements ()).Filter<uint32_t> (isPrimeCheck).GetLength ());
        }
    }
}

namespace {
    void Test8_DiscreteRangeTestFromDocs_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test8_DiscreteRangeTestFromDocs_"};
        // From Docs in DiscreteRange<> class
        vector<int> v = DiscreteRange<int> (1, 10).Elements ().As<vector<int>> ();
        VerifyTestResult (v == vector<int> ({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
        for (auto i : DiscreteRange<int> (1, 10).Elements ()) {
            VerifyTestResult (1 <= i and i <= 10); // rough verification
        }
    }
}

namespace {
    void Test9_Generators_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test9_Generators_"};
        {
            constexpr int kMin      = 1;
            constexpr int kMax      = 10;
            auto          myContext = shared_ptr<int> (new int (kMin - 1));
            auto          getNext   = [myContext] () -> Memory::Optional<int> {
                (*myContext)++;
                if (*myContext > 10) {
                    return Memory::Optional<int> ();
                }
                return *myContext;
            };

            int sum = 0;
            for (auto i : CreateGenerator<int> (getNext)) {
                VerifyTestResult (1 <= i and i <= 10);
                sum += i;
            }
            VerifyTestResult (sum == (kMax - kMin + 1) * (kMax + kMin) / 2);
        }
    }
}

namespace {
    void Test10_MakeIterableFromIterator_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test10_MakeIterableFromIterator_"};
        {
            Containers::Sequence<int> a    = {1, 3, 5, 7, 9};
            Iterator<int>             iter = a.MakeIterator ();

            int sum = 0;
            for (auto i : MakeIterableFromIterator (iter)) {
                sum += i;
            }
            VerifyTestResult (sum == 25);
        }
    }
}

namespace {
    void Test11_GetDistanceSpanned_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test11_GetDistanceSpanned_"};
        using IntRange = Range<unsigned int>;
        IntRange foo{3, 9};
        VerifyTestResult (foo.GetDistanceSpanned () == 6);
    }
}

namespace {
    void Test12_RangeConstExpr_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test12_RangeConstExpr_"};
        using IntRange = Range<unsigned int>;
        constexpr IntRange     kFoo_{3, 9};
        constexpr unsigned int l = kFoo_.GetLowerBound ();
        constexpr unsigned int u = kFoo_.GetUpperBound ();
        VerifyTestResult (l == 3);
        VerifyTestResult (u == 9);
        constexpr unsigned int m = kFoo_.GetMidpoint ();
        VerifyTestResult (m == 6);
    }
}

namespace {
    void Test13_DisjointRange_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test13_DisjointRange_"};
        {
            DisjointRange<float> dr{};
            VerifyTestResult (dr.empty ());
            VerifyTestResult (dr.GetBounds ().empty ());
            VerifyTestResult (dr.SubRanges ().empty ());
            VerifyTestResult (not dr.Contains (3));
        }
        {
            DisjointRange<float> dr{Range<float> (2.1f, 5.0f)};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == Range<float> (2.1f, 5.0f));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (2));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointRange<int, RT>;
            DRT dr{};
            VerifyTestResult (dr.empty ());
            VerifyTestResult (dr.GetBounds ().empty ());
            VerifyTestResult (dr.SubRanges ().empty ());
            VerifyTestResult (not dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointRange<int, RT>;
            DRT dr{RT (2, 5)};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (2, 5));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (1));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 2}, RT{4, 5}};
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.SubRanges ().size () == 2);
            VerifyTestResult (dr.Contains (2));
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.Contains (4));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{4, 5}, RT{1, 2}};
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.SubRanges ().size () == 2);
            VerifyTestResult (dr.Contains (2));
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.Contains (4));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{};
            VerifyTestResult (dr.empty ());
            dr.Add (4);
            VerifyTestResult (dr.GetBounds () == RT (4, 4));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.Contains (4));
            for (int i = 5; i <= 100; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            VerifyTestResult (dr.GetBounds () == RT (4, 100));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            for (int i = 501; i < 600; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            dr.Add (3);
            VerifyTestResult (dr.SubRanges ().size () == 2);
            VerifyTestResult (dr.GetBounds () == RT (3, 600 - 1));
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (300));
            VerifyTestResult (dr.Contains (599));
            VerifyTestResult (not dr.Contains (600));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 1}, RT{3, 3}, RT{5, 5}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.SubRanges ().size () == 3);
            VerifyTestResult (not dr.Contains (4));
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{3, 7}, RT{5, 9}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT  = Range<float>;
            using DRT = DisjointRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{3, 7}, RT{5, 9}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT  = Range<float>;
            using DRT = DisjointRange<RT::value_type, RT>;
            DRT dr{RT{1, 5}, RT{2, 2}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 4}, RT{5, 9}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            dr.Add (10);
            VerifyTestResult (dr.GetBounds () == RT (1, 10));
            VerifyTestResult (dr.SubRanges ().size () == 1);
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 1}, RT{3, 3}, RT{5, 5}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.SubRanges ().size () == 3);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (dr.GetNext (1) == 3);
            VerifyTestResult (dr.GetNext (2) == 3);
            VerifyTestResult (dr.GetNext (3) == 5);
            VerifyTestResult (dr.GetNext (4) == 5);
            VerifyTestResult (not dr.GetNext (5).has_value ());
            VerifyTestResult (not dr.GetPrevious (1).has_value ());
            VerifyTestResult (dr.GetPrevious (2) == 1);
            VerifyTestResult (dr.GetPrevious (3) == 1);
            VerifyTestResult (dr.GetPrevious (4) == 3);
            VerifyTestResult (dr.GetPrevious (5) == 3);
            VerifyTestResult (dr.GetPrevious (6) == 5);
            VerifyTestResult (dr.GetPrevious (7) == 5);
        }
        {
            using RT  = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT::value_type, RT>;
            DRT dr{RT{1, 2}, RT{4, 5}, RT{7, 8}};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 8));
            VerifyTestResult (dr.SubRanges ().size () == 3);
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.GetNext (1) == 2);
            VerifyTestResult (dr.GetNext (2) == 4);
            VerifyTestResult (dr.GetNext (3) == 4);
            VerifyTestResult (dr.GetNext (4) == 5);
            VerifyTestResult (dr.GetNext (5) == 7);
            VerifyTestResult (dr.GetNext (6) == 7);
            VerifyTestResult (dr.GetNext (7) == 8);
            VerifyTestResult (not dr.GetNext (8).has_value ());
            VerifyTestResult (not dr.GetNext (99).has_value ());
            VerifyTestResult (not dr.GetPrevious (1).has_value ());
            VerifyTestResult (dr.GetPrevious (2) == 1);
            VerifyTestResult (dr.GetPrevious (3) == 2);
            VerifyTestResult (dr.GetPrevious (4) == 2);
            VerifyTestResult (dr.GetPrevious (5) == 4);
            VerifyTestResult (dr.GetPrevious (6) == 5);
            VerifyTestResult (dr.GetPrevious (7) == 5);
            VerifyTestResult (dr.GetPrevious (8) == 7);
            VerifyTestResult (dr.GetPrevious (9) == 8);
            {
                // test iterate over ranges
                int timeThru = 0;
                for (RT rng : dr.SubRanges ()) {
                    switch (timeThru++) {
                        case 0:
                            VerifyTestResult ((rng == RT{1, 2}));
                            break;
                        case 1:
                            VerifyTestResult ((rng == RT{4, 5}));
                            break;
                        case 2:
                            VerifyTestResult ((rng == RT{7, 8}));
                            break;
                        default:
                            VerifyTestResult (false);
                    }
                }
            }
            {
                // test iterate over elements
                VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({1, 2, 4, 5, 7, 8}));
            }
            {
                // Test intersection
                VerifyTestResult (Containers::Sequence<int> (dr.Intersection (RT{2, 4}).Elements ()) == Containers::Sequence<int> ({2, 4}));
            }
        }
        {
            using DRT = DisjointDiscreteRange<int>;
            DRT                       dr;
            static const unsigned int kMax_ = Debug::IsRunningUnderValgrind () ? 1000u : 10000u;
            for (int i = 0; i < (int)kMax_; ++i) {
                dr.Add (i);
            }
            VerifyTestResult (dr.SubRanges ().size () == 1);
            VerifyTestResult (dr.Elements ().size () == kMax_);
        }
        {
            using DRT = DisjointDiscreteRange<int>;
            DRT dr;
            dr.Add (872);
            VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({872}));
            dr.Add (231);
            VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 872}));
            dr.Add (329);
            VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 872}));
            dr.Add (665);
            VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 665, 872}));
            dr.Add (581);
            VerifyTestResult (Containers::Sequence<int> (dr.Elements ()) == Containers::Sequence<int> ({231, 329, 581, 665, 872}));
        }
        {
            using Containers::Set;
            using Containers::SortedSet;
            using DRT      = DisjointDiscreteRange<int>;
            auto roundTrip = [] (const Set<int>& s) {
                DRT tmp{s};
                VerifyTestResult (tmp.Elements ().size () == s.size ());
                VerifyTestResult (Set<int> (tmp.Elements ()) == s);
            };
            roundTrip (Set<int>{3, 4});
            roundTrip (Set<int>{1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 100});
            roundTrip (Set<int>{4, 5, 6, 7, 8, 9, 10, 11, 100, 102, 103, 104});
            roundTrip (Set<int> ((DiscreteRange<int>{1, 1000}).Elements ()));
        }
    }
}

namespace {
    void Test14_ToString_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test14_ToString_"};
        VerifyTestResult (Range<int> (3, 4).ToString ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4)");
        VerifyTestResult (Range<int> (3, 4).ToString () == L"[3 ... 4)");
        {
            using namespace Time;
            VerifyTestResult (DateTimeRange (DateTime (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4))), DateTime (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5)))).ToString () == L"[4/4/03 ... 4/5/03]");
        }
        {
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            using namespace Time;
            VerifyTestResult (DateTimeRange (DateTime (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4))), DateTime (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5)))).ToString () == L"[4/4/1903 ... 4/5/1903]");
#endif
        }
    }
}

namespace {
    void Test15_Partition_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test15_Partition_"};
        {
            using Containers::Sequence;
            using RT = Range<int>;
            VerifyTestResult (not IsPartition (Sequence<RT>{RT{1, 2}, RT{3, 4}}));
            VerifyTestResult (IsPartition (Sequence<RT>{RT{1, 2}, RT{2, 4}}));
        }
#if 0
        VerifyTestResult (Range<int> (3, 4).Format ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4)");
        VerifyTestResult (Range<int> (3, 4).Format () == L"[3 ... 4)");
        {
            using   namespace   Time;
            VerifyTestResult (DateTimeRange (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4)), Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5))).Format () == L"[4/4/03 ... 4/5/03]");
        }
        {
            Configuration::ScopedUseLocale tmpLocale { Configuration::FindNamedLocale (L"en", L"us") };
            using   namespace   Time;
            VerifyTestResult (DateTimeRange (Date (Year (1903), MonthOfYear::eApril, DayOfMonth (4)), Date (Year (1903), MonthOfYear::eApril, DayOfMonth (5))).Format () == L"[4/4/1903 ... 4/5/1903]");
        }
#endif
    }
}

namespace {
    void Test16_LinqLikeFunctions_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test16_LinqLikeFunctions_"};
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Where ([] (int i) { return i % 2 == 0; }).SequnceEquals (Iterable<int>{2, 4, 6}));
            VerifyTestResult (c.Where ([] (int i) { return i % 2 == 1; }).SequnceEquals (Iterable<int>{1, 3, 5}));
            {
                Iterable<int> w = c.Where ([] (int i) { return i % 2 == 0; });
                VerifyTestResult (w.SequnceEquals (Iterable<int>{2, 4, 6}));
                VerifyTestResult (w.SequnceEquals (Iterable<int>{2, 4, 6}));
            }
        }
        {
            Iterable<int> c{1, 2, 2, 5, 9, 4, 5, 6};
            VerifyTestResult (c.Distinct ().SetEquals (Iterable<int>{1, 2, 4, 5, 6, 9}));
            auto resultsEqualMod5 = c.Distinct ([] (int l, int r) { return l % 5 == r % 5; });
            DbgTrace (L"x=%s", Characters::ToString (resultsEqualMod5).c_str ());
            VerifyTestResult (resultsEqualMod5.size () == 4);
            for (auto i : resultsEqualMod5) {
                VerifyTestResult (i != 3);
            }
        }
        {
            Iterable<pair<int, char>> c{{1, 'a'}, {2, 'b'}, {3, 'c'}};
            VerifyTestResult (c.Select<int> ([] (pair<int, char> p) { return p.first; }).SequnceEquals (Iterable<int>{1, 2, 3}));
        }
        {
            using Characters::String;
            Iterable<int> c{3, 4, 7};
            VerifyTestResult (c.Select<String> ([] (int i) { return Characters::Format (L"%d", i); }).SequnceEquals (Iterable<String>{L"3", L"4", L"7"}));
        }
        {
            Iterable<int> c = {1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Any ([] (int i) { return i % 2 == 0; }));
            VerifyTestResult (not c.Any ([] (int i) { return i > 7; }));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Skip (3).SequnceEquals (Iterable<int>{4, 5, 6}));
        }
        {
            using Containers::Sequence;
            using Containers::Set;
            Sequence<int> a{1, 3, 5, 7, 9};
            a = Sequence<int>{a.Skip (2)}; // https://stroika.atlassian.net/browse/STK-532 - crash
            VerifyTestResult ((a == Sequence<int>{5, 7, 9}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Take (3).SequnceEquals (Iterable<int>{1, 2, 3}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Slice (3, 5).SequnceEquals (Iterable<int>{4, 5}));
            VerifyTestResult (c.Slice (3, 9999).SequnceEquals (Iterable<int>{4, 5, 6}));
        }
        {
            Iterable<int> c{3, 5, 9, 38, 3, 5};
            VerifyTestResult (c.OrderBy ().SequnceEquals (Iterable<int>{3, 3, 5, 5, 9, 38}));
            VerifyTestResult (c.OrderBy ([] (int lhs, int rhs) -> bool { return lhs < rhs; }).SequnceEquals (Iterable<int>{3, 3, 5, 5, 9, 38}));
            VerifyTestResult (c.OrderBy ([] (int lhs, int rhs) -> bool { return lhs > rhs; }).SequnceEquals (Iterable<int>{38, 9, 5, 5, 3, 3}));
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Min () == 1);
        }
        {
            Iterable<int> c{1, 2, 3, 4, 5, 6};
            VerifyTestResult (c.Max () == 6);
        }
        {
            using Math::NearlyEquals;
            Iterable<int> c{1, 2, 9, 4, 5, 3};
            VerifyTestResult (c.Median () == 3);
            VerifyTestResult (NearlyEquals (c.Median<double> (), 3.5));
        }
        {
            Iterable<int> c{1, 2, 9, 4, 5, 3};
            VerifyTestResult (c.MeanValue () == 4);
        }
        {
            // From Iterable::First/Iterable::Last docs
            Iterable<int> c{3, 5, 9, 38, 3, 5};
            VerifyTestResult (*c.First () == 3);
            VerifyTestResult (*c.First ([] (int i) { return i % 2 == 0; }) == 38);
            VerifyTestResult (*c.Last () == 5);
            VerifyTestResult (*c.Last ([] (int i) { return i % 2 == 0; }) == 38);
        }
        {
            // From Iterable::All docs
            Iterable<int> c{3, 5, 9, 3, 5};
            VerifyTestResult (c.All ([] (int i) { return i % 2 == 1; }));
        }
    }
}

namespace {
    void Test17_DurationRange_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test17_DurationRange_"};
        using Time::Duration;
        using Time::DurationRange;

        {
            DurationRange a{Duration ("PT.5S"), Duration ("PT2M")};
            DurationRange b{Duration ("PT1S"), Duration ("PT2M")};
            Verify ((a ^ b) == b);
        }
        {
            DurationRange a{Duration ("PT.5S"), Duration ("PT2M")};
            VerifyTestResult (a.Pin (Duration ("PT.5S")) == Duration ("PT.5S"));
            VerifyTestResult (a.Pin (Duration ("PT0S")) == Duration ("PT.5S"));
            VerifyTestResult (a.Pin (Duration ("PT5M")) == Duration ("PT2M"));
            VerifyTestResult (a.Pin (Duration ("PT10S")) == Duration ("PT10S"));
        }
    }
}

namespace {
    void Test18_IterableConstructors_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::Test18_IterableConstructors_"};

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
    void DoRegressionTests_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_"};
        Test_1_BasicRange_ ();
        Test_2_BasicDiscreteRangeIteration_ ();
        Test_3_SimpleDiscreteRangeWithEnumsTest_ ();
        Test4_MapTest_ ();
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
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
