/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <algorithm>

#include "Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     ************************************ Mean **************************************
     ********************************************************************************
     */
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, typename RESULT_TYPE>
    auto Mean_R (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> RESULT_TYPE
    {
        Require (start != end); // the mean of 0 items would be undefined
        unsigned int cnt{};
        RESULT_TYPE  result{};
        for (ITERATOR_OF_T i = start; i != forward<ITERATOR_OF_T2> (end); ++i) {
            result += *i;
            ++cnt;
        }
        return result / cnt;
    }
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
    inline auto Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type
    {
        using RETURN_TYPE = typename iterator_traits<ITERATOR_OF_T>::value_type;
        return Mean_R<ITERATOR_OF_T, ITERATOR_OF_T2, RETURN_TYPE> (start, forward<ITERATOR_OF_T2> (end));
    }
    template <ranges::range CONTAINER_OF_T>
    inline auto Mean (CONTAINER_OF_T&& container) -> typename CONTAINER_OF_T::value_type
    {
        Require (not container.empty ());
        using ITERATOR_TYPE  = remove_cvref_t<decltype (begin (container))>;
        using ITERATOR_TYPE2 = remove_cvref_t<decltype (end (container))>;
        using RETURN_TYPE    = typename CONTAINER_OF_T::value_type;
        return Mean_R<ITERATOR_TYPE, ITERATOR_TYPE2, RETURN_TYPE> (begin (container), end (container));
    }

    /*
     ********************************************************************************
     ********************************** Median **************************************
     ********************************************************************************
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, Common::IInOrderComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION>
    RESULT_TYPE Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare)
    {
        Require (start != end);                                                     // the median of no values would be undefined
        Memory::StackBuffer<RESULT_TYPE> tmp{forward<ITERATOR_OF_T> (start), forward<ITERATOR_OF_T2> (end)}; // copy cuz data modified
        size_t                           size = ranges::distance (forward<ITERATOR_OF_T> (start), forward<ITERATOR_OF_T2> (end));
        nth_element (tmp.begin (), tmp.begin () + size / 2, tmp.end (), forward<INORDER_COMPARE_FUNCTION> (compare));
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
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, Common::IInOrderComparer<typename iterator_traits<ITERATOR_OF_T>::value_type> INORDER_COMPARE_FUNCTION>
    inline auto Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare) -> typename iterator_traits<ITERATOR_OF_T>::value_type
    {
        using RETURN_TYPE = typename iterator_traits<ITERATOR_OF_T>::value_type;
        return Median<RETURN_TYPE> (forward<ITERATOR_OF_T> (start), forward<ITERATOR_OF_T2> (end), forward<INORDER_COMPARE_FUNCTION> (compare));
    }
    template <ranges::range CONTAINER_OF_T, Common::IInOrderComparer<typename CONTAINER_OF_T::value_type> INORDER_COMPARE_FUNCTION>
    inline auto Median (CONTAINER_OF_T&& container, INORDER_COMPARE_FUNCTION&& compare = {}) -> typename CONTAINER_OF_T::value_type
    {
        Require (not container.empty ());
        return Median<typename CONTAINER_OF_T::value_type> (begin (container), end (container), forward<INORDER_COMPARE_FUNCTION> (compare));
    }

    /*
     ********************************************************************************
     **************************** StandardDeviation *********************************
     ********************************************************************************
     */
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, typename RESULT_TYPE>
    RESULT_TYPE StandardDeviation_R (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end)
    {
        Require (ranges::distance (start, end) >= 1); // the std-deviation of no values would be undefined
        RESULT_TYPE mean = Mean_R<ITERATOR_OF_T, ITERATOR_OF_T2, RESULT_TYPE> (start, forward<ITERATOR_OF_T2> (end));
        RESULT_TYPE accum{};
        size_t      n{};
        for (auto i = start; i != end; ++i) {
            ++n;
            accum += (*i - mean) * (*i - mean);
        }
        Require (n >= 1); // the std-deviation of no values would be undefined
        return sqrt (accum / (n - 1));
    }
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
    auto StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type
    {
        Require (ranges::distance (start, end) >= 1); // the std-deviation of no values would be undefined
        return StandardDeviation_R<ITERATOR_OF_T, ITERATOR_OF_T2, typename iterator_traits<ITERATOR_OF_T>::value_type> (
            forward<ITERATOR_OF_T> (start), forward<ITERATOR_OF_T2> (end));
    }
    template <ranges::range CONTAINER_OF_T>
    inline auto StandardDeviation (CONTAINER_OF_T&& container) -> typename CONTAINER_OF_T::value_type
    {
        Require (container.size () >= 1); // the std-deviation of no values would be undefined
        using ITERATOR_TYPE  = remove_cvref_t<decltype (begin (container))>;
        using ITERATOR_TYPE2 = remove_cvref_t<decltype (end (container))>;
        using RETURN_TYPE    = typename CONTAINER_OF_T::value_type;
        return StandardDeviation_R<ITERATOR_TYPE, ITERATOR_TYPE2, RETURN_TYPE> (begin (container), end (container));
    }

}
