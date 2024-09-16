/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedMultiSet_stdmap.h"

namespace Stroika::Foundation::Containers::Concrete {
    template <typename T, typename TRAITS>
    class SortedMultiSet_stdmap; // avoid issue with #include deadly embrace
}
namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     *************** SortedMultiSet_Factory<T, TRAITS, COMPARER> ********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, COMPARER>::SortedMultiSet_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, COMPARER>::SortedMultiSet_Factory ()
        : SortedMultiSet_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, COMPARER>::SortedMultiSet_Factory ([[maybe_unused]] const Hints& hints)
        : SortedMultiSet_Factory{[] () -> FactoryFunctionType {
            return [] (const COMPARER& comparer) {
                if constexpr (Common::IInOrderComparer<T, COMPARER>) {
                    return Concrete::SortedMultiSet_stdmap<T, TRAITS>{comparer};
                }
                else {
                    return Concrete::SortedMultiSet_stdmap<T, TRAITS>{Common::InOrderComparerAdapter<T, COMPARER>{comparer}};
                }
            };
        }()}
    {
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, COMPARER>::Default () -> const SortedMultiSet_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, COMPARER>::operator() (const COMPARER& comparer) const -> ConstructedType
    {
        return this->fFactory_ (comparer);
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    void SortedMultiSet_Factory<T, TRAITS, COMPARER>::Register (const optional<SortedMultiSet_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedMultiSet_Factory{Hints{}};
    }
    template <typename T, typename TRAITS, ITotallyOrderingComparer<T> COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, COMPARER>::AccessDefault_ () -> SortedMultiSet_Factory&
    {
        static SortedMultiSet_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
