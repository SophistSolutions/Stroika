/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
     ***************************** ThreeWayComparer<T> ******************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS>
    constexpr ThreeWayComparer<T, ARGS...>::ThreeWayComparer (ARGS... args)
        : fArgs_ (make_tuple (args...))
    {
    }
    template <typename T, typename... ARGS>
    template <typename Q, enable_if_t<Private_::HasThreeWayComparer_v<Q>>*>
    constexpr strong_ordering ThreeWayComparer<T, ARGS...>::operator() (const T& lhs, const T& rhs) const
    {
#if qCompilerAndStdLib_make_from_tuple_Buggy
        if constexpr (tuple_size_v<decltype (fArgs_)> == 0) {
            return typename Q::ThreeWayComparer{}(lhs, rhs);
        }
        else {
            return make_from_tuple<typename Q::ThreeWayComparer> (fArgs_) (lhs, rhs);
        }
#else
        return make_from_tuple<typename Q::ThreeWayComparer> (fArgs_) (lhs, rhs);
#endif
    }
    template <typename T, typename... ARGS>
    template <typename Q, enable_if_t<Private_::HasThreeWayComparerTemplate_v<Q>>*>
    constexpr strong_ordering ThreeWayComparer<T, ARGS...>::operator() (const T& lhs, const T& rhs) const
    {
#if qCompilerAndStdLib_make_from_tuple_Buggy
        if constexpr (tuple_size_v<decltype (fArgs_)> == 0) {
            return typename Q::template ThreeWayComparer<>{}(lhs, rhs);
        }
        else {
            return make_from_tuple<typename Q::template ThreeWayComparer<>> (fArgs_) (lhs, rhs);
        }
#else
        return make_from_tuple<typename Q::template ThreeWayComparer<>> (fArgs_) (lhs, rhs);
#endif
    }
    template <typename T, typename... ARGS>
    template <typename Q, enable_if_t<not Private_::HasThreeWayComparer_v<Q> and not Private_::HasThreeWayComparerTemplate_v<Q>>*>
    constexpr strong_ordering ThreeWayComparer<T, ARGS...>::operator() (const T& lhs, const T& rhs) const
    {
        return ThreeWayComparerDefaultImplementation<T>{}(lhs, rhs);
    }

    /*
     ********************************************************************************
     ********************* ThreeWayComparerDefaultImplementation<T> *****************
     ********************************************************************************
     */
    template <typename T>
    constexpr strong_ordering ThreeWayComparerDefaultImplementation<T>::operator() (const T& lhs, const T& rhs) const
    {
#if __cpp_lib_three_way_comparison >= 201907L
        return compare_three_way{}(lhs, rhs);
#else
        // in general, can do this much more efficiently (subtract left and right), but for now, KISS
        if (equal_to<T>{}(lhs, rhs)) {
            return kEqual;
        }
        return less<T>{}(lhs, rhs) ? kLess : kGreater;
#endif
    }

    /*
     ********************************************************************************
     *************************** ThreeWayCompare<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Common::strong_ordering ThreeWayCompare (const T& lhs, const T& rhs)
    {
        return ThreeWayComparer<T>{}(lhs, rhs);
    }

    /*
     ********************************************************************************
     *************** OptionalThreeWayCompare<T, TCOMPARER> **************************
     ********************************************************************************
     */
    template <typename T, typename TCOMPARER>
    constexpr OptionalThreeWayCompare<T, TCOMPARER>::OptionalThreeWayCompare (const TCOMPARER& comparer)
        : fTComparer_ (comparer)
    {
    }
    template <typename T, typename TCOMPARER>
    constexpr strong_ordering OptionalThreeWayCompare<T, TCOMPARER>::operator() (const optional<T>& lhs, const optional<T>& rhs) const
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
     ************************ ThreeWayCompareNormalizer *****************************
     ********************************************************************************
     */
    namespace PRIVATE_ {
        template <typename TYPE, enable_if_t<is_arithmetic_v<TYPE>>*>
        constexpr strong_ordering ThreeWayCompareNormalizer_ (TYPE lhs, TYPE rhs, void*)
        {
            return strong_ordering{lhs - rhs};
        }
        template <typename TYPE>
        constexpr strong_ordering ThreeWayCompareNormalizer_ (TYPE lhs, TYPE rhs, ...)
        {
            if (lhs < rhs) {
                return kLess;
            }
            else if (lhs == rhs) {
                return kEqual;
            }
            else {
                return kGreater;
            }
        }
    }
    template <typename TYPE>
    constexpr strong_ordering ThreeWayCompareNormalizer (TYPE lhs, TYPE rhs)
    {
        return PRIVATE_::ThreeWayCompareNormalizer_ (lhs, rhs, nullptr);
    }

    /*
     ********************************************************************************
     *************************** CompareResultNormalizeHelper ***********************
     ********************************************************************************
     */
    template <typename FROM_INT_TYPE>
    inline strong_ordering CompareResultNormalizeHelper (FROM_INT_TYPE f)
    {
        if (f < 0) {
            return Common::kLess;
        }
        else if (f > 0) {
            return Common::kGreater;
        }
        else {
            return Common::kEqual;
        }
    }

    /*
     ********************************************************************************
     ****************** IsPotentiallyComparerRelation<FUNCTOR> **********************
     ********************************************************************************
     */
    namespace PRIVATE_ {
        template <typename FUNCTOR_ARG, typename FUNCTOR, typename RESULT = result_of_t<FUNCTOR (FUNCTOR_ARG, FUNCTOR_ARG)>>
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
        : fActualComparer (actualComparer)
    {
    }
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    inline constexpr ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer)
        : fActualComparer (move (actualComparer))
    {
    }
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
    template <typename T>
    inline constexpr bool ComparisonRelationDeclaration<KIND, ACTUAL_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        return fActualComparer (lhs, rhs);
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
        : fBASE_COMPARER_ (baseComparer)
    {
    }
    template <typename BASE_COMPARER>
    constexpr inline InOrderComparerAdapter<BASE_COMPARER>::InOrderComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_ (move (baseComparer))
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr inline bool InOrderComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
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
     ********************* mkInOrderComparerAdapter<BASE_COMPARER> ******************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    inline constexpr auto mkInOrderComparerAdapter (const BASE_COMPARER& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>
    {
        return InOrderComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    inline constexpr auto mkInOrderComparerAdapter (BASE_COMPARER&& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>
    {
        return InOrderComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }

    /*
     ********************************************************************************
     ********************* EqualsComparerAdapter<BASE_COMPARER> *********************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_ (baseComparer)
    {
    }
    template <typename BASE_COMPARER>
    constexpr EqualsComparerAdapter<BASE_COMPARER>::EqualsComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_ (move (baseComparer))
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr bool EqualsComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        /*
         *  It would be nice to be able to write this as a switch statement, but some expressions in some cases would not
         *  compile.
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
     ********************* mkEqualsComparerAdapter<BASE_COMPARER> *******************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    inline constexpr auto mkEqualsComparerAdapter (const BASE_COMPARER& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>
    {
        return EqualsComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    inline constexpr auto mkEqualsComparerAdapter (BASE_COMPARER&& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>
    {
        return EqualsComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }

    /*
     ********************************************************************************
     ********************* ThreeWayComparerAdapter<BASE_COMPARER> *******************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer)
        : fBASE_COMPARER_ (baseComparer)
    {
    }
    template <typename BASE_COMPARER>
    constexpr ThreeWayComparerAdapter<BASE_COMPARER>::ThreeWayComparerAdapter (BASE_COMPARER&& baseComparer)
        : fBASE_COMPARER_ (move (baseComparer))
    {
    }
    template <typename BASE_COMPARER>
    template <typename T>
    constexpr strong_ordering ThreeWayComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
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

    /*
     ********************************************************************************
     ********************* mkThreeWayComparerAdapter<BASE_COMPARER> *****************
     ********************************************************************************
     */
    template <typename BASE_COMPARER>
    inline constexpr auto mkThreeWayComparerAdapter (const BASE_COMPARER& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>
    {
        return ThreeWayComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    inline constexpr auto mkThreeWayComparerAdapter (BASE_COMPARER&& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>
    {
        return ThreeWayComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }

}

#endif /*_Stroika_Foundation_Common_Compare_inl_*/
