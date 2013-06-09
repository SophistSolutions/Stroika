/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Stack.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   VERY INCOMPLETE/WRONG IMPLEMENTATION - ESPECIALLY COPYING WRONG!!!
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Stack_LinkedList<T> is an LinkedList-based concrete implementation of the Stack<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   Stack_LinkedList : public Stack<T> {
                private:
                    typedef     Stack<T>  inherited;

                public:
                    Stack_LinkedList ();
                    Stack_LinkedList (const Stack_LinkedList<T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Stack_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Stack_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);


                public:
                    nonvirtual  Stack_LinkedList<T>& operator= (const Stack_LinkedList<T>& m);

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

#include    "Stack_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Stack_LinkedList_h_ */
