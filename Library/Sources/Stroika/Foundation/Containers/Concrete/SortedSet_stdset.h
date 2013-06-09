/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchonizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *      @todo   Could optimize Equals() test for if both sorted, faster way to compare.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief   SortedSet_stdset<T> is an std::map-based concrete implementation of the SortedMapping<Key,T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class   SortedSet_stdset : public SortedSet<T> {
                private:
                    typedef     SortedSet<T>  inherited;

                public:
                    SortedSet_stdset ();
                    SortedSet_stdset (const SortedSet_stdset<T>& s);
                    template    <typename CONTAINER_OF_T>
                    explicit SortedSet_stdset (const CONTAINER_OF_T& cp);
                    template    <typename COPY_FROM_ITERATOR_OF_T>
                    explicit SortedSet_stdset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);


                public:
                    nonvirtual  SortedSet_stdset<T>& operator= (const SortedSet_stdset<T>& m);


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
#include    "SortedSet_stdset.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_ */
