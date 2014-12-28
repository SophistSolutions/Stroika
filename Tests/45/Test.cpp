/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Traveral
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Containers/Sequence.h"
#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Configuration/Enumeration.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Traversal/DiscreteRange.h"
#include    "Stroika/Foundation/Traversal/DisjointDiscreteRange.h"
#include    "Stroika/Foundation/Traversal/DisjointRange.h"
#include    "Stroika/Foundation/Traversal/FunctionalApplication.h"
#include    "Stroika/Foundation/Traversal/Generator.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Traversal;


namespace   {

    void    Test_1_BasicRange_ ()
    {
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
            using   RT  =   RangeTraits::ExplicitRangeTraits_Integral < int, -3, 100 , Openness::eClosed, Openness::eClosed, int, unsigned int >;
            Range<int, RT> x    =   Range<int, RT>::FullRange ();
            VerifyTestResult (x.GetLowerBound () == -3);
            VerifyTestResult (x.GetUpperBound () == 100);
        }
    }

    void    Test_2_BasicDiscreteRangeIteration_ ()
    {
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
            VerifyTestResult (r.size () == 1);
        }
        {
            int nItemsHit = 0;
            int lastItemHit = 0;
            for (auto i : DiscreteRange<int> (3, 5)) {
                nItemsHit++;
                VerifyTestResult (lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == 5);    /// IN DISCUSSION - OPEN ENDED RHS?
        }
    }

}



namespace {
    void    Test_3_SimpleDiscreteRangeWithEnumsTest_ ()
    {
        enum    class   Color {
            red, blue, green,

            Stroika_Define_Enum_Bounds (red, green)
        };

        using   Memory::Optional;

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
            int nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color>::FullRange ()) {
                nItemsHit++;
                VerifyTestResult (lastItemHit.IsMissing () or * lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color, RangeTraits::DefaultDiscreteRangeTraits<Color>>::FullRange ()) {
                nItemsHit++;
                VerifyTestResult (lastItemHit.IsMissing () or * lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int nItemsHit = 0;
            Optional<Color> lastItemHit;
            for (auto i : DiscreteRange<Color> (Optional<Color> (), Optional<Color> ())) {
                nItemsHit++;
                VerifyTestResult (lastItemHit.IsMissing () or * lastItemHit < i);
                lastItemHit = i;
            }
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
        {
            int nItemsHit = 0;
            Optional<Color> lastItemHit;
            DiscreteRange<Color> (Optional<Color> (), Optional<Color> ()).Apply ([&nItemsHit, &lastItemHit] (Color i) {
                nItemsHit++;
                VerifyTestResult (lastItemHit.IsMissing () or * lastItemHit < i);
                lastItemHit = i;
            });
            VerifyTestResult (nItemsHit == 3);
            VerifyTestResult (lastItemHit == Color::green);
        }
    }
}




namespace {
    void    Test4_MapTest_ ()
    {
        {
            Containers::Sequence<int>   n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            Containers::Sequence<int>   n1 = Containers::Sequence<int> (FunctionalApplicationContext<int> (n).Map<int> ([] (int i) -> int { return i + 1;}));
            VerifyTestResult (n1.size () == 3);
            VerifyTestResult (n1[0] == 2);
            VerifyTestResult (n1[1] == 3);
            VerifyTestResult (n1[2] == 4);
        }
    }
}







namespace {
    void    Test5_ReduceTest_ ()
    {
        {
            Containers::Sequence<int>   n;
            n.Append (1);
            n.Append (2);
            n.Append (3);
            int sum = FunctionalApplicationContext<int> (n).Reduce<int> ([] (int l, int r) -> int { return l + r; }, 0);
            VerifyTestResult (sum == 6);
        }
    }
}


namespace {
    void    Test6_FunctionApplicationContext_ ()
    {
        using Containers::Sequence;

        {
            Containers::Sequence<int> s =   { 1, 2, 3 };
            {
                shared_ptr<int> countSoFar = shared_ptr<int> (new int (0));
                size_t answer =
                    FunctionalApplicationContext<int>(s).
                    Filter<int> ([countSoFar] (int) -> bool { ++(*countSoFar); return (*countSoFar) & 1; }).
                Map<int> ([] (int s) { return s + 5; }).
                Reduce<size_t> ([] (int s, size_t memo) { return memo + 1; })
                ;
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer =
                    FunctionalApplicationContext<int>(s).
                    Filter<int> ([&countSoFar] (int) -> bool { ++countSoFar; return countSoFar & 1; }).
                Map<int> ([] (int s) { return s + 5; }).
                Reduce<size_t> ([] (int s, size_t memo) { return memo + 1; });
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<int> r = Containers::Sequence<int> (
                                                  FunctionalApplicationContext<int>(s).
                                                  Filter<int> ([&countSoFar] (int) -> bool { ++countSoFar; return countSoFar & 1; }).
                Map<int> ([] (int s) { return s + 5; })
                                              );
                VerifyTestResult (r.length () == 2);
                VerifyTestResult (r[0] == 6 and r[1] == 8);
            }
            {
                Memory::Optional<int> answer =
                    FunctionalApplicationContext<int>(s).
                    Filter<int> ([] (int i) -> bool { return (i & 1); }).
                    Find<int> ([] (int i) -> bool { return i == 1 ; })
                    ;
                VerifyTestResult (*answer == 1);
            }
            {
                Memory::Optional<int> answer =
                    FunctionalApplicationContext<int>(s).
                    Filter<int> ([] (int i) -> bool { return (i & 1); }).
                    Find<int> ([] (int i) -> bool { return i == 8 ; })
                    ;
                VerifyTestResult (answer.IsMissing ());
            }
        }

        {
            using   Characters::String;
            Sequence<String> s = { L"alpha", L"beta", L"gamma" };
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                size_t answer =
                    FunctionalApplicationContext<String>(s).
                    Filter<String> ([&countSoFar] (String) -> bool { ++countSoFar; return countSoFar & 1; }).
                Map<String> ([] (String s) { return s + L" hello"; }).
                Reduce<size_t> ([] (String s, size_t memo) { return memo + 1; });
                VerifyTestResult (answer == 2);
            }
            {
                int countSoFar = 0; // ONLY OK - cuz FunctionalApplicationContext <> and resulting iterators go
                // out of scope before this does
                Containers::Sequence<String> r = Containers::Sequence<String> (
                                                     FunctionalApplicationContext<String>(s).
                                                     Filter<String> ([&countSoFar] (String) -> bool { ++countSoFar; return countSoFar & 1; }).
                Map<String> ([] (String s) { return s + L" hello"; })
                                                 );
                VerifyTestResult (r.length () == 2);
                VerifyTestResult (r[0] == L"alpha hello" and r[1] == L"gamma hello");
            }
        }
    }
}




namespace {
    void    Test7_FunctionApplicationContextWithDiscreteRangeEtc_ ()
    {
        using   Containers::Sequence;
        {
            const uint32_t  kRefCheck_[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
            auto isPrimeCheck = [] (uint32_t n) -> bool { return Math::IsPrime (n); };
            for (auto i : FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100)).Filter<uint32_t> (isPrimeCheck)) {
                VerifyTestResult (Math::IsPrime (i));
            }
            Sequence<uint32_t> s = Sequence<uint32_t> (FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100)).Filter<uint32_t> (isPrimeCheck));
            VerifyTestResult (s == Sequence<uint32_t> (begin (kRefCheck_), end (kRefCheck_)));
            VerifyTestResult (NEltsOf (kRefCheck_) == FunctionalApplicationContext<uint32_t> (DiscreteRange<uint32_t> (1, 100)).Filter<uint32_t> (isPrimeCheck).GetLength ());
        }
    }
}


