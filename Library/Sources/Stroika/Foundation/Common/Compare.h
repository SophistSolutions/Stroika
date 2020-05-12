/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

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

#if defined(__cpp_impl_three_way_comparison) && qCompilerAndStdLib_strong_ordering_equals_Buggy
namespace std {
    inline bool operator== (const strong_ordering& lhs, const strong_ordering& rhs)
    {
        if (((lhs == 0) == (rhs == 0)) and ((lhs > 0) == (rhs > 0)) and ((lhs < 0) == (rhs < 0))) {
            return true;
        }
        return false;
    }
}
#endif

namespace Stroika::Foundation::Common {

    namespace Private_ {
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (ThreeWayComparer, (typename X::ThreeWayComparer{}(x, x)));
        STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (ThreeWayComparerTemplate, (typename X::template ThreeWayComparer<>{}(x, x)));
    }

    /**
     *  In C++, you can only use strong_ordering, strong_ordering::less, strong_ordering::equal, strong_ordering::greater if 
     *  __cpp_impl_three_way_comparison >= 201907
     *
     *  In Stroika, these defines are usable no matter what (in in C++ correspond to the C++ type/value)
     */
#if __cpp_impl_three_way_comparison < 201907
    using strong_ordering              = int;
    constexpr strong_ordering kLess    = -1;
    constexpr strong_ordering kEqual   = 0;
    constexpr strong_ordering kGreater = 1;
#else
    using strong_ordering              = std::strong_ordering;
    constexpr strong_ordering kLess    = strong_ordering::less;
    constexpr strong_ordering kEqual   = strong_ordering::equal;
    constexpr strong_ordering kGreater = strong_ordering::greater;
#endif

    /**
     *  Portable to C++17 version of std::compare_three_way (in Stroika::Foundation::Common namespace to avoid rule 
     *  of not adding stuff to std namespace).
     *
     *  For code which requires C++20 or later, simply call std::compare_three_way instead.
     */
#if __cpp_lib_three_way_comparison < 201907L
    template <class T, class U>
    struct compare_three_way {
        constexpr auto operator() (T&& lhs, U&& rhs) const
        {
            // ISSUE HERE - PRE C++20, no distinction made between strong_ordering, weak_ordering, and partial_ordering, because
            // this counts on cooperation with various types and mechanismns like operator<=> = default which we don't have (and declared strong_ordering=int)
            if (equal_to<T>{}(lhs, rhs)) {
                return Stroika::Foundation::Common::kEqual;
            }
            return less<T>{}(lhs, rhs) ? Stroika::Foundation::Common::kLess : Stroika::Foundation::Common::kGreater;
        }
    };
#else
    template <class T, class U>
    using compare_three_way = std::compare_three_way<T, U>;
#endif

