/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_

#include    "Sequence_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 *********************** Sequence_Factory<T, TRAITS> ****************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Sequence<T, TRAITS> (*) ()>  Sequence_Factory<T, TRAITS>::sFactory_ (nullptr);
                template    <typename T, typename TRAITS>
                inline  Sequence<T, TRAITS>  Sequence_Factory<T, TRAITS>::mk ()
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     *
                     */
                    return (sFactory_.load () == nullptr) ? Default_() : sFactory_ ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_Factory<T, TRAITS>::Register (Sequence<T, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename T, typename TRAITS>
                Sequence<T, TRAITS>  Sequence_Factory<T, TRAITS>::Default_ ()
                {
                    return Sequence_Array<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_ */
