/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkMultiSet_Fastest,
 *              mkMultiSet_Smallest, mkMultiSetWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T, typename TRAITS>
            class MultiSet;

            namespace Factory {

                /**
                *  \brief   Singleton factory object - Used to create the default backend implementation of a MultiSet<> container
                *
                *  Note - you can override the underlying factory dynamically by calling MultiSet_Factory<T,TRAITS>::Register (), or
                *  replace it statically by template-specailizing MultiSet_Factory<T,TRAITS>::New () - though the later is trickier.
                *
                *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                */
                template <typename T, typename TRAITS>
                class MultiSet_Factory {
                private:
                    static atomic<MultiSet<T, TRAITS> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the MultiSet<T,TRAITS> CTOR does so automatically.
                     */
                    nonvirtual MultiSet<T, TRAITS> operator() () const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given MultiSet<T,TRAITS>. Note this is a global change.
                     */
                    static void Register (MultiSet<T, TRAITS> (*factory) () = nullptr);

                private:
                    static MultiSet<T, TRAITS> Default_ ();
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
#include "MultiSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_ */
