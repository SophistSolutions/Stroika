/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Statistics_h_
#define _Stroika_Foundation_Math_Statistics_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Look at http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=290
 *
 *      @todo   Consider something like  typename remove_cv<typename remove_reference<decltype (*container.begin ())>::type>::type for RESULT_TYPE
 *
 *      @todo   Math statistics utilities:
 *              Mean()
 *              Mode()
 *              Median()
 *          If random-accessable iterators, (array etc), use
 *          http://en.cppreference.com/w/cpp/algorithm/nth_element
 *          else copy to vector<> and then use nth_element?
 *          Just need something quickie
 *
 *          Use std::nth_element from <algorithm> which is O(N):
 *          nth_element(a, a + size / 2, a + size);
 *          median = a[size/2];
 *
 *      @todo   Add 'quartile' or some other such functions.
 *
 *      @todo   Do template specializations that copy to vector<> or some such, when you cannot
 *              do random-access
 *
 *      @todo   redo templates so can specify larger accumulator type for Mean()
 */

namespace Stroika::Foundation::Math {

    /**
     *  \brief Mean (average) of a collection of numbers computed
     * 
     *  \req not empty (or start != end)
     * 
     *  \note O(N) time complexity
     */
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
    auto Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T>
    auto Mean (CONTAINER_OF_T&& container) -> typename CONTAINER_OF_T::value_type;
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, typename RESULT_TYPE>
    RESULT_TYPE Mean_R (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end);

    /**
     *  \brief Median of a collection of numbers computed
     * 
     *  \req not empty
     * 
     *  \note O(N) time complexity
     */
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, typename RESULT_TYPE,
              Common::IInOrderComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION = less<RESULT_TYPE>>
    RESULT_TYPE Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare = {});
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2,
              Common::IInOrderComparer<typename iterator_traits<ITERATOR_OF_T>::value_type> INORDER_COMPARE_FUNCTION = less<typename iterator_traits<ITERATOR_OF_T>::value_type>>
    auto Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare = {}) ->
        typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T, typename RESULT_TYPE = typename CONTAINER_OF_T::value_type, Common::IInOrderComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION = less<RESULT_TYPE>>
    RESULT_TYPE Median (CONTAINER_OF_T&& container, INORDER_COMPARE_FUNCTION&& compare = {});


    /**
     *  SUPER ROUGH DRAFT 
     *  \req size of container >= 1
     *
     *  \brief Alias: sd, standard-deviation, stddev
     *  
     *  \see https://en.wikipedia.org/wiki/Standard_deviation
     */
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
    auto StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T>
    auto StandardDeviation (CONTAINER_OF_T&& container) -> typename CONTAINER_OF_T::value_type;
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2, typename RESULT_TYPE>
    RESULT_TYPE StandardDeviation_R (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Statistics.inl"

#endif /*_Stroika_Foundation_Math_Statistics_h_*/
