/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ********** ComparisonRelationDeclaration<TYPE, ACTUAL_COMPARER> ****************
     ********************************************************************************
     */
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
        requires (not is_reference_v<ACTUAL_COMPARER>)
    inline constexpr ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::ComparisonRelationDeclaration (const ACTUAL_COMPARER& actualComparer)
        : ACTUAL_COMPARER{actualComparer}
    {
    }
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
        requires (not is_reference_v<ACTUAL_COMPARER>)
    inline constexpr ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer)
        : ACTUAL_COMPARER{move (actualComparer)}
    {
    }

    /*
     ********************************************************************************
     **************************** DeclareEqualsComparer *****************************
     ********************************************************************************
     */
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, remove_cvref_t<FUNCTOR>> DeclareEqualsComparer (FUNCTOR&& f)
    {
        static_assert (
            IPotentiallyComparer<remove_cvref_t<FUNCTOR>, typename Configuration::FunctionTraits<remove_cvref_t<FUNCTOR>>::template arg<0>::type>);
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, remove_cvref_t<FUNCTOR>>{std::forward<FUNCTOR> (f)};
    }

    /*
     ********************************************************************************
     ******************************* DeclareInOrderComparer *************************
     ********************************************************************************
     */
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, remove_cvref_t<FUNCTOR>>
    DeclareInOrderComparer (FUNCTOR&& f)
    {
        static_assert (
            IPotentiallyComparer<remove_cvref_t<FUNCTOR>, typename Configuration::FunctionTraits<remove_cvref_t<FUNCTOR>>::template arg<0>::type>);
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, remove_cvref_t<FUNCTOR>>{std::forward<FUNCTOR> (f)};
    }

    /*
     ********************************************************************************
     ********************* EqualsComparerAdapter<BASE_COMPARER> *********************
     ********************************************************************************
     */
    template <IComparer BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <IComparer BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <IComparer BASE_COMPARER>
    template <typename LT, typename RT>
    constexpr bool EqualsComparerAdapter<BASE_COMPARER>::operator() (LT&& lhs, RT&& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits_v<BASE_COMPARER>;
        auto           baseComparison = fBASE_COMPARER_ (forward<LT> (lhs), forward<RT> (rhs));
        if constexpr (kRelationKind == ComparisonRelationType::eEquals) {
            return baseComparison;
        }
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            if (baseComparison) {
                return false; // if less, cannot be equal
            }
            else {
                return not fBASE_COMPARER_ (forward<RT> (rhs), forward<LT> (lhs)); // if not (LHS < RHS), maybe RHS < LHS?
            }
        }
        if constexpr (kRelationKind == ComparisonRelationType::eInOrderOrEquals) {
            if (baseComparison) {
                return fBASE_COMPARER_ (forward<RT> (rhs), forward<LT> (lhs)); // if  (LHS <= RHS), then EQ if reverse true
            }
            else {
                return false; // not not <=, then clearly not equal
            }
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison == strong_ordering::equal;
        }
        AssertNotReached ();
        return false;
    }

    /*
     ********************************************************************************
     ********************* InOrderComparerAdapter<BASE_COMPARER> ********************
     ********************************************************************************
     */
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eInOrderOrEquals or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare)
    constexpr inline InOrderComparerAdapter<BASE_COMPARER>::InOrderComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eInOrderOrEquals or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare)
    constexpr inline InOrderComparerAdapter<BASE_COMPARER>::InOrderComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eInOrderOrEquals or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare)
    template <typename LT, typename RT>
    constexpr inline bool InOrderComparerAdapter<BASE_COMPARER>::operator() (LT&& lhs, RT&& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits_v<BASE_COMPARER>;
        auto           baseComparison = fBASE_COMPARER_ (forward<LT> (lhs), forward<RT> (rhs));
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            return kRelationKind;
        }
        if constexpr (kRelationKind == ComparisonRelationType::eInOrderOrEquals) {
            return baseComparison and not fBASE_COMPARER_ (forward<RT> (rhs), forward<LT> (lhs)); // eliminate equals case
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison == strong_ordering::less;
        }
        AssertNotReached ();
        return false;
    }

    /*
     ********************************************************************************
     ********************* ThreeWayComparerAdapter<BASE_COMPARER> *******************
     ********************************************************************************
     */
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder)
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder)
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder)
    template <typename LT, typename RT>
    constexpr strong_ordering ThreeWayComparerAdapter<BASE_COMPARER>::operator() (LT&& lhs, RT&& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits_v<BASE_COMPARER>;
        auto           baseComparison = fBASE_COMPARER_ (forward<LT> (lhs), forward<RT> (rhs));
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison;
        }
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            if (baseComparison) {
                return strong_ordering::less;
            }
            // if not <, then either equal or greater: if RHS < LHS, then LHS > RHS
            return fBASE_COMPARER_ (forward<RT> (rhs), forward<LT> (lhs)) ? strong_ordering::greater : strong_ordering::equal;
        }
        AssertNotReached ();
        return strong_ordering::equal;
    }

    /*
     ********************************************************************************
     *************** OptionalThreeWayCompare<T, TCOMPARER> **************************
     ********************************************************************************
     */
    template <typename T, IComparer TCOMPARER>
    constexpr OptionalThreeWayComparer<T, TCOMPARER>::OptionalThreeWayComparer (TCOMPARER&& comparer)
        : fTComparer_{move (comparer)}
    {
    }
    template <typename T, IComparer TCOMPARER>
    constexpr OptionalThreeWayComparer<T, TCOMPARER>::OptionalThreeWayComparer (const TCOMPARER& comparer)
        : fTComparer_{comparer}
    {
    }
    template <typename T, IComparer TCOMPARER>
    constexpr strong_ordering OptionalThreeWayComparer<T, TCOMPARER>::operator() (const optional<T>& lhs, const optional<T>& rhs) const
    {
        if (lhs and rhs) {
            return fTComparer_ (*lhs, *rhs);
        }
        if (not lhs and not rhs) {
            return strong_ordering::equal;
        }
        // treat missing as less than present
        if (lhs) {
            return strong_ordering::greater;
        }
        else {
            return strong_ordering::less;
        }
    }

    /*
     ********************************************************************************
     *************************** CompareResultNormalizer ****************************
     ********************************************************************************
     */
    template <typename FROM_INT_TYPE>
    constexpr strong_ordering CompareResultNormalizer (FROM_INT_TYPE f)
    {
        if (f == 0) {
            return strong_ordering::equal;
        }
        else {
            Assert (f < 0 or f > 0);
            return f < 0 ? strong_ordering::less : strong_ordering::greater;
        }
    }

    /*
     ********************************************************************************
     ******************************* ReverseCompareOrder ****************************
     ********************************************************************************
     */
    constexpr strong_ordering ReverseCompareOrder (strong_ordering so)
    {
        if (so == strong_ordering::less) {
            return strong_ordering::greater;
        }
        else if (so == strong_ordering::greater) {
            return strong_ordering::less;
        }
        return so;
    }

    /********************************DEPRECATED STUFF ************************************************** */
    /********************************DEPRECATED STUFF ************************************************** */
    /********************************DEPRECATED STUFF ************************************************** */

    // @TODO SEEMS STILL NEEDED ON CLANG++-10???
    // @TODO PROBABLY DEPRECATE THIS CLASS - and use compare_three_way directly
