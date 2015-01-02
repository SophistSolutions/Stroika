/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Bijection.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
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
                 *  \brief   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS = Bijection_DefaultTraits<DOMAIN_TYPE, RANGE_TYPE>>
                class   Bijection_LinkedList : public Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType> {
                private:
                    using   inherited   =   Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>;

                public:
                    Bijection_LinkedList ();
                    Bijection_LinkedList (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src);
                    template    <typename CONTAINER_OF_PAIR_KEY_T>
                    explicit Bijection_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp);
                    template    <typename COPY_FROM_ITERATOR_KEY_T>
                    explicit Bijection_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

                public:
                    nonvirtual  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator= (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs);

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

#include    "Bijection_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_ */
