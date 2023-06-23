/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_inl_
#define _Stroika_Foundation_Common_Compare_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     *************** OptionalThreeWayCompare<T, TCOMPARER> **************************
     ********************************************************************************
     */
    template <typename T, typename TCOMPARER>
    constexpr OptionalThreeWayComparer<T, TCOMPARER>::OptionalThreeWayComparer (TCOMPARER&& comparer)
        : fTComparer_{move (comparer)}
    {
    }
    template <typename T, typename TCOMPARER>
    constexpr OptionalThreeWayComparer<T, TCOMPARER>::OptionalThreeWayComparer (const TCOMPARER& comparer)
        : fTComparer_{comparer}
    {
    }
    template <typename T, typename TCOMPARER>
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
     ********** ComparisonRelationDeclaration<TYPE, ACTUAL_COMPARER> ****************
     ********************************************************************************
     */
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    inline constexpr ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::ComparisonRelationDeclaration (const ACTUAL_COMPARER& actualComparer)
        : fActualComparer{actualComparer}
    {
    }
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    inline constexpr ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer)
        : fActualComparer{move (actualComparer)}
    {
    }
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    template <typename LT, typename RT>
    inline constexpr bool ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::operator() (LT&& lhs, RT&& rhs) const
    {
        return fActualComparer (forward<LT> (lhs), forward<RT> (rhs));
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
     ********************************* DeclareInOrderComparer ***********************
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
     ********************* InOrderComparerAdapter<BASE_COMPARER> ********************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    constexpr inline InOrderComparerAdapter<BASE_COMPARER>::InOrderComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <typename BASE_COMPARER>
    constexpr inline InOrderComparerAdapter<BASE_COMPARER>::InOrderComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr inline bool InOrderComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind;
        auto           baseComparison = fBASE_COMPARER_ (lhs, rhs);
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            return kRelationKind;
        }
        if constexpr (kRelationKind == ComparisonRelationType::eInOrderOrEquals) {
            return baseComparison and not fBASE_COMPARER_ (rhs, lhs);
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison < 0;
        }
        AssertNotReached ();
        return false;
    }

    /*
     ********************************************************************************
     ********************* EqualsComparerAdapter<BASE_COMPARER> *********************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <typename BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr bool EqualsComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind;
        auto           baseComparison = fBASE_COMPARER_ (lhs, rhs);
        if constexpr (kRelationKind == ComparisonRelationType::eEquals) {
            return baseComparison;
        }
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            return not baseComparison and not fBASE_COMPARER_ (rhs, lhs);
        }
        if constexpr (kRelationKind == ComparisonRelationType::eInOrderOrEquals) {
            return baseComparison and fBASE_COMPARER_ (rhs, lhs);
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison == strong_ordering::equal;
        }
        AssertNotReached ();
        return false;
    }

    /*
     ********************************************************************************
     ********************* ThreeWayComparerAdapter<BASE_COMPARER> *******************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_{baseComparer}
    {
    }
    template <typename BASE_COMPARER>
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_{move (baseComparer)}
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr strong_ordering ThreeWayComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        /*
         *  It would  be nice to be able to use switch statement but use constexpr if because 
         *  inappropriate 'cases' that wouldn't get executed might not compile -- LGP 2020-05-05
         */
        constexpr auto kRelationKind  = ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind;
        auto           baseComparison = fBASE_COMPARER_ (lhs, rhs);
        if constexpr (kRelationKind == ComparisonRelationType::eStrictInOrder) {
            return baseComparison ? strong_ordering::less : (fBASE_COMPARER_ (rhs, lhs) ? strong_ordering::greater : strong_ordering::equal);
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison;
        }
        AssertNotReached ();
        return strong_ordering::equal;
    }

    /*******DEPRECATED STUFF  */
    template <typename LT, typename RT>
    [[deprecated ("Since Stroika 3.0d1 - use compare_three_way{} or <=>")]] constexpr strong_ordering ThreeWayCompare (LT&& lhs, RT&& rhs)
    {
        return compare_three_way{}(forward<LT> (lhs), forward<RT> (rhs));
    }

    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IEqualsComparer")]] constexpr bool IsEqualsComparer ()
    {
        return ExtractComparisonTraits<std::remove_cvref_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eEquals;
    }
    template <typename COMPARER, typename ARG_T>
    [[deprecated ("Since Stroika 3.0d1 - use IEqualsComparer")]] constexpr bool IsEqualsComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits<std::remove_cvref_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eEquals;
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
        return ExtractComparisonTraits<std::remove_cvref_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
    }
    template <typename COMPARER, typename ARG_T>
    [[deprecated ("Since Stroika 3.0d1 - use IInOrderComparer")]] constexpr bool IsStrictInOrderComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits<std::remove_cvref_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
        }
    }
    template <typename COMPARER>
    [[deprecated ("Since Stroika 3.0d1 - use IInOrderComparer")]] constexpr bool IsStrictInOrderComparer (const COMPARER&)
    {
        return ExtractComparisonTraits<std::remove_cvref_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
    }

}

#endif /*_Stroika_Foundation_Common_Compare_inl_*/
