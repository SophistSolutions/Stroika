/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Concrete/Sequence_Array.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     **************************** Sequence_Factory<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Sequence_Factory<T>::Sequence_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Sequence_Factory<T>::Sequence_Factory ()
        : Sequence_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Sequence_Factory<T>::Sequence_Factory ([[maybe_unused]] const Hints& hints)
        : Sequence_Factory{nullptr}
    {
    }
    template <typename T>
    inline auto Sequence_Factory<T>::Default () -> const Sequence_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Sequence_Factory<T>::operator() () const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) {
            // Sequence_Array not always the best. Linked list can perform better for some uses,
            // but not clear how best to characterize them in hints --LGP 2023-04-19
            static const auto kDefault_ = Concrete::Sequence_Array<T>{}; // internal call to CloneEmpty if ever added to, but some sequences are empty, so why not share
            return kDefault_;
        }
        else {
            return this->fFactory_ ();
        }
    }
    template <typename T>
    void Sequence_Factory<T>::Register (const optional<Sequence_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Sequence_Factory{Hints{}};
    }
    template <typename T>
    inline auto Sequence_Factory<T>::AccessDefault_ () -> Sequence_Factory&
    {
        static Sequence_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
