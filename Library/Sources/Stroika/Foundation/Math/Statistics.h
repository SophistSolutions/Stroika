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
     *  \par Example Usage:
     *      \code
     *           EXPECT_EQ (Mean<double> (vector<int>{1, 3, 5}), 3);
     *      \endcode
     * 
     *  \note O(N) time complexity
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    RESULT_TYPE Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end);
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    auto Mean (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T>
    auto Mean (const CONTAINER_OF_T& container) -> typename CONTAINER_OF_T::value_type;

    /**
     *  \brief Median of a collection of numbers computed
     * 
     *  First template requires explicit RESULT_TYPE argument.
     *  Second and third infer it from the iterator/container arguments.
     * 
     *  \req not empty
     * 
     *  \note O(N) time complexity
     * 
     *  \par Example Usage:
     *      \code
     *           EXPECT_EQ (Median (vector<int>{1, 3, 5}), 3);
     *      \endcode
     * 
     *  \note template arguments changed significantly in Stroika v3.0d10
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2,
              Common::IInOrderComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION = less<RESULT_TYPE>>
    RESULT_TYPE Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare = {});
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2,
              Common::IInOrderComparer<typename iterator_traits<ITERATOR_OF_T>::value_type> INORDER_COMPARE_FUNCTION = less<typename iterator_traits<ITERATOR_OF_T>::value_type>>
    auto Median (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end, INORDER_COMPARE_FUNCTION&& compare = {}) ->
        typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T, Common::IInOrderComparer<typename CONTAINER_OF_T::value_type> INORDER_COMPARE_FUNCTION = less<typename CONTAINER_OF_T::value_type>>
    auto Median (const CONTAINER_OF_T& container, INORDER_COMPARE_FUNCTION&& compare = {}) -> typename CONTAINER_OF_T::value_type;

    /**
     *  \req size of container >= 1
     *
     *  \brief Alias: sd, standard-deviation, stddev
     * 
     *  \brief sqrt (sum(mean-di))/(n-1) - 'Bessel's correction'
     *  
     *  \par Example Usage:
     *      \code
     *           EXPECT_TRUE (Math::NearlyEquals (Math::StandardDeviation (vector<double> ({5, 3, 19, 1})), 8.164966, .0001));
     *      \endcode
     * 
     *  \see https://en.wikipedia.org/wiki/Standard_deviation
     */
    template <typename RESULT_TYPE, input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    RESULT_TYPE StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end);
    template <input_iterator ITERATOR_OF_T, sentinel_for<ITERATOR_OF_T> ITERATOR_OF_T2>
    auto StandardDeviation (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end) -> typename iterator_traits<ITERATOR_OF_T>::value_type;
    template <ranges::range CONTAINER_OF_T>
    auto StandardDeviation (const CONTAINER_OF_T& container) -> typename CONTAINER_OF_T::value_type;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Statistics.inl"

#endif /*_Stroika_Foundation_Math_Statistics_h_*/
