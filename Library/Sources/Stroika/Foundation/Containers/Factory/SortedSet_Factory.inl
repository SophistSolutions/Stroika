/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/SortedSet_stdset.h"

namespace Stroika::Foundation::Containers::Concrete {
    template <typename T>
    class SortedSet_stdset; // avoid issue with #include deadly embrace
}
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
