/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedAssociation_stdmultimap.h"

namespace Stroika::Foundation::Containers::Concrete {
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap;
}
namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********* SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::SortedAssociation_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::SortedAssociation_Factory ()
        : SortedAssociation_Factory{AccessDefault_ ()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::SortedAssociation_Factory ([[maybe_unused]] const Hints& hints)
        : SortedAssociation_Factory{[] () -> FactoryFunctionType {
            return [] (const COMPARER& keyComparer) {
                if constexpr (IInOrderComparer<KEY_TYPE, COMPARER>) {
                    return Concrete::SortedAssociation_stdmultimap<KEY_TYPE, VALUE_TYPE>{keyComparer};
                }
                else {
                    return Concrete::SortedAssociation_stdmultimap<KEY_TYPE, VALUE_TYPE>{Common::InOrderComparerAdapter<KEY_TYPE, COMPARER>{keyComparer}};
                }
            };
        }()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::Default () -> const SortedAssociation_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::operator() (const COMPARER& keyComparer) const -> ConstructedType
    {
        return this->fFactory_ (keyComparer);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    void SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::Register (const optional<SortedAssociation_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedAssociation_Factory{Hints{}};
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, ITotallyOrderingComparer<KEY_TYPE> COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, COMPARER>::AccessDefault_ () -> SortedAssociation_Factory&
    {
        static SortedAssociation_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
