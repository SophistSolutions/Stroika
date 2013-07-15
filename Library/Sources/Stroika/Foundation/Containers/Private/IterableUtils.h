/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IterableUtils_h_
#define _Stroika_Foundation_Containers_Private_IterableUtils_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"



/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                template    <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                bool    Contains_ (const Iterable<T>& c, T item);


                template    <typename T>
                int     Compare_ (const Iterable<T>& lhs, const Iterable<T>& rhs);


                template    <typename T, typename ELEMENT_COMPARE_EQUALS_TYPE>
                bool    Equals_ (const Iterable<T>& lhs, const Iterable<T>& rhs);


                template    <typename T>
                size_t  IndexOf_ (const Iterable<T>& c, T item, size_t sentinalValueForNotFound);


                template    <typename T>
                size_t  IndexOf_ (const Iterable<T>& c, const Iterable<T>& rhs, size_t sentinalValueForNotFound);


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IterableUtils.inl"

#endif  /*_Stroika_Foundation_Containers_Private_IterableUtils_h_ */
