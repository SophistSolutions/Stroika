/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_

#include    "SortedMapping_stdmap.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                // Not sure why this pre-declare needed on GCC 4.7? Either a bug with my mutual #include file stuff or??? Hmmm...
                // no biggie for now...
                // -- LGP 2013-07-21
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   SortedMapping_stdmap;


                /*
                 ********************************************************************************
                 ************ SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> ***************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                atomic<SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*) ()>  SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::sFactory_ (nullptr);
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>  SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()
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
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Register (SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>  SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_ ()
                {
                    return SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_ */
