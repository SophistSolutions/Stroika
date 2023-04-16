/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_

#include "../Concrete/Association_Array.h"
#include "../Concrete/Association_LinkedList.h"
#include "../Concrete/Association_stdmultimap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ***** Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    constexpr Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Association_Factory (const KEY_EQUALS_COMPARER& keyEqualsComparer,
                                                                                                   const Hints& hints)
        : fKeyEqualsComparer_{keyEqualsComparer}
        , fHints_{hints}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::operator() () const
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (fKeyEqualsComparer_);
        }
        else {
            return Default_ (fKeyEqualsComparer_, fHints_);
        }
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    void Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_EQUALS_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, VALUE_TYPE>
    Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Default_ ([[maybe_unused]] const KEY_EQUALS_COMPARER& keyEqualsComparer,
                                                                              [[maybe_unused]] const Hints&               hints)
    {
        if constexpr (is_same_v<KEY_EQUALS_COMPARER, equal_to<KEY_TYPE>> and Configuration::has_lt_v<KEY_TYPE>) {
            return Concrete::Association_stdmultimap<KEY_TYPE, VALUE_TYPE>{}; // OK to omit comparer, because we have less-than defined and using default equal_to<>
        }
        else {
            if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                // array has better memory locality properties so lookups faster
                return Concrete::Association_Array<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
            }
            else {
                return Concrete::Association_LinkedList<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
            }
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_ */
