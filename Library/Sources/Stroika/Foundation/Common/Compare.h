/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

#if qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
// Quirky workaround for clang++-14 on XCode 14 (and probably others).
// No bug define for now - specific to clang++, because not sure what it depends on besides this, and this is bad enuf...
// this value being too low...
// This is PROBABLY an issue with the LIBC++ STD LIBRARY, and not CLANG COMPILER, BUT VERIFY THIS....
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

    namespace Private_ {
        template <typename COMPARE_FUNCTION>
        struct ExtractComparisonTraits_ {
            static constexpr ComparisonRelationType kComparisonRelationKind = COMPARE_FUNCTION::kComparisonRelationKind;
        };
        template <typename T>
        struct ExtractComparisonTraits_<equal_to<T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eEquals;
        };
        template <typename T>
        struct ExtractComparisonTraits_<less<T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
        };
        template <typename T>
        struct ExtractComparisonTraits_<greater<T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
        };
        template <typename T>
        struct ExtractComparisonTraits_<less_equal<T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
        };
        template <typename T>
        struct ExtractComparisonTraits_<greater_equal<T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
        };
        template <>
        struct ExtractComparisonTraits_<compare_three_way> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
        };

        template <typename COMPARE_FUNCTION>
        struct HasExtractComparisonSpecialization_ {
            static constexpr bool value = false;
        };
        template <typename T>
        struct HasExtractComparisonSpecialization_<equal_to<T>> {
            static constexpr bool value = true;
        };
        template <typename T>
        struct HasExtractComparisonSpecialization_<less<T>> {
            static constexpr bool value = true;
        };
        template <typename T>
        struct HasExtractComparisonSpecialization_<greater<T>> {
            static constexpr bool value = true;
        };
        template <typename T>
        struct HasExtractComparisonSpecialization_<less_equal<T>> {
            static constexpr bool value = true;
        };
        template <typename T>
        struct HasExtractComparisonSpecialization_<greater_equal<T>> {
            static constexpr bool value = true;
        };
        template <>
        struct HasExtractComparisonSpecialization_<compare_three_way> {
            static constexpr bool value = true;
        };

        template <typename T>
        concept HasRelationKind_ = requires (T) {
            {
                T::kComparisonRelationKind
            };
        };
    }

    /**
     *  This concept checks if the given function argument (COMPARER) appears to compare 'ARG_T's and return true/false.
     *  This doesn't require that that you've annotated the comparer, so it can false-positive (like mixing up
     *  an equality comparer for an in-order comparer).
     * 
     *  \see IComparer or IEqualsComparer for something stricter
     */
    template <typename COMPARER, typename ARG_T>
    concept IPotentiallyComparer = relation<COMPARER, ARG_T, ARG_T>;

    /**
     *  Concept IComparer checks if the argument is a (declared comparison type) Stroika comparer object.
     * 
     *  Basically, this means we KNOW if its a LESS or EQUALS etc comparer (see ExtractComparisonTraits_v).
     */
    template <typename POTENTIALLY_COMPARER>
    concept IComparer = (Private_::HasRelationKind_<remove_cvref_t<POTENTIALLY_COMPARER>> and requires (POTENTIALLY_COMPARER) {
                            {
                                Private_::ExtractComparisonTraits_<remove_cvref_t<POTENTIALLY_COMPARER>>::kComparisonRelationKind
                            } -> convertible_to<ComparisonRelationType>;
                        }) or Private_::HasExtractComparisonSpecialization_<remove_cvref_t<POTENTIALLY_COMPARER>>::value;

    /**
     *  \brief ExtractComparisonTraits_v<> extracts the @ComparisonRelationType for the given argument comparer. 
     *
     *  For common builtin types this is known with no user effort. For user-defined comparers, this will need to be declared (e.g. via ComparisonRelationDeclarationBase)
     *
     *  This is ONLY defined for builtin c++ comparison objects, though your code can define it however you wish for
     *  specific user-defined types using ComparisonRelationDeclarationBase<>.
     */
    template <IComparer COMPARE_FUNCTION>
    static constexpr ComparisonRelationType ExtractComparisonTraits_v =
        Private_::ExtractComparisonTraits_<remove_cvref_t<COMPARE_FUNCTION>>::kComparisonRelationKind;

    /**
     *  Checks that the argument comparer compares values of type ARG_T, and returns an equals comparison result.
     * 
     *  This won't let confuse equal_to with actual in-order comparison functions.
     * 
     *  \see IPotentiallyComparer, and use DeclareEqualsComparer to mark a given function as an in-order comparer.
     * 
     *  \par Example Usage
     *      \code
     *          static_assert (IEqualsComparer<equal_to<int>, int>);
     *          static_assert (not IEqualsComparer<less<int>, int>);
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
     *          KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
     *          ...
     *      \endcode
     */
    template <typename COMPARER, typename ARG_T>
    concept IEqualsComparer = IPotentiallyComparer<COMPARER, ARG_T> and IComparer<COMPARER> and
                              ExtractComparisonTraits_v<remove_cvref_t<COMPARER>> == ComparisonRelationType::eEquals;

    /**
     *  Checks that the argument comparer compares values of type ARG_T, and returns a (strict) in-order comparison result.
     * 
     *  This won't let confuse equal_to with actual in-order comparison functions.
     * 
     *  \see IPotentiallyComparer, and use DeclareInOrderComparer to mark a given function as an in-order comparer.
     */
    template <typename COMPARER, typename ARG_T>
    concept IInOrderComparer = IPotentiallyComparer<COMPARER, ARG_T> and IComparer<COMPARER> and
                               ExtractComparisonTraits_v<std::remove_cvref_t<COMPARER>> == ComparisonRelationType::eStrictInOrder;

    /**
     *  Utility class to serve as base class when constructing a comparison 'function' object comparer so ExtractComparisonTraits<> knows
     *  the type, or (with just one argument) as base for class that itself provives the operator() method.
     *         
     *  \par Example Usage
     *      \code
     *          struct String::EqualsComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
     *              nonvirtual bool operator() (const String& lhs, const String& rhs) const;
     *          };
     *      \endcode
     */
    template <ComparisonRelationType KIND>
    struct ComparisonRelationDeclarationBase {
        static constexpr inline ComparisonRelationType kComparisonRelationKind{KIND}; // accessed by ExtractComparisonTraits<>
    };

    /**
      *  Utility class to combine a (comparison) function object with ComparisonRelationDeclaration, which marks it as being
      *  of a particular comparison relation kind (e.g equality vs. less than).
      *         
      *  \par Example Usage
      *      \code
      *         using KeyEqualsCompareFunctionType =
      *              ComparisonRelationDeclaration<ComparisonRelationType::eEquals, function<bool (key_type, key_type)>>
      *             ;
      *      \endcode
      * 
      *     @see DeclareEqualsComparer
      *     @see DeclareInOrderComparer
      */
    template <ComparisonRelationType KIND, typename ACTUAL_COMPARER>
        requires (not is_reference_v<ACTUAL_COMPARER>)
    struct ComparisonRelationDeclaration : ComparisonRelationDeclarationBase<KIND>, ACTUAL_COMPARER {
    public:
        /**
         */
        constexpr ComparisonRelationDeclaration () = default;
        constexpr ComparisonRelationDeclaration (const ACTUAL_COMPARER& actualComparer);
        constexpr ComparisonRelationDeclaration (ACTUAL_COMPARER&& actualComparer);
        constexpr ComparisonRelationDeclaration (const ComparisonRelationDeclaration&) = default;
        constexpr ComparisonRelationDeclaration (ComparisonRelationDeclaration&&)      = default;
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
    constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eEquals, remove_cvref_t<FUNCTOR>> DeclareEqualsComparer (FUNCTOR&& f);

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
    constexpr Common::ComparisonRelationDeclaration<ComparisonRelationType::eStrictInOrder, remove_cvref_t<FUNCTOR>> DeclareInOrderComparer (FUNCTOR&& f);

    /**
     *  \brief Use this to wrap any basic comparer, and produce an Equals comparer.
     *
     *  This is done by querying the 'type' of the baseComparer with @see ExtractComparisonTraits_v, and mapping the logic accordingly.
     */
    template <IComparer BASE_COMPARER>
    struct EqualsComparerAdapter : ComparisonRelationDeclarationBase<ComparisonRelationType::eEquals> {
        /**
         */
        constexpr EqualsComparerAdapter (const BASE_COMPARER& baseComparer);
        constexpr EqualsComparerAdapter (BASE_COMPARER&& baseComparer);

        /**
         */
        template <typename LT, typename RT>
        constexpr bool operator() (LT&& lhs, RT&& rhs) const;

    private:
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    /**
     *  \brief Use this to wrap any basic comparer, and produce a Less comparer
     *
     *  \note this requires the argument comparer is eStrictInOrder, eInOrderOrEquals, or eThreeWayCompare
     */
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eInOrderOrEquals or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare)
    struct InOrderComparerAdapter : ComparisonRelationDeclarationBase<ComparisonRelationType::eStrictInOrder> {
        /**
         */
        constexpr InOrderComparerAdapter (const BASE_COMPARER& baseComparer);
        constexpr InOrderComparerAdapter (BASE_COMPARER&& baseComparer);

        /**
         */
        template <typename LT, typename RT>
        constexpr bool operator() (LT&& lhs, RT&& rhs) const;

    private:
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    /**
     *  \brief Use this to wrap a basic comparer, and produce a Three-Way comparer
     */
    template <IComparer BASE_COMPARER>
        requires (ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare or
                  ExtractComparisonTraits_v<BASE_COMPARER> == ComparisonRelationType::eStrictInOrder)
    struct ThreeWayComparerAdapter : ComparisonRelationDeclarationBase<ComparisonRelationType::eThreeWayCompare> {
        /**
         */
        constexpr ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer);
        constexpr ThreeWayComparerAdapter (BASE_COMPARER&& baseComparer);

        /**
         */
        template <typename LT, typename RT>
        constexpr strong_ordering operator() (LT&& lhs, RT&& rhs) const;

    private:
        [[no_unique_address]] BASE_COMPARER fBASE_COMPARER_;
    };

    /**
     *  \brief ThreeWayComparer for optional types, like builtin one, except this lets you pass in explciit 'T' comparer for the T in optional<T>
     *
     *  You dont need this when the default comparer for 'T' works as you wish. But for example, ThreeWayComparer<optional<String>> - where you want
     *  to use a case insensitive comparer for the strings, is tricky. THIS class solves that, by letting you pass in explicitly the 
     *  'base comparer'.
     */
    template <typename T, IComparer TCOMPARER = std::compare_three_way>
    struct OptionalThreeWayComparer : ComparisonRelationDeclarationBase<ComparisonRelationType::eThreeWayCompare> {
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
    constexpr strong_ordering CompareResultNormalizer (FROM_INT_TYPE f);

    /**
     *  Map equals to equals, but less becomes greater and greater becomes less
     */
    constexpr strong_ordering ReverseCompareOrder (strong_ordering so);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Common_Compare_h_*/
