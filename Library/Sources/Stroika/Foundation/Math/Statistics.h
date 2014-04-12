/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Statistics_h_
#define _Stroika_Foundation_Math_Statistics_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Look at http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=290
 *
 *      @todo   Math statuiscs utilities:
 *              Mean()
 *              Mode()
 *              Median()
 *          If random-accessable iteraotrs, (array etc) – use
 *          http://en.cppreference.com/w/cpp/algorithm/nth_element
 *          else copy to vector<> and then use nth_element?
 *          Just need something quickie…
 *
 *          Use std::nth_element from <algorithm> which is O(N):
 *          nth_element(a, a + size / 2, a + size);
 *          median = a[size/2];
 *
 *      @todo   Add 'quartile' or some other such functions.
 *
 *      @todo   Do template specializaitons that copy to vector<> or some such, when you cannot
 *              do random-access
 *
 *      @todo   redo templates so can specify larger accumulator type for Mean()
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /**
             *  SUPER ROUGH DRAFT
             *  \req not empty
             */
            template    <typename   ITERATOR_OF_T>
            auto    Mean (ITERATOR_OF_T start, ITERATOR_OF_T end) -> typename   remove_reference<typename remove_cv<decltype (*start)>::type>::type;
            template    <typename   CONTAINER_OF_T>
            auto    Mean (const CONTAINER_OF_T& container) -> typename  remove_reference<typename remove_cv<decltype (*container.begin ())>::type>::type;


            /**
             *  SUPER ROUGH DRAFT
             *  \req not empty
             */
            template    <typename   ITERATOR_OF_T>
            auto    Median (ITERATOR_OF_T start, ITERATOR_OF_T end) -> typename remove_reference<typename remove_cv<decltype (*start)>::type>::type;
            template    <typename   CONTAINER_OF_T>
            auto    Median (const CONTAINER_OF_T& container) -> typename    remove_reference<typename remove_cv<decltype (*container.begin ())>::type>::type;



        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Statistics.inl"

#endif  /*_Stroika_Foundation_Math_Statistics_h_*/
