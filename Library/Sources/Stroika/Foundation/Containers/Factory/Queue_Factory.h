/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkQueue_Fastest,
 *              mkQueue_Smallest, mkQueueWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 */

namespace Stroika::Foundation::Containers {
    template <typename T>
    class Queue;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Queue<> container
     *
     *  Note - you can override the underlying factory dynamically by calling Queue_Factory<T>::Register (), or
     *  replace it statically by template-specializing Queue_Factory<T>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Queue_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<Queue<T> (*) ()> sFactory_;
#else
        static inline atomic<Queue<T> (*) ()> sFactory_{nullptr};
#endif

    public:
        /**
         *  You can call this directly, but there is no need, as the Queue<T> CTOR does so automatically.
         */
        nonvirtual Queue<T> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Queue<T>. Note this is a global change.
         */
        static void Register (Queue<T> (*factory) () = nullptr);

    private:
        static Queue<T> Default_ ();
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Queue_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_Factory_h_ */
