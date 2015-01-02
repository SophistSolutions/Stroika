/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_

#include    "Stack_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ********************************** Stack_Factory<T> ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                atomic<Stack<T> (*) ()>     Stack_Factory<T>::sFactory_ (nullptr);
                template    <typename T>
                inline  Stack<T>  Stack_Factory<T>::mk ()
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
                        f = &Default_;
                    }
                    return f ();
                }
                template    <typename T>
                void    Stack_Factory<T>::Register (Stack<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename T>
                Stack<T>  Stack_Factory<T>::Default_ ()
                {
                    return Stack_LinkedList<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_ */
