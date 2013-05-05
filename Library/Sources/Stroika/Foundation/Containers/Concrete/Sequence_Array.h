/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_h_

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
                 * \brief   Sequence_Array<T> is an Array-based concrete implementation of the Sequence<T> container pattern.
                 */
                template    <class T>
                class   Sequence_Array : public Sequence<T> {
                private:
                    typedef     Sequence<T>  inherited;

                public:
                    Sequence_Array ();
                    Sequence_Array (const Sequence_Array<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_Array (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR>
                    explicit Sequence_Array (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

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




#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_Array_h_ */


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Sequence_Array.inl"



