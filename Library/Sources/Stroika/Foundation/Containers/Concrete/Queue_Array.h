/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Queue.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Queue_Array<T, TRAITS> is an Array-based concrete implementation of the Queue<T, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Queue_DefaultTraits<T>>
                class   Queue_Array : public Queue<T, TRAITS> {
                private:
                    using   inherited   =   Queue<T, TRAITS>;

                public:
                    Queue_Array ();
                    Queue_Array (const Queue_Array<T, TRAITS>& src);
                    template <typename CONTAINER_OF_T>
                    explicit Queue_Array (const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  Queue_Array<T, TRAITS>& operator= (const Queue_Array<T, TRAITS>& rhs) = default;

                public:
                    /**
                     *  \brief  Reduce the space used to store the Sequence<T, TRAITS> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete sequence, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();

                public:
                    /**
                     * This optional API allows pre-reserving space as an optimizaiton.
                     */
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;

                private:
                    nonvirtual  void    AssertRepValidType_ () const;
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

#include    "Queue_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Queue_Array_h_ */