#if __cpp_lib_three_way_comparison < 201907L
    struct [[deprecated ("Since Stroika 3.0d1 - use std::compare_three_way")]] ThreeWayComparer {
        template <typename LT, typename RT>
        constexpr auto operator() (LT&& lhs, RT&& rhs) const
        {
            using CT = common_type_t<LT, RT>;
            // ISSUE HERE - PRE C++20, no distinction made between strong_ordering, weak_ordering, and partial_ordering, because
            // this counts on cooperation with various types and mechanismns like operator<=> = default which we don't have (and declared strong_ordering=int)
            if (equal_to<CT>{}(forward<LT> (lhs), forward<RT> (rhs))) {
                return strong_ordering::equal;
            }
            return less<CT>{}(forward<LT> (lhs), forward<RT> (rhs)) ? strong_ordering::less : strong_ordering::greater;
        }
    };
#else
    struct [[deprecated ("Since Stroika 3.0d1 - use std::compare_three_way")]] ThreeWayComparer {
        template <typename LT, typename RT>
        constexpr auto operator() (LT&& lhs, RT&& rhs) const
        {
            return compare_three_way{}(forward<LT> (lhs), forward<RT> (rhs));
        }
    };
#endif
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    namespace Private_ {
        template <>
        struct ExtractComparisonTraits_<ThreeWayComparer> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
        };
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

    constexpr std::strong_ordering kLess [[deprecated ("Since Stroika 3.0d1 - use std::strong_ordering")]]  = std::strong_ordering::less;
    constexpr std::strong_ordering kEqual [[deprecated ("Since Stroika 3.0d1 - use std::strong_ordering")]] = std::strong_ordering::equal;
    constexpr std::strong_ordering kGreater [[deprecated ("Since Stroika 3.0d1 - use std::strong_ordering")]] = std::strong_ordering::greater;

