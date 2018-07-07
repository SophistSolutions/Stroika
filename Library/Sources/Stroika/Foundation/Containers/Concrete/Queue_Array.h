/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Queue.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Add "MAX ENTRIES" feature - so throws/crashes when exceeded. Useful for a variety
 *              of queues, but I'm thinking mostly of Queue of incoming singals and need to avoid
 *              malloc call.
 *
 *      @todo   Redo using circular array strategy (we have this code in OLD stroika!!!)
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Queue_Array<T> is an Array-based concrete implementation of the Queue<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class Queue_Array : public Queue<T> {
                private:
                    using inherited = Queue<T>;

                public:
                    /**
                     */
                    Queue_Array ();
                    Queue_Array (const Queue_Array<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const Queue_Array<T>*>::value>>
                    explicit Queue_Array (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    /**
                     */
                    nonvirtual Queue_Array<T>& operator= (const Queue_Array<T>& rhs) = default;

                public:
                    /**
                     *  \brief  Reduce the space used to store the Queue<T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete sequence, calling this may save memory.
                     */
                    nonvirtual void Compact ();

                public:
                    /**
                     *  This optional API allows pre-reserving space as an optimization. Note that this does not affect
                     *  the semantics of Queue().
                     */
                    nonvirtual size_t GetCapacity () const;
                    nonvirtual void   SetCapacity (size_t slotsAlloced);

                public:
                    /**
                     *  STL-ish alias for GetCapacity ();
                     */
                    nonvirtual size_t capacity () const;

                public:
                    /**
                     *  STL-ish alias for SetCapacity ();
                     */
                    nonvirtual void reserve (size_t slotsAlloced);

                private:
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
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

#include "Queue_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Queue_Array_h_ */
