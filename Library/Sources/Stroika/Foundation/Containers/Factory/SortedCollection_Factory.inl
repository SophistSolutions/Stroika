/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_

#include "../Concrete/SortedCollection_stdmultiset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     **************** SortedCollection_Factory<T, INORDER_COMPARER> *****************
     ********************************************************************************
     */
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedCollection_Factory<T, INORDER_COMPARER>::SortedCollection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedCollection_Factory<T, INORDER_COMPARER>::SortedCollection_Factory ()
        : SortedCollection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedCollection_Factory<T, INORDER_COMPARER>::SortedCollection_Factory ([[maybe_unused]] const Hints& hints)
        : SortedCollection_Factory{[] () -> FactoryFunctionType {
            return [] (const INORDER_COMPARER& inorderComparer) { return Concrete::SortedCollection_stdmultiset<T>{inorderComparer}; };
        }()}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedCollection_Factory<T, INORDER_COMPARER>::Default () -> const SortedCollection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedCollection_Factory<T, INORDER_COMPARER>::operator() (const INORDER_COMPARER& inorderComparer) const -> ConstructedType
    {
        return this->fFactory_ (inorderComparer);
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    void SortedCollection_Factory<T, INORDER_COMPARER>::Register (const optional<SortedCollection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedCollection_Factory{Hints{}};
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedCollection_Factory<T, INORDER_COMPARER>::AccessDefault_ () -> SortedCollection_Factory&
    {
        static SortedCollection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
