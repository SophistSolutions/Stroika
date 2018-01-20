/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_

#include "../Concrete/SortedMultiSet_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-23
                template <typename T, typename TRAITS>
                class SortedMultiSet_stdmap;

                /*
                 ********************************************************************************
                 ********************* SortedMultiSet_Factory<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                atomic<SortedMultiSet<T, TRAITS> (*) ()> SortedMultiSet_Factory<T, TRAITS>::sFactory_ (nullptr);
                template <typename T, typename TRAITS>
                inline SortedMultiSet<T, TRAITS> SortedMultiSet_Factory<T, TRAITS>::New ()
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
                void SortedMultiSet_Factory<T, TRAITS>::Register (SortedMultiSet<T, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename TRAITS>
                inline SortedMultiSet<T, TRAITS> SortedMultiSet_Factory<T, TRAITS>::Default_ ()
                {
                    return Concrete::SortedMultiSet_stdmap<T, TRAITS> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_ */
