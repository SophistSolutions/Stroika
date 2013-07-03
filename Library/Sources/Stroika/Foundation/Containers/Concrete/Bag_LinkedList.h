/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Bag.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 *  \brief Bag_LinkedList<T> is an LinkedList-based concrete implementation of the Bag<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T, typename TRAITS = Bag_DefaultTraits<T>>
                class  Bag_LinkedList : public Bag<T, TRAITS> {
                private:
                    typedef     Bag<T, TRAITS>  inherited;

                public:
                    Bag_LinkedList ();
                    Bag_LinkedList (const T* start, const T* end);
                    Bag_LinkedList (const Bag<T, TRAITS>& bag);
                    Bag_LinkedList (const Bag_LinkedList<T, TRAITS>& bag);

                    nonvirtual  Bag_LinkedList<T, TRAITS>&  operator= (const Bag_LinkedList<T, TRAITS>& rhs);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
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
#include    "Bag_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_ */
