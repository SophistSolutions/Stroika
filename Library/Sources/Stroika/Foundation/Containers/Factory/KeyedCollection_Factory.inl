/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_

#include "../Concrete/KeyedCollection_LinkedList.h"
#include "../Concrete/KeyedCollection_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     * KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER> *
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    constexpr KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::KeyedCollection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    constexpr KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::KeyedCollection_Factory ()
        : KeyedCollection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    constexpr KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::KeyedCollection_Factory ([[maybe_unused]] const Hints& hints)
        : KeyedCollection_Factory{[hints] () -> FactoryFunctionType {
            if constexpr (is_same_v<KEY_EQUALS_COMPARER, equal_to<KEY_TYPE>> and Configuration::has_lt_v<KEY_TYPE>) {
                return [] (const KEY_EXTRACTOR& keyExtractor, [[maybe_unused]] const KEY_EQUALS_COMPARER& keyComparer) {
                    return Concrete::KeyedCollection_stdset<T, KEY_TYPE, TRAITS>{keyExtractor}; // if using == as equals comparer, just map to < for in-order comparison
                };
            }
            else {
                /*
                 *  Note - though this is not an efficient implementation of KeyedCollection<> for large sizes, its probably the most
                 *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                 *  a hash function. And its quite reasonable for small KeyedCollection's - which are often the case.
                 *
                 *  Calls may use an explicit initializer of KeyedCollection_xxx<> to get better performance for large sized
                 *  maps.
                 */
                return [] (const KEY_EXTRACTOR& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer) {
                    return Concrete::KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>{keyExtractor, keyComparer};
                };
            }
        }()}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    inline auto KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::Default () -> const KeyedCollection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    inline auto KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::operator() (const KEY_EXTRACTOR& keyExtractor,
                                                                                                              const KEY_EQUALS_COMPARER& keyComparer) const
        -> ConstructedType
    {
        return this->fFactory_ (keyExtractor, keyComparer);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    void KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::Register (const optional<KeyedCollection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : KeyedCollection_Factory{Hints{}};
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    inline auto KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>::AccessDefault_ () -> KeyedCollection_Factory&
    {
        static KeyedCollection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_ */
