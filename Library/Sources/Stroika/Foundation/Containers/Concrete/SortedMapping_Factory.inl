/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "SortedMapping_stdmap.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>    mkSortedMapping_Default ()
                {
                    return SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_ */
