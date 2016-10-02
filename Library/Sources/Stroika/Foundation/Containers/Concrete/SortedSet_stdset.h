/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_

#include    "../../StroikaPreComp.h"

#include    "../SortedSet.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *      @todo   Could optimize Equals() test for if both sorted, faster way to compare.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   SortedSet_stdset<T, TRAITS> is an std::map-based concrete implementation of the SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = DefaultTraits::SortedSet<T>>
                class   SortedSet_stdset : public SortedSet<T, TRAITS> {
                private:
                    using   inherited   =   SortedSet<T, TRAITS>;

                public:
                    SortedSet_stdset (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    SortedSet_stdset (const SortedSet_stdset<T, TRAITS>& s);
                    SortedSet_stdset (const std::initializer_list<T>& s, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    template    < typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const SortedSet_stdset<T, TRAITS>*>::value >::type >
                    explicit SortedSet_stdset (const CONTAINER_OF_T& cp, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit SortedSet_stdset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                public:
                    /**
                     */
                    nonvirtual  SortedSet_stdset<T, TRAITS>& operator= (const SortedSet_stdset<T, TRAITS>& m);

                private:
                    class   UpdateSafeIterationContainerRep_;

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
#include    "SortedSet_stdset.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_ */
