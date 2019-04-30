/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include "../StroikaPreComp.h"

#include <functional>
#include <memory>
#include <optional>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/Enumeration.h"
#include "../Configuration/TypeHints.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Common {

    namespace Private_ {
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (Comparer, (typename X::Comparer{}(x, x)));
    }

    /**
     *  Stand-in until C++20, three way compare - used for Calling three-way-comparer
     *
     *  Not every class implements the three-way comparer (especailly before C++20).
     *  @see ThreeWayComparerDefaultImplementation<>
     */
    template <typename T>
    struct ThreeWayComparer {
        constexpr ThreeWayComparer () = default;
        template <typename Q = T, enable_if_t<Private_::has_Comparer<Q>::value, char>* = nullptr>
        constexpr int operator() (const T& lhs, const T& rhs) const;
        template <typename Q = T, enable_if_t<not Private_::has_Comparer<Q>::value, short>* = nullptr>
        constexpr int operator() (const T& lhs, const T& rhs) const;
    };

    /**
     *  Stand-in until C++20, three way compare - used for implementing three-way-comparer
     *
     *  @see ThreeWayComparer<>
     */
    template <typename T>
    struct ThreeWayComparerDefaultImplementation {
        constexpr ThreeWayComparerDefaultImplementation () = default;
        constexpr int operator() (const T& lhs, const T& rhs) const;
    };

    /**
     *  EXPERIMENTAL - API subject to change - see if I can find a way to mix  with (partial specialization) of ThreeWayCompare but still pass in other base comparer (to say case insensitive)
     */
    template <typename T, typename TCOMPARER = ThreeWayComparer<T>>
    struct OptionalThreeWayCompare {
        constexpr OptionalThreeWayCompare (const TCOMPARER& tComparer);
        constexpr int operator() (const optional<T>& lhs, const optional<T>& rhs) const;
        TCOMPARER     fTComparer_;
    };

    /**
     *  \par Example Usage
     *      \code
     *        return Common::ThreeWayCompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
     *      \endcode
     */
    template <typename TYPE>
    constexpr int ThreeWayCompareNormalizer (TYPE lhs, TYPE rhs);

    /**
     *  \brief return true if argument is a function like object (callable) taking 2 arguments (FUNCTOR_ARG) and
     *         returning a bool or integer.
     *
     *  \note - this just checks if its a callable (not necessarily valid argument to ExtractComparisonTraits). Its just used to filter which templates get into the overload set.
     */
    template <typename FUNCTOR_ARG, typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation ();
    template <typename FUNCTOR_ARG, typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation (const FUNCTOR&);

    /**
     *
     *  Comparison logic:
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
     *  c++ also appears to support this three-way-compare approach rarely (eg. string<>::compare).
     *
     *  The biggest problem with how std-c++ supports these comparison operators, is that typeid(equal_to<int>) is
     *  essentially unrelated to typeid(equal_to<char>). There is no 'tag' (as with bidirectional iterators etc) to
     *  identify different classes of comparison and so no easy to to leverage the natural relationships between
     *  equal_to and less_equal.
     *
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
        eThreeWayCompare,

        Stroika_Define_Enum_Bounds (eEquals, eThreeWayCompare)
    };

    /**
     *  \brief ExtractComparisonTraits<> extracts the @ComparisonRelationType for the given argument comparer. For common builtin types this is known with no user effort. For user-defined comparers, this will need to be declared (e.g. via ComparisonRelationDeclaration)
     *
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
    struct ExtractComparisonTraits<ThreeWayComparer<T>> {
        static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
    };

    /**
     *  \brief Checks (via ExtractComparisonTraits) if argument is an Equals comparer - one that takes two arguments of type T, and returns a bool, and compares
     *         if one of the items equal to the other (e.g. std::equals).
     *
     *  \note @see ComparisonRelationDeclaration<> to construct an Equals comparer from an arbitrary std::function...
     */
    template <typename COMPARER>
    constexpr bool IsEqualsComparer ();
    template <typename COMPARER>
    constexpr bool IsEqualsComparer (const COMPARER&);

    /**
     *  \brief Checks (via ExtractComparisonTraits) if argument is a StictInOrder comparer - one that takes two arguments of type T, and returns a bool, and compares
     *         if one of the items is STRICTLY in-order with respect to the other - e.g. std::less, or std::greater, but but notably NOT std::equal_to, or std::less_equal.
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
     *          using EqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(T, T)>>;
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
        static constexpr ComparisonRelationType kComparisonRelationKind = KIND; // accessed by ExtractComparisonTraits<>
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
        static constexpr ComparisonRelationType kComparisonRelationKind = KIND; // accessed by ExtractComparisonTraits<>
    };

    /**
     *  \brief  DeclareEqualsComparer () marks a FUNCTOR (lambda or not) as being a FUNCTOR which compares for equality
     *
     *  DeclareEqualsComparer is a trivial wrapper on ComparisonRelationDeclaration, but takes advantage of the fact that you
     *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
     *
     *  @see DeclareInOrderComparer
     *  @see mkEqualsComparerAdapter
     *
     *  \note similar to mkInOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
     *        Whereas mkInOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
     */
    template <typename FUNCTOR>
    constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (const FUNCTOR& f);
    template <typename FUNCTOR>
    constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (FUNCTOR&& f);

    /**
     *  \brief  DeclareInOrderComparer () marks a FUNCTOR (lambda or not) as being a FUNCTOR which compares for in-order
     * 
     *  DeclareInOrderComparer is a trivial wrapper on ComparisonRelationDeclaration, but takes advantage of the fact that you
     *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
     *
     *  @see DeclareEqualsComparer
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
     *  \brief Use this to wrap any basic comparer, and produce an Equals comparer.
     *
     *  This is done by querying the 'type' of the baseComparer with @see ExtractComparisonTraits, and mapping the logic accordingly.
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Common_Compare_h_*/
