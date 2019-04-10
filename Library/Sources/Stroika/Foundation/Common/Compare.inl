/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
     ***************************** ThreeWayCompare<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr int ThreeWayCompare<T>::operator() (const T& lhs, const T& rhs) const
    {
        // in general, can do this much more efficiently (subtract left and right), but for now, KISS
        if (equal_to<T>{}(lhs, rhs)) {
            return 0;
        }
        return less<T>{}(lhs, rhs) ? -1 : 1;
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
    constexpr int OptionalThreeWayCompare<T, TCOMPARER>::operator() (const optional<T>& lhs, const optional<T>& rhs) const
    {
        if (lhs and rhs) {
            return fTComparer_ (*lhs, *rhs);
        }
        if (not lhs and not rhs) {
            return 0;
        }
        // treat missing as less than present
        if (lhs) {
            return 1;
        }
        else {
            return -1;
        }
    }

    /*
     ********************************************************************************
     ************************ ThreeWayCompareNormalizer *****************************
     ********************************************************************************
     */
    namespace PRIVATE_ {
        template <typename TYPE, enable_if_t<is_arithmetic_v<TYPE>>*>
        constexpr int ThreeWayCompareNormalizer_ (TYPE lhs, TYPE rhs, void*)
        {
            return lhs - rhs;
        }
        template <typename TYPE>
        constexpr int ThreeWayCompareNormalizer_ (TYPE lhs, TYPE rhs, ...)
        {
            if (lhs < rhs) {
                return -1;
            }
            else if (lhs == rhs) {
                return 0;
            }
            else {
                return 1;
            }
        }
    }
    template <typename TYPE>
    constexpr int ThreeWayCompareNormalizer (TYPE lhs, TYPE rhs)
    {
        return PRIVATE_::ThreeWayCompareNormalizer_ (lhs, rhs, nullptr);
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
        switch (ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind) {
            case ComparisonRelationType::eStrictInOrder:
                return fBASE_COMPARER_ (lhs, rhs);
            case ComparisonRelationType::eInOrderOrEquals:
                return fBASE_COMPARER_ (lhs, rhs) and not fBASE_COMPARER_ (rhs, lhs);
            case ComparisonRelationType::eThreeWayCompare:
                return fBASE_COMPARER_ (lhs, rhs) < 0;
            default:
                AssertNotReached ();
                return false;
        }
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
        switch (ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind) {
            case ComparisonRelationType::eEquals:
                return fBASE_COMPARER_ (lhs, rhs);
            case ComparisonRelationType::eStrictInOrder:
                return not fBASE_COMPARER_ (lhs, rhs) and not fBASE_COMPARER_ (rhs, lhs);
            case ComparisonRelationType::eInOrderOrEquals:
                return fBASE_COMPARER_ (lhs, rhs) and fBASE_COMPARER_ (rhs, lhs);
            case ComparisonRelationType::eThreeWayCompare:
                return fBASE_COMPARER_ (lhs, rhs) == 0;
            default:
                AssertNotReached ();
                return false;
        }
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
    constexpr int ThreeWayComparerAdapter<BASE_COMPARER>::operator() (const T& lhs, const T& rhs) const
    {
        switch (ExtractComparisonTraits<BASE_COMPARER>::kComparisonRelationKind) {
            case ComparisonRelationType::eStrictInOrder:
                return fBASE_COMPARER_ (lhs, rhs) ? -1 : (fBASE_COMPARER_ (rhs, lhs) ? 1 : 0);
            case ComparisonRelationType::eThreeWayCompare:
                return fBASE_COMPARER_ (lhs, rhs);
            default:
                AssertNotReached ();
                return false;
        }
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
