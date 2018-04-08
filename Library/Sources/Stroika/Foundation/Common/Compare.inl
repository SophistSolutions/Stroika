/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_inl_
#define _Stroika_Foundation_Common_Compare_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Common {

            namespace Private_ {
                // deprecated - lose when we lose Has_Compare
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

            template <typename T>
            struct[[deprecated ("in Stroika v2.0a231 - use ThreeWayCompare -- not quite - no way to check for it now... but add if needed")]] Has_Compare : Configuration::substitution_succeeded<typename Private_::Has_Compare_Helper_::get_Compare_result<T>::type>{};

            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_MSC_WARNING_START (4996)
            template <typename T>
            struct [[deprecated ("in Stroika v2.0a231 - use std::equal_to<T>")]] ComparerWithEqualsOptionally {
                using value_type = T;
                static constexpr bool Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
                {
                    return v1 == v2;
                }
            };
            template <typename T>
            struct[[deprecated ("in Stroika v2.0a231 - use std::equal_to<T>")]] ComparerWithEquals : ComparerWithEqualsOptionally<T>{};
            template <typename T>
            struct [[deprecated ("in Stroika v2.0a231 - use ThreeWayCompare instead - but not slight change of API (functor instead of static ::Compare method)")]] ComparerWithWellOrder {
                using value_type = T;
                static constexpr int  Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2);
                static constexpr bool Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
                {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    return not(v1 < v2 or v2 < v1);
#else
                    bool result{not(v1 < v2 or v2 < v1)};
                    return result;
#endif
                }
            };
            template <typename T, typename SFINAE = typename conditional<(Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value), ComparerWithEquals<T>, typename conditional<Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value, ComparerWithWellOrder<T>, shared_ptr<int>>::type>::type>
            struct[[deprecated ("in Stroika v2.0a231 - use std::equal_to<T> instead")]] DefaultEqualsComparer : SFINAE{};
            template <typename T>
            struct[[deprecated ("in Stroika v2.0a231 - use std::equal_to<T> instead")]] DefaultEqualsComparerOptionally : conditional<(Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value) or (Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value), DefaultEqualsComparer<T>, shared_ptr<int>>::type{};
            DISABLE_COMPILER_MSC_WARNING_END (4996)
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");

            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_MSC_WARNING_START (4996)

            namespace Private_ {
                template <typename T>
                inline constexpr int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<Has_Compare<T>::value>::type* = nullptr)
                {
                    return v1.Compare (v2);
                }
                template <typename T>
                inline constexpr int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<not Has_Compare<T>::value>::type* = nullptr)
                {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    return (v1 < v2) ? -1 : ((v2 < v1) ? 1 : 0);
#else
                    if (v1 < v2) {
                        return -1;
                    }
                    else if (v2 < v1) {
                        return 1;
                    }
                    else {
                        return 0;
                    }
#endif
                }
            }

            template <typename T>
            inline constexpr int ComparerWithWellOrder<T>::Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
            {
#if qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return Private_::Compare_Helper_<T> (v1, v2);
#else
                int result{Private_::Compare_Helper_<T> (v1, v2)};
                Assert ((result == 0) == Equals (v1, v2));
                return result;
#endif
            }
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated\"");
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_MSC_WARNING_END (4996)

            namespace STL {
                /**
                *  An STL-compatible variant of 'less' which leverages a stroika comparer. This can be used to use
                *  Stroika comparares (almost directly) with std STL classes such as map<>, and set<>.
                */
                template <typename T, typename STROIKA_COMPARER>
                struct [[deprecated ("in Stroika v2.0a231 - these are now the same type - use as is.)")]] less {
                    // @todo not sure we need first_argument_type/second_argument_type/result_type but there for backward compat (std::binary_function<T,T,bool>)
                    typedef T       first_argument_type;
                    typedef T       second_argument_type;
                    typedef bool    result_type;
                    nonvirtual bool operator() (Configuration::ArgByValueType<T> _Left, Configuration::ArgByValueType<T> _Right) const
                    {
                        return STROIKA_COMPARER::Compare (_Left, _Right) < 0;
                    }
                };
            }

            /*
             ********************************************************************************
             ***************************** CompareNormalizer ********************************
             ********************************************************************************
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
             *********************** InOrderComparerAdapter<BASE_COMPARER> ******************
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
                : fBASE_COMPARER_ (std::move (baseComparer))
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
                return EqualsComparerAdapter<BASE_COMPARER>{std::move (baseComparer)};
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
                return ThreeWayComparerAdapter<BASE_COMPARER>{std::move (baseComparer)};
            }
        }
    }
}
#endif /*_Stroika_Foundation_Common_Compare_inl_*/
