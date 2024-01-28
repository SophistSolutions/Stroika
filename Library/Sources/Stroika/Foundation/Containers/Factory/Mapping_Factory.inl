/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_

#include <type_traits>

#include "../Concrete/Mapping_Array.h"
#include "../Concrete/Mapping_LinkedList.h"
#include "../Concrete/Mapping_stdhashmap.h"
#include "../Concrete/Mapping_stdmap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ****** Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    constexpr Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Mapping_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    constexpr Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Mapping_Factory ()
        : Mapping_Factory{AccessDefault_ ()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    constexpr Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Mapping_Factory (const Hints& hints)
        : fFactory_{nullptr}
        , fHints_OptimizeForLookupSpeedOverUpdateSpeed{hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline auto Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Default () -> const Mapping_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline auto Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::operator() (const KEY_EQUALS_COMPARER& keyEqualsComparer) const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) [[likely]] {
            if constexpr (is_same_v<KEY_EQUALS_COMPARER, equal_to<KEY_TYPE>> and
                          is_default_constructible_v<Concrete::Mapping_stdhashmap<KEY_TYPE, VALUE_TYPE>>) {
                static const auto kDefault_ = Concrete::Mapping_stdhashmap<KEY_TYPE, VALUE_TYPE>{};
                return kDefault_;
            }
            else if constexpr (is_same_v<KEY_EQUALS_COMPARER, equal_to<KEY_TYPE>> and totally_ordered<KEY_TYPE>) {
                static const auto kDefault_ = Concrete::Mapping_stdmap<KEY_TYPE, VALUE_TYPE>{};
                return kDefault_;
            }
            else {
                if (fHints_OptimizeForLookupSpeedOverUpdateSpeed) [[likely]] {
                    // array has better memory locality properties so lookups faster
                    return Concrete::Mapping_Array<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
                }
                else {
                    return Concrete::Mapping_LinkedList<KEY_TYPE, VALUE_TYPE>{keyEqualsComparer};
                }
            }
        }
        else {
            return this->fFactory_ (keyEqualsComparer);
        }
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    void Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Register (const optional<Mapping_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Mapping_Factory{Hints{}};
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline auto Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::AccessDefault_ () -> Mapping_Factory&
    {
        static Mapping_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
