/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Deque.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Deque_DoublyLinkedList<T> is an Array-based concrete implementation of the Deque<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <class T>
                class   Deque_DoublyLinkedList : public Deque<T> {
                private:
                    typedef     Deque<T>  inherited;

                public:
                    Deque_DoublyLinkedList ();
                    Deque_DoublyLinkedList (const Deque_DoublyLinkedList<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Deque_DoublyLinkedList (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR>
                    explicit Deque_DoublyLinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

                    nonvirtual  Deque_DoublyLinkedList<T>& operator= (const Deque_DoublyLinkedList<T>& s);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
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

#include    "Deque_DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_h_ */
