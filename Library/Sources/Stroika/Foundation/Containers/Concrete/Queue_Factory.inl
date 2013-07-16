/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_

#include    "Queue_DoublyLinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T, typename TRAITS>
                inline  Queue<T, TRAITS>  mkQueue_Default ()
                {
                    return Queue_DoublyLinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_ */
