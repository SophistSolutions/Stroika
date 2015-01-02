/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Common_inl_
#define _Stroika_Foundation_Containers_Common_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    <algorithm>

#include    "../Math/Common.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            template    <typename CONTAINER>
            inline  typename CONTAINER::value_type* Start (CONTAINER& c)
            {
                size_t  cnt =   c.size ();
                return cnt == 0 ? nullptr : &c[0];
            }
            template    <typename CONTAINER>
            inline   const typename CONTAINER::value_type*  Start (const CONTAINER& c)
            {
                size_t  cnt =   c.size ();
                return cnt == 0 ? nullptr : &c[0];
            }
            template    <typename CONTAINER>
            inline  typename CONTAINER::value_type* End (CONTAINER& c)
            {
                size_t  cnt =   c.size ();
                return cnt == 0 ? nullptr : &c[0] + cnt;
            }
            template    <typename CONTAINER>
            inline  const typename CONTAINER::value_type*   End (const CONTAINER& c)
            {
                size_t  cnt =   c.size ();
                return cnt == 0 ? nullptr : &c[0] + cnt;
            }


            template    <typename FROM_INT_TYPE, typename TO_INT_TYPE>
            inline  TO_INT_TYPE CompareResultNormalizeHelper (FROM_INT_TYPE f)
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

            template    <typename   CONTAINER>
            inline  void    ReserveSpeedTweekAdd1 (CONTAINER& c, size_t kMinChunk)
            {
                size_t  size    =   c.size ();
                //Assert (size <= c.capacity ());   we don't want to include the assertion library in this .h file.... for now... --LGP 2007-03-08
                if (size == c.capacity ()) {
                    size *= 6;
                    size /= 5;
                    size = Stroika::Foundation::Math::RoundUpTo (size, kMinChunk);
                    c.reserve (size);
                }
            }

            template    <typename   CONTAINER>
            inline  void    ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t kMinChunk)
            {
                size_t  size    =   c.size () + n;
                if (size >= c.capacity ()) {
                    size *= 6;
                    size /= 5;
                    size = Stroika::Foundation::Math::RoundUpTo (size, kMinChunk);
                    c.reserve (size);
                }
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Common_inl_*/

