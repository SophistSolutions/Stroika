/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/Stack_LinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************************* Stack_Factory<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Stack_Factory<T>::Stack_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Stack_Factory<T>::Stack_Factory ()
        : Stack_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Stack_Factory<T>::Stack_Factory ([[maybe_unused]] const Hints& hints)
        : Stack_Factory{nullptr}
    {
    }
    template <typename T>
    inline auto Stack_Factory<T>::Default () -> const Stack_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Stack_Factory<T>::operator() () const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) [[likely]] {
            static const auto kDefault_ = Concrete::Stack_LinkedList<T>{}; // some stacks remain empty, so why not share them...
            return kDefault_;
        }
        else {
            return this->fFactory_ ();
        }
    }
    template <typename T>
    template <typename IT>
    auto Stack_Factory<T>::operator() (IT&& start, IT&& end) const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) [[likely]] {
            return Concrete::Stack_LinkedList<T>{forward<IT> (start), forward<IT> (end)};
        }
        else {
            ConstructedType r = this->fFactory_ ();
            vector<T>       tmp;
            copy (forward<IT> (start), forward<IT> (end), back_inserter (tmp));
            for (auto ri = tmp.rbegin (); ri != tmp.rend (); ++ri) {
                r.Push (*ri);
            }
            return r;
        }
    }
    template <typename T>
    void Stack_Factory<T>::Register (const optional<Stack_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Stack_Factory{Hints{}};
    }
    template <typename T>
    inline auto Stack_Factory<T>::AccessDefault_ () -> Stack_Factory&
    {
        static Stack_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
