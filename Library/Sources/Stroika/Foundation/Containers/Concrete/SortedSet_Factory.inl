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


                template    <typename T>
                SortedSet<T>  mkSortedSet_Default ()
                {
                    return SortedSet_stdset<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
