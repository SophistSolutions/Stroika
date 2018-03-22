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
 * TODO:
 *
 *      @todo   REPLACE
 *                  Has_Operator_LessThan<T>::value
 *              with:
 *                  Configuration::LessThanComparable<T> ()
 *              BUT - CANNOT DO until we have vc++ fixed for constexpr functions
 *
 *      @todo   use http://en.cppreference.com/w/cpp/types/enable_if to make this work better
 *              So for example - we want something like:
 *                  enable_if<is_integral<T>> int compare (T, T) { return v1 - v2; }
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Common {

            namespace Private_ {
                namespace Has_Compare_Helper_ {
                    using namespace Configuration;
                    template <typename T>
                    struct get_Compare_result {
                    private:
                        template <typename X>
                        static auto                 check (X const& x) -> decltype (declval<X> ().Compare (declval<X> ()));
                        static substitution_failure check (...);

                    public:
                        using type = decltype (check (std::declval<T> ()));
                    };
                }
            }

            /**
             *  has value member which is true iff 'T' supports the less than operator.
             */
            template <typename T>
            struct Has_Compare : Configuration::substitution_succeeded<typename Private_::Has_Compare_Helper_::get_Compare_result<T>::type> {
            };

            /**
             *  Utility you can specialize to define how two types are to be compared equality using the defined operator==(T,T).
             */
            template <typename T>
            struct ComparerWithEqualsOptionally {
                using value_type = T;

                /**
                 *  Returns true if "v1 == v2" - or more precisely - if Compare (v1, v2) == 0. Users can template specialize to
                 *  replace these, but they must remain consistent.
                 */
                static constexpr bool Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2);
            };

            // ADAPT OLD STYLE EQUALS COMPARER TO NEW STYLE (really C++ style)
            template <typename OLD_COMPARER>
            struct NEW_EQUALS_COMPARER {
                template <typename T>
                constexpr bool operator() (T v1, T v2) const
                {
                    return OLD_COMPARER::Equals (v1, v2);
                }
            };

            // ADAPT OLD STYLE EQUALS COMPARER TO NEW STYLE (really C++ style)
            template <typename EQUALS_COMPARER>
            struct OldStyleEqualsComparerFromNewStyleEqualsComparer {
                EQUALS_COMPARER fEqualsComparer_;
                constexpr OldStyleEqualsComparerFromNewStyleEqualsComparer (const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{})
                    : fEqualsComparer_ (equalsComparer)
                {
                }

                template <typename T>
                bool Equals (const T& v1, const T& v2) const
                {
                    return fEqualsComparer_ (v1, v2);
                }
            };

            /**
             *  Utility you can specialize to define how two types are to be compared equality using the defined operator==(T,T).
             *
             *  \note   Generally use DefaultEqualsComparer<> instead of this, as it automatically selects the right way to compare.
             */
            template <typename T>
            struct ComparerWithEquals : ComparerWithEqualsOptionally<T> {
                RequireConceptAppliesToTypeMemberOfClass (RequireOperatorEquals, T);
                static_assert (Configuration::EqualityComparable<T> (), "T must be EqualityComparable");
            };

            /**
             *  Utility you can specialize to define how two types are to be compared for ordering (and how that fits with equality)
             *  The default implementation only requires you define operator< (T,T) - and it fiugres out other cases from just that.
             *
             *  @todo - writeup !!! NOTE - ASSERTS ComparerWithWellOrder and ComparerWithEquals compatible - namely a < b and b > a iff .... writeup!!!
             */
            template <typename T>
            struct ComparerWithWellOrder { /*: ComparerWithEquals<T>*/
                using value_type = T;

                static_assert (Configuration::LessThanComparable<T> (), "T must be LessThanComparable");
                RequireConceptAppliesToTypeMemberOfClass (RequireOperatorLess, T);

                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                static constexpr int  Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2);
                static constexpr bool Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
                {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    return not(v1 < v2 or v2 < v1);
#else
                    bool result{not(v1 < v2 or v2 < v1)};
                    //Ensure (not Configuration::EqualityComparable<T> () or result == (v1 == v2));  must check more indirectly to avoid compile error when not equality comparable
                    return result;
#endif
                }
            };

            /**
             *  DefaultEqualsComparer will procduce an Equals() method from a variety of sources automatically:
             *      o   operator==
             *      o   operator< (!(a<b or b<a)
             *
             *  and SOON
             *      existing Equals() function(global?/method of T?)
             *      existing Compares() function(global?/method of T?)
             *      maybe also other variants like operator!=, etc.
             *
             *      @todo kind of a kludge how we implement - (shared_ptr<int> - but void not a valid base class).
             */
            template <typename T, typename SFINAE = typename conditional<(Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value), ComparerWithEquals<T>, typename conditional<Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value, ComparerWithWellOrder<T>, shared_ptr<int>>::type>::type>
            struct DefaultEqualsComparer : SFINAE {
            };

            /**
             *  DefaultEqualsComparerOptionally will procduce an Equals() if possible, but will still compile otherwise. Its just
             *  that attempts to call its Equals() method will fail.
             *
             *      @todo improve this so it has an Equals () method with a static_assert, so we get a clearer message when used.
             *      @todo kind of a kludge how we implement - (shared_ptr<int> - but void not a valid base class).
             */
            template <typename T>
            struct DefaultEqualsComparerOptionally : conditional<(Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value) or (Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value), DefaultEqualsComparer<T>, shared_ptr<int>>::type {
            };

            /**
             *  \par Example Usage
             *      \code
             *        return Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
             *      \endcode
             */
            template <typename INTEGERLIKETYPE>
            constexpr int CompareNormalizer (INTEGERLIKETYPE lhs, INTEGERLIKETYPE rhs)
            {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return (lhs < rhs) ? -1 : ((lhs == rhs) ? 0 : 1);
#else
                //using ST = make_signed_t<INTEGERLIKETYPE>;   // could do this and then diff
                //return static_cast<ST> (lhs) - static_cast<ST> (rhs);
                if (lhs < rhs) {
                    return -1;
                }
                else if (lhs == rhs) {
                    return 0;
                }
                else {
                    return 1;
                }
#endif
            }
            // @todo more specializations
            template <>
            inline constexpr int CompareNormalizer (int lhs, int rhs)
            {
                return lhs - rhs;
            }

            namespace STL {
                /**
                 *  An STL-compatible variant of 'less' which leverages a stroika comparer. This can be used to use
                 *  Stroika comparares (almost directly) with std STL classes such as map<>, and set<>.
                 */
                template <typename T, typename STROIKA_COMPARER>
                struct less {
                    // @todo not sure we need first_argument_type/second_argument_type/result_type but there for backward compat (std::binary_function<T,T,bool>)
                    typedef T       first_argument_type;
                    typedef T       second_argument_type;
                    typedef bool    result_type;
                    nonvirtual bool operator() (Configuration::ArgByValueType<T> _Left, Configuration::ArgByValueType<T> _Right) const;
                };
            }

            /*
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
             *  So to get started, we fill that gap: ComparisonTraits.
             */

            /**
             *  Stand-in until C++20, three way compare
             */
            template <typename T>
            struct ThreeWayCompare {
                constexpr int operator() (const T& _Left, const T& _Right) const
                {
                    // in general, can do this much more efficiently (subtract left and right), but for now, KISS
                    if (equal_to<T>{}(_Left, _Right)) {
                        return 0;
                    }
                    return less<T>{}(_Left, _Right) ? -1 : 1;
                }
            };

            /**
             *  THIS is what drives how we do containers/related algorithms (less is equiv to greater for most of them)
             */
            enum class OrderingRelationType {
                eEquals,

                /**
                 *   e.g. less<T>, or greater<T>
                 */
                eInOrder,

                /**
                 *   e.g. less_equal<T>, or greater_equal<T>
                 *   \note considered the name 'partial' order here but that could be confusing, since partial order frequently
                 *         refers to not covering the entire domain - not less vs. less_equal.
                 */
                eInOrderOrEquals,

                /**
                 *   e.g. function<int(T,T)> - where < 0 return is 'in order', 0 means equal, and > 0 means reversed order
                 */
                eThreeWayCompare
            };

            /**
             *  Utility class to serve as base class when constructing user-defined 'function' object comparer so ComparisonTraits<> knows
             *  the type.
             */
            template <OrderingRelationType TYPE>
            struct ComparisonTraitsBase {
                static constexpr OrderingRelationType kOrderingRelationKind = TYPE; // default - so user-defined types can do this to automatically define their Comparison Traits
            };

            /**
             *  This is ONLY defined for builtin c++ comparison objects, though your code can define it however you wish for
             *  specific user-defined types.
             */
            template <typename COMPARE_FUNCTION>
			struct ComparisonTraits {
				static constexpr OrderingRelationType kOrderingRelationKind = COMPARE_FUNCTION::kOrderingRelationKind;
			};

            template <typename T>
            struct ComparisonTraits<equal_to<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eEquals;
            };
            template <typename T>
            struct ComparisonTraits<less<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eInOrder;
            };
            template <typename T>
            struct ComparisonTraits<greater<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eInOrder;
            };
            template <typename T>
            struct ComparisonTraits<less_equal<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eInOrderOrEquals;
            };
            template <typename T>
            struct ComparisonTraits<greater_equal<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eInOrderOrEquals;
            };
            template <typename T>
            struct ComparisonTraits<ThreeWayCompare<T>> {
                static constexpr OrderingRelationType kOrderingRelationKind = OrderingRelationType::eThreeWayCompare;
            };

            /**
             */
            template <typename COMPARER>
            constexpr bool IsEqualsComparer ()
            {
                return ComparisonTraits<COMPARER>::kOrderingRelationKind == OrderingRelationType::eEquals;
            }
            template <typename COMPARER>
            constexpr bool IsEqualsComparer (const COMPARER&)
            {
                return ComparisonTraits<COMPARER>::kOrderingRelationKind == OrderingRelationType::eEquals;
            }

            /**
             */
            template <typename COMPARER>
            constexpr bool IsInOrderComparer ()
            {
                return ComparisonTraits<COMPARER>::kOrderingRelationKind == OrderingRelationType::eInOrder;
            }
            template <typename COMPARER>
            constexpr bool IsInOrderComparer (const COMPARER&)
            {
                return ComparisonTraits<COMPARER>::kOrderingRelationKind == OrderingRelationType::eInOrder;
            }

            /**
             *  Utility class to serve as base class when constructing user-defined 'function' object comparer so ComparisonTraits<> knows
             *  the type.
             *
             *  \par Example Usage
             *      \code
             *          using EqualityComparerType = Common::FunctionComparerAdapter<Common::OrderingRelationType::eEquals, function<bool(T, T)>>;
             *      \endcode
             */
            template <OrderingRelationType TYPE, typename ACTUAL_COMPARER>
            struct FunctionComparerAdapter {
                static constexpr OrderingRelationType kOrderingRelationKind = TYPE; // default - so user-defined types can do this to automatically define their Comparison Traits
                ACTUAL_COMPARER                       fActualComparer;
                constexpr FunctionComparerAdapter (const ACTUAL_COMPARER& actualComparer)
                    : fActualComparer (actualComparer)
                {
                }
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const
                {
                    return fActualComparer (lhs, rhs);
                }
            };

            /**
             *  \brief Use this to wrap any basic comparer, and produce a Less comparer
             */
            template <typename BASE_COMPARER>
            struct InOrderComparerAdapter {
                constexpr InOrderComparerAdapter (const BASE_COMPARER& baseComparer)
                    : fBASE_COMPARER_ (baseComparer)
                {
                }
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const
                {
                    switch (ComparisonTraits<BASE_COMPARER>::kOrderingRelationKind) {
                        case OrderingRelationType::eInOrder:
                            return fBASE_COMPARER_ (lhs, rhs);
                        case OrderingRelationType::eInOrderOrEquals:
                            return fBASE_COMPARER_ (lhs, rhs) and not fBASE_COMPARER_ (rhs, lhs);
                        case OrderingRelationType::eThreeWayCompare:
                            return fBASE_COMPARER_ (lhs, rhs) < 0;
                        default:
                            AssertNotReached ();
                            return false;
                    }
                }

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *  \brief Use this to wrap any basic comparer, and produce an Equals comparer
             */
            template <typename BASE_COMPARER>
            struct EqualsComparerAdapter {
                constexpr EqualsComparerAdapter (const BASE_COMPARER& baseComparer)
                    : fBASE_COMPARER_ (baseComparer)
                {
                }
                template <typename T>
                constexpr bool operator() (const T& lhs, const T& rhs) const
                {
                    switch (ComparisonTraits<BASE_COMPARER>::kOrderingRelationKind) {
                        case OrderingRelationType::eEquals:
                            return fBASE_COMPARER_ (lhs, rhs);
                        case OrderingRelationType::eInOrder:
                            return not fBASE_COMPARER_ (lhs, rhs) and not fBASE_COMPARER_ (rhs, lhs);
                        case OrderingRelationType::eInOrderOrEquals:
                            return fBASE_COMPARER_ (lhs, rhs) and fBASE_COMPARER_ (rhs, lhs);
                        case OrderingRelationType::eThreeWayCompare:
                            return fBASE_COMPARER_ (lhs, rhs) == 0;
                        default:
                            AssertNotReached ();
                            return false;
                    }
                }

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *  \brief Use this to wrap any basic comparer, and produce a Three-Way comparer
             */
            template <typename BASE_COMPARER>
            struct ThreeWayComparerAdapter {
                constexpr ThreeWayComparerAdapter (const BASE_COMPARER& baseComparer)
                    : fBASE_COMPARER_ (baseComparer)
                {
                }
                template <typename T>
                constexpr int operator() (const T& lhs, const T& rhs) const
                {
                    switch (ComparisonTraits<BASE_COMPARER>::kOrderingRelationKind) {
                        case OrderingRelationType::eInOrder return fBASE_COMPARER_ (lhs, rhs) ? -1:
                            (fBASE_COMPARER_ (rhs, lhs) ? 1 : 0);
                        case OrderingRelationType::eThreeWayCompare:
                            return fBASE_COMPARER_ (lhs, rhs);
                        default:
                            AssertNotReached ();
                            return false;
                    }
                }

            private:
                BASE_COMPARER fBASE_COMPARER_;
            };

            /**
             *           *  THEN can add enable_if or static_assert() into Set/SortedSet so we know rihgt type of comparer? Maybe?
             *  Maybe not easy with getCompareEquals() result (may need differnt type).
             *
             */
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
