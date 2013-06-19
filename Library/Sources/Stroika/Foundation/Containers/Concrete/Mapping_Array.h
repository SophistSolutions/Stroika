/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Mapping_Array<Key, T> is an Array-based concrete implementation of the Mapping<Key,T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename Key, typename T>
                class   Mapping_Array : public Mapping<Key, T> {
                private:
                    typedef     Mapping<Key, T>  inherited;

                public:
                    Mapping_Array ();
                    Mapping_Array (const Mapping_Array<Key, T>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_Array (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);


                public:
                    nonvirtual  Mapping_Array<Key, T>& operator= (const Mapping_Array<Key, T>& m);


                public:
                    /**
                     *  \brief  Reduce the space used to store the Mapping_Array<Key, T> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete Mapping, calling this may save memory.
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

#include    "Mapping_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_Array_h_ */
