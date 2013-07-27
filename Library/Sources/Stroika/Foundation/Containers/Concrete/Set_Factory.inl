/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_

#include    "Set_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************** Set_Factory<T, TRAITS> ******************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Set<T, TRAITS> (*) ()>   Set_Factory<T, TRAITS>::sFactory_ (&Default_);
                template    <typename T, typename TRAITS>
                inline  Set<T, TRAITS>  Set_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Set_Factory<T, TRAITS>::Register (Set<T, TRAITS> (*factory) ())
                {
                    sFactory_ = (factory == nullptr) ? &Default_ : factory;
                }
                template    <typename T, typename TRAITS>
                Set<T, TRAITS>  Set_Factory<T, TRAITS>::Default_ ()
                {
                    return Set_LinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_ */
