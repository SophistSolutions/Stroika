/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_

#include    "../Concrete/SortedMultiSet_stdmap.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template    <typename T, typename TRAITS>
                class   SortedMultiSet_stdmap;


                /*
                 ********************************************************************************
                 ********************* SortedMultiSet_Factory<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<SortedMultiSet<T, TRAITS> (*) (ContainerUpdateIteratorSafety)>   SortedMultiSet_Factory<T, TRAITS>::sFactory_ (nullptr);
                template    <typename T, typename TRAITS>
                inline  SortedMultiSet<T, TRAITS>  SortedMultiSet_Factory<T, TRAITS>::mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
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
                    return f (containerUpdateSafetyPolicy);
                }
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_Factory<T, TRAITS>::Register (SortedMultiSet<T, TRAITS> (*factory) (ContainerUpdateIteratorSafety))
                {
                    sFactory_ = factory;
                }
                template    <typename T, typename TRAITS>
                SortedMultiSet<T, TRAITS>  SortedMultiSet_Factory<T, TRAITS>::Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    return SortedMultiSet_stdmap<T, TRAITS> (containerUpdateSafetyPolicy);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_ */
