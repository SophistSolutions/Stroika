/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include "../StroikaPreComp.h"

#include <functional>
#include <memory>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"

/**
 *  NEW (as of 2.0a231) Comparison logic
 *
 *  \par Total Ordering (http://mathworld.wolfram.com/TotallyOrderedSet.html)
 *          o   Reflexivity: a <= a
 *          o   Antisymmetry:  a <= b and b <= a implies a=b
 *          o   Transitivity: a <= b and b <= c implies a <= c
 *          o   Comparability: either a <= b or b <= a
 *
 *  Our comparisons ALL require Comparability; and all the <,<=,>,>= comparisons require Transitivity
 *
 *  Types of Comparers:
 *      <, <=, ==, etc, are functions from SxS => bool. They are also called relations (where the
 *      relationship need not be comparable). But in our domain, they are all comparable, so all comparers
 *      are function<bool(T,T)> - at least logically.
 *
 *      Most of Stroika's containers concern themselves with == testing. The value of == can always be derived from
 *      !=, <=, <, >, >=, though not necessarily very efficiently (the greater/less than ones translate one call to two).
 *
 *      A frequently more efficient strategy is compare(T,T)-> int (<0 is <, ==0, means equals, >0 means >).
 *      This strategy was widely used in the "C" world (e.g. strcmp, quicksort, etc).
 *
 *      It also appears to be making a comeback for C++20 (http://open-std.org/JTC1/SC22/WG21/docs/papers/2017/p0515r0.pdf)
 *
 *  std-c++ favors a handful of these predefined functions
 *      >   less
 *      >   equal_to ( to a lesser degree)
 *  but also provides
 *      >   greater
 *      >   less_equal
 *      >   greater_equal
 *      >   not_equal_to
 *
 *  c++ also appears to support this funtion<int(T,T)> approach rarely (eg. string<>::compare).
 *
 *  The biggest problem with how std-c++ supports these comparison operators, is that typeid(equal_to<int>) is
 *  essentially unrelated to typeid(equal_to<char>). There is no 'tag' (as with bidirectional iterators etc) to
 *  identify different classes of comparison and so no easy to to leverage the natural relationships between
 *  equal_to and less_equal.
 *
 * TODO:
 *
 *      @todo   use http://en.cppreference.com/w/cpp/types/enable_if to make this work better
 *              So for example - we want something like:
 *                  enable_if<is_integral<T>> int compare (T, T) { return v1 - v2; }
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Common {

            /**
             *  \par Example Usage
             *      \code
             *        return Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
             *      \endcode
             */
            template <typename INTEGERLIKETYPE>
            constexpr int CompareNormalizer (INTEGERLIKETYPE lhs, INTEGERLIKETYPE rhs);
            // @todo more specializations
            template <>
            constexpr int CompareNormalizer (int lhs, int rhs);

            /**
             *  Stand-in until C++20, three way compare
             */
            template <typename T>
            struct ThreeWayCompare {
                constexpr ThreeWayCompare () = default;
                constexpr int operator() (const T& lhs, const T& rhs) const;
            };

            /**
             *  THIS is what drives how we do containers/related algorithms (less is equiv to greater for most of them)
             */
            enum class ComparisonRelationType {
                eEquals,

                /**
                 *   e.g. less<T>, or greater<T>
                 *
                 *  From http://mathworld.wolfram.com/StrictOrder.html
                 *      A relation < is a strict order on a set S if it is
                 *          1. Irreflexive: a<a does not hold for any a in S.
                 *          2. Asymmetric: if a<b, then b<a does not hold.
                 *          3. Transitive: a<b and b<c implies a<c.
                 */
                eStrictInOrder,

                /**
                 *  \brief <=, or >=, e.g. less_equal<T>, or greater_equal<T>
                 *
                 *   \note considered the name 'partial' order here but that could be confusing, since partial order frequently
                 *         refers to not covering the entire domain - not less vs. less_equal.
                 */
                eInOrderOrEquals,

                /**
                 *   e.g. function<int(T,T)> - where < 0 return is 'in order' (eStrictInOrder), 0 means equal, and > 0 means reversed order
                 */
                eThreeWayCompare
            };

            /**
             *  This is ONLY defined for builtin c++ comparison objects, though your code can define it however you wish for
             *  specific user-defined types using ComparisonRelationDeclaration<>.
             */
            template <typename COMPARE_FUNCTION>
            struct ExtractComparisonTraits {
                static constexpr ComparisonRelationType kComparisonRelationKind = COMPARE_FUNCTION::kComparisonRelationKind;
            };
            template <typename T>
            struct ExtractComparisonTraits<equal_to<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eEquals;
            };
            template <typename T>
            struct ExtractComparisonTraits<less<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
            };
            template <typename T>
            struct ExtractComparisonTraits<greater<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
            };
            template <typename T>
            struct ExtractComparisonTraits<less_equal<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
            };
            template <typename T>
            struct ExtractComparisonTraits<greater_equal<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
            };
            template <typename T>
            struct ExtractComparisonTraits<ThreeWayCompare<T>> {
                static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
            };

            /**
             *  \brief An Equals comparer is one that takes two arguments of type T, and returns a bool, and compares
             *         if one of the items equal to the other (e.g. std::equals).
             *
             *  \note @see ComparisonRelationDeclaration<> to construct an Equals comparer from an arbitrary std::function...
             */
            template <typename COMPARER>
            constexpr bool IsEqualsComparer ();
            template <typename COMPARER>
            constexpr bool IsEqualsComparer (const COMPARER&);

            /**
             *  \brief An InOrder comparer is one that takes two arguments of type T, and returns a bool, and compares
             *         if one of the items is STRICTLY in-order with respect to the other - e.g. std::less, or std::greater, but
             *         but notably NOT std::equal_to, or std::less_equal.
             *
             *  \note @see ComparisonRelationDeclaration<> to construct an InOrder comparer from an arbitrary std::function...
             */
            template <typename COMPARER>
            constexpr bool IsStrictInOrderComparer ();
            template <typename COMPARER>
            constexpr bool IsStrictInOrderComparer (const COMPARER&);

            /**
             *  Utility class to serve as base class when constructing user-defined 'function' object comparer so ExtractComparisonTraits<> knows
             *  the type, or (with just one argument) as base for class that itself provives the operator() method.
             *
             *  \par Example Usage
             *      \code
             *          using EqualityComparerType = Common::ComparisonRelationDeclaration<function<bool(T, T)>, Common::ComparisonRelationType::eEquals>;
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          struct String::EqualToCI : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
             *              nonvirtual bool operator() (const String& lhs, const String& rhs) const;
             *           };
             *      \endcode
             */
            template <ComparisonRelationType KIND, typename ACTUAL_COMPARER = void>
            struct ComparisonRelationDeclaration {
                static constexpr ComparisonRelationType kComparisonRelationKind = KIND; // default - so user-defined types can do this to automatically define their Comparison Traits
                ACTUAL_COMPARER                         fActualComparer;

                /**
                 */
                constexpr ComparisonRelationDeclaration (const ACTUAL_COMPARER& actualComparer);
                constexpr ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer);

                /**
                 */
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const;
            };
            template <ComparisonRelationType KIND>
            struct ComparisonRelationDeclaration<KIND, void> {
                static constexpr ComparisonRelationType kComparisonRelationKind = KIND; // default - so user-defined types can do this to automatically define their Comparison Traits
            };

            /*
             *  \brief  DeclareEqualsComparer () marks a FUNCTOR (lambda or not) as being a FUNCTOR which compares for equality
             *
             *  DeclareEqualsComparer is a trivial wrapper on ComparisonRelationDeclaration, but takes advantage of the fact that you
             *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
             *
             *  @see mkInOrderComparerAdapter
             *
             *  \note similar to mkInOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
             *        Whereas mkInOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
             */
            template <typename FUNCTOR>
            constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (const FUNCTOR& f);
            template <typename FUNCTOR>
            constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (FUNCTOR&& f);

            /*
             *  \brief  DeclareInOrderComparer () marks a FUNCTOR (lambda or not) as being a FUNCTOR which compares for in-order
             * 
             *  mkInOrderComparer is a trivial wrapper on ComparisonRelationDeclaration, but takes advantage of the fact that you
             *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
             *
             *  @see mkInOrderComparerAdapter
             *
             *  \note similar to mkInOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
             *        Whereas mkInOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
             */
            template <typename FUNCTOR>
            constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR> DeclareInOrderComparer (const FUNCTOR& f);
            template <typename FUNCTOR>
            constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, FUNCTOR> DeclareInOrderComparer (FUNCTOR&& f);

            /**
             *  \brief Use this to wrap any basic comparer, and produce a Less comparer
             */
            template <typename BASE_COMPARER>
            struct InOrderComparerAdapter {
                /**
                 */
                constexpr InOrderComparerAdapter (const BASE_COMPARER& baseComparer);
                constexpr InOrderComparerAdapter (BASE_COMPARER&& baseComparer);

                /**
                 */
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const;

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *  mkInOrderComparerAdapter is a trivial wrapper on InOrderComparerAdapter, but takes advantage of the fact that you
             *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
             *
             *  @see DeclareInOrderComparer
             *
             *  \note similar to DeclareInOrderComparer(), except DeclareInOrderComparer ignores the TYEP of 'f' and just marks it as an InOrder comparer
             *        Whereas this function looks at the type of 'f' and does the appropriate mapping logic.
             */
            template <typename BASE_COMPARER>
            constexpr auto mkInOrderComparerAdapter (const BASE_COMPARER& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>;
            template <typename BASE_COMPARER>
            constexpr auto mkInOrderComparerAdapter (BASE_COMPARER&& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>;

            /**
             *  \brief Use this to wrap any basic comparer, and produce an Equals comparer
             */
            template <typename BASE_COMPARER>
            struct EqualsComparerAdapter {
                /**
                 */
                constexpr EqualsComparerAdapter (const BASE_COMPARER& baseComparer);
                constexpr EqualsComparerAdapter (BASE_COMPARER&& baseComparer);

                /**
                 */
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const;

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *  mkEqualsComparerAdapter is a trivial wrapper on EqualsComparerAdapter, but takes advantage of the fact that you
             *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
             */
            template <typename BASE_COMPARER>
            constexpr auto mkEqualsComparerAdapter (const BASE_COMPARER& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>;
            template <typename BASE_COMPARER>
            constexpr auto mkEqualsComparerAdapter (BASE_COMPARER&& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>;

            /**
             *  \brief Use this to wrap any basic comparer, and produce a Three-Way comparer
             */
            template <typename BASE_COMPARER>
            struct ThreeWayComparerAdapter {
                /**
                 */
                constexpr ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer);
                constexpr ThreeWayComparerAdapter (BASE_COMPARER&& baseComparer);

                /**
                 */
                template <typename T>
                constexpr int operator() (const T& lhs, const T& rhs) const;

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *  mkThreeWayComparerAdapter is a trivial wrapper on ThreeWayComparerAdapter, but takes advantage of the fact that you
             *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
             */
            template <typename BASE_COMPARER>
            constexpr auto mkThreeWayComparerAdapter (const BASE_COMPARER& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>;
            template <typename BASE_COMPARER>
            constexpr auto mkThreeWayComparerAdapter (BASE_COMPARER&& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>;
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Common_Compare_h_*/
