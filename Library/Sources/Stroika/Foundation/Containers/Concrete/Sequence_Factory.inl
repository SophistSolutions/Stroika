/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_

#include    "Sequence_stdvector.h"

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
                atomic<Sequence<T> (*) ()>  Sequence_Factory<T>::sFactory_ (nullptr);
                template    <typename T>
                inline  Sequence<T>  Sequence_Factory<T>::mk ()
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
                void    Sequence_Factory<T>::Register (Sequence<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename T>
                Sequence<T>  Sequence_Factory<T>::Default_ ()
                {
                    return Sequence_stdvector<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Factory_inl_ */
