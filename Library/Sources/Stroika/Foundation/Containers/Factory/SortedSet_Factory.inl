/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include    "../Concrete/SortedSet_stdset.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template    <typename T, typename TRAITS>
                class   SortedSet_stdset;


                /*
                 ********************************************************************************
                 ************************ SortedSet_Factory<T, TRAITS> **************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<SortedSet<T, TRAITS> (*) (ContainerUpdateIteratorSafety)>     SortedSet_Factory<T, TRAITS>::sFactory_ (nullptr);
                template    <typename T, typename TRAITS>
                inline  SortedSet<T, TRAITS>  SortedSet_Factory<T, TRAITS>::mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
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
                template    <typename T, typename TRAITS>
                void    SortedSet_Factory<T, TRAITS>::Register (SortedSet<T, TRAITS> (*factory) (ContainerUpdateIteratorSafety))
                {
                    sFactory_ = factory;
                }
                template    <typename T, typename TRAITS>
                inline  SortedSet<T, TRAITS>  SortedSet_Factory<T, TRAITS>::Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    return SortedSet_stdset<T, TRAITS> (containerUpdateSafetyPolicy);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
