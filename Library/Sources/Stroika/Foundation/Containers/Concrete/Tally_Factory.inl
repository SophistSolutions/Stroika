/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_

#include    "Tally_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************ Tally_Factory<T, TRAITS> ******************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Tally<T, TRAITS> (*) ()>     Tally_Factory<T, TRAITS>::sFactory_ (&Default_);
                template    <typename T, typename TRAITS>
                inline  Tally<T, TRAITS>  Tally_Factory<T, TRAITS>::mk ()
                {
                    return sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Factory<T, TRAITS>::Register (Tally<T, TRAITS> (*factory) ())
                {
                    sFactory_ = (factory == nullptr) ? &Default_ : factory;
                }
                template    <typename T, typename TRAITS>
                Tally<T, TRAITS>  Tally_Factory<T, TRAITS>::Default_ ()
                {
                    return Tally_Array<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Tally_Factory_inl_ */
