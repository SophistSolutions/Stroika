/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/Enumeration.h"
#include "../Configuration/TypeHints.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

// Quirky workaround for clang++-14 on XCode 14 (and probably others).
// No bug define for now - specific to clang++, because not sure what it depends on besides this, and this is bad enuf...
// this value being too low...
// This is PROBABLY an issue with the LIBC++ STD LIBRARY, and not CLANG COMPILER, BUT VERIFY THIS....
#if qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
namespace std {
    struct compare_three_way {
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
}
#endif

namespace Stroika::Foundation::Common {

    constexpr std::strong_ordering kLess [[deprecated ("Since Stroika 3.0d1 - use std::strong_ordering")]]    = std::strong_ordering::less;
    constexpr std::strong_ordering kEqual [[deprecated ("Since Stroika 3.0d1 - use std::strong_ordering")]]   = std::strong_ordering::equal;
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

    /**
     *  \brief ThreeWayComparer for optional types, like builtin one, except this lets you pass in explciit 'T' comparer for the T in optional<T>
     *
     *  You dont need this when the default comparer for 'T' works as you wish. But for example, ThreeWayComparer<optional<String>> - where you want
     *  to use a case insensitive comparer for the strings, is tricky. THIS class solves that, by letting you pass in explicitly the 
     *  'base comparer'.
     */
    template <typename T, typename TCOMPARER = std::compare_three_way>
    struct OptionalThreeWayComparer {
        constexpr OptionalThreeWayComparer (TCOMPARER&& tComparer);
        constexpr OptionalThreeWayComparer (const TCOMPARER& tComparer);
        constexpr strong_ordering operator() (const optional<T>& lhs, const optional<T>& rhs) const;

    private:
        [[no_unique_address]] TCOMPARER fTComparer_;
    };

    /**
     * Take the given value and map it to -1, 0, 1 - without any compiler warnings. Handy for 32/64 bit etc codiing when you maybe comparing
     * different sized values and just returning an int, but don't want the warnings about overflow etc.
     */
    template <typename FROM_INT_TYPE>
    strong_ordering CompareResultNormalizer (FROM_INT_TYPE f);

    /**
     *  \brief return true if argument is a function like object (callable) taking 2 arguments (FUNCTOR_ARG) and
     *         returning a bool or integer.
     *
     *  \note - the one-typed version of IsPotentiallyComparerRelation may not be able to deduce the FUNCTOR_ARG of the arguments in all cases.
     *
     *  \note - this just checks if its a callable (not necessarily valid argument to ExtractComparisonTraits). Its just used to filter which templates get into the overload set.
     *
     *  \note Prior to Stroika 2.1b14, this routine took template arguments reversed, but switched to this way because FUNCTOR_ARG can be deduced from FUNCTOR and that can be handy
     * 
     *  \see IsEqualsComparer
     */
    template <typename FUNCTOR, typename FUNCTOR_ARG>
    constexpr bool IsPotentiallyComparerRelation ();
    template <typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation ();
    template <typename FUNCTOR>
    constexpr bool IsPotentiallyComparerRelation (const FUNCTOR& functor);

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
     *      >   equal_to (to a lesser degree)
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
        // @todo fix this with SFINAE (has_XX) so it gives a good explanation
        // static_assert (has_kComparisonRelationKind filed);
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
    template <>
    struct ExtractComparisonTraits<compare_three_way> {
        static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
    };

    /**
     *  \brief Checks (via ExtractComparisonTraits) if argument is an Equals comparer - one that takes two arguments of type T, and returns a bool, and compares
     *         if one of the items equal to the other (e.g. std::equals).
     *
     *  \note @see ComparisonRelationDeclaration<> to construct an Equals comparer from an arbitrary std::function...
     *  \note @see IsStrictInOrderComparer
     *  \note @see IsPotentiallyComparerRelation
     */
    template <typename COMPARER>
    constexpr bool IsEqualsComparer ();
    template <typename COMPARER, typename ARG_T>
    constexpr bool IsEqualsComparer ();
    template <typename COMPARER>
    constexpr bool IsEqualsComparer (const COMPARER&);

    /**
     *  \brief Checks (via ExtractComparisonTraits) if argument is a StictInOrder comparer - one that takes two arguments of type T, and returns a bool, and compares
     *         if one of the items is STRICTLY in-order with respect to the other - e.g. std::less, or std::greater, but but notably NOT std::equal_to, or std::less_equal.
     *
     *  \note @see ComparisonRelationDeclaration<> to construct an InOrder comparer from an arbitrary std::function...
     *  \note @see IsEqualsComparer
     *  \note @see IsPotentiallyComparerRelation
     */
    template <typename COMPARER>
    constexpr bool IsStrictInOrderComparer ();
    template <typename COMPARER, typename ARG_T>
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
     *          struct String::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
     *              nonvirtual bool operator() (const String& lhs, const String& rhs) const;
     *           };
     *      \endcode
     */
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER = void>
    struct ComparisonRelationDeclaration {
        static_assert (not is_reference_v<ACTUAL_COMPARER>);
        static constexpr inline ComparisonRelationType kComparisonRelationKind{KIND}; // accessed by ExtractComparisonTraits<>
        [[no_unique_address]] ACTUAL_COMPARER          fActualComparer;

        /**
         */
        constexpr ComparisonRelationDeclaration () = default;
        constexpr ComparisonRelationDeclaration (const ACTUAL_COMPARER& actualComparer);
        constexpr ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer);
        constexpr ComparisonRelationDeclaration (const ComparisonRelationDeclaration& src) = default;

        /**
         */
        template <typename LT, typename RT>
        constexpr bool operator() (LT&& lhs, RT&& rhs) const;
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
     *  @see EqualsComparerAdapter
     *
     *  \note similar to InOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
     *        Whereas InOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
     */
    template <typename FUNCTOR>
    constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, FUNCTOR> DeclareEqualsComparer (FUNCTOR&& f);

    /**
     *  \brief  DeclareInOrderComparer () marks a FUNCTOR (lambda or not) as being a FUNCTOR which compares for in-order
     * 
     *  DeclareInOrderComparer is a trivial wrapper on ComparisonRelationDeclaration, but takes advantage of the fact that you
     *  can deduce types on functions arguments not not on type of object for constructor (at least as of C++17).
     *
     *  @see DeclareEqualsComparer
     *  @see InOrderComparerAdapter
     *
     *  \note similar to InOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
     *        Whereas InOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
     */
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
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    /**
     *  \brief Use this to wrap any basic comparer, and produce a Less comparer
     *
     *  \note this requires the argument comparer is eStrictInOrder, eInOrderOrEquals, or eThreeWayCompare
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
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    /**
     *  \brief Use this to wrap any basic comparer, and produce a Three-Way comparer
     *
     *  \note - this requires the argument comparer be already a three-way-comparer or a less (strict inorder) comparer
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
        constexpr strong_ordering operator() (const T& lhs, const T& rhs) const;

    private:
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    // clang-format off
    /**
     *  \brief like std::compare_three_way{} (lhs, rhs), except class templated on T1/T2 instead of function, so you can bind function object for example in templates expecting one
     *
     *  \see See also ThreeWayCompare - nearly identical - but function template and can be used to deduce template arguments more easily
     *  
     *  \note DO NOT SPECIALIZE ThreeWayComparer<>, since its just a utility which trivailly wraps
     *        std::compare_three_way in c++20, so just specialize std::compare_three_way<>::operator()...
     */
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
    template <>
    struct ExtractComparisonTraits<ThreeWayComparer> {
        static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

// clang-format on
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Common_Compare_h_*/
