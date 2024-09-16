/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     **************** SortedCollection_Factory<T, COMPARER> *****************
     ********************************************************************************
     */
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    constexpr SortedCollection_Factory<T, COMPARER>::SortedCollection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    constexpr SortedCollection_Factory<T, COMPARER>::SortedCollection_Factory ()
        : SortedCollection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    constexpr SortedCollection_Factory<T, COMPARER>::SortedCollection_Factory ([[maybe_unused]] const Hints& hints)
        : SortedCollection_Factory{[] () -> FactoryFunctionType {
            return [] (const COMPARER& comparer) {
                if constexpr (IInOrderComparer<COMPARER, T>) {
                    return Concrete::SortedCollection_stdmultiset<T>{comparer};
                }
                else {
                    return Concrete::SortedCollection_stdmultiset<T>{Common::ThreeWayComparerAdapter<T, COMPARER>{comparer}};
                }
            };
        }()}
    {
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    inline auto SortedCollection_Factory<T, COMPARER>::Default () -> const SortedCollection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    inline auto SortedCollection_Factory<T, COMPARER>::operator() (const COMPARER& comparer) const -> ConstructedType
    {
        return this->fFactory_ (comparer);
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    void SortedCollection_Factory<T, COMPARER>::Register (const optional<SortedCollection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedCollection_Factory{Hints{}};
    }
    template <typename T, IThreeWayAdaptableComparer<T> COMPARER>
    inline auto SortedCollection_Factory<T, COMPARER>::AccessDefault_ () -> SortedCollection_Factory&
    {
        static SortedCollection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