namespace {
    void    Test8_DiscreteRangeTestFromDocs_ ()
    {
        // From Docs in DiscreteRange<> class
        vector<int> v = DiscreteRange<int> (1, 10).As<vector<int>> ();
        VerifyTestResult (v == vector<int> ({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
        for (auto i : DiscreteRange<int> (1, 10)) {
            VerifyTestResult (1 <= i and i <= 10);  // rough verification
        }
    }
}




namespace {
    void    Test9_Generators_ ()
    {
        {
            constexpr int kMin = 1;
            constexpr int kMax = 10;
            auto myContext = shared_ptr<int> (new int (kMin - 1));
            auto getNext = [myContext] () -> Memory::Optional<int> {
                (*myContext)++;
                if (*myContext > 10)
                {
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
    void    Test10_MakeIterableFromIterator_ ()
    {
        {
            Containers::Sequence<int>   a = {1, 3, 5, 7, 9};
            Iterator<int>   iter = a.MakeIterator ();

            int sum = 0;
            for (auto i : MakeIterableFromIterator (iter)) {
                sum += i;
            }
            VerifyTestResult (sum == 25);
        }

    }
}


namespace {
    void    Test11_GetDistanceSpanned_ ()
    {
        using IntRange =  Range<unsigned int>;
        IntRange foo {3, 9};
        VerifyTestResult (foo.GetDistanceSpanned () == 6);
    }
}





namespace {
    void    Test12_RangeConstExpr_ ()
    {
        using IntRange =  Range<unsigned int>;
        constexpr IntRange kFoo_ {3, 9};
        constexpr unsigned int l = kFoo_.GetLowerBound ();
        constexpr unsigned int u = kFoo_.GetUpperBound ();
        VerifyTestResult (l == 3);
        VerifyTestResult (u == 9);
        constexpr   unsigned int m = kFoo_.GetMidpoint ();
        VerifyTestResult (m == 6);
    }
}






namespace {
    void    Test13_DisjointRange_ ()
    {
        {
            DisjointRange<Range<float>> dr {};
            VerifyTestResult (dr.empty ());
            VerifyTestResult (dr.GetBounds ().empty ());
            VerifyTestResult (dr.GetSubRanges ().empty ());
            VerifyTestResult (not dr.Contains (3));
        }
        {
            DisjointRange<Range<float>> dr {Range<float> (2.1f, 5.0f)};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == Range<float> (2.1f, 5.0f));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (2));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointRange<RT>;
            DRT dr {};
            VerifyTestResult (dr.empty ());
            VerifyTestResult (dr.GetBounds ().empty ());
            VerifyTestResult (dr.GetSubRanges ().empty ());
            VerifyTestResult (not dr.Contains (3));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointRange<RT>;
            DRT dr {RT (2, 5)};
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (2, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (1));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 2}, RT {4, 5}};
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 2);
            VerifyTestResult (dr.Contains (2));
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.Contains (4));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {4, 5}, RT {1, 2}};
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 2);
            VerifyTestResult (dr.Contains (2));
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.Contains (4));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {};
            VerifyTestResult (dr.empty ());
            dr.Add (4);
            VerifyTestResult (dr.GetBounds () == RT (4, 4));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult ( dr.Contains (4));
            for (int i = 5; i <= 100; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            VerifyTestResult (dr.GetBounds () == RT (4, 100));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            for (int i = 501; i < 600; ++i) {
                dr.Add (4);
                dr.Add (i);
            }
            dr.Add (3);
            VerifyTestResult (dr.GetSubRanges ().size () == 2);
            VerifyTestResult (dr.GetBounds () == RT (3, 600 - 1));
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (not dr.Contains (300));
            VerifyTestResult (dr.Contains (599));
            VerifyTestResult (not  dr.Contains (600));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 1}, RT {3, 3}, RT {5, 5} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 3);
            VerifyTestResult (not dr.Contains (4));
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 5}, RT {3, 7}, RT {5, 9} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT = Range<float>;
            using DRT = DisjointRange<RT>;
            DRT dr {RT {1, 5}, RT {3, 7}, RT {5, 9} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT = Range<float>;
            using DRT = DisjointRange<RT>;
            DRT dr {RT {1, 5}, RT {2, 2} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 4}, RT {5, 9} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 9));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
            VerifyTestResult (dr.Contains (3));
            dr.Add (10);
            VerifyTestResult (dr.GetBounds () == RT (1, 10));
            VerifyTestResult (dr.GetSubRanges ().size () == 1);
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 1}, RT {3, 3}, RT {5, 5} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 5));
            VerifyTestResult (dr.GetSubRanges ().size () == 3);
            VerifyTestResult (dr.Contains (3));
            VerifyTestResult (dr.GetNext (1) == 3);
            VerifyTestResult (dr.GetNext (2) == 3);
            VerifyTestResult (dr.GetNext (3) == 5);
            VerifyTestResult (dr.GetNext (4) == 5);
            VerifyTestResult (dr.GetNext (5).IsMissing ());
