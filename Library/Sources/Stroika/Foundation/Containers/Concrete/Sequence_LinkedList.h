/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Sequence.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Sequence_LinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Sequence_DefaultTraits<T>>
                class   Sequence_LinkedList : public Sequence<T, TRAITS> {
                private:
                    typedef     Sequence<T, TRAITS>  inherited;

                public:
                    Sequence_LinkedList ();
                    Sequence_LinkedList (const Sequence_LinkedList<T, TRAITS>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_LinkedList (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Sequence_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  Sequence_LinkedList<T, TRAITS>& operator= (const Sequence_LinkedList<T, TRAITS>& s);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;
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

#include    "Sequence_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_ */
