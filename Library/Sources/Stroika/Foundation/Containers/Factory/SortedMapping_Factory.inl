/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"

namespace Stroika::Foundation::Containers::Concrete {
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap; // avoid issue with #include deadly embrace
}
namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************ SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedMapping_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedMapping_Factory ()
        : SortedMapping_Factory{AccessDefault_ ()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedMapping_Factory ([[maybe_unused]] const Hints& hints)
        : SortedMapping_Factory{[] () -> FactoryFunctionType {
            return [] (const KEY_INORDER_COMPARER& inOrderComparer) {
                return Concrete::SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE>{inOrderComparer};
            };
        }()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Default () -> const SortedMapping_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::operator() (const KEY_INORDER_COMPARER& inOrderComparer) const
        -> ConstructedType
    {
        return this->fFactory_ (inOrderComparer);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    void SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Register (const optional<SortedMapping_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedMapping_Factory{Hints{}};
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::AccessDefault_ () -> SortedMapping_Factory&
    {
        static SortedMapping_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
