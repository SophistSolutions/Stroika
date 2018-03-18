/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include "../Concrete/SortedSet_stdset.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

#if 0
                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template <typename T>
                class SortedSet_stdset;
#endif

                /*
                 ********************************************************************************
                 ************************ SortedSet_Factory<T, TRAITS> **************************
                 ********************************************************************************
                 */
                template <typename T, typename LESS_COMPARER>
                atomic<SortedSet<T> (*) ()> SortedSet_Factory<T, LESS_COMPARER>::sFactory_ (nullptr);
                template <typename T, typename LESS_COMPARER>
                inline SortedSet<T> SortedSet_Factory<T, LESS_COMPARER>::operator() () const
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
                        return Default_ (std::less<T> ());
                    }
                }
                template <typename T, typename LESS_COMPARER>
                inline SortedSet<T> SortedSet_Factory<T, LESS_COMPARER>::operator() (const LESS_COMPARER& lessComparer) const
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
                        return Default_ (lessComparer);
                    }
                }
                template <typename T, typename LESS_COMPARER>
                void SortedSet_Factory<T, LESS_COMPARER>::Register (SortedSet<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename LESS_COMPARER>
                inline SortedSet<T> SortedSet_Factory<T, LESS_COMPARER>::Default_ (const LESS_COMPARER& lessComparer)
                {
                    return Concrete::SortedSet_stdset<T> (lessComparer);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
