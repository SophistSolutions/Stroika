/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Concrete/Deque_DoublyLinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************************* Deque_Factory<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Deque_Factory<T>::Deque_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Deque_Factory<T>::Deque_Factory ()
        : Deque_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Deque_Factory<T>::Deque_Factory ([[maybe_unused]] const Hints& hints)
        : Deque_Factory{[] () -> FactoryFunctionType {
            // @todo some alternatives
            return [] () { return Concrete::Deque_DoublyLinkedList<T>{}; };
        }()}
    {
    }
    template <typename T>
    inline auto Deque_Factory<T>::Default () -> const Deque_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Deque_Factory<T>::operator() () const -> ConstructedType
    {
        return this->fFactory_ ();
    }
    template <typename T>
    void Deque_Factory<T>::Register (const optional<Deque_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Deque_Factory{Hints{}};
    }
    template <typename T>
    inline auto Deque_Factory<T>::AccessDefault_ () -> Deque_Factory&
    {
        static Deque_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
