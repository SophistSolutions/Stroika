/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_h_
#define _Stroika_Foundation_Containers_Private_IterableUtils_h_

/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */


#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                constexpr   size_t  kBadIndex   =   numeric_limits<size_t>::max ();


                template    <typename T>
                bool    Contains_ (const Iterable<T>& c, T item);


                template    <typename T>
                int    Compare_ (const Iterable<T>& lhs, const Iterable<T>& rhs);


                template    <typename T>
                int   Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs);


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, T item);


                template    <typename T>
                size_t    IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs);


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Private_IterableUtils_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IterableUtils.inl"
