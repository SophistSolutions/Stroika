/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Mapping_Array : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
                private:
                    using   inherited   =     Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>;

                public:
                    Mapping_Array ();
                    Mapping_Array (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& m);
                    template    < typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>*>::value >::type >
                    explicit Mapping_Array (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& m);

                public:
                    /**
                     *  \brief  Reduce the space used to store the Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS> contents.
                     *
                     *  This has no semantics, no observable behavior. But depending on the representation of
                     *  the concrete Mapping, calling this may save memory.
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

#include    "Mapping_Array.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_Array_h_ */
