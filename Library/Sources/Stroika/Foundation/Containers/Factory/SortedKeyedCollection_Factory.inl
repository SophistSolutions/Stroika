/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedKeyedCollection_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> *********
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    constexpr SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::SortedKeyedCollection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    constexpr SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::SortedKeyedCollection_Factory ()
        : SortedKeyedCollection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    constexpr SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::SortedKeyedCollection_Factory ([[maybe_unused]] const Hints& hints)
        : SortedKeyedCollection_Factory{[] () -> FactoryFunctionType {
            return [] (const KeyExtractorType& keyExtractor, const KEY_COMPARER& keyComparer) {
                if constexpr (Common::IInOrderComparer<KEY_TYPE, KEY_COMPARER>) {
                    return Concrete::SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>{keyExtractor, keyComparer};
                }
                else {
                    return Concrete::SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>{
                        keyExtractor, Common::InOrderComparerAdapter<KEY_TYPE, KEY_COMPARER>{keyComparer}};
                }
            };
        }()}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    inline auto SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::Default () -> const SortedKeyedCollection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    inline auto SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::operator() (const KeyExtractorType& keyExtractor,
                                                                                              const KEY_COMPARER& keyComparer) const -> ConstructedType
    {
        return this->fFactory_ (keyExtractor, keyComparer);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    void SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::Register (const optional<SortedKeyedCollection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedKeyedCollection_Factory{Hints{}};
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    inline auto SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_COMPARER>::AccessDefault_ () -> SortedKeyedCollection_Factory&
    {
        static SortedKeyedCollection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
