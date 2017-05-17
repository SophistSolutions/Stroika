/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Deque_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Deque_Factory_inl_

#include "../Concrete/Deque_DoublyLinkedList.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ******************************* Deque_Factory<T> *******************************
                 ********************************************************************************
                 */
                template <typename T>
                atomic<Deque<T> (*) ()> Deque_Factory<T>::sFactory_ (nullptr);
                template <typename T>
                inline Deque<T> Deque_Factory<T>::mk ()
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
                        return Default_ ();
                    }
                    else {
                        return f ();
                    }
                }
                template <typename T>
                void Deque_Factory<T>::Register (Deque<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T>
                inline Deque<T> Deque_Factory<T>::Default_ ()
                {
                    return Concrete::Deque_DoublyLinkedList<T> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Deque_Factory_inl_ */
