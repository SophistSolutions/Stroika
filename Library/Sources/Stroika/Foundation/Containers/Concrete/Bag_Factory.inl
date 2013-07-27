/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_Factory_inl_

#include    "Bag_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {



                template    <typename T, typename TRAITS>
                Bag<T, TRAITS>       (*Bag_Factory<T, TRAITS>::sFactory_) ()      =   &Default_;

                template    <typename T, typename TRAITS>
                inline  Bag<T, TRAITS>  Bag_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_Factory<T, TRAITS>::Register (Bag<T, TRAITS> (*factory) ())
                {
                }
                template    <typename T, typename TRAITS>
                Bag<T, TRAITS>  Bag_Factory<T, TRAITS>::Default_ ()
                {
                    return Bag_Array<T, TRAITS> ();
                }



                template    <typename T, typename TRAITS>
                Bag<T, TRAITS>  mkBag_Default ()
                {
                    return Bag_Array<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Bag_Factory_inl_ */
