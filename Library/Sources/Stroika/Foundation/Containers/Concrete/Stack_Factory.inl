/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                 ************************** Stack_Factory<T, TRAITS> ****************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Stack<T, TRAITS> (*) ()> Stack_Factory<T, TRAITS>::sFactory_ (&Default_);

                template    <typename T, typename TRAITS>
                inline  Stack<T, TRAITS>  Stack_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Stack_Factory<T, TRAITS>::Register (Stack<T, TRAITS> (*factory) ())
                {
                    sFactory_ = (factory == nullptr) ? &Default_ : factory;
                }
                template    <typename T, typename TRAITS>
                Stack<T, TRAITS>  Stack_Factory<T, TRAITS>::Default_ ()
                {
                    return Stack_LinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_Factory_inl_ */
