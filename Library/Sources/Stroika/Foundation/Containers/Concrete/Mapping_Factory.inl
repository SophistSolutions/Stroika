/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_

#include    "Mapping_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>  mkMapping_Default ()
                {
                    /*
                     *  Note - though this is not an efficient implementation of Mapping<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Mapping's - which are often the case.
                     *
                     *  Use explicit initializer of Mapping_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
