/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_

#include    "../Concrete/SortedCollection_LinkedList.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 **************** SortedCollection_Factory<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<SortedCollection<T, TRAITS> (*) (ContainerUpdateIteratorSafety)>     SortedCollection_Factory<T, TRAITS>::sFactory_ (nullptr);
                template    <typename T, typename TRAITS>
                inline  SortedCollection<T, TRAITS>  SortedCollection_Factory<T, TRAITS>::mk (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
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
                void    SortedCollection_Factory<T, TRAITS>::Register (SortedCollection<T, TRAITS> (*factory) (ContainerUpdateIteratorSafety))
                {
                    sFactory_ = factory;
                }
                template    <typename T, typename TRAITS>
                inline  SortedCollection<T, TRAITS>  SortedCollection_Factory<T, TRAITS>::Default_ (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                {
                    return SortedCollection_LinkedList<T, TRAITS> (containerUpdateSafetyPolicy);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
