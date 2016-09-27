/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_

#include    "../../StroikaPreComp.h"

#include    "../SortedMapping.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Cleanup IteratorRep_ code.
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   SortedMapping_stdmap<Key,T> is an std::map-based concrete implementation of the SortedMapping<Key,T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = DefaultTraits::SortedMapping<KEY_TYPE, VALUE_TYPE>>
                class   SortedMapping_stdmap : public SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> {
                private:
                    using   inherited   =     SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>;

                public:
                    SortedMapping_stdmap (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    SortedMapping_stdmap (const SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                    template    < typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>*>::value >::type >
                    explicit SortedMapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit SortedMapping_stdmap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                public:
                    /**
                     */
                    nonvirtual  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) = default;

                private:
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
#include    "SortedMapping_stdmap.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_h_ */
