/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Mapping.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
                class   Mapping_LinkedList : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType> {
                private:
                    using   inherited   =     Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>;

                public:
                    Mapping_LinkedList ();
                    Mapping_LinkedList (const Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Mapping_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Mapping_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) = default;

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

#include    "Mapping_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_ */
