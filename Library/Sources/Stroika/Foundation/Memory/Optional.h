/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <optional>
#include <shared_mutex>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/Empty.h"
#include "Stroika/Foundation/Containers/Adapters/Adder.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/NullMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      NOTE TO SUGGEST TO C++ standards - 
 *          Things I miss most about my Optional versus std::optional
 *              >   Value () - what they call value_or - should take T{} as default argument. About 25% of teh time
 *                  that's what I want, and its much more clear/terse.
 *
 *              >   Accumulate method, and operator +=, operator-= etc overloads calling Accumulate(). Much simpler
 *                  and more elegant code with those methods.
 */

namespace Stroika::Foundation::Memory {

    /**
     *  \brief  AccumulateIf () add in the rhs argument value to lhs optional, but if both were missing leave 'lhs'
     *          as still missing, and if only RHS available, assign it to the left.
     *
     *  \par Example Usage
     *      \code
     *          optional<int>   accumulator;
     *          optional<int>   SomeFunctionToGetOptionalValue();
     *          if (accumulator or (tmp = SomeFunctionToGetOptionalValue())) {
     *              accumulator = accumulator.Value () + tmp;
     *          }
     *      \endcode
     *      VERSUS
     *      \code
     *          AccumulateIf (&accumulator, SomeFunctionToGetOptionalValue ());
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<Sequence<InternetAddress>>>   addresses;
     *          Memory::AccumulateIf (&addresses, IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()});
     *      \endcode
     *
     *  Notes:
     *      \req    lhsOptionalValue != nullptr (for optional*  first argument)
     *      \note   Overloads that take optional* first argument accumulate in place and return nothing, while
     *              overloads taking optional<T> as the first augment return the computed result.
     *              overloads taking optional<CONTAINER> or optional<CONTAINER>* dont take an op as argument, but assume the operation is 'Add' to the container
     *
     *      \note   ITS CONFUSING direction of if-test for this versus CopyToIf
     *
     *      \note
     *          typical OP arguments would be:
     *              std::plus{} **the default**
     *              std::minus{}
     *              std::multiplies{}
     *              std::divides{}
     */
    template <typename T, convertible_to<T> CONVERTIBLE_TO_T, convertible_to<function<T (T, T)>> OP = plus<T>>
    void AccumulateIf (optional<T>* lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op = OP{});
    template <typename T, convertible_to<function<T (T, T)>> OP = plus<T>>
    void AccumulateIf (optional<T>* lhsOptionalValue, const T& rhsValue, const OP& op = OP{});
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const optional<T>& rhsOptionalValue);
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    void AccumulateIf (optional<CONTAINER<T>>* lhsOptionalValue, const T& rhsValue);
    template <typename T, convertible_to<T> CONVERTIBLE_TO_T, convertible_to<function<T (T, T)>> OP = plus<T>>
    optional<T> AccumulateIf (const optional<T>& lhsOptionalValue, const optional<CONVERTIBLE_TO_T>& rhsOptionalValue, const OP& op = OP{});
    template <typename T, convertible_to<function<T (T, T)>> OP = plus<T>>
    optional<T> AccumulateIf (const optional<T>& lhsOptionalValue, const T& rhsValue, const OP& op = OP{});
    template <typename T, template <typename> typename CONTAINER>
        requires (is_convertible_v<typename Containers::Adapters::Adder<CONTAINER<T>>::value_type, T>)
    optional<CONTAINER<T>> AccumulateIf (const optional<CONTAINER<T>>& lhsOptionalValue, const optional<T>& rhsOptionalValue);
    template <typename T, template <typename> typename CONTAINER>
    optional<CONTAINER<T>> AccumulateIf (const optional<CONTAINER<T>>& lhsOptionalValue, const T& rhsValue);

    /**
     *  Assign (overwriting) the value held by this optional (first argument) if one is present with destination (second) argument if engaged. Assigns from right to left.
     *
     *  The point of this to to facilitate a common idiom, where you want to maintain an existing value unless you
     *  get an update. This function is ANALAGOUS to
     *      if (o.has_value()) {
     *          destArgVal = *o;
     *      }
     *
     *  but can be done in a single line.
     *
     *  \par Example Usage
     *      \code
     *          int curValue = 3;
     *          Memory::CopyToIf (&curValue, someMap.Lookup (KEY_VALUE));   // curValue will be 3, or overwritten by whatever value MAY have been in someMap
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          optional<int> curValue = getSomeValue ();
     *          optional<long>  oVal = someMap.Lookup (KEY_VALUE);
     *          Memory::CopyToIf (&curValue, oVal);      // curValue retains its value from before CopyToIf if oVal was missing
     *      \endcode
     *
     *  @see Value
     * 
     *  @todo CONSIDER overload where first arg is not ptr (by value) and result is written to return value
     */
    template <typename T, typename CONVERTABLE_TO_TYPE>
    void CopyToIf (CONVERTABLE_TO_TYPE* to, const optional<T>& copyFromIfHasValue);
    template <typename T, typename CONVERTABLE_TO_OPTIONAL_OF_TYPE>
    void CopyToIf (optional<CONVERTABLE_TO_OPTIONAL_OF_TYPE>* to, const optional<T>& copyFromIfHasValue);

    namespace Private_ {
        template <typename T>
        concept INullCoalescable = requires (T t) {
            static_cast<bool> (t);
            *t;
        };
        template <typename OT>
        using OptionalType2ValueType = remove_cvref_t<decltype (*declval<OT> ())>;
    }

    /**
     * \brief return one of l, or r, with first preference for which is engaged, and second preference for left-to-right.
     *
     *  So Equivalent to (depending on overload)
     *      static_cast<bool> (l)? l : r;
     *      or 
     *      static_cast<bool> (l)? *l : r;
     * 
     *  This is similar to/inspired by C# ?? operator  (https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/operators/null-coalescing-operator)
     *
     *  \note This is handy because there is no default argument for std::optional<>::value_or () - there should be (like this).
     *  \note a bit like value_or, but RHS arg can be optional or T, and depending returns optional or T and this takes default value
     *  @see Value ()
     *
     *  \par Example Usage
     *      \code
     *          optional<uint64_t> workingOrResidentSetSize = NullCoalesce (thisProcess.fWorkingSetSize, thisProcess.fResidentMemorySize);
     *          uint64_t useMemUsageSz = NullCoalesce (workingOrResidentSetSize, 1024);
     *          uint64_t useMemUsageSz2 = NullCoalesce (workingOrResidentSetSize);
     *      \endcode
     *
     *  \note NullCoalesce overloads returns a const T& internal pointer: that means the caller
     *        MAY need to be careful to finish using the result of the function before the end of the full expression calling NullCoalesce ().
     */
    template <Private_::INullCoalescable OT>
    const OT& NullCoalesce (const OT& l, const OT& r);
    template <Private_::INullCoalescable OT, convertible_to<const Private_::OptionalType2ValueType<OT>&> DEFAULT_TYPE = Private_::OptionalType2ValueType<OT>>
    const Private_::OptionalType2ValueType<OT>& NullCoalesce (const OT& l, const DEFAULT_TYPE& r = DEFAULT_TYPE{});

    /**
     *  \brief Same as *t, but Requires that 't' is engaged.
     *  \note operator* for optional returns a const T& internal pointer, and so does this. That means the caller
     *        MAY need to be careful to finish using the result of the function before the end of the full expression calling ValueOf ().
     *        But again, this is the same as if they used *v, which is the obvious alternative.
     */
    template <typename T>
    constexpr const T& ValueOf (const optional<T>& t);

    /**
     */
    template <typename EXCEPT, typename T>
    const T& ValueOfOrThrow (const optional<T>& t, const EXCEPT& throwIfNull = {});

    /**
     *  wrappers on std c++23 monadic optional support, til we can assume c++23
     */
    template <typename T, class F>
    constexpr auto And_Then (const optional<T>& o, F&& f)
    {
#if __cplusplus > 202302L || _HAS_CXX23 || (_LIBCPP_STD_VER >= 23)
        return o.and_then (forward<F> (f));
#else
        if (o.has_value ()) {
            return std::invoke (std::forward<F> (f), *o);
        }
        else {
            return std::remove_cvref_t<std::invoke_result_t<F, T>>{};
        }
#endif
    }
    /**
     *  wrappers on std c++23 monadic optional support, til we can assume c++23
     */
    template <typename T, class F>
    constexpr auto Or_Else (const optional<T>& o, F&& f)
    {
#if __cplusplus > 202302L || _HAS_CXX23 || (_LIBCPP_STD_VER >= 23)
        return o.or_else (forward<F> (f));
#else
        if (o.has_value ()) {
            return o;
        }
        else {
            return forward<F> (f) ();
        }
#endif
    }
    /**
     *  wrappers on std c++23 monadic optional support, til we can assume c++23
     */
    template <typename T, class F>
    constexpr auto Transform (const optional<T>& o, F&& f)
    {
#if __cplusplus > 202302L || _HAS_CXX23 || (_LIBCPP_STD_VER >= 23)
        return o.transform (forward<F> (f));
#else
        using U = std::remove_cv_t<std::invoke_result_t<F, T>>;
        if (o.has_value ()) {
            return optional<U>{forward<F> (f) (*o)};
        }
        else {
            return optional<U>{};
        }
#endif
    }

    /**
     *  'Constructor' taking const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* is to allow easier interoperability
     *  with code that uses null-pointers to mean 'is-missing': nullptr means missing, and if non null,
     *  dereference and copy.
     *
     *  \par Example Usage
     *      \code
     *          float*  d1 = nullptr;
     *          double* d2 = nullptr;
     *          EXPECT_TRUE (not OptionalFromNullable (d1).has_value ());
     *          EXPECT_TRUE (not OptionalFromNullable (d2).has_value ());
     *      \endcode
     */
    template <typename RHS_CONVERTIBLE_TO_OPTIONAL_OF_T, constructible_from<RHS_CONVERTIBLE_TO_OPTIONAL_OF_T> T = RHS_CONVERTIBLE_TO_OPTIONAL_OF_T>
    constexpr optional<T> OptionalFromNullable (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T* from);

    /**
     *  if lhs and rhs engaged, this returns *lhs + *rhs, and otherwise nullopt
     * 
     *  \note this used to use AccumulateIf() before Stroika 2.1b12, but that produced confusing results. This is
     *        slightly safer, I think, and if you want the AccumulateIf () semantics, call AccumulateIf()
     */
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const optional<T>& rhs);
    template <typename T>
    optional<T> operator+ (const optional<T>& lhs, const T& rhs);
    template <typename T>
    optional<T> operator+ (const T& lhs, const optional<T>& rhs);

    /**
     *  if lhs and rhs engaged, this returns *lhs - *rhs, and otherwise nullopt
     * 
     *  \note this used to use AccumulateIf() before Stroika 2.1b12, but that produced confusing results. This is
     *        slightly safer, I think, and if you want the AccumulateIf () semantics, call AccumulateIf()
     */
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const optional<T>& rhs);
    template <typename T>
    optional<T> operator- (const optional<T>& lhs, const T& rhs);
    template <typename T>
    optional<T> operator- (const T& lhs, const optional<T>& rhs);

    /**
     *  if lhs and rhs engaged, this returns *lhs * *rhs, and otherwise nullopt
     * 
     *  \note this used to use AccumulateIf() before Stroika 2.1b12, but that produced confusing results. This is
     *        slightly safer, I think, and if you want the AccumulateIf () semantics, call AccumulateIf()
     */
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const optional<T>& rhs);
    template <typename T>
    optional<T> operator* (const optional<T>& lhs, const T& rhs);
    template <typename T>
    optional<T> operator* (const T& lhs, const optional<T>& rhs);

    /**
     *  if lhs and rhs engaged, this returns *lhs / *rhs, and otherwise nullopt
     * 
     *  \note this used to use AccumulateIf() before Stroika 2.1b12, but that produced confusing results. This is
     *        slightly safer, I think, and if you want the AccumulateIf () semantics, call AccumulateIf()
     */
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const optional<T>& rhs);
    template <typename T>
    optional<T> operator/ (const optional<T>& lhs, const T& rhs);
    template <typename T>
    optional<T> operator/ (const T& lhs, const optional<T>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Optional.inl"

#endif /*_Stroika_Foundation_Memory_Optional_h_*/
