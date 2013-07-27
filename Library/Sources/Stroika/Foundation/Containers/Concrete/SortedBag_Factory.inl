/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedBag_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedBag_Factory_inl_

#include    "SortedBag_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 *********************** SortedBag_Factory<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<SortedBag<T, TRAITS> (*) ()> SortedBag_Factory<T, TRAITS>::sFactory_  =   &Default_;;

                template    <typename T, typename TRAITS>
                inline  SortedBag<T, TRAITS>  SortedBag_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    SortedBag_Factory<T, TRAITS>::Register (SortedBag<T, TRAITS> (*factory) ())
                {
                }
                template    <typename T, typename TRAITS>
                SortedBag<T, TRAITS>  SortedBag_Factory<T, TRAITS>::Default_ ()
                {
                    return SortedBag_LinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
