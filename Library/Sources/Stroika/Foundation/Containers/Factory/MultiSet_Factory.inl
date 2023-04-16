/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_

#include "../Concrete/MultiSet_Array.h"
#include "../Concrete/MultiSet_LinkedList.h"
#include "../Concrete/MultiSet_stdmap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************ MultiSet_Factory<T, TRAITS, EQUALS_COMPARER> **********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    constexpr MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::MultiSet_Factory (const EQUALS_COMPARER& equalsComparer, const Hints& hints)
        : fEqualsComparer_{equalsComparer}
        , fHints_{hints}
    {
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline MultiSet<T, TRAITS> MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::operator() () const
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
            return Default_ (fEqualsComparer_, fHints_);
        }
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline void MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Register (MultiSet<T, TRAITS> (*factory) (const EQUALS_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline MultiSet<T, TRAITS> MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Default_ (const EQUALS_COMPARER& equalsComparer, const Hints& hints)
    {
        if constexpr (is_same_v<EQUALS_COMPARER, equal_to<T>> and Configuration::has_lt_v<T>) {
            return Concrete::MultiSet_stdmap<T, TRAITS>{};
        }
        else {
            if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                // array has better memory locality properties so lookups faster
                return Concrete::MultiSet_Array<T, TRAITS>{equalsComparer};
            }
            else {
                return Concrete::MultiSet_LinkedList<T, TRAITS>{equalsComparer};
            }
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_ */
