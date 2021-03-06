/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        return ThreeWayComparer<LT, RT>{}(forward<LT> (lhs), forward<RT> (rhs));
    }

    /*
     ********************************************************************************
     *************** OptionalThreeWayCompare<T, TCOMPARER> **************************
     ********************************************************************************
     */
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
    namespace PRIVATE_ {
        template <typename FUNCTOR_ARG, typename FUNCTOR, typename RESULT = invoke_result_t<FUNCTOR, FUNCTOR_ARG, FUNCTOR_ARG>>
        constexpr bool IsPotentiallyComparerRelation_Helper_ (nullptr_t)
        {
            return Configuration::is_callable<FUNCTOR>::value and is_convertible_v<RESULT, bool>;
        }
        template <typename FUNCTOR_ARG, typename FUNCTOR>
        constexpr bool IsPotentiallyComparerRelation_Helper_ (...)
        {
            return false;
        }
    }
    template <typename FUNCTOR_ARG, typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation ()
    {
        return PRIVATE_::IsPotentiallyComparerRelation_Helper_<FUNCTOR_ARG, FUNCTOR> (nullptr);
    }
    template <typename FUNCTOR_ARG, typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation (const FUNCTOR&)
    {
        return IsPotentiallyComparerRelation<FUNCTOR_ARG, FUNCTOR> ();
    }

    /*
     ********************************************************************************
     ********************* IsEqualsComparer<COMPARER> *******************************
     ********************************************************************************
     */
    template <typename COMPARER>
    constexpr bool IsEqualsComparer ()
    {
        return ExtractComparisonTraits<COMPARER>::kComparisonRelationKind == ComparisonRelationType::eEquals;
    }
    template <typename COMPARER>
    constexpr bool IsEqualsComparer (const COMPARER&)
    {
        return ExtractComparisonTraits<COMPARER>::kComparisonRelationKind == ComparisonRelationType::eEquals;
    }

    /*
     ********************************************************************************
     ********************* IsStrictInOrderComparer<COMPARER> ************************
     ********************************************************************************
     */
    template <typename COMPARER>
    constexpr bool IsStrictInOrderComparer ()
    {
        return ExtractComparisonTraits<COMPARER>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
    }
    template <typename COMPARER>
    constexpr bool IsStrictInOrderComparer (const COMPARER&)
    {
        return ExtractComparisonTraits<COMPARER>::kComparisonRelationKind == ComparisonRelationType::eStrictInOrder;
    }

    /*
     ********************************************************************************
     ********** ComparisonRelationDeclaration<TYPE, ACTUAL_COMPARER> ****************
     ********************************************************************************
     */
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    constexpr ComparisonRelationType ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::kComparisonRelationKind;
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
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (const FUNCTOR& f)
    {
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR>{f};
    }
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (FUNCTOR&& f)
    {
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR>{move (f)};
    }

    /*
     ********************************************************************************
     ********************************* DeclareInOrderComparer ***********************
     ********************************************************************************
     */
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR> DeclareInOrderComparer (const FUNCTOR& f)
    {
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR>{f};
    }
    template <typename FUNCTOR>
    constexpr inline Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR> DeclareInOrderComparer (FUNCTOR&& f)
    {
        return Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR>{move (f)};
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
