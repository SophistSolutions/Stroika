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


                template    <typename T, typename TRAITS>
                inline  SortedTally<T, TRAITS>  mkSortedTally_Default ()
                {
                    return SortedTally_stdmap<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedTally_Factory_inl_ */