#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
    struct compare_three_way_BWA {
        // NOTE - this workaround is GENERALLY INADEQUATE, but is adequate for my current use in Stroika -- LGP 2022-11-01
        template <typename LT, typename RT>
        constexpr auto operator() (LT&& lhs, RT&& rhs) const
        {
            using CT = common_type_t<LT, RT>;
            if (equal_to<CT>{}(forward<LT> (lhs), forward<RT> (rhs))) {
                return strong_ordering::equal;
            }
            return less<CT>{}(forward<LT> (lhs), forward<RT> (rhs)) ? strong_ordering::less : strong_ordering::greater;
        }
        using is_transparent = void;
    };
#endif
    template <typename FUNCTOR, typename FUNCTOR_ARG>
    [[deprecated ("Since Stroika v3.0d1 - use IPotentiallyComparer ")]] constexpr bool IsPotentiallyComparerRelation ()
    {
        return IPotentiallyComparer<FUNCTOR, FUNCTOR_ARG>;
    }
    template <typename FUNCTOR>
    [[deprecated ("Since Stroika v3.0d1 - use IPotentiallyComparer ")]] constexpr bool IsPotentiallyComparerRelation ()
    {
        if constexpr (Configuration::FunctionTraits<FUNCTOR>::kArity == 2) {
            using TRAITS = typename Configuration::FunctionTraits<FUNCTOR>;
            return is_same_v<typename TRAITS::template arg<0>::type, typename TRAITS::template arg<1>::type> and
                   IsPotentiallyComparerRelation<FUNCTOR, typename Configuration::FunctionTraits<FUNCTOR>::template arg<0>::type> ();
        }
        else {
            return false;
        }
    }
    template <typename FUNCTOR>
    [[deprecated ("Since Stroika v3.0d1 - use IPotentiallyComparer ")]] constexpr bool IsPotentiallyComparerRelation (const FUNCTOR& f)
    {
        return IsPotentiallyComparerRelation<FUNCTOR> ();
    }
    template <typename LT, typename RT>
    [[deprecated ("Since Stroika 3.0d1 - use compare_three_way{} or <=>")]] constexpr strong_ordering ThreeWayCompare (LT&& lhs, RT&& rhs)
    {
        return compare_three_way{}(forward<LT> (lhs), forward<RT> (rhs));
    }

    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IEqualsComparer")]] constexpr bool IsEqualsComparer ()
    {
        return ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eEquals;
    }
    template <typename COMPARER, typename ARG_T>
    [[deprecated ("Since Stroika 3.0d1 - use IEqualsComparer")]] constexpr bool IsEqualsComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eEquals;
        }
    }
    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IEqualsComparer")]] constexpr bool IsEqualsComparer (const COMPARER&)
    {
        return IsEqualsComparer<COMPARER> ();
    }
    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IInOrderComparer")]] constexpr bool IsStrictInOrderComparer ()
    {
        return ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eStrictInOrder;
    }
    template <typename COMPARER, typename ARG_T>
    [[deprecated ("Since Stroika 3.0d1 - use IInOrderComparer")]] constexpr bool IsStrictInOrderComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eStrictInOrder;
        }
    }
    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IInOrderComparer")]] constexpr bool IsStrictInOrderComparer (const COMPARER&)
    {
        return ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eStrictInOrder;
    }

    template <typename COMPARE_FUNCTION>
    using ExtractComparisonTraits [[deprecated ("Since Stroika v3.0d1 - use ExtractComparisonTraits_v instead")]] =
        Private_::ExtractComparisonTraits_<COMPARE_FUNCTION>;
}
