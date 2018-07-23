/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_

#include "../Concrete/Stack_LinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************************* Stack_Factory<T> *******************************
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename T>
    atomic<Stack<T> (*) ()> Stack_Factory<T>::sFactory_ (nullptr);
#endif
    template <typename T>
    inline Stack<T> Stack_Factory<T>::operator() () const
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
    inline void Stack_Factory<T>::Register (Stack<T> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename T>
    inline Stack<T> Stack_Factory<T>::Default_ ()
    {
        return Concrete::Stack_LinkedList<T>{};
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_ */
