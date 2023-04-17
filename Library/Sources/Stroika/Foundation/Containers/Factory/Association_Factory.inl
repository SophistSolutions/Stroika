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
    constexpr Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Association_Factory (const Hints& hints)
    {
        if (auto f = sDefaultFactory_.load ()) {
            this->fFactory_ = [f] (const KEY_EQUALS_COMPARER& keyEqualsComparer) { return (*f) (keyEqualsComparer); };
        }
        else {
            if constexpr (is_same_v<KEY_EQUALS_COMPARER, equal_to<KEY_TYPE>> and Configuration::has_lt_v<KEY_TYPE>) {
                this->fFactory_ = [] (const KEY_EQUALS_COMPARER&) {
                    return Concrete::Association_stdmultimap<KEY_TYPE, VALUE_TYPE>{}; // OK to omit comparer, because we have less-than defined and using default equal_to<>
                };
            }
            else {
                if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                    // array has better memory cache locality properties so lookups probably faster
                    this->fFactory_ = [] (const KEY_EQUALS_COMPARER& keyEqualsComparer) {
                        return Concrete::Association_Array<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
                    };
                }
                else {
                    this->fFactory_ = [] (const KEY_EQUALS_COMPARER& keyEqualsComparer) {
                        return Concrete::Association_LinkedList<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
                    };
                }
            }
        }
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    constexpr Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Association_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, VALUE_TYPE>
    Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::operator() (const KEY_EQUALS_COMPARER& keyEqualsComparer) const
    {
        return this->fFactory_ (keyEqualsComparer);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    void Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Register (optional<Association_Factory> f)
    {
        sDefaultFactory_ = f.has_value () ? make_shared<Association_Factory> (*f) : nullptr;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_ */
