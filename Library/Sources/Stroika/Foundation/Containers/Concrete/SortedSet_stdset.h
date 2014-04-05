/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                template    <typename T, typename TRAITS = SortedSet_DefaultTraits<T>>
                class   SortedSet_stdset : public SortedSet<T, TRAITS> {
                private:
                    using   inherited   =   SortedSet<T, TRAITS>;

                public:
                    SortedSet_stdset ();
                    SortedSet_stdset (const SortedSet_stdset<T, TRAITS>& s);
                    SortedSet_stdset (const std::initializer_list<T>& s);
                    template    <typename CONTAINER_OF_T>
                    explicit SortedSet_stdset (const CONTAINER_OF_T& cp);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit SortedSet_stdset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    nonvirtual  SortedSet_stdset<T, TRAITS>& operator= (const SortedSet_stdset<T, TRAITS>& m);


                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   Rep_;
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
