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
     **************** MultiSet_Factory<T, TRAITS, EQUALS_COMPARER> ******************
     ********************************************************************************
     */
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    constexpr MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::MultiSet_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    constexpr MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::MultiSet_Factory ()
        : MultiSet_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    constexpr MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::MultiSet_Factory ([[maybe_unused]] const Hints& hints)
        : MultiSet_Factory{[hints] () -> FactoryFunctionType {
            if constexpr (is_same_v<EQUALS_COMPARER, equal_to<T>> and Configuration::has_lt_v<T>) {
                return [] ([[maybe_unused]]const EQUALS_COMPARER& equalsComparer) { return Concrete::MultiSet_stdmap<T, TRAITS>{}; };
            }
            else {
                if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                    // array has better memory locality properties so lookups faster
                    return [] (const EQUALS_COMPARER& equalsComparer) { return Concrete::MultiSet_Array<T, TRAITS>{equalsComparer}; };
                }
                else {
                    return [] (const EQUALS_COMPARER& equalsComparer) { return Concrete::MultiSet_LinkedList<T, TRAITS>{equalsComparer}; };
                }
            }
        }()}
    {
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline auto MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Default () -> const MultiSet_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline auto MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::operator() (const EQUALS_COMPARER& equalsComparer) const -> ConstructedType
    {
        return this->fFactory_ (equalsComparer);
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    void MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Register (const optional<MultiSet_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : MultiSet_Factory{Hints{}};
    }
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    inline auto MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::AccessDefault_ () -> MultiSet_Factory&
    {
        static MultiSet_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_ */
