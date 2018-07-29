/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Common_inl_
#define _Stroika_Foundation_Containers_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <algorithm>

#include "../Math/Common.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************* Start ************************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline typename CONTAINER::value_type* Start (CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0];
    }
    template <typename CONTAINER>
    inline const typename CONTAINER::value_type* Start (const CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0];
    }

    /*
     ********************************************************************************
     ************************************* End **************************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline typename CONTAINER::value_type* End (CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0] + cnt;
    }
    template <typename CONTAINER>
    inline const typename CONTAINER::value_type* End (const CONTAINER& c)
    {
        size_t cnt = c.size ();
        return cnt == 0 ? nullptr : &c[0] + cnt;
    }

    /*
     ********************************************************************************
     ****************************** CompareResultNormalizeHelper ********************
     ********************************************************************************
     */
    template <typename FROM_INT_TYPE, typename TO_INT_TYPE>
    inline TO_INT_TYPE CompareResultNormalizeHelper (FROM_INT_TYPE f)
    {
        if (f < 0) {
            return static_cast<TO_INT_TYPE> (-1);
        }
        else if (f > 0) {
            return static_cast<TO_INT_TYPE> (1);
        }
        else {
            return static_cast<TO_INT_TYPE> (0);
        }
    }

    /*
     ********************************************************************************
     *************************** ReserveSpeedTweekAdjustCapacity ********************
     ********************************************************************************
     */
    inline size_t ReserveSpeedTweekAdjustCapacity (size_t targetCapacity, size_t minChunk)
    {
        size_t capacity{targetCapacity};
        capacity *= 6;
        capacity /= 5;
        capacity = Math::RoundUpTo (capacity, minChunk);
        return capacity;
    }

    /*
     ********************************************************************************
     *************************** ReserveSpeedTweekAddNCapacity **********************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline size_t ReserveSpeedTweekAddNCapacity (const CONTAINER& c, size_t n, size_t minChunk)
    {
        size_t size{c.size () + n};
        size_t capacity{c.capacity ()};
        if (size >= capacity) {
            return ReserveSpeedTweekAdjustCapacity (size, minChunk);
        }
        return static_cast<size_t> (-1);
    }

    /*
     ********************************************************************************
     *************************** ReserveSpeedTweekAddN ******************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline void ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t minChunk)
    {
        size_t size = ReserveSpeedTweekAddNCapacity (c, n, minChunk);
        if (size != -1) {
            c.reserve (size);
        }
    }

    /*
     ********************************************************************************
     *************************** ReserveSpeedTweekAdd1 ******************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    inline void ReserveSpeedTweekAdd1 (CONTAINER& c, size_t minChunk)
    {
        ReserveSpeedTweekAddN (c, 1, minChunk);
    }

}

#endif /*_Stroika_Foundation_Containers_Common_inl_*/
