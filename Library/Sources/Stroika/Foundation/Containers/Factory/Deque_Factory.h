/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Deque_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Deque_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkDeque_Fastest,
 *              mkDeque_Smallest, mkDequeWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T>
            class Deque;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Deque<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Deque_Factory<T>::Register (), or
                 *  replace it statically by template-specailizing Deque_Factory<T>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T>
                class Deque_Factory {
                private:
                    static atomic<Deque<T> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Deque<T> CTOR does so automatically.
                     */
                    static Deque<T> New ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Deque<T>. Note this is a global change.
                     */
                    static void Register (Deque<T> (*factory) () = nullptr);

                private:
                    static Deque<T> Default_ ();
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Deque_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Deque_Factory_h_ */
