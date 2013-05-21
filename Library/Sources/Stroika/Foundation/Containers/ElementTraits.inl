/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_ElementTraits_inl_
#define _Stroika_Foundation_Containers_ElementTraits_inl_

#include    "../Debug/Assertions.h"




namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

#if 0
            template    <typename T>
            inline  bool    TWithCompareEquals<T>::opererator == (const T& lhs, const T& rhs)
            {
                return lhs == rhs;
            }



            template    <typename T>
            inline  unsigned int    TWithCompareEqualsAndHashFunction<T>::HashValue (const T& val)
            {
                // This is NOT a generally suitable hash function, but its often adequate
                // Also - its a super sucky hash function - but will be workable/testable to get started with. The bigger issue is that for structures it could examine intersitial space,
                // and that would be BAD!
                //      -- LGP 2011-09-26
                unsigned int    sum =   0;
                for (const Byte* p = reinterpret_cast<const Byte*> (&val); p <  reinterpret_cast<const Byte*> (&val + 1); ++p) {
                    sum <<= 1;
                    sum += *p * 3 + 1;
                }
            }


            template    <typename T>
            inline  bool    TWithCompareEqualsAndLess<T>::opererator < (const T& lhs, const T& rhs)
            {
                return lhs < rhs;
            }



            template    <typename T>
            inline  bool    TWithCompareEqualsAndLessOrEquals<T>::opererator <= (const T& lhs, const T& rhs)
            {
                return lhs <= rhs;
            }
#endif


        }
    }
}

#endif /* _Stroika_Foundation_Containers_ElementTraits_inl_ */
