/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_

#include "../Concrete/DenseDataHyperRectangle_Vector.h"

namespace Stroika::Foundation::Containers {
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle_Vector;
}

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************** DenseDataHyperRectangle_Factory<T> ************************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    constexpr DenseDataHyperRectangle_Factory<T, INDEXES...>::DenseDataHyperRectangle_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename... INDEXES>
    constexpr DenseDataHyperRectangle_Factory<T, INDEXES...>::DenseDataHyperRectangle_Factory ()
        : DenseDataHyperRectangle_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename... INDEXES>
    constexpr DenseDataHyperRectangle_Factory<T, INDEXES...>::DenseDataHyperRectangle_Factory ([[maybe_unused]] const Hints& hints)
        : DenseDataHyperRectangle_Factory{[] () -> FactoryFunctionType {
            return [] (INDEXES... dimensions) {
                return Concrete::DenseDataHyperRectangle_Vector<T, INDEXES...>{forward<INDEXES> (dimensions)...};
            };
        }()}
    {
    }
    template <typename T, typename... INDEXES>
    inline auto DenseDataHyperRectangle_Factory<T, INDEXES...>::Default () -> const DenseDataHyperRectangle_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename... INDEXES>
    inline auto DenseDataHyperRectangle_Factory<T, INDEXES...>::operator() (INDEXES... dimensions) const -> ConstructedType
    {
        return this->fFactory_ (forward<INDEXES> (dimensions)...);
    }
    template <typename T, typename... INDEXES>
    void DenseDataHyperRectangle_Factory<T, INDEXES...>::Register (const optional<DenseDataHyperRectangle_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : DenseDataHyperRectangle_Factory{Hints{}};
    }
    template <typename T, typename... INDEXES>
    inline auto DenseDataHyperRectangle_Factory<T, INDEXES...>::AccessDefault_ () -> DenseDataHyperRectangle_Factory&
    {
        static DenseDataHyperRectangle_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_ */
