/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_

#include    "../Concrete/Sequence_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 **************************** Sequence_Factory<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                atomic<Sequence<T> (*) (ContainerUpdateIteratorSafety)>  Sequence_Factory<T>::sFactory_ (nullptr);
                template    <typename T>
                inline  Sequence<T>  Sequence_Factory<T>::mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
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
                        return Default_ (containerUpdateSafetyPolicy);
                    }
                    else {
                        return f (containerUpdateSafetyPolicy);
                    }
                }
                template    <typename T>
                void    Sequence_Factory<T>::Register (Sequence<T> (*factory) (ContainerUpdateIteratorSafety))
                {
                    sFactory_ = factory;
                }
                template    <typename T>
                inline  Sequence<T>  Sequence_Factory<T>::Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    return Sequence_Array<T> (containerUpdateSafetyPolicy);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_ */
