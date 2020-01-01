/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_

#include "../Concrete/Queue_DoublyLinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************************* Queue_Factory<T> *******************************
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename T>
    atomic<Queue<T> (*) ()> Queue_Factory<T>::sFactory_ (nullptr);
#endif
    template <typename T>
    inline Queue<T> Queue_Factory<T>::operator() () const
    {
        /*
            *  Would have been more performant to just and assure always properly set, but to initialize
            *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
            *  with containers constructed before main.
            *
            *  This works more generally (and with hopefully modest enough performance impact).
            */
        if (auto f = sFactory_.load ()) {
            return f ();
        }
        else {
            return Default_ ();
        }
    }
    template <typename T>
    inline void Queue_Factory<T>::Register (Queue<T> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename T>
    inline Queue<T> Queue_Factory<T>::Default_ ()
    {
        return Concrete::Queue_DoublyLinkedList<T> ();
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Factory_inl_ */
