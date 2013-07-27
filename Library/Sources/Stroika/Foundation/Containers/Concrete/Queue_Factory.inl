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


                /*
                 ********************************************************************************
                 *************************** Queue_Factory<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Queue<T, TRAITS> (*) ()>     Queue_Factory<T, TRAITS>::sFactory_ (&Default_);
                template    <typename T, typename TRAITS>
                inline  Queue<T, TRAITS>  Queue_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Queue_Factory<T, TRAITS>::Register (Queue<T, TRAITS> (*factory) ())
                {
                    sFactory_ = (factory == nullptr) ? &Default_ : factory;
                }
                template    <typename T, typename TRAITS>
                Queue<T, TRAITS>  Queue_Factory<T, TRAITS>::Default_ ()
                {
                    return Queue_DoublyLinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_ */