    /**
     * @todo  MAYBE DEPRECATE THIS - IN FAVOR OF Common::compare_three_way

     RATIONALE TO KEEP THIS:
            EITHER we need nested class approach T::ThreeWayCompare - even in c++20 - for things like String::EqualsComparer (with extra args)
            OR we need this 

            CONSIDERING. RARE ISSUE where we need the extra params. More modular to do T::Comparer... C++ DOES appear to allow specialization 
            of stuff in std namespace for this purpose???

            TRY in a few places, like String/Character to see how it goes...

     *
     *  \brief - std::compare_three_way - Stand-in until C++20, three way compare - used for Calling three-way-comparer
     *
     *  Not every class implements the three-way comparer (spaceship operator) - especailly before C++20.
     *
     *  \note   Common::ThreeWayComparer<> will work for any type for which:
     *          o   Common::ThreeWayComparer<> has already been explicitly specialized
     *          o   has function (three way comparer object) member ThreeWayComparer (e.g. String::ThreeWayComparer)
     *          o   has a spaceship operator defined for it (C++20 or later - and NYI)
     *          o   has less<> and equal_to<> defined (either explicitly, or implicitly by
     *              having < and == work with it)   
     *              
     *  \par Example Usage
     *      \code
     *          int Common::ThreeWayComparer<Authority>::operator() (const Authority& lhs, const Authority& rhs) const
     *          {
     *              if (int cmp = Common::ThreeWayCompare (lhs.GetHost (), rhs.GetHost ())) {
     *                  return cmp;
     *              }
     *              if (int cmp = Common::ThreeWayCompare (lhs.GetUserInfo (), rhs.GetUserInfo ())) {
     *                  return cmp;
     *              }
     *              return Common::ThreeWayCompare (lhs.GetPort (), rhs.GetPort ());
     *          }
     *      \endcode
     *
     *  \note TODO
     *      @todo   Consider if this is needed - MAYBE OK (with both pre/post C++20) to use three_way_compare function cuz its impl
     *              should be Ok - just use Stroika::Foundation::Common::compare_three_way for portability.
     *      @todo   Support spaceship operator - like we do with HasComparer_v - detecting if class Q has spaceship operator and
     *              using that (after check for HasComparer_v and before default using default implementation).
     */
    template <typename T, typename... ARGS>
    struct ThreeWayComparer {
        constexpr ThreeWayComparer (ARGS... args);
        template <class TT, class UU, typename Q = T, enable_if_t<Private_::HasThreeWayComparer_v<Q>>* = nullptr>
        constexpr auto operator() (UU&& lhs, TT&& rhs) const;
        template <class TT, class UU, typename Q = T, enable_if_t<Private_::HasThreeWayComparerTemplate_v<Q>>* = nullptr>
        constexpr auto operator() (UU&& lhs, TT&& rhs) const;
        template <class TT, class UU, typename Q = T, enable_if_t<not Private_::HasThreeWayComparer_v<Q> and not Private_::HasThreeWayComparerTemplate_v<Q>>* = nullptr>
        constexpr auto operator() (UU&& lhs, TT&& rhs) const;
        tuple<ARGS...> fArgs_;
    };

    /**


    &&& @todo see if this can be deprecated and if 

        return three_way_compare{} (lhs,rhs) works/deduces right types - not sure how it would be I've seen examples in docs of this



     *  \brief trivial wrapper calling ThreeWayComparer<T>{}(lhs,rhs) i.e. std::compare_three_way{} (lhs, rhs)
     *
     *  Since the type of ThreeWayComparer cannot be deduced, you must write a painful:
     *      \code
     *          ThreeWayComparer<T>{} (lhs, rhs);   // this often looks much worse when 'T' is a long typename
     *      \endcode
     *
     *  This helper function allows for the type deduction, at the cost of not working with arguments to
     *  the comparer, and the cost of not re-using the comparer object. However, since the comparer is typically
     *  constexpr, that should be a modest (zero?) cost.
     *
     *  \par Example Usage
     *      \code
     *          if (int cmp = Common::ThreeWayCompare (lhs.GetHost (), rhs.GetHost ())) {
     *             return cmp;
     *          }
     *          if (int cmp = Common::ThreeWayCompare (lhs.GetUserInfo (), rhs.GetUserInfo ())) {
     *             return cmp;
     *          }
     *          return Common::ThreeWayCompare (lhs.GetPort (), rhs.GetPort ());
     *      \endcode
     *
     *  \note Until we start assuming C++20, this will be the preferred way of calling the three way comparison
     *        in Stroika, EXCEPT when extra args are needed to the ThreeWayComparison function (such as with String::ThreeWayComparer)
     *        or when the is cost in constructing (not constexpr) the ThreeWayComparer (luckily basically the same cases - when there are args).
     *              -- LGP 2019-05-07
     */
    template <typename T>
    constexpr Common::strong_ordering ThreeWayCompare (const T& lhs, const T& rhs);

