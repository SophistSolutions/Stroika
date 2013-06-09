/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_

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
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Sequence_stdvector<T> is an std::vector-based concrete implementation of the Sequence<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <class T>
                class   Sequence_stdvector : public Sequence<T> {
                private:
                    typedef     Sequence<T>  inherited;

                public:
                    Sequence_stdvector ();
                    Sequence_stdvector (const Sequence_stdvector<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_stdvector (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    explicit Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  Sequence_stdvector<T>& operator= (const Sequence_stdvector<T>& s);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Sequence<T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete sequence, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();


                public:
                    /*
                     * This optional API allows pre-reserving space as an optimizaiton.
                     */
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
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
#include    "Sequence_stdvector.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_ */
