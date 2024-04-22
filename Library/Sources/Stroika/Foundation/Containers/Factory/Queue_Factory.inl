/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Concrete/Queue_DoublyLinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************************* Queue_Factory<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Queue_Factory<T>::Queue_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Queue_Factory<T>::Queue_Factory ()
        : Queue_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Queue_Factory<T>::Queue_Factory ([[maybe_unused]] const Hints& hints)
        : Queue_Factory{[] () -> FactoryFunctionType {
            // @todo could use array impl due to better locality
            return [] () { return Concrete::Queue_DoublyLinkedList<T>{}; };
        }()}
    {
    }
    template <typename T>
    inline auto Queue_Factory<T>::Default () -> const Queue_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Queue_Factory<T>::operator() () const -> ConstructedType
    {
        return this->fFactory_ ();
    }
    template <typename T>
    void Queue_Factory<T>::Register (const optional<Queue_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Queue_Factory{Hints{}};
    }
    template <typename T>
    inline auto Queue_Factory<T>::AccessDefault_ () -> Queue_Factory&
    {
        static Queue_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
