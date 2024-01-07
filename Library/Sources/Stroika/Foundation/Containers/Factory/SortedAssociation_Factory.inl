/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_

#include "../Concrete/SortedAssociation_stdmultimap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********* SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedAssociation_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedAssociation_Factory ()
        : SortedAssociation_Factory{AccessDefault_ ()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    constexpr SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedAssociation_Factory ([[maybe_unused]] const Hints& hints)
        : SortedAssociation_Factory{[] () -> FactoryFunctionType {
            return [] (const KEY_INORDER_COMPARER& keyInOrderComparer) {
                return Concrete::SortedAssociation_stdmultimap<KEY_TYPE, VALUE_TYPE>{keyInOrderComparer};
            };
        }()}
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Default () -> const SortedAssociation_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::operator() (const KEY_INORDER_COMPARER& keyInOrderComparer) const
        -> ConstructedType
    {
        return this->fFactory_ (keyInOrderComparer);
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    void SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Register (const optional<SortedAssociation_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedAssociation_Factory{Hints{}};
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline auto SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::AccessDefault_ () -> SortedAssociation_Factory&
    {
        static SortedAssociation_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_ */
