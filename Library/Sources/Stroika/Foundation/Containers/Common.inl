/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

}

#endif /*_Stroika_Foundation_Containers_Common_inl_*/
