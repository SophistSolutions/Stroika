/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T>
            class SortedSet;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedSet<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SortedSet_Factory<T,LESS_COMPARER>::Register (), or
                 *  replace it statically by template-specailizing SortedSet_Factory<T,TRAITS>::operator() () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename LESS_COMPARER = less<T>>
                class SortedSet_Factory {
                private:
                    static atomic<SortedSet<T> (*) (const LESS_COMPARER&)> sFactory_;

                public:
                    SortedSet_Factory (const LESS_COMPARER& lessComparer);

                public:
                    /**
                     *  You can call this directly, but there is no need, as the SortedSet<T,TRAITS> CTOR does so automatically.
                     */
                    nonvirtual SortedSet<T> operator() () const;

                private:
                    LESS_COMPARER fLessComparer_;

                public:
                    /**
                     *  Register a replacement creator/factory for the given SortedSet<T,TRAITS>. Note this is a global change.
                     */
                    static void Register (SortedSet<T> (*factory) (const LESS_COMPARER&) = nullptr);

                private:
                    static SortedSet<T> Default_ (const LESS_COMPARER& lessComparer);
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
#include "SortedSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_ */
