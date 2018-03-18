/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSet_Fastest,
 *              mkSet_Smallest, mkSetWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T>
            class Set;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Set<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Set_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Set_Factory<T,TRAITS>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename EQUALS_COMPARER>
                class Set_Factory {
                private:
                    static atomic<Set<T> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Set<T,TRAITS> CTOR does so automatically.
                     */
                    nonvirtual Set<T> operator() () const;
                    nonvirtual Set<T> operator() (const EQUALS_COMPARER& equalsComparer) const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given Set<T,TRAITS>. Note this is a global change.
                     */
                    static void Register (Set<T> (*factory) () = nullptr);

                private:
                    static Set<T> Default_ (const EQUALS_COMPARER& equalsComparer);

                private:
                    template <typename CHECK_T>
                    static Set<T> Default_SFINAE_ (const EQUALS_COMPARER& equalsComparer, CHECK_T*, typename enable_if<Configuration::has_lt<CHECK_T>::value>::type* = 0);
                    static Set<T> Default_SFINAE_ (const EQUALS_COMPARER& equalsComparer, ...);
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
#include "Set_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_Factory_h_ */
