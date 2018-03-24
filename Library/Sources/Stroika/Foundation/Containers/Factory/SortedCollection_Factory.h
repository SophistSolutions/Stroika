/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T>
            class SortedCollection;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedCollection<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling SortedCollection_Factory<T,LESS_COMPARER>::Register (), or
                 *  replace it statically by template-specailizing SortedCollection_Factory<T,LESS_COMPARER>::New () - though the later is trickier.
                 *
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T, typename LESS_COMPARER = less<T>>
                class SortedCollection_Factory {
                private:
                    static atomic<SortedCollection<T> (*) (const LESS_COMPARER&)> sFactory_;

                public:
                    SortedCollection_Factory (const LESS_COMPARER& lessComparer);

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Collection<T> CTOR does so automatically.
                     */
                    nonvirtual SortedCollection<T> operator() () const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given Collection<T>. Note this is a global change.
                     */
                    static void Register (SortedCollection<T> (*factory) (const LESS_COMPARER&) = nullptr);

                private:
                    LESS_COMPARER fLessComparer_;

                private:
                    static SortedCollection<T> Default_ (const LESS_COMPARER& lessComparer);
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
#include "SortedCollection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_ */
