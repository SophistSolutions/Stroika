/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkCollection_Fastest,
 *              mkCollection_Smallest, mkCollectionWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *              MAYBE????
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T>
            class Collection;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Collection<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Collection_Factory<T>::Register (), or
                 *  replace it statically by template-specailizing Collection_Factory<T>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T>
                class Collection_Factory {
                private:
                    static atomic<Collection<T> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Collection<T> CTOR does so automatically.
                     */
                    nonvirtual Collection<T> operator() () const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given Collection<T>. Note this is a global change.
                     */
                    static void Register (Collection<T> (*factory) () = nullptr);

                private:
                    static Collection<T> Default_ ();
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
#include "Collection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_Factory_h_ */
