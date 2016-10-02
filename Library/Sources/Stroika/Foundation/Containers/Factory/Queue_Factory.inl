/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_

#include    "../Concrete/Queue_DoublyLinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************************* Queue_Factory<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                atomic<Queue<T> (*) (ContainerUpdateIteratorSafety)>     Queue_Factory<T>::sFactory_ (nullptr);
                template    <typename T>
                inline  Queue<T>  Queue_Factory<T>::mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    auto f = sFactory_.load ();
                    if (f == nullptr) {
                        return Default_ (containerUpdateSafetyPolicy);
                    }
                    else {
                        return f (containerUpdateSafetyPolicy);
                    }
                }
                template    <typename T>
                void    Queue_Factory<T>::Register (Queue<T> (*factory) (ContainerUpdateIteratorSafety))
                {
                    sFactory_ = factory;
                }
                template    <typename T>
                inline  Queue<T>  Queue_Factory<T>::Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    return Queue_DoublyLinkedList<T> (containerUpdateSafetyPolicy);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_ */