#if 0
            VerifyTestResult (dr.GetPrevious (1).IsMissing ());
            VerifyTestResult (dr.GetPrevious (2) == 1);
#endif
        }
        {
            using RT = DiscreteRange<int>;
            using DRT = DisjointDiscreteRange<RT>;
            DRT dr {RT {1, 2}, RT {4, 5}, RT {7, 8} };
            VerifyTestResult (not dr.empty ());
            VerifyTestResult (dr.GetBounds () == RT (1, 8));
            VerifyTestResult (dr.GetSubRanges ().size () == 3);
            VerifyTestResult (not dr.Contains (3));
            VerifyTestResult (dr.GetNext (1) == 2);
            VerifyTestResult (dr.GetNext (2) == 4);
            VerifyTestResult (dr.GetNext (3) == 4);
            VerifyTestResult (dr.GetNext (4) == 5);
            VerifyTestResult (dr.GetNext (5) == 7);
            VerifyTestResult (dr.GetNext (6) == 7);
            VerifyTestResult (dr.GetNext (7) == 8);
            VerifyTestResult (dr.GetNext (8).IsMissing ());
            VerifyTestResult (dr.GetNext (99).IsMissing ());
        }
    }
}




namespace   {
    void    DoRegressionTests_ ()
    {
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
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

