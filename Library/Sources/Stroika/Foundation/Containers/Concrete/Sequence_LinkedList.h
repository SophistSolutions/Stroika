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
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 * \brief   Sequence_LinkedList<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 */
                template    <class T>
                class   Sequence_LinkedList : public Sequence<T> {
                private:
                    typedef     Sequence<T>  inherited;

                public:
                    Sequence_LinkedList ();
                    Sequence_LinkedList (const Sequence_LinkedList<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_LinkedList (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR>
                    explicit Sequence_LinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

                public:
                    nonvirtual  Sequence_LinkedList<T>& operator= (const Sequence_LinkedList<T>& s);

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

#include    "Sequence_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_h_ */
