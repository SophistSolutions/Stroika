/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T>
    class Stack;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Stack<> container
     *
     *  Note - you can override the underlying factory dynamically by calling Stack_Factory<T>::Register (), or
     *  replace it statically by template-specializing Stack_Factory<T>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Stack_Factory {
    private:
        static inline atomic<Stack<T> (*) ()> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<T>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {};

    public:
        constexpr Stack_Factory (const Hints& hints = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the Stack<T> CTOR does so automatically.
         */
        nonvirtual Stack<T> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Stack<T>. Note this is a global change.
         */
        static void Register (Stack<T> (*factory) () = nullptr);

    private:
        [[no_unique_address]] const Hints fHints_;

    private:
        static Stack<T> Default_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Stack_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Stack_Factory_h_ */
