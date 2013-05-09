/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SortedMapping_stdmap.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename Key, typename T>
                inline  SortedMapping<Key, T>    mkSortedMapping_Default ()
                {
                    return SortedMapping_stdmap<Key, T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_ */
