/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkStack_Fastest,
 *              mkStack_Smallest, mkStackWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
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
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Stack_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<Stack<T> (*) ()> sFactory_;
#else
        static inline atomic<Stack<T> (*) ()> sFactory_{nullptr};
#endif

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
