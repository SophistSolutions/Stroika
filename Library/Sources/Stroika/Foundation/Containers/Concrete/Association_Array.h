/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_h_

#include    "../../StroikaPreComp.h"

#include    "../Association.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Association_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Association_Array : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType> {
                private:
                    using   inherited   =     Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>;

                public:
                    Association_Array ();
                    Association_Array (const Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& m);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Association_Array (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Association_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual  Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& m);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete Association, calling this may save memory.
                     */
                    nonvirtual  void    Compact ();

                public:
                    /*
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

#include    "Association_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Association_Array_h_ */
