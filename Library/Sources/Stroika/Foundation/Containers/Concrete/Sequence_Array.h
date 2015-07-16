/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Sequence.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
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
                 *  \brief   Sequence_Array<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   Sequence_Array : public Sequence<T> {
                private:
                    using   inherited   =     Sequence<T>;

                public:
                    Sequence_Array ();
                    Sequence_Array (const Sequence_Array<T>& s);
                    Sequence_Array (const initializer_list<T>& s);
                    Sequence_Array (const vector<T>& s);
                    template    <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value>::type>
                    explicit Sequence_Array (const CONTAINER_OF_T& s);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Sequence_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  Sequence_Array<T>& operator= (const Sequence_Array<T>& s);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Sequence<T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete sequence, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();

                public:
                    /**
                     * This optional API allows pre-reserving space as an optimization.
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

#include    "Sequence_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_Array_h_ */
