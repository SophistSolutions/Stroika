/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_inl_

#include    "SortedTally_stdmap.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template    <typename T, typename TRAITS>
                class SortedTally_stdmap;


                template    <typename T, typename TRAITS>
                inline  SortedTally<T, TRAITS>  mkSortedTally_Default ()
                {
                    return SortedTally_stdmap<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_inl_ */
