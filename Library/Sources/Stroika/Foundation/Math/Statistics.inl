/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <algorithm>
#include <ranges>

#include "Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     ************************************ Mean **************************************
     ********************************************************************************
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    auto Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> RESULT_TYPE
    {
        // 'end' is only ever compared against here (and the loop below re-reads it every iteration), so it is
        // never forwarded - there is nothing to move it into.
        Require (start != end); // the mean of 0 items would be undefined
        unsigned int cnt{};
        RESULT_TYPE  result{};
        for (ITERATOR_OF_T i = start; i != end; ++i) {
            result += *i;
            ++cnt;
        }
        return result / cnt;
    }
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    inline auto Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type
    {
        Require (start != end); // the mean of 0 items would be undefined - compare only, so no forward here
        return Mean<typename iterator_traits<ITERATOR_OF_T>::value_type> (start, forward<ITERATOR_OF_T2> (end)); // last use
    }
    template <ranges::range CONTAINER_OF_T>
    inline auto Mean (const CONTAINER_OF_T& container) -> typename CONTAINER_OF_T::value_type
    {
        Require (not container.empty ());
        return Mean<typename CONTAINER_OF_T::value_type> (ranges::begin (container), ranges::end (container));
    }

    /*
     ********************************************************************************
     ********************************** Median **************************************
     ********************************************************************************
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2, Common::IInOrderComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION>
    RESULT_TYPE Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare)
    {
        // @todo only do the COPY conditionally - if ITERATOR_OF_T isn't already a random-access iterator...
        // NB: 'end' and 'compare' are each forwarded at most ONCE, at their last use, and passed as plain
        // lvalues before that. nth_element () takes its comparer BY VALUE, so forwarding into the first call
        // would move out of 'compare' and leave the second call (below, for the even case) reading a
        // moved-from object. Harmless for a stateless comparer, silently wrong for a stateful one.
        Require (start != end);                                                     // the median of no values would be undefined
        Memory::StackBuffer<RESULT_TYPE> tmp{start, forward<ITERATOR_OF_T2> (end)}; // copy cuz data modified
        size_t                           size = tmp.size ();
        nth_element (tmp.begin (), tmp.begin () + size / 2, tmp.end (), compare);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\""); // warning with gcc cross-compile to raspberrypi - no idea why --LGP 2018-09-13
        RESULT_TYPE result{tmp[size / 2]};
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"");
        if ((size % 2) == 0) {
            Assert (size >= 2); // cuz require at start >=1 and since even
            // NB: Could use sort instead of nth_element, and some on the web suggest faster, but sort is O(n*log(n)), and nth_element is O(n) (even
            // when you do it twice.
            nth_element (tmp.begin (), tmp.begin () + size / 2 - 1, tmp.end (), forward<INORDER_COMPARE_FUNCTION> (compare));
            result += tmp[size / 2 - 1];
            result /= 2;
        }
        return result;
    }
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2, Common::IInOrderComparer<typename iterator_traits<ITERATOR_OF_T>::value_type> INORDER_COMPARE_FUNCTION>
    inline auto Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare) ->
        typename iterator_traits<ITERATOR_OF_T>::value_type
    {
        return Median<typename iterator_traits<ITERATOR_OF_T>::value_type> (start, forward<ITERATOR_OF_T2> (end),
                                                                            forward<INORDER_COMPARE_FUNCTION> (compare));
    }
    template <ranges::range CONTAINER_OF_T, Common::IInOrderComparer<typename CONTAINER_OF_T::value_type> INORDER_COMPARE_FUNCTION>
    inline auto Median (const CONTAINER_OF_T& container, INORDER_COMPARE_FUNCTION&& compare) -> typename CONTAINER_OF_T::value_type
    {
        Require (not container.empty ());
        return Median<typename CONTAINER_OF_T::value_type> (ranges::begin (container), ranges::end (container),
                                                            forward<INORDER_COMPARE_FUNCTION> (compare));
    }

    /*
     ********************************************************************************
     **************************** StandardDeviation *********************************
     ********************************************************************************
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    RESULT_TYPE StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end)
    {
        // 'end' is used again by the loop below, so it is NOT forwarded into Mean (). It cannot be passed as a
        // plain lvalue either - Mean () takes ITERATOR_OF_T2&& constrained by sentinel_for<>, and an lvalue would
        // deduce that to a reference type, which is not semiregular<> and so fails the constraint. Hand it a copy.
        Require (start != end); // the std-deviation of no values would be undefined
        RESULT_TYPE mean = Mean<RESULT_TYPE> (start, remove_cvref_t<ITERATOR_OF_T2>{end});
        RESULT_TYPE accum{};
        size_t      n{};
        for (auto i = start; i != end; ++i) {
            ++n;
            accum += (*i - mean) * (*i - mean);
        }
        Require (n >= 1); // the std-deviation of no values would be undefined
        return sqrt (accum / (n - 1));
    }
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    inline auto StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) ->
        typename iterator_traits<remove_cvref_t<ITERATOR_OF_T>>::value_type
    {
        Require (start != end); // the std-deviation of no values would be undefined - compare only, so no forward
        return StandardDeviation<typename iterator_traits<ITERATOR_OF_T>::value_type> (start, forward<ITERATOR_OF_T2> (end)); // last use
    }
    template <ranges::range CONTAINER_OF_T>
    inline auto StandardDeviation (const CONTAINER_OF_T& container) -> typename CONTAINER_OF_T::value_type
    {
        Require (not container.empty ()); // the std-deviation of no values would be undefined
        return StandardDeviation<typename CONTAINER_OF_T::value_type> (ranges::begin (container), ranges::end (container));
    }

    /*
     ********************************************************************************
     **************************** ComputeCommonStatistics ***************************
     ********************************************************************************
     */
    template <typename T, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    CommonStatistics<T> ComputeCommonStatistics (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end)
    {
        CommonStatistics<T> results;
        if (start != end) {
            /*
             *  NB: 'end' is consumed up to five times below, ALL ON THE SAME PATH, so it must not be forwarded -
             *  that would move out of it and leave the later calls reading a moved-from sentinel.
             *
             *  It cannot simply be passed as an lvalue either: Mean ()/Median ()/StandardDeviation () take
             *  ITERATOR_OF_T2&& constrained by sentinel_for<>, so an lvalue would deduce ITERATOR_OF_T2 to a
             *  REFERENCE type, and sentinel_for<> requires semiregular<> (hence default_initializable<>), which
             *  no reference type satisfies - the constraint would fail and it would not compile.
             *
             *  So hand each call its own copy, as a prvalue.
             */
            using EndType_ = remove_cvref_t<ITERATOR_OF_T2>;
            if constexpr (Common::IBuiltinArithmetic<T>) {
                results.fMin = *min_element (start, EndType_{end});
                results.fMax = *max_element (start, EndType_{end});
            }
            results.fMean   = Mean (start, EndType_{end});
            results.fMedian = Median (start, EndType_{end});
            if constexpr (Common::IBuiltinArithmetic<T>) {
                results.fStandardDeviation = StandardDeviation (start, EndType_{end});
            }
        }
        return results;
    }
    template <ranges::range CONTAINER_OF_T>
    inline auto ComputeCommonStatistics (const CONTAINER_OF_T& container) -> CommonStatistics<typename CONTAINER_OF_T::value_type>
    {
        return ComputeCommonStatistics<typename CONTAINER_OF_T::value_type> (ranges::begin (container), ranges::end (container));
    }

}
