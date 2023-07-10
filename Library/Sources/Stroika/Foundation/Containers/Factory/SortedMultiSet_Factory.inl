/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_

#include "../Concrete/SortedMultiSet_stdmap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********************* SortedMultiSet_Factory<T, TRAITS> ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::SortedMultiSet_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::SortedMultiSet_Factory ()
        : SortedMultiSet_Factory{AccessDefault_ ()}
    {
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    constexpr SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::SortedMultiSet_Factory ([[maybe_unused]] const Hints& hints)
        : SortedMultiSet_Factory{[] () -> FactoryFunctionType {
            return [] (const INORDER_COMPARER& inOrderComparer) { return Concrete::SortedMultiSet_stdmap<T, TRAITS>{inOrderComparer}; };
        }()}
    {
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::Default () -> const SortedMultiSet_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::operator() (const INORDER_COMPARER& inOrderComparer) const -> ConstructedType
    {
        return this->fFactory_ (inOrderComparer);
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    void SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::Register (const optional<SortedMultiSet_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : SortedMultiSet_Factory{Hints{}};
    }
    template <typename T, typename TRAITS, IInOrderComparer<T> INORDER_COMPARER>
    inline auto SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::AccessDefault_ () -> SortedMultiSet_Factory&
    {
        static SortedMultiSet_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_ */
