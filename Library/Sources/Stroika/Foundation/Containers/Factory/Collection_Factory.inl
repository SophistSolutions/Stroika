/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_

#include "../Concrete/Collection_LinkedList.h"
#include "../Concrete/Collection_stdmultiset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ****************************** Collection_Factory<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection<T> Collection_Factory<T>::operator() () const
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
    template <typename T>
    void Collection_Factory<T>::Register (Collection<T> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename T>
    inline Collection<T> Collection_Factory<T>::Default_ ()
    {
        constexpr bool kUse_stdmultiset_IfPossible_ = false;
        if constexpr (Configuration::has_lt_v<T> and kUse_stdmultiset_IfPossible_) {
            return Concrete::Collection_stdmultiset<T>{};
        }
        else {
            // This generally performs well, so long as you don't call 'size'
            return Concrete::Collection_LinkedList<T>{};
        }
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_ */
