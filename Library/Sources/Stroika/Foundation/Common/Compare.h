/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

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
        template <typename ARG_T, typename COMPARE_FUNCTION>
        struct ExtractComparisonTraits_ {};
        template <typename ARG_T, typename COMPARE_FUNCTION>
            requires requires (COMPARE_FUNCTION) {
                {
                    COMPARE_FUNCTION::kComparisonRelationKind
                } -> convertible_to<ComparisonRelationType>;
            }
        struct ExtractComparisonTraits_<ARG_T, COMPARE_FUNCTION> {
            static constexpr ComparisonRelationType kComparisonRelationKind = COMPARE_FUNCTION::kComparisonRelationKind;
        };
        template <typename ARG_T, typename COMPARE_FUNCTION>
            requires (
                requires (COMPARE_FUNCTION c, ARG_T l, ARG_T r) {
                    {
                        c (l, r)
                    } -> convertible_to<strong_ordering>;
                } and
                not requires (COMPARE_FUNCTION) {
                    {
                        COMPARE_FUNCTION::kComparisonRelationKind
                    } -> convertible_to<ComparisonRelationType>;
                })
        struct ExtractComparisonTraits_<ARG_T, COMPARE_FUNCTION> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, equal_to<ARG_T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eEquals;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, less<ARG_T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, greater<ARG_T>> {
            static constexpr ARG_T kComparisonRelationKind = ComparisonRelationType::eStrictInOrder;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, less_equal<ARG_T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, greater_equal<ARG_T>> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eInOrderOrEquals;
        };
        template <typename ARG_T>
        struct ExtractComparisonTraits_<ARG_T, compare_three_way> {
            static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
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
    concept IPotentiallyComparer = relation<COMPARER, ARG_T, ARG_T> or (same_as<COMPARER, compare_three_way> and three_way_comparable<ARG_T>) or
                                   (regular_invocable<COMPARER, ARG_T, ARG_T> and requires (COMPARER c, ARG_T l, ARG_T r) {
                                       {
                                           c (l, r)
                                       } -> convertible_to<strong_ordering>;
                                   });

    /**
     *  Concept IComparer checks if the argument is a (declared comparison type) Stroika comparer object.
     * 
     *  Basically, this means we KNOW if its a LESS or EQUALS etc comparer (see ExtractComparisonTraits_v).
     * 
     *  \note Any function object (eg lambda) taking 2 ARG_T arguments and returning ComparisonRelationType works automatically).
     */
    template <typename POTENTIALLY_COMPARER, typename ARG_T>
    concept IComparer = requires (POTENTIALLY_COMPARER, ARG_T) {
        {
            Private_::ExtractComparisonTraits_<ARG_T, remove_cvref_t<POTENTIALLY_COMPARER>>::kComparisonRelationKind
        } -> convertible_to<ComparisonRelationType>;
    };

    /**
     *  \brief ExtractComparisonTraits_v<> extracts the @ComparisonRelationType for the given argument comparer. 
     *
     *  For common builtin types this is known with no user effort. For user-defined comparers, this will need to be declared (e.g. via ComparisonRelationDeclarationBase)
     *
     *  This is ONLY defined for builtin c++ comparison objects, though your code can define it however you wish for
     *  specific user-defined types using ComparisonRelationDeclarationBase<>.
     */
    template <typename ARG_T, IComparer<ARG_T> COMPARE_FUNCTION>
    static constexpr ComparisonRelationType ExtractComparisonTraits_v =
        Private_::ExtractComparisonTraits_<ARG_T, remove_cvref_t<COMPARE_FUNCTION>>::kComparisonRelationKind;

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
    concept IEqualsComparer = IPotentiallyComparer<COMPARER, ARG_T> and IComparer<COMPARER, ARG_T> and
                              ExtractComparisonTraits_v<ARG_T, remove_cvref_t<COMPARER>> == ComparisonRelationType::eEquals;

    /**
     *  Checks that the argument comparer compares values of type ARG_T, and returns a (strict) in-order comparison result.
     * 
     *  This won't let confuse equal_to with actual in-order comparison functions.
     * 
     *  \see IPotentiallyComparer, and use DeclareInOrderComparer to mark a given function as an in-order comparer.
     *  \see also InOrderComparerAdapter (if you have another sort of comparer, and want to use it as an In-Order comparer).
     */
    template <typename COMPARER, typename ARG_T>
    concept IInOrderComparer = IPotentiallyComparer<COMPARER, ARG_T> and IComparer<COMPARER, ARG_T> and
                               ExtractComparisonTraits_v<ARG_T, remove_cvref_t<COMPARER>> == ComparisonRelationType::eStrictInOrder;

    /**
     *  Checks that the argument comparer compares values of type ARG_T, and returns a three-way-compare comparison result.
     * 
     *  This won't let confuse equal_to with actual in-order comparison functions.
     * 
     *  \see IPotentiallyComparer, and use DeclareInOrderComparer to mark a given function as a three-way comparer.
     */
    template <typename COMPARER, typename ARG_T>
    concept IThreeWayComparer = IPotentiallyComparer<COMPARER, ARG_T> and IComparer<COMPARER, ARG_T> and
                                ExtractComparisonTraits_v<ARG_T, remove_cvref_t<COMPARER>> == ComparisonRelationType::eThreeWayCompare;

    /**
     *  Checks that the argument comparer can be converted (via ThreeWayComparerAdapter) to three_way comparer (on type T).
     * 
     *  \see IInOrderComparer, IThreeWayComparer, and ThreeWayComparerAdapter
     */
    template <typename COMPARER, typename ARG_T>
    concept IThreeWayAdaptableComparer = IInOrderComparer<COMPARER, ARG_T> or IThreeWayComparer<COMPARER, ARG_T>;

    /**
     *  Utility class to serve as base class when constructing a comparison 'function' object comparer so ExtractComparisonTraits<> knows
     *  the type, or (with just one argument) as base for class that itself provides the operator() method.
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
        template <typename... ARGS>
        constexpr ComparisonRelationDeclaration (ARGS... args);
        constexpr ComparisonRelationDeclaration (const ComparisonRelationDeclaration&)     = default;
        constexpr ComparisonRelationDeclaration (ComparisonRelationDeclaration&&) noexcept = default;

        constexpr ComparisonRelationDeclaration& operator= (const ComparisonRelationDeclaration&)     = default;
        constexpr ComparisonRelationDeclaration& operator= (ComparisonRelationDeclaration&&) noexcept = default;
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
     *  \par Example Usage:
     *      \code
     *          constexpr auto kDefaultPrioritizedName_OrderByDefault_Less =
     *              Stroika::Foundation::Common::DeclareInOrderComparer ([] (const PrioritizedName& lhs, const PrioritizedName& rhs) -> bool {
     *                  if (lhs.fPriority > rhs.fPriority) {
     *                      return true;
     *                  }
     *                  else if (lhs.fPriority < rhs.fPriority) {
     *                      return false;
     *                  }
     *                  return lhs.fName < rhs.fName;
     *              });
     *      \endcode
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
    template <typename ARG_T, IComparer<ARG_T> BASE_COMPARER>
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
    template <typename T, IComparer<T> BASE_COMPARER>
        requires (ExtractComparisonTraits_v<T, BASE_COMPARER> == ComparisonRelationType::eStrictInOrder or
                  ExtractComparisonTraits_v<T, BASE_COMPARER> == ComparisonRelationType::eInOrderOrEquals or
                  ExtractComparisonTraits_v<T, BASE_COMPARER> == ComparisonRelationType::eThreeWayCompare)
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
    template <typename T, IThreeWayAdaptableComparer<T> BASE_COMPARER>
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
     *  \brief ThreeWayComparer for optional types, like builtin one, except this lets you pass in explicit 'T' comparer for the T in optional<T>
     *
     *  You dont need this when the default comparer for 'T' works as you wish. But for example, ThreeWayComparer<optional<String>> - where you want
     *  to use a case insensitive comparer for the strings, is tricky. THIS class solves that, by letting you pass in explicitly the 
     *  'base comparer'.
     */
    template <typename ARG_T, IComparer<ARG_T> TCOMPARER = std::compare_three_way>
    struct OptionalThreeWayComparer : ComparisonRelationDeclarationBase<ComparisonRelationType::eThreeWayCompare> {
        constexpr OptionalThreeWayComparer (TCOMPARER&& tComparer);
        constexpr OptionalThreeWayComparer (const TCOMPARER& tComparer);
        constexpr strong_ordering operator() (const optional<ARG_T>& lhs, const optional<ARG_T>& rhs) const;

    private:
        [[no_unique_address]] TCOMPARER fTComparer_;
    };

    /**
     * Take the given value and map it to -1, 0, 1 - without any compiler warnings. Handy for 32/64 bit etc coding when you maybe comparing
     * different sized values and just returning an int, but don't want the warnings about overflow etc.
     */
    template <typename FROM_INT_TYPE>
    constexpr strong_ordering CompareResultNormalizer (FROM_INT_TYPE f);

    /**
     */
    constexpr int ToInt (strong_ordering f)
    {
        if (f == strong_ordering::less) {
            return -1;
        }
        else if (f == strong_ordering::equal or f == strong_ordering::equivalent) {
            return 0;
        }
        else {
            return 1;
        }
    }

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
