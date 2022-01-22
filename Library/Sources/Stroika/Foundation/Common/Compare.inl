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
     ************************* ThreeWayCompare<LT, RT> ******************************
     ********************************************************************************
     */
    template <typename LT, typename RT>
    constexpr Common::strong_ordering ThreeWayCompare (LT&& lhs, RT&& rhs)
    {
        return ThreeWayComparer{}(forward<LT> (lhs), forward<RT> (rhs));
    }

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
            return kEqual;
        }
        // treat missing as less than present
        if (lhs) {
            return kGreater;
        }
        else {
            return kLess;
        }
    }

    /*
     ********************************************************************************
     *************************** CompareResultNormalizer ****************************
     ********************************************************************************
     */
    template <typename FROM_INT_TYPE>
    inline strong_ordering CompareResultNormalizer (FROM_INT_TYPE f)
    {
        if (f == 0) {
            return Common::kEqual;
        }
        else {
            Assert (f < 0 or f > 0);
            return f < 0 ? Common::kLess : Common::kGreater;
        }
    }

    /*
     ********************************************************************************
     ****************** IsPotentiallyComparerRelation<FUNCTOR> **********************
     ********************************************************************************
     */
    template <typename FUNCTOR, typename FUNCTOR_ARG>
    constexpr bool IsPotentiallyComparerRelation ()
    {
        if constexpr (is_invocable_v<FUNCTOR, FUNCTOR_ARG, FUNCTOR_ARG>) {
            return std::is_convertible_v<std::invoke_result_t<FUNCTOR, FUNCTOR_ARG, FUNCTOR_ARG>, bool>;
        }
        return false;
    }
    template <typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation ()
    {
        if constexpr (Configuration::function_traits<FUNCTOR>::kArity == 2) {
            using TRAITS = typename Configuration::function_traits<FUNCTOR>;
            return is_same_v<typename TRAITS::template arg<0>::type, typename TRAITS::template arg<1>::type> and
                   IsPotentiallyComparerRelation<FUNCTOR, typename Configuration::function_traits<FUNCTOR>::template arg<0>::type> ();
        }
        else {
            return false;
        }
    }
    template <typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation (const FUNCTOR& f)
    {
        return IsPotentiallyComparerRelation<FUNCTOR> ();
    }

    /*
     ********************************************************************************
     ************************ IsEqualsComparer<COMPARER> ****************************
     ********************************************************************************
     */
    template <typename COMPARER>
    constexpr bool IsEqualsComparer ()
    {
        return ExtractComparisonTraits<std::decay_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eEquals;
    }
    template <typename COMPARER, typename ARG_T>
    constexpr bool IsEqualsComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits<std::decay_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eEquals;
        }
    }
    template <typename COMPARER>
    constexpr bool IsEqualsComparer (const COMPARER&)
    {
        return IsEqualsComparer<COMPARER> ();
    }

    /*
     ********************************************************************************
     ********************* IsStrictInOrderComparer<COMPARER> ************************
     ********************************************************************************
     */
    template <typename COMPARER>
    constexpr bool IsStrictInOrderComparer ()
    {
        return ExtractComparisonTraits<std::decay_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
    }
    template <typename COMPARER, typename ARG_T>
    constexpr bool IsStrictInOrderComparer ()
    {
        if constexpr (not IsPotentiallyComparerRelation<COMPARER, ARG_T> ()) {
            return false;
        }
        else {
            return ExtractComparisonTraits<std::decay_t<COMPARER>>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
        }
    }
    template <typename COMPARER>
    constexpr bool IsStrictInOrderComparer (const COMPARER&)
    {
        return IsStrictInOrderComparer<COMPARER> ();
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
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (FUNCTOR&& f)
    {
        static_assert (IsPotentiallyComparerRelation<FUNCTOR, typename Configuration::function_traits<FUNCTOR>::template arg<0>::type> ());
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR>{std::forward<FUNCTOR> (f)};
    }

    /*
     ********************************************************************************
     ********************************* DeclareInOrderComparer ***********************
     ********************************************************************************
     */
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR> DeclareInOrderComparer (FUNCTOR&& f)
    {
        static_assert (IsPotentiallyComparerRelation<FUNCTOR, typename Configuration::function_traits<FUNCTOR>::template arg<0>::type> ());
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR>{std::forward<FUNCTOR> (f)};
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
            return baseComparison == kEqual;
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
            return baseComparison ? kLess : (fBASE_COMPARER_ (rhs, lhs) ? kGreater : kEqual);
        }
        if constexpr (kRelationKind == ComparisonRelationType::eThreeWayCompare) {
            return baseComparison;
        }
        AssertNotReached ();
        return kEqual;
    }

}

#endif /*_Stroika_Foundation_Common_Compare_inl_*/