    /**
     *  \brief ThreeWayComparer for optional types, like builtin one, except this lets you pass in explciit 'T' comparer for the T in optional<T>
     *
     *  You dont need this when the default comparer for 'T' works as you wish. But for example, ThreeWayComparer<optional<String>> - where you want
     *  to use a case insensitive comparer for the strings, is tricky. THIS class solves that, by letting you pass in explicitly the 
     *  'base comparer'.
     */
    template <typename T, typename TCOMPARER = ThreeWayComparer<T>>
    struct OptionalThreeWayComparer {
        constexpr OptionalThreeWayComparer (const TCOMPARER& tComparer);
        constexpr strong_ordering operator() (const optional<T>& lhs, const optional<T>& rhs) const;
        TCOMPARER                 fTComparer_;
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
#if __cpp_lib_three_way_comparison >= 201907
    template <typename T>
    struct ExtractComparisonTraits<compare_three_way<T>> {
        static constexpr ComparisonRelationType kComparisonRelationKind = ComparisonRelationType::eThreeWayCompare;
    };
#endif

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
     *          struct String::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
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
     *  @see EqualsComparerAdapter
     *
     *  \note similar to InOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
     *        Whereas InOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
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
     *  @see InOrderComparerAdapter
     *
     *  \note similar to InOrderComparerAdapter(), except this function ignores the TYEP of 'f' and just marks it as an InOrder comparer
     *        Whereas InOrderComparerAdapter looks at the type of 'f' and does the appropriate mapping logic.
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
        BASE_COMPARER fBASE_COMPARER_;
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
        BASE_COMPARER fBASE_COMPARER_;
    };

    ///////////////////// DEPRECATED //////////////////////////

    template <typename TYPE>
    [[deprecated ("Since Stroika 2.1a5 - use CompareResultNormalizer or ThreeWayCompare")]] constexpr strong_ordering ThreeWayCompareNormalizer (TYPE lhs, TYPE rhs)
    {
        return ThreeWayCompare (lhs, rhs);
    }
    template <typename FROM_INT_TYPE>
    [[deprecated ("since Stroika 2.1a5 - use CompareResultNormalizer")]] inline strong_ordering CompareResultNormalizeHelper (FROM_INT_TYPE f)
    {
        return CompareResultNormalizer (f);
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use EqualsComparerAdapter directly")]] constexpr auto mkEqualsComparerAdapter (const BASE_COMPARER& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>
    {
        return EqualsComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use EqualsComparerAdapter directly")]] constexpr auto mkEqualsComparerAdapter (BASE_COMPARER&& baseComparer) -> EqualsComparerAdapter<BASE_COMPARER>
    {
        return EqualsComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use InOrderComparerAdapter directly")]] constexpr auto mkInOrderComparerAdapter (const BASE_COMPARER& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>
    {
        return InOrderComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use InOrderComparerAdapter directly")]] constexpr auto mkInOrderComparerAdapter (BASE_COMPARER&& baseComparer) -> InOrderComparerAdapter<BASE_COMPARER>
    {
        return InOrderComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use ThreeWayComparerAdapter directly")]] constexpr auto mkThreeWayComparerAdapter (const BASE_COMPARER& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>
    {
        return ThreeWayComparerAdapter<BASE_COMPARER>{baseComparer};
    }
    template <typename BASE_COMPARER>
    [[deprecated ("Since Stroika v2.1a5 - use ThreeWayComparerAdapter directly")]] constexpr auto mkThreeWayComparerAdapter (BASE_COMPARER&& baseComparer) -> ThreeWayComparerAdapter<BASE_COMPARER>
    {
        return ThreeWayComparerAdapter<BASE_COMPARER>{move (baseComparer)};
    }
    // clang-format off
    template <typename T>
    struct [[deprecated ("Since Stroika 2.1a5")]] ThreeWayComparerDefaultImplementation{
        constexpr strong_ordering operator() (const T& lhs, const T& rhs) const {
#if __cpp_lib_three_way_comparison >= 201907L
            return compare_three_way{}(lhs, rhs);
#else
            if (equal_to<T>{}(lhs, rhs))
                return kEqual;
            if ( less<T>{}(lhs, rhs) ) 
                return kLess ;
            else 
                return kGreater;
#endif
        }
    };
// clang-format on
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Compare.inl"

#endif /*_Stroika_Foundation_Common_Compare_h_*/
