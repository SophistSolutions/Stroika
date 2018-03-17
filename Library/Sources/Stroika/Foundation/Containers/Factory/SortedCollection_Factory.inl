/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_

#include "../Concrete/SortedCollection_LinkedList.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 **************** SortedCollection_Factory<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                atomic<SortedCollection<T, TRAITS> (*) ()> SortedCollection_Factory<T, TRAITS>::sFactory_ (nullptr);
                template <typename T, typename TRAITS>
                inline SortedCollection<T, TRAITS> SortedCollection_Factory<T, TRAITS>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f ();
                    }
                    else {
                        return Default_ ();
                    }
                }
                template <typename T, typename TRAITS>
                void SortedCollection_Factory<T, TRAITS>::Register (SortedCollection<T, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename TRAITS>
                inline SortedCollection<T, TRAITS> SortedCollection_Factory<T, TRAITS>::Default_ ()
                {
                    return Concrete::SortedCollection_LinkedList<T, TRAITS> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
