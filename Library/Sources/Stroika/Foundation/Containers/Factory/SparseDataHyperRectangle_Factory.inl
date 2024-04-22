/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Concrete/SparseDataHyperRectangle_stdmap.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle_stdmap;
}

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************************ SparseDataHyperRectangle_Factory<T> *******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    constexpr SparseDataHyperRectangle_Factory<T, INDEXES...>::SparseDataHyperRectangle_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename... INDEXES>
    constexpr SparseDataHyperRectangle_Factory<T, INDEXES...>::SparseDataHyperRectangle_Factory ()
        : SparseDataHyperRectangle_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename... INDEXES>
    constexpr SparseDataHyperRectangle_Factory<T, INDEXES...>::SparseDataHyperRectangle_Factory ([[maybe_unused]] const Hints& hints)
        : SparseDataHyperRectangle_Factory{[] () -> FactoryFunctionType {
            return [] (Configuration::ArgByValueType<T> defaultItem) {
                return Concrete::SparseDataHyperRectangle_stdmap<T, INDEXES...>{defaultItem};
            };
        }()}
    {
    }
    template <typename T, typename... INDEXES>
    inline auto SparseDataHyperRectangle_Factory<T, INDEXES...>::Default () -> const SparseDataHyperRectangle_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename... INDEXES>
    inline auto SparseDataHyperRectangle_Factory<T, INDEXES...>::operator() (Configuration::ArgByValueType<T> defaultItem) const -> ConstructedType
    {
        return this->fFactory_ (defaultItem);
    }
    template <typename T, typename... INDEXES>
    void SparseDataHyperRectangle_Factory<T, INDEXES...>::Register (const optional<SparseDataHyperRectangle_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SparseDataHyperRectangle_Factory{Hints{}};
    }
    template <typename T, typename... INDEXES>
    inline auto SparseDataHyperRectangle_Factory<T, INDEXES...>::AccessDefault_ () -> SparseDataHyperRectangle_Factory&
    {
        static SparseDataHyperRectangle_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
