/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/Set_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Set_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************************ Set_Factory<T, EQUALS_COMPARER> ***********************
     ********************************************************************************
     */
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    constexpr Set_Factory<T, EQUALS_COMPARER>::Set_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    constexpr Set_Factory<T, EQUALS_COMPARER>::Set_Factory ()
        : Set_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    constexpr Set_Factory<T, EQUALS_COMPARER>::Set_Factory ([[maybe_unused]] const Hints& hints)
        : Set_Factory{nullptr}
    {
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    inline auto Set_Factory<T, EQUALS_COMPARER>::Default () -> const Set_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    inline auto Set_Factory<T, EQUALS_COMPARER>::operator() (const EQUALS_COMPARER& equalsComparer) const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) [[likely]] {
            if constexpr (same_as<EQUALS_COMPARER, equal_to<T>> and totally_ordered<T>) {
                static const auto kDefault_ = Concrete::Set_stdset<T>{};
                return kDefault_;
            }
            else {
                /*
                 *  Not good for large sets, due to lack of indexing/quick lookup. So issue with realloc not such a biggie
                 *  and probably better than linked list since better locality (and have to walk whole list anyhow to see if present).
                 */
                return Concrete::Set_Array<T>{equalsComparer};
            }
        }
        else {
            return this->fFactory_ (equalsComparer);
        }
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    void Set_Factory<T, EQUALS_COMPARER>::Register (const optional<Set_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Set_Factory{Hints{}};
    }
    template <typename T, IEqualsComparer<T> EQUALS_COMPARER>
    inline auto Set_Factory<T, EQUALS_COMPARER>::AccessDefault_ () -> Set_Factory&
    {
        static Set_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
