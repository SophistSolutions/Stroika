/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_

#include "../Concrete/SortedMapping_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ************ SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> ***************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                atomic<SortedMapping<KEY_TYPE, VALUE_TYPE> (*) ()> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::sFactory_ (nullptr);
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline SortedMapping<KEY_TYPE, VALUE_TYPE> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::operator() () const
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
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Register (SortedMapping<KEY_TYPE, VALUE_TYPE> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline SortedMapping<KEY_TYPE, VALUE_TYPE> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_ ()
                {
                    return Concrete::SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE> ();
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_ */
