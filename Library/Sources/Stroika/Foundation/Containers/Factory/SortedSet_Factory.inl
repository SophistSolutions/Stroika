/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include "../Concrete/SortedSet_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********************** SortedSet_Factory<T, INORDER_COMPARER> ******************
     ********************************************************************************
     */
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedSet_Factory<T, INORDER_COMPARER>::SortedSet_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedSet_Factory<T, INORDER_COMPARER>::SortedSet_Factory ()
        : SortedSet_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedSet_Factory<T, INORDER_COMPARER>::SortedSet_Factory ([[maybe_unused]] const Hints& hints)
        : SortedSet_Factory{[] () -> FactoryFunctionType {
            return [] (const INORDER_COMPARER& inorderComparer) { return Concrete::SortedSet_stdset<T>{inorderComparer}; };
        }()}
    {
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedSet_Factory<T, INORDER_COMPARER>::Default () -> const SortedSet_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedSet_Factory<T, INORDER_COMPARER>::operator() (const INORDER_COMPARER& inorderComparer) const -> ConstructedType
    {
        return this->fFactory_ (inorderComparer);
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    void SortedSet_Factory<T, INORDER_COMPARER>::Register (const optional<SortedSet_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedSet_Factory{Hints{}};
    }
    template <typename T, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedSet_Factory<T, INORDER_COMPARER>::AccessDefault_ () -> SortedSet_Factory&
    {
        static SortedSet_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */
