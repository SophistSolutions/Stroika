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

#include "../Concrete/Collection_Array.h"
#include "../Concrete/Collection_LinkedList.h"
#include "../Concrete/Collection_stdmultiset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ****************************** Collection_Factory<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory ()
        : Collection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory ([[maybe_unused]] const Hints& hints)
        : Collection_Factory{[hints] () -> FactoryFunctionType {
            if constexpr (Configuration::has_lt_v<T>) {
                // faster adds/removes - same size - so better if possible to use (unless very small collections maybe)
                return [] () { return Concrete::Collection_stdmultiset<T>{}; };
            }
            else {
                if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                    // questionable choice. For smaller sizes, probably faster, due to better locality.
                    // but adds can occionally be slow (realloc/O(N)) instead of O(1).
                    return [] () { return Concrete::Collection_Array<T>{}; };
                }
                else {
                    // This generally performs well, so long as you don't call 'size'
                    return [] () { return Concrete::Collection_LinkedList<T>{}; };
                }
            }
        }()}
    {
    }
    template <typename T>
    inline auto Collection_Factory<T>::Default () -> const Collection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Collection_Factory<T>::operator() () const -> ConstructedType
    {
        return this->fFactory_ ();
    }
    template <typename T>
    void Collection_Factory<T>::Register (const optional<Collection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Collection_Factory{Hints{}};
    }
    template <typename T>
    inline auto Collection_Factory<T>::AccessDefault_ () -> Collection_Factory&
    {
        static Collection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_ */
