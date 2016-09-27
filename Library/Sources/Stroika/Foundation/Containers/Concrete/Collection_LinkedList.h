/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Collection.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
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
                 *  \brief Collection_LinkedList<T> is an LinkedList-based concrete implementation of the Collection<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
                 *
                 */
                template    <typename T>
                class  Collection_LinkedList : public Collection<T> {
                private:
                    using   inherited   =     Collection<T>;

                public:
                    Collection_LinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    Collection_LinkedList (const T* start, const T* end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    Collection_LinkedList (const Collection<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);
                    Collection_LinkedList (const Collection_LinkedList<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy = ContainerUpdateIteratorSafety::eDEFAULT);

                    nonvirtual  Collection_LinkedList<T>&  operator= (const Collection_LinkedList<T>& rhs) = default;

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
#include    "Collection_LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Collection_LinkedList_h_ */
