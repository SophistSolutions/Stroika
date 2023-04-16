/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_

#include "../Concrete/Set_Array.h"
#include "../Concrete/Set_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************************ Set_Factory<T, EQUALS_COMPARER> ***********************
     ********************************************************************************
     */
    template <typename T, typename EQUALS_COMPARER>
    constexpr Set_Factory<T, EQUALS_COMPARER>::Set_Factory (const EQUALS_COMPARER& equalsComparer, const Hints& hints)
        : fEqualsComparer_{equalsComparer}
        , fHints_{hints}
    {
    }
    template <typename T, typename EQUALS_COMPARER>
    inline Set<T> Set_Factory<T, EQUALS_COMPARER>::operator() () const
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (fEqualsComparer_);
        }
        else {
            return Default_ (fEqualsComparer_);
        }
    }
    template <typename T, typename EQUALS_COMPARER>
    inline void Set_Factory<T, EQUALS_COMPARER>::Register (Set<T> (*factory) (const EQUALS_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename T, typename EQUALS_COMPARER>
    inline Set<T> Set_Factory<T, EQUALS_COMPARER>::Default_ ([[maybe_unused]] const EQUALS_COMPARER& equalsComparer)
    {
        if constexpr (is_same_v<EQUALS_COMPARER, equal_to<T>> and Configuration::has_lt_v<T>) {
            return Concrete::Set_stdset<T>{};
        }
        else {
            /*
             *  Not good for large sets, due to lack of indexing/quick lookup. So issue with realloc not such a biggie
             *  and probably better than linkedlist since better locality (and have to walk whole list anyhow to see if present).
             */
            return Concrete::Set_Array<T>{equalsComparer};
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_ */
