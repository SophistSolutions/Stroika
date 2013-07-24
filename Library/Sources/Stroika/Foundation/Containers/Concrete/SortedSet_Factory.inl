/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include    "SortedSet_stdset.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template    <typename T, typename TRAITS>
                class SortedSet_stdset;


                template    <typename T, typename TRAITS>
                SortedSet<T, TRAITS>  mkSortedSet_Default ()
                {
                    return SortedSet_stdset<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
